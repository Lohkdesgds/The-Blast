#pragma once

#include <Lunaris/all.h>
#include "defaults.h"

using namespace Lunaris;

struct WorldUser {
	int32_t user_id = 0;
	char name[username_length_max];
	uint32_t posx = 0;
	uint32_t posy = 0;
};