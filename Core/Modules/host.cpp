#include "host.h"

void Host::loop_listen()
{
	EachClient cli;

	cli.client = hosting->listen();
	if (!cli.client.has_socket()) return;

	if (clients.size() >= max_users_amount) {
		cout << "Disconnected someone because the lobby is full";
		cli.client.close_socket(); // drop
		return;
	}

	const package pkg = cli.client.recv(sizeof(PACK));
	PACK pack;
	pack.load_package(pkg);

	if (pack.get_type() != static_cast<int32_t>(PACK::package_type::PACKS_USERNAME_UPDATE)) {
		cli.client.close_socket();
		return;
	}

	packs_username_update nuser = pack.get_as_username_update();
		
	if (memcpy_s(&cli.name, sizeof(cli.name), &nuser.name, sizeof(nuser.name)) != 0) {
		cli.client.close_socket();
		return;
	}

	clients.csafe([&](const std::vector<EachClient>& vec) {
		while (1) {
			cli.user_id = static_cast<int32_t>(random() % (1 << 16));

			bool can_continue = true; // true if no user with this random id

			for (const auto& it : vec) {
				if (it.user_id == cli.user_id) {
					can_continue = false;
					break;
				}
			}

			if (!can_continue) continue;
			break;
		}
	});

	bool found_one_good = false;

	clients.csafe([&](const std::vector<EachClient>& vec) {
		for (uint32_t p = 0; p < this->worldmap.get_user_amount(); p++) {
			auto& eache = worldmap.get_user_map()[p];
			if (eache.user_id == 0) {
				eache.user_id = cli.user_id;
				memcpy_s(&eache.name, sizeof(eache.name), &cli.name, sizeof(cli.name));
				eache.posx = 1 + random() % 10;
				eache.posy = 1 + random() % 10;
				found_one_good = true;
				return;
			}
		}
	});

	if (!found_one_good) {
		cout << "Disconnected someone because it failed to find a place in the map.";
		cli.client.close_socket(); // drop
		return;
	}

	nuser.id = cli.user_id;

	pack.make_this_username_update(nuser);

	if (!cli.client.send(pack.make_package())) {
		cli.client.close_socket();
		return;
	}

	cout << "Client \"" << cli.name << "\" connected as ID '" << cli.user_id << "'";

	clients.push_back(std::move(cli));
}

void Host::loop_update_users_and_map_gen()
{
	clients.safe([this](std::vector<EachClient>& vec) {

		// generate new info
		package pkg = this->worldmap.generate_pack().make_package(); // most up to date EVER

		// update users
		for(size_t p = 0; p < vec.size(); p++) 
		{
			auto& it = vec[p];

			if (!it.client.send(pkg)) {
				if (!it.client.has_socket()) {
					cout << "Client \"" << it.name << "\" aka ID '" << it.user_id << "' disconnected.";

					for (uint32_t p = 0; p < this->worldmap.get_user_amount(); p++) {
						auto& eache = worldmap.get_user_map()[p];
						if (eache.user_id == it.user_id) {
							eache.user_id = 0;
							break;
						}
					}

					it.client.close_socket(); // disconnect
					vec.erase(vec.begin() + p);
					p--;
					continue;
				}
				else {
					cout << "Client \"" << it.name << "\" aka ID '" << it.user_id << "' is having buffering issues.";
				}
			}
			else {
				//cout << console::color::DARK_AQUA << "- update map #" << it.user_id << " ->";
			}
		}


		// update user stuff they sent
		for (size_t p = 0; p < vec.size(); p++)
		{
			auto& it = vec[p];

			package recvd;
			while ([&] { recvd = it.client.recv(sizeof(PACK), false); return recvd.read_as_array().size() > 0; }())
			{
				PACK pack;
				pack.load_package(recvd);

				switch (pack.get_type()) {
				case static_cast<int32_t>(PACK::package_type::PACKS_USER_UPDATE):
				{
					const packs_user_update& pku = pack.get_as_user_update();
					cout << console::color::DARK_AQUA << "<- update player #" << pku.id << " -";

					for (uint32_t p = 0; p < this->worldmap.get_user_amount(); p++) {
						auto& eache = worldmap.get_user_map()[p];
						if (eache.user_id == pku.id) {
							if (IABS(static_cast<int32_t>(pku.posx) - static_cast<int32_t>(eache.posx)) <= 1 && IABS(static_cast<int32_t>(pku.posy) - static_cast<int32_t>(eache.posy)) <= 1 && pku.posx < worldmap.get_width() && pku.posy < worldmap.get_height()) {
								eache.posx = pku.posx;
								eache.posy = pku.posy;
							}
						}
					}
				}
				break;
				default:
					cout << console::color::RED << "Skipped unexpected package type @ Host: TYPEID=" << pack.get_type() << ";TOTALSIZE=" << recvd.read_as_array().size();
					break;

				}
			}
		}
	});
}

bool Host::start()
{
	hosting = std::make_unique<TCP_host>();
	if (!hosting->setup(socket_config().set_port(default_host_port))) return false;

	worldmap.generate();
	client_handling_timed.task_async([this] {loop_update_users_and_map_gen(); }, thread::speed::INTERVAL, world_delta_update_time_package);

	return true;
}

void Host::yield()
{
	while (1) loop_listen();
}

void Host::close()
{
	client_handling_timed.join();
	clients.clear(); // disconnect everyone
	hosting.reset();
}
