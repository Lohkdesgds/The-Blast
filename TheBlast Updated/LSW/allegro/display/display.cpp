#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Display {

			const bool _display_raw::loadModes(const int modes)
			{
				Log::gfile logg;

				logg << Log::START << "[DRAW:GR_LM][INFO] Loading screen modes..." << Log::ENDL;

				int nva = al_get_num_video_adapters();
				if (nva < 1) return false;

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
						for (auto& i : d_mods.modes) {
							if (mod.x == i.x && mod.y == i.y && mod.hz == i.hz) {
								push = false;
								break;
							}
						}
						if (push) {
							d_mods.modes.push_back(mod);
							logg << Log::START << "[DRAW:GR_LM][INFO] Added new valid resolution: " << mod.x << "x" << mod.y << "@" << mod.hz << Log::ENDL;
						}
					}
				}
				al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
				return true;
			}
			void _display_raw::_clearUp()
			{
				if (display) al_destroy_display(display);
				//if (buffer) al_destroy_bitmap(buffer);
				display = nullptr;
				//buffer = nullptr;
			}

			_display_raw::~_display_raw()
			{
				Log::gfile logg;
				logg << Log::NEEDED_START << "[DRAW:~DEST][QUIT] Display ended." << Log::NEEDED_ENDL;
				_clearUp();
			}

			const bool _display_raw::launch(const int x, const int y, const int flags)
			{
				Log::gfile logg;
				//Camera::camera_g cam;

				bool b = al_init();
				if (b) b = al_init_image_addon();
				if (b) b = al_init_primitives_addon();
				if (b) b = al_init_font_addon();
				if (b) b = al_init_ttf_addon();
				if (b) b = al_install_audio();
				if (b) b = al_init_acodec_addon();
				if (b) b = al_reserve_samples(4);
				if (b) b = al_install_keyboard();
				if (b) b = al_install_mouse();
				if (!b) {
					logg << Log::NEEDED_START << "[DRAW:LAUNC][ERRR] Failed to start graphics api" << Log::NEEDED_ENDL;
					return false;
				}

				logg << Log::START << "[DRAW:LAUNC][INFO] Launching new display..." << Log::ENDL;

				if (flags & ALLEGRO_FULLSCREEN)
				{
					isFullscreen = true;
				}

				int assistance = 0;
				lsw_mode mode_selected;
				bool failed = true;				

				if (x > 0 && y > 0)
				{
					logg << Log::START << "[DRAW:LAUNC][INFO] Trying to set resolution for display manually (call) [" << x << "x" << y << "]" << Log::ENDL;

					assistance = -1;
				    if (!loadModes(flags)) return false;
					if (d_mods.modes.size() != 0)
					{
						for (auto& i : d_mods.modes)
						{
							if (i.x == x && i.y == y && i.hz >= mode_selected.hz)
							{
								mode_selected.x = x;
								mode_selected.y = y;
								mode_selected.hz = i.hz;
								failed = false;
								logg << Log::START << "[DRAW:LAUNC][INFO] Got a valid option for resolution: " << x << "x" << y << "@" << i.hz << Log::ENDL;
							}
						}
					}
				}
				if (failed) {
					if (x > 0 && y > 0)
					{
						logg << Log::START << "[DRAW:LAUNC][WARN] Failed searching for valid value for " << x << "x" << y << ". Looking for defaults..." << Log::ENDL;
					}
					if (d_mods.modes.size() == 0)
					{
						if (!loadModes(flags)) return false;
						if (d_mods.modes.size() == 0) return false;


						for (auto& i : d_mods.modes)
						{
							if ((i.x > mode_selected.x || i.y > mode_selected.y) || (i.x == mode_selected.x && i.y == mode_selected.y && i.hz > mode_selected.hz))
							{
								mode_selected = i;
								d_mods.using_rn = assistance;
							}
							assistance++;
						}
					}
					else {
						mode_selected = d_mods.modes[d_mods.using_rn];
					}
				}
				logg << Log::NEEDED_START << "[DRAW:LAUNC][INFO] Using " << mode_selected.x << "x" << mode_selected.y << "@" << mode_selected.hz << Log::NEEDED_ENDL;

				al_set_new_display_flags(flags);
				al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
				if (mode_selected.hz > 0) al_set_new_display_refresh_rate(mode_selected.hz);
					
					
				if (!(display = al_create_display(mode_selected.x, mode_selected.y))) {
					_clearUp();

					logg << Log::ERRDV;
					logg << Log::NEEDED_START << "[DRAW:LAUNC][ERRR] Failed creating display." << Log::NEEDED_ENDL;
					logg.flush();

					return false;
				}
				al_hide_mouse_cursor(display);

				d_sprite_database spr_data; // ya, does need to draw!
				d_images_database img_data;

				img_low = Image::getOrCreate("_INTERNAL_BUFFER", true);
				img_low->set(Image::CREATE_X, -1);
				img_low->set(Image::CREATE_Y, -1);
				img_low->set(Image::FORCE_ON_MEMORY, true);
				img_low->load();

				spr_data.create(spr_low);
				spr_low->add("_INTERNAL_BUFFER");
				spr_low->setID("_INTERNAL_BUFFER");
				spr_low->set(Sprite::DRAW, false);
				spr_low->set(Sprite::USE_TINTED_DRAWING, true);
				spr_low->set(Sprite::TINT, al_map_rgba_f(Defaults::blur_buf_display, Defaults::blur_buf_display, Defaults::blur_buf_display, Defaults::blur_buf_display));
				spr_low->set(Sprite::SCALEG, 2.0);

				img_low2 = Image::getOrCreate("_INTERNAL_BUFFER2", true);
				img_low2->set(Image::CREATE_X, -1);
				img_low2->set(Image::CREATE_Y, -1);
				img_low2->set(Image::FORCE_ON_MEMORY, true);
				img_low2->load();

				spr_data.create(spr_low2);
				spr_low2->add("_INTERNAL_BUFFER2");
				spr_low2->setID("_INTERNAL_BUFFER2");
				spr_low2->set(Sprite::DRAW, false);
				//spr_low2->set(Sprite::USE_TINTED_DRAWING, true);
				//spr_low2->setTint(blur);
				spr_low2->set(Sprite::SCALEG, 2.0);

				setTarget();

				return true;
			}

			void _display_raw::setTarget()
			{
				if (!usebuf) al_set_target_backbuffer(display);
				else {
					ALLEGRO_BITMAP* b;
					img_low->get(Image::BMP, b);
					al_set_target_bitmap(b);
				}
			}

			lsw_mode _display_raw::getMode(const int u)
			{
				if ((const int)d_mods.modes.size() < u && u >= 0) {
					return d_mods.modes[u];
				}
				if (u < 0) {
					return d_mods.modes[d_mods.using_rn];
				}
				return lsw_mode();
			}

			void _display_raw::enableBlur()
			{
				usebuf = true;
			}

			void _display_raw::disableBlur()
			{
				usebuf = false;
			}

			void _display_raw::setBlur(const ALLEGRO_COLOR c)
			{
				blur = c;
				spr_low->set(Sprite::TINT, blur);
			}

			void _display_raw::flip()
			{
				/*setTarget();
				al_clear_to_color(al_map_rgb(0, 0, 0));
				al_draw_scaled_bitmap(buffer, 0, 0, al_get_bitmap_width(buffer), al_get_bitmap_height(buffer), 0, 0, al_get_display_width(display), al_get_display_height(display), 0);*/

				if (usebuf) {
					ALLEGRO_BITMAP* b, *b2;
					
					
					/*al_draw_tinted_scaled_bitmap(blur_b, blur, 0, 0, al_get_bitmap_width(blur_b), al_get_bitmap_height(blur_b), -1, -1, 1, 1, 0);
					al_set_target_bitmap(blur_b);
					al_clear_to_color(al_map_rgb(0, 0, 0));*/

					if (al_get_time() - lastrastro > Defaults::fx_blur_timing)
					{
						/*ALLEGRO_TRANSFORM g_t;
						al_identity_transform(&g_t);
						al_use_transform(&g_t);*/


						lastrastro = al_get_time();

						img_low2->get(Image::BMP, b);
						al_set_target_bitmap(b);

						//al_clear_to_color(al_map_rgba_f(Defaults::blur_buf_frame_droppin, Defaults::blur_buf_frame_droppin, Defaults::blur_buf_frame_droppin, Defaults::blur_buf_frame_droppin));

						//spr_low->forceDraw();

						img_low->get(Image::BMP, b2);
						//al_draw_bitmap(b2, 0, 0, 0);
						al_draw_tinted_bitmap(b2, blur, 0, 0, 0);

						/*Camera::camera_g cam;
						cam.apply();*/
					}



					al_set_target_backbuffer(display);

					/*spr_low->set(Sprite::POSX, 0.001);
					spr_low->set(Sprite::POSY, 0.000);
					spr_low->setTint(al_map_rgba_f(blur.r, 0, 0, blur.a));
					spr_low->forceDraw();
					
					spr_low->set(Sprite::POSX, -0.001);
					spr_low->set(Sprite::POSY, 0.001);
					spr_low->setTint(al_map_rgba_f(0, blur.g, 0, blur.a));
					spr_low->forceDraw();

					spr_low->set(Sprite::POSX, 0.000);
					spr_low->set(Sprite::POSY, -0.001);
					spr_low->setTint(al_map_rgba_f(0, 0, blur.b, blur.a));
					spr_low->forceDraw();*/

					/*spr_low->set(Sprite::POSX, 0.0);
					spr_low->set(Sprite::POSY, 0.0);
					spr_low->setTint(blur);*/

					al_clear_to_color(al_map_rgb(0, 0, 0));
					spr_low2->forceDraw();
					spr_low->forceDraw();
					
					

					img_low->get(Image::BMP, b);
					al_set_target_bitmap(b);
					al_clear_to_color(al_map_rgb(0, 0, 0));

				}


				al_flip_display();

				setTarget();
			}

			void _display_raw::toggleFullscreen()
			{
				if (al_get_time() - lastToggleFS < Defaults::display_fullscreen_toggle_min_time) return;
				lastToggleFS = al_get_time();
				isFullscreen = !isFullscreen;
				al_toggle_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, isFullscreen);
				al_acknowledge_resize(display);

				Events::big_event bev;
				bev.g()._setKey(Events::CUSTOMEVENT_DISPLAY_TOGGLED, true);
			}


			void display::custom_launch(const int x, const int y, const int f)
			{
				close();
				disp_raw = new Display::_display_raw();
				disp_raw->launch(x, y, f);

				disp_raw->setTarget(/*Display::BUFFER*/);
				al_convert_bitmaps();
			}

			void display::launch()
			{
				close();
				disp_raw = new Display::_display_raw();
				disp_raw->launch();

				disp_raw->setTarget(/*Display::BUFFER*/);
				al_convert_bitmaps();
			}
			void display::close()
			{
				if (isOpen()) {
					delete disp_raw;
					disp_raw = nullptr;
				}
			}

			void display::checkUpImages()
			{
				al_convert_bitmaps();
			}

			void display::setCustomRaw(_display_raw* newdp)
			{
				if (!newdp) return;

				if (isOpen()) close();

				disp_raw = newdp;
			}

			const bool display::isOpen()
			{
				return (this->disp_raw);
			}

			const bool display::flip(const bool l, const ALLEGRO_COLOR c)
			{
				if (!isOpen()) return false;
				if (keep) {
					if (!(*keep)) {
						close();
						return false;
					}
				}

				double delta_t = al_get_time() - time_frame;


				if (cap_fps_to > 0) {
					if (l) {
						for (; (delta_t < 1.0 / cap_fps_to) && (cap_fps_to > 0); delta_t = al_get_time() - time_frame) Sleep(0.2 / cap_fps_to);
					}
					else{
						if (delta_t < 1.0 / cap_fps_to)
						{
							al_clear_to_color(c);
							return true;
						}
					}
				}

				time_frame = al_get_time();

				fps = (3.0*fps + (1.0 / delta_t))*0.25;


				//disp_raw->setTarget(Display::DISPLAY);

				disp_raw->setTarget();

				disp_raw->flip();

				//disp_raw->setTarget(Display::BUFFER);

				al_clear_to_color(c);

				return true;
			}

			void display::setKeep(bool * v)
			{
				keep = v;
			}

			float display::getFPS()
			{
				return fps;
			}

			void display::capFPS(const int i)
			{
				cap_fps_to = i;
			}

			void display::toggleFS()
			{
				if (disp_raw) disp_raw->toggleFullscreen();
			}

			_display_raw & display::_get()
			{
				assert(disp_raw);
				return *disp_raw;
			}
					   
		}
	}
}