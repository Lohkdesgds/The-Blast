#pragma once

#include <Lunaris/all.h>
#include "defaults.h"

using namespace Lunaris;

struct EachClient {
	int32_t user_id = 0;
	TCP_client client;
	char name[username_length_max];
};