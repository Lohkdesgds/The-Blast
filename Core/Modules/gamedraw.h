#pragma once

#include <Lunaris/all.h>
#include "worldclient.h"

using namespace Lunaris;


struct TextureMap {
	uint32_t block_id = 0;
	hybrid_memory<texture> bmp;
};

struct UserInMap {
	uint32_t user_id = 0;
	block blk; // draw smooth on screen
	//text txt; // its name // non implemented yet

	UserInMap();
	void set_name(const std::string&);
	void set_color(const color&);
	void reset();
};

class GameDraw {
	WorldClient connection;
	std::vector<TextureMap> textures;
	bool is_display_drawing_this = false;
	display& disp;
	config& conf_keys;
	keys kbkeys;
	std::string user_name;
	int32_t w_key = 0, a_key = 0, s_key = 0, d_key = 0; // shortcut
	UserInMap users_transl[max_users_amount];

	void draw_map_function();
	void on_key_send_function(const keys::key_event&);
public:
	GameDraw(display&, config&);
	void add_texture(hybrid_memory<texture>, uint32_t);
	void set_user_name(const std::string&);
	void check_config(); // guarantee WASD
	bool connect_and_enable_keyboard();
	void draw_this();
	void disconnect();
};