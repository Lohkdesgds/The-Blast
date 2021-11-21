#include "worldserver.h"

void WorldServer::generate()
{
	// must generate better worlds in the future

	for (uint32_t yy = 0; yy < this->get_height(); yy++) {
		for (uint32_t xx = 0; xx < this->get_width(); xx++) {

			if (yy == 0 || yy == (get_height() - 1) || xx == 0 || xx == (get_width() - 1))
			{
				block_map[yy * get_width() + xx] = static_cast<uint32_t>(World::block_types::BLOCK);
			}
			else {
				block_map[yy * get_width() + xx] = static_cast<uint32_t>(World::block_types::EMPTY);
			}

		}
	}

	for (uint32_t p = 0; p < get_user_amount(); p++) {
		auto& eache = get_user_map()[p];
		eache.user_id = 0;
		eache.posx = 1;
		eache.posy = 1;
	}
}

PACK WorldServer::generate_pack() const
{
	PACK pack;
	packs_map_update mup;
	memcpy_s(&mup.block_map, sizeof(mup.block_map), &block_map, sizeof(block_map));
	memcpy_s(&mup.user_map, sizeof(mup.user_map), &user_map, sizeof(user_map));
	pack.make_this_map_update(mup);
	return pack;
}
