#pragma once

#include <Lunaris/all.h>
#include "world.h"

using namespace Lunaris;

struct packs_map_update : public World {
	using World::block_map;
	using World::user_map;
	using World::width;
	using World::height;
	packs_map_update() {}
};

struct packs_user_update {
	uint32_t posx;
	uint32_t posy;
	int32_t id;
};

struct packs_username_update {
	int32_t id;
	char name[username_length_max];
};

class PACK {
public:
	enum class package_type : int32_t {PACKS_USER_UPDATE, PACKS_MAP_UPDATE, PACKS_USERNAME_UPDATE};
private:
	int32_t type = 0;
	union {
		packs_map_update map_update;
		packs_user_update user_update;
		packs_username_update username_update;
	} data{};
public:
	PACK() = default;
	PACK(const PACK&);

	void load_package(const package&);
	package make_package();

	int32_t get_type() const;
	const packs_map_update& get_as_map_update() const;
	const packs_user_update& get_as_user_update() const;
	const packs_username_update& get_as_username_update() const;

	void make_this_map_update(packs_map_update&);
	void make_this_username_update(packs_username_update&);
	void make_this_user_update(packs_user_update&);
};