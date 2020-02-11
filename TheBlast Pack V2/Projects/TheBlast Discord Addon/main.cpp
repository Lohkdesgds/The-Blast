#include "..\..\Libraries\LSW related\LSW\organizer\organizer.h"
#include "..\..\Libraries\LSW related\LSW external optional dependencies\discord_thread\discord_thread.h"

using namespace LSW::v4;

int main(int argc, char* argv[]) {
	gfile logg;


	DiscordConnection* connection = nullptr;

	try {
		connection = new DiscordConnection();
		connection->lock();
		connection->getActivity().GetAssets().SetLargeImage("icon_large");
		connection->getActivity().GetAssets().SetSmallImage("icon_large");
		connection->getActivity().GetAssets().SetLargeText("The Blast V2");
		connection->getActivity().GetAssets().SetSmallText("by LSW");
		connection->getActivity().SetName("Loading");
		connection->getActivity().SetDetails("Syncing...");
		connection->unlock();
	}
	catch (Abort::warn w) {
		logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got Discord integration error: " << w.from() << " -> " << w.details() << "; errID: " << w.getErrN() << L::ELF;
	}
	catch (Abort::abort a) {
		throw a; // must close the game, so throw to main try
	}
	
	con_client client;
	client.hookPrint([&logg](const std::string u) { logg << L::SLF << fsr(__FUNCSIG__) << "CONNECTION: " << u << L::ELF; });

	if (!client.connect()) {
		logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Couldn't connect to any local game running!" << L::ELF;
		logg.flush();
		return -1;
	}

	/*
	Let's setup like this
	0 = command
	1 = EXIT
	*/

	while (1) {
		if (!client.still_on()) {
			logg << L::SLF << fsr(__FUNCSIG__) << "CONNECTION DIED" << L::ELF;
			return 0;
		}

		if (client.hasPackage()) {
			Constants::final_package pkg;
			client.recv(pkg);

			if (pkg.data_type == 1) { // die
				logg << L::SLF << fsr(__FUNCSIG__) << "CALL KILL " << pkg.variable_data << L::ELF;
				client.kill_connection();
				return 0;
			}
			if (pkg.data_type == 2) { // cmd
				logg << L::SLF << fsr(__FUNCSIG__) << "Received RAW: " << pkg.variable_data << L::ELF;

				auto gen = Tools::breakLines(pkg.variable_data, ":", "");

				connection->lock();
				for (auto& i : gen) {
					if (i.first == "SetDetails") {
						connection->getActivity().SetDetails(i.second.c_str());
					}
					else if (i.first == "SetStart") {
						connection->getActivity().GetTimestamps().SetStart(atoll(i.second.c_str()));
					}
				}
				connection->unlock();
			}
		}
	}
}
