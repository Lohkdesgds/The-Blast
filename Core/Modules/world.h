#pragma once

#include <Lunaris/all.h>
#include "worlduser.h"
#include "defaults.h"

using namespace Lunaris;

class World : NonCopyable, NonMovable {
public:
	enum class block_types : uint32_t {EMPTY, BLOCK, __LAST = BLOCK};
protected:
	uint32_t block_map[world_map_width * world_map_height];
	WorldUser user_map[max_users_amount];
	uint32_t width = world_map_width;
	uint32_t height = world_map_height;
public:
	uint32_t get_width() const;
	uint32_t get_height() const;
	uint32_t get_user_amount() const;
	uint32_t* get_block_map() const;
	WorldUser* get_user_map() const;
};