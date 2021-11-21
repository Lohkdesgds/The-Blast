#pragma once

#include <Lunaris/all.h>
#include "worldserver.h"
#include "eachclient.h"
#include "defaults.h"

using namespace Lunaris;

class Host {
	std::unique_ptr<TCP_host> hosting;
	safe_vector<EachClient> clients;
	WorldServer worldmap;
	thread client_handling_timed;

	void loop_listen();
	void loop_update_users_and_map_gen();
public:
	bool start();
	void yield();
	void close();
};