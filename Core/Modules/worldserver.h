#pragma once

#include <Lunaris/all.h>
#include "world.h"
#include "pack.h"

using namespace Lunaris;

class WorldServer : public World {
public:
	void generate();
	PACK generate_pack() const;
};