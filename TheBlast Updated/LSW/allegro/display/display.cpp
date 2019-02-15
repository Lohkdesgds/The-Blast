#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Display {

			_big_display_data big_display::data;
			long long big_display::instance_count = 0;


			void big_display::_realDraw(const ALLEGRO_COLOR c)
			{
				if (data.color_shift) {

					if (!data.spr[0]) {

						for (size_t p = 0; p < 3; p++) {
							data.spr[p] = Sprite::getOrCreate("__INTERNAL_FX_" + std::to_string(p), true);
							data.img_lw[p] = Image::getOrCreate("__INTERNAL_FX_" + std::to_string(p), true);
							data.img_lw[p]->set(Image::CREATE_X, -1);
							data.img_lw[p]->set(Image::CREATE_Y, -1);
							data.img_lw[p]->load();

							data.spr[p]->add("__INTERNAL_FX_" + std::to_string(p));

							data.spr[p]->set(Sprite::SCALEG, 2.007);
							data.spr[p]->set(Sprite::USE_TINTED_DRAWING, true);
							data.spr[p]->set(Sprite::DRAW, false); // does not work if true (not as expected)
							data.spr[p]->set(Sprite::LAYER, Defaults::Display::layer_fx);
							data.spr[p]->set(Sprite::AFFECTED_BY_CAM, false);
						}

						data.spr[0]->set(Sprite::TINT, al_map_rgba_f(0.45, 0.0, 0.0, 0.45));
						data.spr[1]->set(Sprite::TINT, al_map_rgba_f(0.0, 0.45, 0.0, 0.45));
						data.spr[2]->set(Sprite::TINT, al_map_rgba_f(0.0, 0.0, 0.45, 0.45));
					}

					if (al_get_time() - data.last_color_shift_FX > Defaults::Display::fx_timer) { // update RGB layer
						data.last_color_shift_FX = al_get_time();

						for (auto& i : data.img_lw) i->verify();

						ALLEGRO_BITMAP* orig = al_get_backbuffer(data.md);
						ALLEGRO_BITMAP* tbmp = nullptr;

						auto il = Image::getOrCreate("__INTERNAL_FX_0");
						if (il) {
							il->get(Image::BMP, tbmp);
							if (tbmp) {
								al_set_target_bitmap(tbmp);
								//al_draw_scaled_bitmap(orig, 0, 0, al_get_bitmap_width(orig), al_get_bitmap_height(orig), 0, 0, al_get_bitmap_width(tbmp), al_get_bitmap_height(tbmp), 0);
								al_draw_bitmap(orig, 0, 0, 0);
							}
						}

						il = Image::getOrCreate("__INTERNAL_FX_1");
						if (il) {
							il->get(Image::BMP, tbmp);
							if (tbmp) {
								al_set_target_bitmap(tbmp);
								//al_draw_scaled_bitmap(orig, 0, 0, al_get_bitmap_width(orig), al_get_bitmap_height(orig), 0, 0, al_get_bitmap_width(tbmp), al_get_bitmap_height(tbmp), 0);
								al_draw_bitmap(orig, 0, 0, 0);
							}
						}
						

						il = Image::getOrCreate("__INTERNAL_FX_2");
						if (il) {
							il->get(Image::BMP, tbmp);
							if (tbmp) {
								al_set_target_bitmap(tbmp);
								//al_draw_scaled_bitmap(orig, 0, 0, al_get_bitmap_width(orig), al_get_bitmap_height(orig), 0, 0, al_get_bitmap_width(tbmp), al_get_bitmap_height(tbmp), 0);
								al_draw_bitmap(orig, 0, 0, 0);
							}
						}
						

						al_set_target_backbuffer(data.md);
					}

					Events::big_event bev;
					double v = bev.g().getFunctionValNow(2), v2 = bev.g().getFunctionValNow(3);

					data.spr[0]->set(Sprite::POSX, v + v2);
					data.spr[0]->set(Sprite::POSY, -v + v2);

					data.spr[1]->set(Sprite::POSX, v - v2);
					data.spr[1]->set(Sprite::POSY, v + v2);

					data.spr[2]->set(Sprite::POSX, -v + v2);
					data.spr[2]->set(Sprite::POSY, -v - v2);

					// apply fx
					Camera::camera_g cam;
					double x, y;
					x = cam.get(cam.getLastApplyID(), Camera::OFFX);
					y = cam.get(cam.getLastApplyID(), Camera::OFFY);

					al_draw_filled_rectangle(-1 + x, -1 + y, al_get_display_width(data.md) * (1.0 + x), al_get_display_height(data.md) * (1.0 + y), al_map_rgba_f(0.0, 0.0, 0.0, 0.5));
					{
						al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
						for (size_t p = 0; p < 3; p++) {
							data.spr[p]->forceDraw();
						}
						al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
					}
				}

				// draw
				al_flip_display();

				// clear
				al_clear_to_color(c);

				// flush
				if (al_get_time() - data.last_flush > Defaults::Display::flush_time) {
					data.last_flush = al_get_time();
					//__flushCache();
				}
			}
			/*void big_display::__flushCache()
			{
				d_entity_database ent_data;
				d_images_database img_data;
				d_musics_database msk_data;
				d_sprite_database spr_data;
				d_texts_database txt_data;
				Log::gfile logg;

				size_t sprs = spr_data.work().flush();
				size_t imgs = img_data.work().flush();
				size_t txts = txt_data.work().flush();
				size_t msks = msk_data.work().flush();
				size_t ents = ent_data.work().flush();

				// NEEDED FOR NOW
				if (sprs > 0) logg << Log::NEEDED_START << Log::_func("main", "__flushCache") << sprs << " sprites have been cleaned." << Log::NEEDED_ENDL;
				if (imgs > 0) logg << Log::NEEDED_START << Log::_func("main", "__flushCache") << imgs << " images have been cleaned." << Log::NEEDED_ENDL;
				if (txts > 0) logg << Log::NEEDED_START << Log::_func("main", "__flushCache") << txts << " texts have been cleaned." << Log::NEEDED_ENDL;
				if (msks > 0) logg << Log::NEEDED_START << Log::_func("main", "__flushCache") << msks << " tracks have been cleaned." << Log::NEEDED_ENDL;
				if (ents > 0) logg << Log::NEEDED_START << Log::_func("main", "__flushCache") << ents << " entities have been cleaned." << Log::NEEDED_ENDL;
			}*/

			big_display::big_display()
			{
				if (instance_count++ == 0)
				{
					if (!al_init()) throw "at big_display::big_display [#" + std::to_string((size_t)this) + "]: Failed initializing Allegro!";
					if (!al_init_image_addon()) throw "at big_display::big_display [#" + std::to_string((size_t)this) + "]: Failed initializing Allegro (image)!";
					if (!al_init_primitives_addon()) throw "at big_display::big_display [#" + std::to_string((size_t)this) + "]: Failed initializing Allegro (primitives)!";

					logg << Log::NEEDED_START << Log::_func("big_display", "~big_display") << "Display is ready to be created!" << Log::NEEDED_ENDL;
				}
			}

			big_display::~big_display()
			{
				if (--instance_count == 0)
				{
					if (data.md) {
						al_destroy_display(data.md);
						data.md = nullptr;

						logg << Log::NEEDED_START << Log::_func("big_display", "~big_display", Log::WARN) << "Display has been disabled!" << Log::NEEDED_ENDL;

						//__flushCache();
					}
					else {
						logg << Log::NEEDED_START << Log::_func("big_display", "~big_display", Log::WARN) << "DISPLAY WAS NOT FOUND BY LAST BIG_DISPLAY INSTANCE! This can be an error, or not. Hmm." << Log::NEEDED_ENDL;
					}
				}
			}

			void big_display::_loadModes(const int modes)
			{
				int nva = al_get_num_video_adapters();

				if (nva < 1) throw "at big_display::loadModes [#" + std::to_string((size_t)this) + "]: Cannot get information about your display!";

				al_set_new_display_flags(modes);
				int num_modes = al_get_num_display_modes();

				for (int j = 0; j < num_modes; ++j) {
					ALLEGRO_DISPLAY_MODE admode;

					if (al_get_display_mode(j, &admode) == &admode) {

						lsw_mode mod;
						mod.x = admode.width;
						mod.y = admode.height;
						mod.hz = admode.refresh_rate;
						bool push = true;
						for (auto& i : data.d_mods.modes) {
							if (mod.x == i.x && mod.y == i.y && mod.hz == i.hz) {
								push = false;
								break;
							}
						}
						if (push) {
							data.d_mods.modes.push_back(mod);
							logg << Log::START << Log::_func("big_display", "loadModes") << "Added new valid resolution: " << mod.x << "x" << mod.y << "@" << mod.hz << Log::ENDL;
						}
					}
				}
			}

			void big_display::set(const vals_bp e, bool* v)
			{
				switch (e) {
				case RUNNING_P:
					if (!v) throw "at big_display::set [#" + std::to_string((size_t)this) + "]: Not expected null pointer to set data.ext_isRunning.";
					data.ext_isRunning = v;
					break;
				}
			}

			void big_display::set(const vals_b e, const bool v)
			{
				switch (e) {
				case COLOR_SHIFTED:
					data.color_shift = v;
					break;
				}
			}

			void big_display::set(const vals_dp e, ALLEGRO_DISPLAY* v)
			{
				switch (e) {
				case DISPLAY:
					if (!Defaults::enable_all_possible_setget) throw "at big_display::set [#" + std::to_string((size_t)this) + "]: Tried to set a value \"read-only\" like (DISPLAY).";
					data.md = v;
					break;
				}
			}

			void big_display::set(const vals_d e, const double v)
			{
				switch (e) {
				case FPS:
					if (!Defaults::enable_all_possible_setget) throw "at big_display::set [#" + std::to_string((size_t)this) + "]: Tried to set a value \"read-only\" like (FPS).";
					data.last_fps = v;
					break;
				case LOCK_FPS_TO:
					data.lock_fps_to = v;
				}
			}

			void big_display::set(const vals_i e, const int v)
			{
				switch (e) {
				case SCREEN_X:
					throw "at big_display::set [#" + std::to_string((size_t)this) + "]: There's no way to set SCREEN_X!.";
					break;
				case SCREEN_Y:
					throw "at big_display::set [#" + std::to_string((size_t)this) + "]: There's no way to set SCREEN_Y!.";
					break;
				case REFRESHRATE:
					throw "at big_display::set [#" + std::to_string((size_t)this) + "]: There's no way to set REFRESHRATE!.";
					break;
				}
			}

			void big_display::get(const vals_bp e, bool*& v)
			{
				switch (e) {
				case RUNNING_P:
					*v = data.ext_isRunning;
					break;
				}
			}

			void big_display::get(const vals_b e, bool& v)
			{
				switch (e) {
				case COLOR_SHIFTED:
					v = data.color_shift;
					break;
				}
			}

			void big_display::get(const vals_dp e, ALLEGRO_DISPLAY*& v)
			{
				switch (e) {
				case DISPLAY:
					v = data.md;
					break;
				}
			}

			void big_display::get(const vals_d e, double& v)
			{
				switch (e) {
				case FPS:
					v = data.last_fps;
					break;
				case LOCK_FPS_TO:
					v = data.lock_fps_to;
					break;
				}
			}

			void big_display::get(const vals_i e, int& v)
			{
				switch (e) {
				case SCREEN_X:
					if (!data.md) throw ""; //
					v = al_get_display_width(data.md);
					break;
				case SCREEN_Y:
					if (!data.md) throw ""; //
					v = al_get_display_height(data.md);
					break;
				case REFRESHRATE:
					if (!data.md) throw ""; //
					v = al_get_display_refresh_rate(data.md);
					break;
				}
			}

			const bool big_display::isEq(const vals_bp e, const bool v)
			{
				bool g;
				bool* pg = &g;
				get(e, pg);
				return (g == v);
			}

			const bool big_display::isEq(const vals_bp e, bool* v)
			{
				bool* g;
				get(e, g);
				return (*g == *v);
			}
			const bool big_display::isEq(const vals_b e, const bool v)
			{
				bool g;
				get(e, g);
				return (g == v);
			}
			const bool big_display::isEq(const vals_dp e, ALLEGRO_DISPLAY* v)
			{
				ALLEGRO_DISPLAY* g;
				get(e, g);
				return (g == v);
			}
			const bool big_display::isEq(const vals_d e, const double v)
			{
				double g;
				get(e, g);
				return (g == v);
			}

			const bool big_display::isEq(const vals_i e, const int v)
			{
				int g;
				get(e, g);
				return (g == v);
			}

			void big_display::launch(const int x, const int y, int flags)
			{
				data.last_wasFullscreen = ((flags & ALLEGRO_FULLSCREEN) || (flags & ALLEGRO_FULLSCREEN_WINDOW));

				lsw_mode mode_selected;
				bool okay = true;
				bool search = (x > 0 && y > 0);

				// pre-call (setting up options for "flags")				
				{
					Safer::safe_string err;
					if (!Tools::try_call(&err, __ext_loadModes, this, flags)) {
						logg << Log::NEEDED_START << Log::_func("big_display", "launch", Log::WARN) << "Failed on loadModes call: " << err << Log::NEEDED_ENDL;
						okay = false;
					}
					okay = (okay ? (data.d_mods.modes.size() != 0) : okay);
				}
				// endof pre-call

				// searching or looking for biggest hz available for this configuration
				if (search) {
					if (okay) { // there are resolutions available on the vector, search for highest hz
						for (auto& i : data.d_mods.modes)
						{
							if (i.hz >= mode_selected.hz)
							{
								mode_selected.x = x;
								mode_selected.y = y;
								mode_selected.hz = i.hz;
							}
						}
					}
					else { // no information. Gonna go default settings
						mode_selected.hz = -1;
						if (!data.last_wasFullscreen) {
							data.last_wasFullscreen = true;
							flags |= ALLEGRO_FULLSCREEN_WINDOW;
						}
						mode_selected.x = mode_selected.y = 0;
					}
				}
				else {
					if (okay) { // there are resolutions available on the vector, search for highest hz and resolution
						for (auto& i : data.d_mods.modes)
						{
							if (i.x == x && i.y == y && i.hz >= mode_selected.hz)
							{
								mode_selected.x = x;
								mode_selected.y = y;
								mode_selected.hz = i.hz;
							}
						}
					}
					else { // no information. Gonna go default settings
						mode_selected.hz = -1;
						if (!data.last_wasFullscreen) {
							data.last_wasFullscreen = true;
							flags |= ALLEGRO_FULLSCREEN_WINDOW;
						}
						mode_selected.x = mode_selected.y = 0;
					}
				}

				logg << Log::NEEDED_START << Log::_func("big_display", "launch") << "Got a valid option for windowed option: "
					<< ((mode_selected.x == 0) ? "AUTO" : std::to_string(mode_selected.x)) << "x" << ((mode_selected.y == 0) ? "AUTO" : std::to_string(mode_selected.y)) << "@" << ((mode_selected.hz <= 0) ? "UNKNOWN" : std::to_string(mode_selected.hz)) << Log::NEEDED_ENDL;

				al_set_new_display_flags(flags);
				al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
				if (mode_selected.hz > 0) al_set_new_display_refresh_rate(mode_selected.hz);



				if (!(data.md = al_create_display(mode_selected.x, mode_selected.y))) {
					logg << Log::NEEDED_START << Log::_func("big_display", "launch", Log::WARN) << "Failed on creating display!" << Log::NEEDED_ENDL;
					throw "at big_display::launch [#" + std::to_string((size_t)this) + "]: Failed creating display.";
				}

				al_hide_mouse_cursor(data.md);
				al_set_new_bitmap_flags(ALLEGRO_MAG_LINEAR | ALLEGRO_MIN_LINEAR);
			}

			const bool big_display::work(const bool wait_for_fps, const ALLEGRO_COLOR c)
			{
				Events::big_event bev;
				Config::config conf;
				double temp = al_get_time() - data.last_fps_set;

				if (data.ext_isRunning)
				{
					if (!*data.ext_isRunning) return false; // gonna automatically die when return from main is called. No need to die here.
				}

				data.frames_count++;

				if (!wait_for_fps && (temp < 1.0 / data.lock_fps_to)) return true;


				// wait
				for (; (temp < 1.0 / data.lock_fps_to) && (data.lock_fps_to > 0); temp = al_get_time() - data.last_fps_set) Sleep(0.2 / data.last_fps_set);

				// easier read
				_realDraw(c);


				// TOOLS
				if (bev.g().getKey(Events::CUSTOMEVENT_DISPLAY_RESIZED)) {
					al_acknowledge_resize(data.md);

					conf.set(Config::SCREEN_X, al_get_display_width(data.md));
					conf.set(Config::SCREEN_Y, al_get_display_height(data.md));
				}
				if (bev.g().getKey(Events::CUSTOMEVENT_DISPLAY_TOGGLED)) {
					if (al_get_time() - data.last_toggleFS > Defaults::Display::fullscreen_toggle_time)
					{
						data.last_toggleFS = al_get_time();
						data.last_wasFullscreen = !data.last_wasFullscreen;

						conf.set(Config::WAS_FULLSCREEN, data.last_wasFullscreen);

						al_toggle_display_flag(data.md, ALLEGRO_FULLSCREEN_WINDOW, data.last_wasFullscreen);
						al_acknowledge_resize(data.md);

						conf.set(Config::SCREEN_X, al_get_display_width(data.md));
						conf.set(Config::SCREEN_Y, al_get_display_height(data.md));
					}
				}

				if ((temp) > 1.0) {
					data.last_fps_set = al_get_time();
					data.last_fps = 1.0 * data.frames_count / temp;
					data.frames_count = 0;
				}

				return true;
			}




			void __ext_loadModes(big_display* bd, int flags)
			{
				if (!bd) throw "null pointer";
				bd->_loadModes(flags);
			}

		}
	}
}