#pragma once

#include <Lunaris/all.h>
#include "world.h"
#include "pack.h"
#include "eachclient.h"

using namespace Lunaris;

class WorldClient : public World {
	std::mutex safe_map_update;
	thread update_map_auto;
	double latest_send_time = 0.0; // sec. Must send up to 1 per 50 ms

	void loop_update();
protected:
	EachClient user;
public:
	bool connect(const std::string&); // username
	void disconnect();
	void make_move_player(int32_t, int32_t); // keyboard send command, server updates position on screen.
	void iterate_map_auto(std::function<void(uint32_t, uint32_t, int32_t)>); // x, y, id
	void iterate_users_auto(std::function<void(WorldUser&)>); // iterate all users
	int32_t get_my_id() const;
};