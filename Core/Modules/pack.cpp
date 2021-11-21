#include "pack.h"

void PACK::load_package(const package& pkg)
{
	pkg.read_as_data((char*)this, sizeof(*this));
}

package PACK::make_package()
{
	package pkg;
	pkg.import_as_data((char*)this, sizeof(*this));
	return pkg;
}

PACK::PACK(const PACK& p)
	: type(p.type)
{
	memcpy_s(&data, sizeof(data), &p.data, sizeof(p.data));
}

int32_t PACK::get_type() const
{
	return type;
}

const packs_map_update& PACK::get_as_map_update() const
{
	return data.map_update;
}

const packs_user_update& PACK::get_as_user_update() const
{
	return data.user_update;
}

const packs_username_update& PACK::get_as_username_update() const
{
	return data.username_update;
}

void PACK::make_this_map_update(packs_map_update& ref)
{
	type = static_cast<int32_t>(package_type::PACKS_MAP_UPDATE);
	memcpy_s((void*)&data, sizeof(packs_map_update), &ref, sizeof(ref));
}

void PACK::make_this_username_update(packs_username_update& ref)
{
	type = static_cast<int32_t>(package_type::PACKS_USERNAME_UPDATE);
	memcpy_s((void*)&data, sizeof(packs_username_update), &ref, sizeof(ref));
}

void PACK::make_this_user_update(packs_user_update& ref)
{
	type = static_cast<int32_t>(package_type::PACKS_USER_UPDATE);
	memcpy_s((void*)&data, sizeof(packs_user_update), &ref, sizeof(ref));
}
