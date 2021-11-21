#pragma once

#include <Lunaris/all.h>

using namespace Lunaris;

constexpr size_t world_map_width = 32;
constexpr size_t world_map_height = 18;
constexpr double world_delta_update_time_package = 1.0 / 80;
constexpr double user_update_max_rate = 1.0 / 1000;

constexpr uint32_t max_users_amount = 8;
constexpr size_t username_length_max = 64;

const std::string default_host_ip = "localhost";
constexpr u_short default_host_port = 36963;


const std::string masterkey_config = "keybinds";
const size_t num_of_keys_wasd = 4;
const std::string key_wasd_keys[num_of_keys_wasd] = { "up", "left", "down", "right" };
const int32_t key_wasd_default[num_of_keys_wasd] = { ALLEGRO_KEY_W, ALLEGRO_KEY_A, ALLEGRO_KEY_S, ALLEGRO_KEY_D };

#define IABS(X) (X > 0 ? X : -X)
#define TOONESCALE(X) (X > 0 ? 1 : (X < 0 ? -1 : 0))