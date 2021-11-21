#include "gamedraw.h"

UserInMap::UserInMap()
{
	reset();
}

void UserInMap::set_name(const std::string& str)
{
	//txt.set<safe_data<std::string>>(enum_text_safe_string_e::STRING, str);
}

void UserInMap::set_color(const color& c)
{
	blk.set<color>(enum_sprite_color_e::DRAW_DRAW_BOX, c);
}

void UserInMap::reset()
{
	blk.set<bool>(enum_sprite_boolean_e::DRAW_DRAW_BOX, true);
	blk.set<float>(enum_sprite_float_e::SCALE_X, 2.0f / world_map_width);
	blk.set<float>(enum_sprite_float_e::SCALE_Y, 2.0f / world_map_height);
	set_color(color(127, 127, 127));
	user_id = 0;
	set_name("");
}


void GameDraw::draw_map_function()
{
	if (textures.size() == 0) {
		cout << console::color::RED << "TEXTURES MAP IS EMPTY!";
		return;
	}

	color(16,16,16).clear_to_this();

	//const float map_x = disp.get_width() * 1.0f / connection.get_width();
	//const float map_y = disp.get_height() * 1.0f / connection.get_height();

	// for now let's just draw

	transform easy;
	easy.build_classic_fixed_proportion_stretched_auto(1.0f);
	easy.apply();

	connection.iterate_map_auto(
		[this](const uint32_t xx, const uint32_t yy, const int32_t blkid) 
		{
			const auto ref = std::find_if(textures.begin(), textures.end(), [&](TextureMap& tm) {return tm.block_id == blkid; });
			if (ref == textures.end()) {
				cout << console::color::YELLOW << "UNKNOWN TEXTURE ID " << blkid;
			}
			else {
				ref->bmp->draw_scaled_at(((xx * 2.0f / world_map_width) - 1.0f), ((yy * 2.0f / world_map_height) - 1.0f), (2.0f / world_map_width), (2.0f / world_map_height));
			}
		}
	);

	std::vector<int32_t> usrs;
	connection.iterate_users_auto(
		[this, &usrs](const WorldUser& usr)
		{
			usrs.push_back(usr.user_id);
		}
	);

	for (auto& it : users_transl) {
		if (std::find_if(usrs.begin(), usrs.end(), [&](int32_t& e) { return e == it.user_id; }) == usrs.end())
		{
			it.reset(); // reset config of this thing
		}
	}

	for (const auto& it : usrs) {
		if (auto fnd = std::find_if(std::begin(users_transl), std::end(users_transl), [&](UserInMap& e) { return e.user_id == it; }); fnd == std::end(users_transl))
		{
			for (auto& e : users_transl) {
				if (e.user_id == 0) {
					e.user_id = it;
					e.set_color(e.user_id == connection.get_my_id() ? color(235, 235, 235) : color(127, 127, 127));
					e.blk.set<float>(enum_sprite_float_e::RO_DRAW_PROJ_POS_X, 0.0f); // from center
					e.blk.set<float>(enum_sprite_float_e::RO_DRAW_PROJ_POS_Y, 0.0f); // from center
					break;
				}
			}
		}
	}

	connection.iterate_users_auto(
		[this](const WorldUser& usr)
		{
			if (auto fnd = std::find_if(std::begin(users_transl), std::end(users_transl), [&](UserInMap& e) { return e.user_id == usr.user_id; }); fnd != std::end(users_transl))
			{
				cout << usr.user_id << " [" << usr.posx << ";" << usr.posy << "]";

				fnd->blk.set<float>(enum_sprite_float_e::POS_X, (((usr.posx * 2.0f / world_map_width) - 1.0f) + 0.5f * fnd->blk.get<float>(enum_sprite_float_e::SCALE_X)));
				fnd->blk.set<float>(enum_sprite_float_e::POS_Y, (((usr.posy * 2.0f / world_map_height) - 1.0f) + 0.5f * fnd->blk.get<float>(enum_sprite_float_e::SCALE_Y)));
				//fnd->blk.think();
				fnd->blk.draw();
			}
			//al_draw_filled_rectangle(usr.posx * 1.0f * map_x, usr.posy * 1.0f * map_y, (usr.posx + 1) * 1.0f * map_x, (usr.posy + 1) * 1.0f * map_y, usr.user_id == connection.get_my_id() ? color(235, 235, 235) : color(127, 127, 127));
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

	if (leftright != 0 || updown != 0) connection.make_move_player(leftright, updown);
}

GameDraw::GameDraw(display& d, config& c)
	: disp(d), conf_keys(c)
{
}

void GameDraw::add_texture(hybrid_memory<texture> textur, uint32_t id)
{
	if (is_display_drawing_this) throw std::runtime_error("You should have set this before connect_and_enable_keyboard!");

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
	if (is_display_drawing_this) throw std::runtime_error("You should have set this before connect_and_enable_keyboard!");
	user_name = nam.substr(0, username_length_max - 1);
}

void GameDraw::check_config()
{
	if (is_display_drawing_this) throw std::runtime_error("You should have set this before connect_and_enable_keyboard!");
	for (size_t p = 0; p < num_of_keys_wasd; p++) conf_keys.ensure(masterkey_config, key_wasd_keys[p], key_wasd_default[p], config::config_section_mode::SAVE); // save config
	w_key = conf_keys.get_as<int32_t>(masterkey_config, key_wasd_keys[0]); // load each
	a_key = conf_keys.get_as<int32_t>(masterkey_config, key_wasd_keys[1]); // load each
	s_key = conf_keys.get_as<int32_t>(masterkey_config, key_wasd_keys[2]); // load each
	d_key = conf_keys.get_as<int32_t>(masterkey_config, key_wasd_keys[3]); // load each
}

bool GameDraw::connect_and_enable_keyboard()
{
	if (!connection.connect(user_name)) return false;
	kbkeys.hook_event([this](const keys::key_event& ev) { on_key_send_function(ev); });
	is_display_drawing_this = true;
	return true;
}

void GameDraw::draw_this()
{
	draw_map_function();
}

void GameDraw::disconnect()
{
	kbkeys.unhook_event();
	connection.disconnect();
}
