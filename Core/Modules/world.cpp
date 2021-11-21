#include "world.h"

uint32_t World::get_width() const
{
	return width;
}

uint32_t World::get_height() const
{
	return height;
}

uint32_t World::get_user_amount() const
{
	return max_users_amount;
}

uint32_t* World::get_block_map() const
{
	return (uint32_t*)block_map;
}

WorldUser* World::get_user_map() const
{
	return (WorldUser*)user_map;
}
