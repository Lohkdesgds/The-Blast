#include "gamedraw.h"

void GameDraw::draw_map_function()
{
	if (textures.size() == 0) {
		cout << console::color::RED << "TEXTURES MAP IS EMPTY!";
		return;
	}

	color(16,16,16).clear_to_this();

	const float map_x = disp.get_width() * 1.0f / connection.get_width();
	const float map_y = disp.get_height() * 1.0f / connection.get_height();

	// for now let's just draw

	transform easy;
	easy.identity();
	easy.apply();

	connection.iterate_map_auto(
		[this, &map_x, &map_y](const uint32_t xx, const uint32_t yy, const int32_t blkid) 
		{
			const auto ref = std::find_if(textures.begin(), textures.end(), [&](TextureMap& tm) {return tm.block_id == blkid; });
			if (ref == textures.end()) {
				cout << console::color::YELLOW << "UNKNOWN TEXTURE ID " << blkid;
			}
			else {


				ref->bmp->draw_scaled_at(xx * map_x, yy * map_y, map_x, map_y);


				//ref->bmp->draw_scaled_at(xx * map_x, yy * map_y, map_x, map_y);
				//al_draw_filled_rectangle(xx * map_x, yy * map_y, map_x, map_y, color((int)(70 + random() % 50), 70 + random() % 50, 70 + random() % 50));
			}
		}
	);
	connection.iterate_users_auto(
		[this, &map_x, &map_y](const WorldUser& usr)
		{
			al_draw_filled_rectangle(usr.posx * 1.0f * map_x, usr.posy * 1.0f * map_y, (usr.posx + 1) * 1.0f * map_x, (usr.posy + 1) * 1.0f * map_y, usr.user_id == connection.get_my_id() ? color(235, 235, 235) : color(127, 127, 127));
		}
	);
}

void GameDraw::on_key_send_function(const keys::key_event& ev)
{
	if (connection.get_my_id() == 0) return;

	int32_t updown = 0;
	int32_t leftright = 0;

	if (kbkeys.is_key_pressed(s_key)) updown = 1;
	if (kbkeys.is_key_pressed(w_key)) updown = updown != 0 ? 0 : -1;
	if (kbkeys.is_key_pressed(a_key)) leftright = -1;
	if (kbkeys.is_key_pressed(d_key)) leftright = leftright != 0 ? 0 : 1;

	connection.make_move_player(leftright, updown);
}

GameDraw::GameDraw(display_async& d, config& c)
	: disp(d), conf_keys(c)
{
}

void GameDraw::add_texture(hybrid_memory<texture> textur, uint32_t id)
{
	if (is_display_drawing_this) throw std::runtime_error("You should have set this before connect_and_enable_draw_and_keyboard!");

	for (auto& it : textures) {
		if (it.block_id == id) {
			it.bmp = textur;
			return;
		}
	}
	textures.push_back({ id, textur });
}

void GameDraw::set_user_name(const std::string& nam)
{
	if (is_display_drawing_this) throw std::runtime_error("You should have set this before connect_and_enable_draw_and_keyboard!");
	user_name = nam.substr(0, username_length_max - 1);
}

void GameDraw::check_config()
{
	if (is_display_drawing_this) throw std::runtime_error("You should have set this before connect_and_enable_draw_and_keyboard!");
	for (size_t p = 0; p < num_of_keys_wasd; p++) conf_keys.ensure(masterkey_config, key_wasd_keys[p], key_wasd_default[p], config::config_section_mode::SAVE); // save config
	w_key = conf_keys.get_as<int32_t>(masterkey_config, key_wasd_keys[0]); // load each
	a_key = conf_keys.get_as<int32_t>(masterkey_config, key_wasd_keys[1]); // load each
	s_key = conf_keys.get_as<int32_t>(masterkey_config, key_wasd_keys[2]); // load each
	d_key = conf_keys.get_as<int32_t>(masterkey_config, key_wasd_keys[3]); // load each
}

bool GameDraw::connect_and_enable_draw_and_keyboard()
{
	if (!connection.connect(user_name)) return false;
	kbkeys.hook_event([this](const keys::key_event& ev) { on_key_send_function(ev); });
	disp.hook_draw_function([this](const auto&) { draw_map_function(); });
	is_display_drawing_this = true;
	return true;
}

void GameDraw::disconnect()
{
	disp.unhook_draw_function();
	kbkeys.unhook_event();
	connection.disconnect();
}
