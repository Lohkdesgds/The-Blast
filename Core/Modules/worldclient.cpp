#include "worldclient.h"

void WorldClient::loop_update()
{
	if (user.client.has_socket())
	{
		const package pkgu = user.client.recv(sizeof(PACK));
		if (pkgu.read_as_array().size() == 0) return;

		PACK pack;
		pack.load_package(pkgu);

		switch (pack.get_type()) {
		case static_cast<int32_t>(PACK::package_type::PACKS_MAP_UPDATE):
		{
			//cout << console::color::DARK_AQUA << "<- update map -";
			const auto& mup = pack.get_as_map_update();
			std::lock_guard<std::mutex> muu(safe_map_update);
			memcpy_s(&block_map, sizeof(block_map), &mup.block_map, sizeof(mup.block_map));
			memcpy_s(&user_map, sizeof(user_map), &mup.user_map, sizeof(mup.user_map));
		}
		break;
		default:
			cout << console::color::RED << "Skipped unexpected package type @ WorldClient: TYPEID=" << pack.get_type() << ";TOTALSIZE=" << pkgu.read_as_array().size();
			break;
		}
	}
	else std::this_thread::sleep_for(std::chrono::milliseconds(100)); // may die, do not kill the cpu!
}

bool WorldClient::connect(const std::string& nam)
{
	sprintf_s(this->user.name, "%s", nam.substr(0, sizeof(this->user.name) - 1).c_str());
	if (user.client.setup(socket_config().set_ip_address(default_host_ip).set_port(default_host_port))) {
		PACK pkg;
		{
			packs_username_update usrupd;
			if (memcpy_s(&usrupd.name, sizeof(usrupd.name), user.name, sizeof(user.name)) != 0) {
				user.client.close_socket();
				return false;
			}
			usrupd.id = 0;
			pkg.make_this_username_update(usrupd);
		}
		if (!user.client.send(pkg.make_package())) {
			user.client.close_socket();
		}
		const package wtr = user.client.recv(sizeof(PACK));
		pkg.load_package(wtr);
		if (pkg.get_type() != static_cast<int32_t>(PACK::package_type::PACKS_USERNAME_UPDATE) || pkg.get_as_username_update().id == 0) {
			user.client.close_socket();
			return false;
		}

		if (memcpy_s(&user.name, sizeof(user.name), pkg.get_as_username_update().name, sizeof(pkg.get_as_username_update().name)) != 0) {
			user.client.close_socket();
			return false;
		}
		user.user_id = pkg.get_as_username_update().id;

		update_map_auto.task_async([this] {loop_update(); }, thread::speed::INTERVAL, world_delta_update_time_package * 0.5); // can recv faster

		return true;
	}
	return false;
}

void WorldClient::disconnect()
{
	update_map_auto.join();
	user.client.close_socket();
	user.user_id = 0;
}

void WorldClient::make_move_player(int32_t x, int32_t y)
{
	if (!user.client.has_socket()) return;
	if (al_get_time() - latest_send_time < user_update_max_rate) return;

	x = TOONESCALE(x);
	y = TOONESCALE(y);

	packs_user_update usrupd;
	usrupd.id = user.user_id;

	for (uint32_t p = 0; p < this->get_user_amount(); p++) {
		WorldUser& each = user_map[p];
		if (each.user_id == usrupd.id) {
			usrupd.posx = each.posx + x;
			usrupd.posy = each.posy + y;

			if (each.posx == 0 && x < 0) return; // not allowed
			if (x > 0 && each.posx + x >= get_width()) return; // not allowed
			if (each.posy == 0 && y < 0) return; // not allowed
			if (y > 0 && each.posy + y >= get_height()) return; // not allowed
			// we good

			usrupd.posx = each.posx + x;
			usrupd.posy = each.posy + y;

			each.posx = usrupd.posx;
			each.posy = usrupd.posy;

			PACK pkg;
			pkg.make_this_user_update(usrupd);
			if (user.client.send(pkg.make_package())) {
				latest_send_time = al_get_time(); // count only valid packages
				cout << console::color::DARK_AQUA << "- update player ->";
			}
			else {
				cout << console::color::RED << "Couldn't send keyboard new event. Send failed.";
			}

			return;
		}
	}
}

void WorldClient::iterate_map_auto(std::function<void(uint32_t, uint32_t, int32_t)> f)
{
	if (!f) return;

	std::lock_guard<std::mutex> muu(safe_map_update);
	for (uint32_t yy = 0; yy < this->get_height(); yy++) {
		for (uint32_t xx = 0; xx < this->get_width(); xx++) {
			f(xx, yy, this->block_map[yy * get_width() + xx]);
		}
	}
}

void WorldClient::iterate_users_auto(std::function<void(WorldUser&)> f)
{
	if (!f) return;

	for (uint32_t p = 0; p < this->get_user_amount(); p++) {
		if (user_map[p].user_id != 0) f(user_map[p]);
	}
}

int32_t WorldClient::get_my_id() const
{
	return user.user_id;
}
