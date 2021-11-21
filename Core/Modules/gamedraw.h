#pragma once

#include <Lunaris/all.h>
#include "worldclient.h"

using namespace Lunaris;

const std::string masterkey_config = "keybinds";

const size_t num_of_keys_wasd = 4;
const std::string key_wasd_keys[num_of_keys_wasd] = {"up", "left", "down", "right"};
const int32_t key_wasd_default[num_of_keys_wasd] = {ALLEGRO_KEY_W, ALLEGRO_KEY_A, ALLEGRO_KEY_S, ALLEGRO_KEY_D};

struct TextureMap {
	uint32_t block_id = 0;
	hybrid_memory<texture> bmp;
};

class GameDraw {
	WorldClient connection;
	std::vector<TextureMap> textures;
	bool is_display_drawing_this = false;
	display_async& disp;
	config& conf_keys;
	keys kbkeys;
	std::string user_name;
	int32_t w_key = 0, a_key = 0, s_key = 0, d_key = 0; // shortcut

	void draw_map_function();
	void on_key_send_function(const keys::key_event&);
public:
	GameDraw(display_async&, config&);
	void add_texture(hybrid_memory<texture>, uint32_t);
	void set_user_name(const std::string&);
	void check_config(); // guarantee WASD
	bool connect_and_enable_draw_and_keyboard();
	void disconnect();
};