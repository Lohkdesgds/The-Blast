#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Main {

			_main_data* main::data = nullptr;

			main::main()
			{
				prepare_commom_templates;
				data = new _main_data();
				srand(time(0));
			}
			void main::unloadAll()
			{
				Events::big_event bev;

				d_sprite_database spr_data;
				d_images_database img_data;
				d_texts_database  txt_data;
				d_musics_database msk_data;
				d_entity_database ent_data;

				bev.g().delAllFunctions();
				bev.g()._setShutDownNow();

				spr_data.clear();
				img_data.clear();
				txt_data.clear();
				msk_data.clear();
				ent_data.clear();

				while (!bev.g().isSafeToQuit());

				if (data) {
					delete data;
					data = nullptr;
				}
			}
			const bool main::check(const Safer::safe_string s)
			{
				if (!data) {
					throw "MAIN::" + s.g() + " - EXPECTED TO BE INITIALIZED! NULL DATA DETECTED, ABORT!";
					return false;
				}
				return true;
			}
			initialization main::__getConfigCopy()
			{
				if (!data) return initialization();
				return data->setup;
			}
			void main::__saveConfig(const initialization init)
			{
				if (!check("__safeConfig")) return;

				data->setup = init;
			}
			void main::__set_camera_quick(const __fast_cam_preset c)
			{
				if (!check("__set_camera_quick")) return;

				data->cam.copy(c.config_number, c.cam_settings);
			}
			void main::__set_layer_quick(const __fast_lyr_preset c)
			{
				if (!check("__set_layer_quick")) return;

				data->lyr.getOne(c.config_number).setMode(c.layer_mode);
				data->lyr.getOne(c.config_number).work() = c.each_layer_settings;
			}
			void main::__apply_cam_number(const int n)
			{
				if (!check("__apply_cam_number")) return;

				data->cam.apply(n);
			}
			void main::__apply_layer_number(const int n)
			{
				if (!check("__apply_layer_number")) return;

				data->lyr.setUsingNow(n);
			}

			Display::display* main::__getDisplay()
			{
				if (!check("__getDisplay")) return nullptr;

				if (!data->hasInit) {
					throw "MAIN::__GETDISPLAY() EXPECTED TO BE INITIALIZED! BOOL WAS FALSE!";
				}
				return &data->disp;
			}

			const bool main::_rgb_clear_screen_load()
			{
				double ti = al_get_time();

				data->txt_data.draw(); // update and draw texts

				if (!data->disp.flip(true, al_map_rgb_f(0.5*cos(ti*2.0)+0.1, 0.5*sin(ti*2.5 + 0.3) + 0.1, 0.5*cos(ti*1.7 + 0.5) + 0.1))) return false; // flip

				__internal_check_hotkeys();

				return true;
			}

			const levels main::_rgb_pause_screen()
			{
				// CAMERA AND LAYER SET
				__apply_layer_number(0);
				__apply_cam_number(1);

				// VARIABLES
				levels return_val;
				Fx::lines lines(-20);
				Sprite::sprite* mouse;
				Sprite::sprite* b00;
				Sprite::sprite* b01;
				Sprite::sprite* b02;
				Text::text* x00;
				Text::text* x01;

				// SEARCHES
				mouse = Sprite::getOrCreate("MOUSE");
				b00 = Sprite::getOrCreate("BAR_00");
				b01 = Sprite::getOrCreate("BAR_01");
				b02 = Sprite::getOrCreate("BAR_02");
				x00 = Text::getOrCreate("BAR_00_TEXT");
				x01 = Text::getOrCreate("BAR_01_TEXT");

				// SETTING VARIABLES VALUES
				x00->set(Text::SETSTRING, "< CONTINUE >");
				x01->set(Text::SETSTRING, "< MENU >");
				b02->set(Sprite::DRAW, false);
				return_val = PLAYING;

				// THE WHILE
				for (bool keep = true; keep;)
				{
					// INPUT
					bool b0, b1;
					b00->get(Sprite::_IS_COLLIDING, b0);
					b01->get(Sprite::_IS_COLLIDING, b1);

					if (b0) b00->set(Sprite::ANIMATIONTIME, -1);
					else b00->set(Sprite::ANIMATIONTIME, 0);
					if (b1) b01->set(Sprite::ANIMATIONTIME, -1);
					else b01->set(Sprite::ANIMATIONTIME, 0);

					if (data->bev.g().getKey(Events::MOUSE_0)) {
						if (b0) { // test 1
							return_val = data->playing;
							keep = false;
						}
						if (b1) { // test 2
							return_val = MENU;
							keep = false;
						}
					}
					if (data->bev.g().getKey(Events::KEY_ESCAPE))
					{
						return_val = data->playing;
						keep = false;
					}

					// FX
					mouse->set(Sprite::ROTATION, data->bev.g().getFunctionValNow(0));
					mouse->set(Sprite::SCALEG, 0.07 * data->bev.g().getFunctionValNow(1));
					lines.draw();

					// FLIP AND STUFF
					if (!__internal_task_level_common()) return MENU;
				}
				return return_val;
			}

			void main::init()
			{
				Log::gfile logg;

				try {
					if (!check("init")) return;

					{
						logg.showOnConsole(data->setup.enable_console);
						logg.longLog(data->setup.full_log);
					}

					// create display
					if (data->setup.res[0] > 0 && data->setup.res[1] > 0) {
						data->disp.custom_launch(data->setup.res[0], data->setup.res[1], data->setup.mode);
					}
					else data->disp.launch();

					data->cam.setup(); // guarantee global camera instance
					data->bev.initInstance(); // guarantee global event instance

					// set events timing
					data->bev.g().setMultiplierForUpdatingImg(data->disp._get().getMode(-1).hz);

					// setup and start events thread
					data->bev.initSetupAndRun();

					// define events bool reference on display so then events can tell display to die when ended.
					data->disp.setKeep(data->bev.g().getKeep());

					// force display target just to be sure.
					data->disp._get().setTarget();

					// each config has its brackets for local usage. This bigger one is just to better reading (can minimize it)
					// CAMERA
					{
						{   /* -------------------- PRESET #-2 -------------------- */
							Main::__fast_cam_preset cam_p;
							cam_p.config_number = -2;
							cam_p.cam_settings.layers_enabled.push_back(-2);
							cam_p.cam_settings.offset_y = 2.7;
							cam_p.cam_settings.scale_g = 1.2;
							cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer

							__set_camera_quick(cam_p);
						}
						{   /* -------------------- PRESET #-1 -------------------- */
							Main::__fast_cam_preset cam_p;
							cam_p.config_number = -1;
							cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);
							cam_p.cam_settings.layers_enabled.push_back(-1);
							cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer
							cam_p.cam_settings.scale_g = 1.15;

							__set_camera_quick(cam_p);
						}
						{   /* -------------------- PRESET #0 -------------------- */
							Main::__fast_cam_preset cam_p;
							cam_p.config_number = 0;
							cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);
							cam_p.cam_settings.layers_enabled.push_back(-20);
							cam_p.cam_settings.layers_enabled.push_back(0);
							cam_p.cam_settings.layers_enabled.push_back(1);
							cam_p.cam_settings.layers_enabled.push_back(99);
							cam_p.cam_settings.layers_enabled.push_back(100);
							cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer

							__set_camera_quick(cam_p);
						}
						{   /* -------------------- PRESET #1 -------------------- */ // PAUSE
							Main::__fast_cam_preset cam_p;
							cam_p.config_number = 1;
							cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);
							cam_p.cam_settings.layers_enabled.push_back(-20);
							cam_p.cam_settings.layers_enabled.push_back(0);
							cam_p.cam_settings.layers_enabled.push_back(Defaults::default_pauseonly_layer);
							cam_p.cam_settings.layers_enabled.push_back(99);
							cam_p.cam_settings.layers_enabled.push_back(100);
							cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer

							__set_camera_quick(cam_p);
						}
						{   /* -------------------- PRESET #2 -------------------- */ // GAME
							Main::__fast_cam_preset cam_p;
							cam_p.config_number = 2;
							cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);
							cam_p.cam_settings.layers_enabled.push_back(Defaults::map_default_layer);
							cam_p.cam_settings.layers_enabled.push_back(Defaults::user_default_layer);
							cam_p.cam_settings.layers_enabled.push_back(Defaults::badboys_default_layer);
							cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer

							__set_camera_quick(cam_p);
						}
						{   /* -------------------- PRESET #3 -------------------- */ // SETTINGS
							Main::__fast_cam_preset cam_p;
							cam_p.config_number = 3;
							cam_p.cam_settings.layers_enabled.push_back(Defaults::default_settings_layer);
							cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);
							cam_p.cam_settings.layers_enabled.push_back(100);
							cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer

							__set_camera_quick(cam_p);
						}
						{   /* -------------------- PRESET #4 -------------------- */ // DEAD
							Main::__fast_cam_preset cam_p;
							cam_p.config_number = 4;
							cam_p.cam_settings.layers_enabled.push_back(-81);
							cam_p.cam_settings.layers_enabled.push_back(100);
							cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer

							__set_camera_quick(cam_p);
						}
					}
					// LAYERS
					{
						{   /* -------------------- PRESET #0 -------------------- */
							Main::__fast_lyr_preset lyr_p;
							lyr_p.config_number = 0;
							lyr_p.layer_mode = Layer::STANDARD;
							lyr_p.each_layer_settings[0].collides_with.push_back(100);

							__set_layer_quick(lyr_p);
						}
						{   /* -------------------- PRESET #2 -------------------- */
							Main::__fast_lyr_preset lyr_p;
							lyr_p.config_number = 2;
							lyr_p.layer_mode = Layer::USEMAP;

							__set_layer_quick(lyr_p);
						}
						{   /* -------------------- PRESET #3 -------------------- */
							Main::__fast_lyr_preset lyr_p;
							lyr_p.config_number = 3;
							lyr_p.layer_mode = Layer::STANDARD;
							lyr_p.each_layer_settings[Defaults::default_settings_layer].collides_with.push_back(100);

							__set_layer_quick(lyr_p);
						}
						{   /* -------------------- PRESET #3 -------------------- */
							Main::__fast_lyr_preset lyr_p;
							lyr_p.config_number = 4;
							lyr_p.layer_mode = Layer::STANDARD;
							lyr_p.each_layer_settings[-81].collides_with.push_back(100);

							__set_layer_quick(lyr_p);
						}
					}
				}
				catch (const Safer::safe_string& s)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:INIT_][ERROR] Caught throw at main::init: " << s << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw s;
				}
				catch (const std::string& s)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:INIT_][ERROR] Caught throw at main::init: " << s << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw s;
				}
				catch (const char* s)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:INIT_][ERROR] Caught throw at main::init: " << s << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw s;
				}
				catch (const int i)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:INIT_][ERROR] Caught throw at main::init: Error code #" << i << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw i;
				}
				catch (...)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:INIT_][ERROR] Caught throw at main::init: Unknown error." << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw 1;
				}

				data->hasInit = true;
			}

			void main::load()
			{
				Log::gfile logg;

				try {
					if (!check("load")) return;

					Text::text* txt = nullptr;

					// EARLY ACCESS
					txt = Text::getOrCreate("EARLY_ACCESS_00", true);
					// PRE-LOAD (shared)
					txt->setMainDisplay(__getDisplay());
					txt->set(Text::SETGLOBALPATH, Defaults::font_altern_name_full);
					txt->set(Text::IS_GLOBALPATH_RAW, true);
					// ENDOF PRE-LOAD (shared)
					txt->set(Text::SETSTRING, ("Early Access ") + Defaults::version_app.g());
					txt->set(Text::POSX, 1.0);
					txt->set(Text::POSY, -1.0);
					txt->set(Text::MODE, Text::ALIGN_RIGHT);
					txt->set(Text::AFFECTED_BY_CAM, false);
					txt->set(Text::COLOR, al_map_rgba_f(0.2, 0.2, 0.2, 0.2));
					txt->set(Text::SCALEG, 1.05);
					txt->set(Text::UPDATETIME, 0.05);
					txt->set(Text::LAYER, -9999);

					// EARLY ACCESS
					txt = Text::getOrCreate("EARLY_ACCESS_01", true);
					txt->set(Text::SETSTRING, "Lohk's Studios Works 2019");
					txt->set(Text::POSX, 1.0);
					txt->set(Text::POSY, -0.95);
					txt->set(Text::MODE, Text::ALIGN_RIGHT);
					txt->set(Text::AFFECTED_BY_CAM, false);
					txt->set(Text::COLOR, al_map_rgba_f(0.15, 0.15, 0.15, 0.15));
					txt->set(Text::SCALEG, 0.8);
					txt->set(Text::UPDATETIME, 0.05);
					txt->set(Text::LAYER, -9999);

					// LOADING SCREEN
					txt = Text::getOrCreate("LOADING_0", true);
					txt->set(Text::SETSTRING, "Loading...");
					txt->set(Text::POSX, 0.0);
					txt->set(Text::POSY, 3.20);
					txt->set(Text::MODE, Text::ALIGN_CENTER);
					txt->set(Text::UPDATETIME, 0.05);
					txt->set(Text::LAYER, -2);

					// LOADING SCREEN
					txt = Text::getOrCreate("LOADING_1", true);
					txt->set(Text::SETSTRING, "Please don't close the game until full load!");
					txt->set(Text::POSX, 0.0);
					txt->set(Text::POSY, 3.26);
					txt->set(Text::MODE, Text::ALIGN_CENTER);
					txt->set(Text::UPDATETIME, 1.00);
					txt->set(Text::LAYER, -2);

					{
						Text::text* title = nullptr;
						Text::text* desc = nullptr;

						title = Text::getOrCreate("LOADING_0");
						desc = Text::getOrCreate("LOADING_1");


						data->disp.capFPS(60);
						data->cam.apply(-2);

						actual_perc = 0.00;
						std::thread thr(__load, this);
						double timer = al_get_time();
						double local_timer = al_get_time();

						while (actual_perc != 1.00)
						{
							data->cam.set(-2, Camera::ROTATION, cos(al_get_time())*0.04);
							data->cam.apply();

							title->set(Text::SETSTRING, "Loading [" + std::to_string(100.0*actual_perc) + "\\%]");

							data->spr_data.draw();
							data->txt_data.draw();

							if (!data->disp.flip()) {
								while (actual_perc != 1.00); // hold
								continue;
							}

							if (al_get_time() - local_timer > 7.0)
							{
								local_timer = al_get_time();
								switch (rand() % 12) {
								case 0:
									desc->set(Text::SETSTRING, "It may take a while! Please wait...");
									break;
								case 1:
									desc->set(Text::SETSTRING, "Now this line is random!");
									break;
								case 2:
									desc->set(Text::SETSTRING, "I'm just loading the entire game, please wait!");
									break;
								case 3:
									desc->set(Text::SETSTRING, "Sure it's safe. Everything is saved at %appdata%/Lohk's Studios!");
									break;
								case 4:
									desc->set(Text::SETSTRING, "Got questions? Ask @lohkdesgds!");
									break;
								case 5:
									desc->set(Text::SETSTRING, "This game has been made for #gamejaaj2 in the first place!");
									break;
								case 6:
									desc->set(Text::SETSTRING, "Hey yeah Lohk is a real cat!");
									break;
								case 7:
									desc->set(Text::SETSTRING, "Subscribete to mei chanul on uTub!");
									break;
								case 8:
									desc->set(Text::SETSTRING, "B*tch Lasagna & YouTube Rewind 2018 pewds edition are VERY NICE");
									break;
								case 9:
									desc->set(Text::SETSTRING, "Sure it's about to end! Just wait a little bit more!");
									break;
								case 10:
									desc->set(Text::SETSTRING, "THANOS CARRR");
									break;
								case 11:
									desc->set(Text::SETSTRING, "Meaw OwO");
									break;
								}
							}
						}

						actual_perc = 0.00;

						thr.join();

						if (!data->disp.isOpen())
							throw "MAIN::LOAD - DISPLAY GOT CLOSED, ABORT!";


						// AFTER-LOAD (shared)
						title->set(Text::SETGLOBALPATH, Defaults::font_default_name);
						title->set(Text::IS_GLOBALPATH_RAW, false);
						// ENDOF AFTER-LOAD (shared)

						title->set(Text::SETSTRING, "Loaded! Time taken: " + std::to_string(al_get_time() - timer) + " seconds.");
						desc->set(Text::SETSTRING, "-- YAY YOU'VE GOT THIS =P --");

						for (double d = al_get_time(); (al_get_time() - d < 3.0);) {
							data->cam.set(-2, Camera::ROTATION, cos(al_get_time())*0.04);
							data->cam.apply();

							title->set(Text::COLOR, al_map_rgba_f(1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0));
							desc->set(Text::COLOR, al_map_rgba_f(1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0));

							data->spr_data.draw();
							data->txt_data.draw();

							if (!data->disp.flip()) break;
						}

						if (!data->disp.isOpen())
							throw "MAIN::LOAD - DISPLAY GOT CLOSED, ABORT!";

						// quick checkup
						data->disp.checkUpImages();

						// reset rotation

						if (data->setup.preload_textures) {
							// ReSet literally
							data->cam.set(-2, -2, true);
							data->cam.set(-2, Camera::ZOOM, 1.0);
							data->cam.set(-2, Camera::OFFY, 0.0);
							data->cam.set(-2, Camera::ROTATION, 0.0);
							data->cam.apply();

							title->set(Text::SETSTRING, "Checking...");
							title->set(Text::POSX, 0.95);
							title->set(Text::POSY, 0.87);
							title->set(Text::MODE, Text::ALIGN_RIGHT);
							title->set(Text::COLOR, al_map_rgba_f(0.5, 0.5, 0.5, 0.5));
							title->set(Text::UPDATETIME, 0.2);
							data->disp.capFPS(30);

							data->img_data.work().lock();
							size_t nnow = 0, sizz = data->img_data.work().getMax();
							for (auto& i : data->img_data.work().work())
							{
								if (data->disp.flip(false)) {
									data->spr_data.draw();
									data->txt_data.draw();
								}

								i->reload();

								Safer::safe_string tempstr;
								i->get(Image::ID, tempstr);
								title->set(Text::SETSTRING, std::string("Checking [") + std::to_string(nnow) + "/" + std::to_string(sizz) + "]: " + tempstr.g());
								nnow++;
							}
							data->img_data.work().unlock();
						}

						// reload of the font to the new expected one
						data->txt_data.work().lock();
						for (auto& i : data->txt_data.work().work()) {
							i->unload();
							i->verify();
						}
						data->txt_data.work().unlock();
					}

					// Some flags
					{
						if (data->setup.fixed_memory_flag) {
							Image::image_low* nulp;
							data->img_data.get(nulp, 0);
							assert(nulp);
							nulp->set(Image::GLOBAL_SET_NO_OPTIMIZING_SETTING, data->setup.fixed_memory_flag);
						}
					}

					Text::easyRemove("LOADING_0");
					Text::easyRemove("LOADING_1");

					// SETUP OF THE FUNCTIONS FOR MOUSE (maybe more later)

					// start functions
					data->bev.g().addFunction(NEWF(Events::add_t_s, 0), &Events::defaultfunction_add);
					data->bev.g().addFunction(NEWF(Events::cos_t_s, 1), &Events::defaultfunction_cos);

					// get instances
					Events::add_t_s* a1 = nullptr;
					Events::cos_t_s* b1 = nullptr;

					a1 = (Events::add_t_s*)data->bev.g().getFVoidArg(0);
					b1 = (Events::cos_t_s*)data->bev.g().getFVoidArg(1);

					// verify
					if (!a1 || !b1) {
						throw "MAIN::LOAD - COULD NOT CREATE EVENTS FUNCTIONS!";
					}

					// real setup
					a1->adding = 2.0;
					data->bev.g().setTimerForF(1.0 / 90, 0);

					b1->ampl = 0.2;
					b1->default_val = 1.0;
					b1->time_multiplier = 8.0;

					srand(al_get_time());

					if (data->setup.skip_intro_flag) data->playing = MENU;
					else data->playing = INTRO;
				}
				catch (const Safer::safe_string& s)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:LOAD_][ERROR] Caught throw at main::load: " << s << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw s;
				}
				catch (const std::string& s)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:LOAD_][ERROR] Caught throw at main::load: " << s << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw s;
				}
				catch (const char* s)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:LOAD_][ERROR] Caught throw at main::load: " << s << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw s;
				}
				catch (const int i)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:LOAD_][ERROR] Caught throw at main::load: Error code #" << i << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw i;
				}
				catch (...)
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:LOAD_][ERROR] Caught throw at main::load: Unknown error." << Log::NEEDED_ENDL;
					if (!data->disp.isOpen()) throw 0;
					throw 1;
				}
			}

			const bool main::play()
			{
				if (!check("play")) return false;

				switch (data->playing)
				{
				case INTRO:
				{
					// VARIABLES
					Sprite::sprite *s00;
					double start_point;

					// SEARCHES
					s00 = Sprite::getOrCreate("INTRO");

					// SETTING VARIABLES VALUES
					s00->set(Sprite::ANIMATIONTIME, -1.0);
					s00->set(Sprite::ANIMATIONTIME, 20.0);
					s00->set(Sprite::TINT, al_map_rgb(255, 255, 255));
					start_point = al_get_time();

					// CAMERA AND LAYER SET
					data->disp.capFPS(60);
					__apply_cam_number(-1);
					__apply_layer_number(-1);

					// THE WHILE
					while (!s00->gotToEnd())
					{
						// FX
						if ((al_get_time() - start_point) < 1.0) data->cam.set(-1, Camera::OFFY, pow(2.0*cos((al_get_time() - start_point)*ALLEGRO_PI*0.5), 1.5));
						else data->cam.set(-1, Camera::OFFY, 0);
						data->cam.apply();

						// FLIP AND STUFF
						if (!__internal_task_level_common()) return false;
					}
					s00->set(Sprite::ANIMATIONTIME, 0.0);
					for (double n = al_get_time(); al_get_time() - n < 3.0;)
					{
						// FX
						s00->set(Sprite::TINT, al_map_rgba_f(1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0));

						// FLIP AND STUFF
						if (!__internal_task_level_common()) return false;
					}

					data->playing = MENU;
				}
					break;
				case MENU:
				{
					// VARIABLES
					bool return_false;
					Fx::bubbles bubbles;
					Sprite::sprite* mouse;
					Sprite::sprite* b00;
					Sprite::sprite* b01;
					Sprite::sprite* b02;
					Sound::track* t00;
					Sound::track* t01;
					Sound::track* t02;
					Text::text* x00;
					Text::text* x01;

					// SEARCHES
					mouse = Sprite::getOrCreate("MOUSE");
					b00 = Sprite::getOrCreate("BAR_00");
					b01 = Sprite::getOrCreate("BAR_01");
					b02 = Sprite::getOrCreate("BAR_02");
					t00 = Sound::getOrCreate("MUSIC_0");
					t01 = Sound::getOrCreate("MUSIC_1");
					t02 = Sound::getOrCreate("MUSIC_2");
					x00 = Text::getOrCreate("BAR_00_TEXT");
					x01 = Text::getOrCreate("BAR_01_TEXT");

					// SETTING VARIABLES VALUES
					x00->set(Text::SETSTRING, "< START >");
					x01->set(Text::SETSTRING, "< SETTINGS >");
					b00->set(Sprite::DRAW, true);
					b01->set(Sprite::DRAW, true);
					b02->set(Sprite::DRAW, true);
					return_false = false;
					bubbles.init(300, 60.0, -20);
					t00->set(Sound::PLAYING, false);
					t01->set(Sound::PLAYING, false);
					t02->set(Sound::PLAYING, false);
					if (rand() % 2)
						t00->set(Sound::PLAYING, true);
					else 
						t01->set(Sound::PLAYING, true);
					
					// CAMERA AND LAYER SET
					data->disp.capFPS();
					__apply_cam_number(0);
					__apply_layer_number(0);

					// THE WHILE
					for (bool keep = true; keep;)
					{
						// INPUT
						bool b0, b1, b2;
						b00->get(Sprite::_IS_COLLIDING, b0);
						b01->get(Sprite::_IS_COLLIDING, b1);
						b02->get(Sprite::_IS_COLLIDING, b2);

						if (b0) b00->set(Sprite::ANIMATIONTIME, -1);
						else b00->set(Sprite::ANIMATIONTIME, 0);
						if (b1) b01->set(Sprite::ANIMATIONTIME, -1);
						else b01->set(Sprite::ANIMATIONTIME, 0);
						if (b2) b02->set(Sprite::ANIMATIONTIME, -1);
						else b02->set(Sprite::ANIMATIONTIME, 0);

						if (data->bev.g().getKey(Events::MOUSE_0, false)) {
							if (b0) { // PLAY
								data->playing = PLAYING;
								keep = false;
							}
							if (b1) { // SETTINGS
								data->playing = SETTINGS;
								data->bev.g().getKey(Events::MOUSE_0, true);
								keep = false;
							}
							if (b2) { // exit
								return_false = true;
								keep = false;
							}
						}

						// FX
						mouse->set(Sprite::ROTATION, data->bev.g().getFunctionValNow(0));
						mouse->set(Sprite::SCALEG, 0.07 * data->bev.g().getFunctionValNow(1));
						bubbles.think();
						bubbles.draw();

						// FLIP AND STUFF
						if (!__internal_task_level_common()) return false;
					}

					if (return_false) return false;
				}
					break;
				case PLAYING:
				{
					// VARIABLES
					Map::map *map;
					Sprite::sprite *mouse;
					Sprite::sprite *player;
					Sound::track *t00;
					Sound::track *t01;
					Sound::track *t02;
					Text::text* level;
					//Text::text* txt;
					double zoom = 1.0;
					double pos_x = 0.0, pos_y = 0.0;
					bool is_following_player = false;
					int temprand = rand();
					double corrosion;
					double music_fx;
					double level_now_fx;

					// SEARCHES
					map = nullptr;
					mouse = Sprite::getOrCreate("MOUSE");
					player = nullptr; // started from map
					t00 = Sound::getOrCreate("MUSIC_0");
					t01 = Sound::getOrCreate("MUSIC_1");
					t02 = Sound::getOrCreate("MUSIC_2");

					// CAMERA (needed before)
					data->disp.capFPS();
					__apply_cam_number(2);
					
					// SETTING VARIABLES VALUES
					level_now = 1;
					if (!(map = new Map::map())) {
						throw "MAIN::PLAY - COULD NOT CREATE MAP POINTER!";
						return false;
					}

					if (!player_settings) map->launch_player("BAR_ON");
					else {
						map->launch_player(player_settings->color);
						map->setPlayerName(player_settings->nickname);
					}
					map->launch_badboys("BAR_OFF", 4);
					map->setSeed(temprand);

					level = Text::getOrCreate("LEVEL_SHOW", true);
					level->set(Text::MODE, Text::ALIGN_CENTER);
					level->set(Text::AFFECTED_BY_CAM, false);
					level->set(Text::SETSTRING, (std::string("LEVEL #") + std::to_string(level_now)));
					level->set(Text::COLOR, al_map_rgba_f(0.0,0.0,0.0,0.0));
					level->set(Text::SCALEG, 1.5);
					level->set(Text::UPDATETIME, 0.05);
					level->set(Text::LAYER, Defaults::map_default_layer);

					{
						map->setCPULock(true);

						Text::text* txt = Text::getOrCreate("LOADING_SCREEN_MAP", true);
						txt->set(Text::SETSTRING, "LOADING NEW MAP...");
						txt->set(Text::MODE, Text::ALIGN_CENTER);
						txt->set(Text::AFFECTED_BY_CAM, false);
						txt->set(Text::COLOR, al_map_rgba_f(0.7, 0.7, 0.7, 0.7));
						txt->set(Text::SCALEG, 1.5);
						txt->set(Text::UPDATETIME, 0.05);
						txt->set(Text::LAYER, Defaults::map_default_layer);

						while (map->isCPUtasking()) {
							_rgb_clear_screen_load();
						}

						std::thread thr_local(__random_map_load, map);

						while (!map->isMapLoaded()) {
							_rgb_clear_screen_load();
						}

						thr_local.join();

						map->start_draw_thr();

						Text::easyRemove("LOADING_SCREEN_MAP");

						map->setCPULock(false);
					}

					data->lyr.getOne(2).save_package(map);
					t00->set(Sound::PLAYING, false);
					t01->set(Sound::PLAYING, false);
					t02->set(Sound::PLAYING, true);
					t02->set(Sound::SPEED, 1.0);
					level_now_fx = al_get_time();
					music_fx = corrosion = al_get_time() + Defaults::corrosion_default_time_start;

					// LAYER CONFIGURATION
					__apply_layer_number(2);

					// THE WHILE
					for (bool k = true; k;) {

						// TASKS
						map->checkDraw();
						if (al_get_time() - corrosion > Defaults::corrosion_default_time) {
							corrosion = al_get_time();
							map->corruptWorldTick();
						}
						if (al_get_time() - music_fx > Defaults::corrosion_default_time)
						{
							double fx_amount = al_get_time() - music_fx - Defaults::corrosion_default_time; // speed
							fx_amount *= Defaults::music_prop_time;
							fx_amount += 1.0; // min 1.0
							if (fx_amount > Defaults::music_maxspeed) fx_amount = Defaults::music_maxspeed;
							t02->set(Sound::SPEED, fx_amount);
						}
						if (al_get_time() - level_now_fx < 5.0) {
							double diff = al_get_time() - level_now_fx;
							diff = 5.0 - diff;
							diff /= 5.0;
							diff *= 0.8;
							level->set(Text::SETSTRING, (std::string("LEVEL #") + std::to_string(level_now)));
							level->set(Text::COLOR, al_map_rgba_f(diff, diff, diff, diff));
						}
						else level->set(Text::COLOR, al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
						//map->checkPositionChange();

						if (map->isDead()) {
							level->set(Text::COLOR, al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
							data->playing = LOSER;
							k = false;
							continue;
						}
						if (map->hasReachedEnd()) {
							level->set(Text::COLOR, al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
							map->setCPULock(true);

							Text::text* txt = Text::getOrCreate("LOADING_SCREEN_MAP", true);
							txt->set(Text::SETSTRING, "LOADING NEW MAP...");
							txt->set(Text::MODE, Text::ALIGN_CENTER);
							txt->set(Text::AFFECTED_BY_CAM, false);
							txt->set(Text::COLOR, al_map_rgba_f(0.7, 0.7, 0.7, 0.7));
							txt->set(Text::SCALEG, 1.5);
							txt->set(Text::UPDATETIME, 0.05);
							txt->set(Text::LAYER, Defaults::map_default_layer);

							while (map->isCPUtasking()) {
								_rgb_clear_screen_load();
							}

							map->reset_draw_thr();
							map->reset_cpu_thr();

							std::thread thr_local(__random_map_load, map);

							while (!map->isMapLoaded()) {
								_rgb_clear_screen_load();
							}

							thr_local.join();

							map->start_draw_thr();
													   
							Text::easyRemove("LOADING_SCREEN_MAP");
							music_fx = corrosion = al_get_time() + Defaults::corrosion_default_time_start;
							t02->set(Sound::SPEED, 1.0);

							map->setCPULock(false);

							// Level
							level_now_fx = al_get_time();
							level_now++;
						}
						if (data->bev.g().getKey(Events::KEY_ESCAPE)) {
							level->set(Text::COLOR, al_map_rgba_f(0.0, 0.0, 0.0, 0.0));
							map->Pause(true);
							levels tl = _rgb_pause_screen();
							map->Pause(false);

							// reapply layer setup
							__apply_cam_number(2);
							__apply_layer_number(2);

							// Level
							level_now_fx = al_get_time();

							if (tl != data->playing) { // end
								data->playing = tl;
								k = false;
								continue;
							}
						}

						// FLIP AND STUFF
						if (!__internal_task_level_common()) {
							map->setCPULock(true);
							while (map->isCPUtasking());
							delete map;
							return false;
						}
					}

					if (map){
						map->setCPULock(true);
						while (map->isCPUtasking());
						delete map;
					}

					Text::easyRemove("LEVEL_SHOW");
				}
					break;
				case SETTINGS:
				{
					// VARIABLES
					Sprite::sprite* menu_00; // enable OSD													///
					Sprite::sprite* menu_01; // set music volume (background fixed)							///
					Sprite::sprite* menu_02; // set music volume (grap)										///
					Sprite::sprite* menu_03; // create player settings, so then creates:					///
					Sprite::sprite* menu_04; // Set Nickname (big block, enter saves)						///
					Sprite::sprite* menu_05; // Set predefined color (RGB+CYM+BW)							///
					Sprite::sprite* menu_06; // back														///
					Text::text* menu_00_t;																	///
					Text::text* menu_01_t;																	///
					//Text::text* menu_02_t;																/// SHOULDN'T EXIST (now)
					Text::text* menu_03_t;																	///
					Text::text* menu_04_t;																	///
					Text::text* menu_05_t;																	///
					Text::text* menu_06_t;																	///
					Fx::bubbles bubbles;
						
					Sprite::sprite* mouse;
					Sound::track* t00;
					float volume_got;
					bool is_typing_name;

					// SEARCHES
					mouse = Sprite::getOrCreate("MOUSE");
					t00 = Sound::getOrCreate("MUSIC_0");


					// SETTING VARIABLES VALUES
					is_typing_name = false;
					t00->get(Sound::VOLUME, volume_got);
					bubbles.init(300, 60.0, Defaults::default_settings_layer);
					{
						/** #00 - enable OSD
						- Static pos
						- Click: _setKey F3 (fake key)
						*/
						menu_00 = Sprite::getOrCreate("L_MENU_00", true);
						menu_00->add("BAR_OFF");
						menu_00->add("BAR_ON");
						menu_00->set(Sprite::SCALEG, 0.6);
						menu_00->set(Sprite::COLLIDE, true);
						menu_00->set(Sprite::SHOWBOX, /*true*/false);
						menu_00->set(Sprite::LAYER, Defaults::default_settings_layer);
						menu_00->set(Sprite::SCALEY, 0.20);
						menu_00->set(Sprite::SCALEX, 1.5);
						menu_00->set(Sprite::POSX, 0.0);
						menu_00->set(Sprite::POSY, -0.50);

						menu_00_t = Text::getOrCreate("L_MENU_00", true);
						menu_00_t->set(Text::SETSTRING, "Enable or disable OSD");
						menu_00_t->set(Text::SETFOLLOW, "L_MENU_00");
						menu_00_t->set(Text::MODE, Text::ALIGN_CENTER);
						menu_00_t->set(Text::COLOR, al_map_rgb_f(1.0,1.0,1.0));
						menu_00_t->set(Text::SCALEG, 1.3);
						menu_00_t->set(Text::UPDATETIME, 0.5);
						menu_00_t->set(Text::POSY, -0.053);
						menu_00_t->set(Text::LAYER, Defaults::default_settings_layer);

						/** #01 - set music volume (background fixed)
						- Static
						*/
						menu_01 = Sprite::getOrCreate("L_MENU_01", true);
						menu_01->add("BAR_OFF");
						menu_01->set(Sprite::SCALEG, 0.6);
						menu_01->set(Sprite::COLLIDE, false); // FIXED
						menu_01->set(Sprite::SHOWBOX, false);
						menu_01->set(Sprite::LAYER, Defaults::default_settings_layer);
						menu_01->set(Sprite::SCALEY, 0.20);
						menu_01->set(Sprite::SCALEX, 1.5);
						menu_01->set(Sprite::POSX, 0.0);
						menu_01->set(Sprite::POSY, -0.25);

						menu_01_t = Text::getOrCreate("L_MENU_01", true);
//						menu_01_t->set(Text::SETSTRING, "Volume: 100\\%"); // defined later
						menu_01_t->set(Text::SETFOLLOW, "L_MENU_01");
						menu_01_t->set(Text::MODE, Text::ALIGN_CENTER);
						menu_01_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 1.0));
						menu_01_t->set(Text::SCALEG, 1.3);
						menu_01_t->set(Text::UPDATETIME, 0.02);
						menu_01_t->set(Text::POSY, -0.053);
						menu_01_t->set(Text::LAYER, Defaults::default_settings_layer);

						/** #02 - set music volume
						- Movable (x axis)
						- Set all music volumes from 0.0 to 1.0 (from posx)
						- Y FIXED
						- X follow MOUSE when COLLISION && Mouse clicked
						*/
						menu_02 = Sprite::getOrCreate("L_MENU_02", true);
						menu_02->add("BAR_ON");
						menu_02->set(Sprite::SCALEG, 0.6);
						menu_02->set(Sprite::COLLIDE, true);
						menu_02->set(Sprite::SHOWBOX, /*true*/false);
						menu_02->set(Sprite::LAYER, Defaults::default_settings_layer);
						menu_02->set(Sprite::SCALEY, 0.20); // FORCE
						menu_02->set(Sprite::SCALEX, 0.20); // IT'S A BAR
						//menu_02->set(Sprite::POSX, 0.39); // 100%
						/* Special part*/
						{
							bool welldone = true;
							float pos;
							try {
								t00->get(Sound::GLOBALVOLUME, pos);
							}
							catch (...)
							{
								welldone = false;
								menu_02->set(Sprite::POSX, 0.39);
								menu_01_t->set(Text::SETSTRING, "Volume: UNKNOWN\\%");
							}
							if (welldone) {
								menu_01_t->set(Text::SETSTRING, "Volume: " + std::to_string((int)(100.0*pos)) + "\\%");
								pos = (pos * 0.780 - 0.390);
								menu_02->set(Sprite::POSX, pos);
							}
						}
						// endof Special part
						menu_02->set(Sprite::POSY, -0.25);

						// EMPTY TEXT SPACE

						/** #03 - create player settings (new player settings)
						- Click: if !player_settings, new; else delete
						*/
						menu_03 = Sprite::getOrCreate("L_MENU_03", true);
						menu_03->add("BAR_OFF");
						menu_03->add("BAR_ON");
						menu_03->set(Sprite::SCALEG, 0.6);
						menu_03->set(Sprite::COLLIDE, true);
						menu_03->set(Sprite::SHOWBOX, /*true*/false);
						menu_03->set(Sprite::LAYER, Defaults::default_settings_layer);
						menu_03->set(Sprite::SCALEY, 0.20); // FORCE
						menu_03->set(Sprite::SCALEX, 1.5);
						menu_03->set(Sprite::POSX, 0.0);
						menu_03->set(Sprite::POSY, 0.0);

						menu_03_t = Text::getOrCreate("L_MENU_03", true);
						menu_03_t->set(Text::SETSTRING, "(Re)create player profile");
						menu_03_t->set(Text::SETFOLLOW, "L_MENU_03");
						menu_03_t->set(Text::MODE, Text::ALIGN_CENTER);
						menu_03_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 1.0));
						menu_03_t->set(Text::SCALEG, 1.3);
						menu_03_t->set(Text::POSY, -0.053);
						menu_03_t->set(Text::LAYER, Defaults::default_settings_layer);

						/** #04 - Set Nickname (big block, enter saves)
						- Click: clear and lock for text input
						- <ENTER>: Saves and "leave"
						*/
						menu_04 = Sprite::getOrCreate("L_MENU_04", true);
						menu_04->add("BAR_OFF");
						menu_04->set(Sprite::SCALEG, 0.6);
						menu_04->set(Sprite::COLLIDE, true);
						menu_04->set(Sprite::SHOWBOX, /*true*/false);
						menu_04->set(Sprite::LAYER, Defaults::default_settings_layer);
						menu_04->set(Sprite::SCALEY, 0.18); // FORCE
						menu_04->set(Sprite::SCALEX, 1.4);
						menu_04->set(Sprite::POSX, 0.0);
						menu_04->set(Sprite::POSY, 0.23);
						menu_04->set(Sprite::DRAW, (player_settings));

						menu_04_t = Text::getOrCreate("L_MENU_04", true);
						menu_04_t->set(Text::SETSTRING, std::string("Nick: <NULL>"));
						menu_04_t->set(Text::SETFOLLOW, "L_MENU_04");
						menu_04_t->set(Text::MODE, Text::ALIGN_CENTER);
						menu_04_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 1.0));
						menu_04_t->set(Text::SCALEG, 1.05);
						menu_04_t->set(Text::POSY, -0.03);
						menu_04_t->set(Text::LAYER, Defaults::default_settings_layer);

						/** #05 - Set predefined color (RGB+CYM+BW)
						- Click: clear and lock for text input
						- <ENTER>: Saves and "leave"
						*/
						menu_05 = Sprite::getOrCreate("L_MENU_05", true);
						menu_05->add("BAR_OFF");
						menu_05->add("BAR_ON");
						menu_05->set(Sprite::SCALEG, 0.6);
						menu_05->set(Sprite::COLLIDE, true);
						menu_05->set(Sprite::SHOWBOX, /*true*/false);
						menu_05->set(Sprite::LAYER, Defaults::default_settings_layer);
						menu_05->set(Sprite::SCALEY, 0.18); // FORCE
						menu_05->set(Sprite::SCALEX, 1.4);
						menu_05->set(Sprite::POSX, 0.0);
						menu_05->set(Sprite::POSY, 0.43);
						menu_05->set(Sprite::DRAW, (player_settings));

						menu_05_t = Text::getOrCreate("L_MENU_05", true);
						menu_05_t->set(Text::SETSTRING, "Color: <NULL>");
						menu_05_t->set(Text::SETFOLLOW, "L_MENU_05");
						menu_05_t->set(Text::MODE, Text::ALIGN_CENTER);
						menu_05_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 1.0));
						menu_05_t->set(Text::SCALEG, 1.05);
						menu_05_t->set(Text::POSY, -0.03);
						menu_05_t->set(Text::LAYER, Defaults::default_settings_layer);

						/** #06 - back
						- Click: Same as <ESCAPE>
						*/
						menu_06 = Sprite::getOrCreate("L_MENU_06", true);
						menu_06->add("BAR_OFF");
						menu_06->add("BAR_ON");
						menu_06->set(Sprite::SCALEG, 0.6);
						menu_06->set(Sprite::COLLIDE, true);
						menu_06->set(Sprite::SHOWBOX, /*true*/false);
						menu_06->set(Sprite::LAYER, Defaults::default_settings_layer);
						menu_06->set(Sprite::SCALEY, 0.20); // FORCE
						menu_06->set(Sprite::SCALEX, 1.2);
						menu_06->set(Sprite::POSX, 0.0);
						menu_06->set(Sprite::POSY, 0.70);

						menu_06_t = Text::getOrCreate("L_MENU_06", true);
						menu_06_t->set(Text::SETSTRING, "<BACK>");
						menu_06_t->set(Text::SETFOLLOW, "L_MENU_06");
						menu_06_t->set(Text::MODE, Text::ALIGN_CENTER);
						menu_06_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 1.0));
						menu_06_t->set(Text::SCALEG, 1.3);
						menu_06_t->set(Text::POSY, -0.053);
						menu_06_t->set(Text::LAYER, Defaults::default_settings_layer);
					}

					// CAMERA AND LAYER SET
					data->disp.capFPS();
					__apply_cam_number(3);
					__apply_layer_number(3);

					// THE WHILE
					for (bool keep = true; keep;)
					{
						// PRE-TASK
						menu_04->set(Sprite::DRAW, (player_settings));
						menu_05->set(Sprite::DRAW, (player_settings));
						if (player_settings) {
							menu_04_t->set(Text::SETSTRING, std::string("Nick: ") + player_settings->nickname.g());
							menu_05_t->set(Text::SETSTRING, std::string("Color: ") + interpret_color(player_settings->color_interp).g());
						}
						if (is_typing_name) { // TODO: limit string size
							if (player_settings) {
								menu_04_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 0.0));
								Safer::safe_string ss, ess;

								data->bev.g().getCurrentString(ss);
								data->bev.g().getLastString(ess, true);

								if (ess.g().length() < 1) {
									std::string ssf = ss.g();
									if (ssf.length() > 22) ssf = ssf.substr(0, 22);

									menu_04_t->set(Text::SETSTRING, std::string("Nick: ") + ssf);
								}
								else {
									is_typing_name = false;
									std::string ssf = ess.g();
									if (ssf.length() > 22) ssf = ssf.substr(0, 22);

									player_settings->nickname = ssf;
								}
							}
							else {
								is_typing_name = false;
							}
						}
						else {
							menu_04_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 1.0));
						}

						// INPUT
						bool b00, b02, b03, b04, b05, b06;
						menu_00->get(Sprite::_IS_COLLIDING, b00);
						menu_02->get(Sprite::_IS_COLLIDING, b02);
						menu_03->get(Sprite::_IS_COLLIDING, b03);
						menu_04->get(Sprite::_IS_COLLIDING, b04);
						menu_05->get(Sprite::_IS_COLLIDING, b05);
						menu_06->get(Sprite::_IS_COLLIDING, b06);

						// show on or off
						if (b00) menu_00->set(Sprite::ANIMATIONTIME, -1);
						else menu_00->set(Sprite::ANIMATIONTIME, 0);

						// mouse fx
						if (b03) menu_03->set(Sprite::ANIMATIONTIME, -1);
						else menu_03->set(Sprite::ANIMATIONTIME, 0);
						if (b04) menu_04->set(Sprite::ANIMATIONTIME, -1);
						else menu_04->set(Sprite::ANIMATIONTIME, 0);
						if (b05) menu_05->set(Sprite::ANIMATIONTIME, -1);
						else menu_05->set(Sprite::ANIMATIONTIME, 0);
						if (b06) menu_06->set(Sprite::ANIMATIONTIME, -1);
						else menu_06->set(Sprite::ANIMATIONTIME, 0);

						if (data->bev.g().getKey(Events::MOUSE_0, false)) {
							if (b00) {
								data->bev.g()._setKey(Events::KEY_F3, true); // MAYBE CHANGE LATER
							}
							else if (b02) {
								float mx;
								data->bev.g().getMouse(mx, Events::X);

								if (mx > 0.390) mx = 0.390;
								if (mx < -0.390) mx = -0.390;

								menu_02->set(Sprite::POSX, mx);

								float volume = ((mx + 0.390) / 0.780);

								menu_01_t->set(Text::SETSTRING, "Volume: " + std::to_string((int)(100.0*volume)) + "\\%");

								try {
									t00->set(Sound::GLOBALVOLUME, volume);
								}
								catch (...)
								{
									menu_01_t->set(Text::SETSTRING, "Volume: (FATAL ERROR PLEASE RESTART GAME!)");
								}
							}
							else if (b03) {
								if (player_settings) {
									delete player_settings;
									player_settings = nullptr;
								}
								if (!player_settings) player_settings = new __player_settings();

								data->bev.g().getKey(Events::MOUSE_0, true);
							}
							else if (b04) {
								Safer::safe_string buf;

								is_typing_name = !is_typing_name;
								data->bev.g()._setKey(Events::CUSTOMEVENT_RESETCURRSTRING, true);

								data->bev.g().getCurrentString(buf, true);
								data->bev.g().getLastString(buf, true);

								data->bev.g().getKey(Events::MOUSE_0, true);
							}
							else if (b05 && player_settings) {
								int tempp = (int)player_settings->color_interp;
								tempp++;
								if (tempp > YELLOW) tempp = 0;
								player_settings->color_interp = (colors)tempp;
								player_settings->color = interpret_to_color(player_settings->color_interp);

								data->bev.g().getKey(Events::MOUSE_0, true);
							}
							else if (b06) {
								keep = false;

								data->bev.g().getKey(Events::MOUSE_0, true);
							}
						}
						if (data->bev.g().getKey(Events::KEY_ESCAPE)) keep = false;

						// FX
						mouse->set(Sprite::ROTATION, data->bev.g().getFunctionValNow(0));
						mouse->set(Sprite::SCALEG, 0.07 * data->bev.g().getFunctionValNow(1));
						bubbles.think();
						bubbles.draw();

						// FLIP AND STUFF
						if (!__internal_task_level_common()) return false;
					}

					Sprite::easyRemove("L_MENU_00");
					Sprite::easyRemove("L_MENU_01");
					Sprite::easyRemove("L_MENU_02");
					Sprite::easyRemove("L_MENU_03");
					Sprite::easyRemove("L_MENU_04");
					Sprite::easyRemove("L_MENU_05");
					Sprite::easyRemove("L_MENU_06");
					Text::easyRemove("L_MENU_00");
					Text::easyRemove("L_MENU_01");
					Text::easyRemove("L_MENU_03");
					Text::easyRemove("L_MENU_04");
					Text::easyRemove("L_MENU_05");
					Text::easyRemove("L_MENU_06");

					data->playing = MENU;
				}
				break;
				case LOSER:
				{
					__apply_layer_number(4);
					__apply_cam_number(4);

					// VARIABLES
					Fx::lines lines(-81);
					Sprite::sprite* mouse;
					Sprite::sprite* menu_00;
					Sprite::sprite* menu_01;
					Text::text* menu_00_t;
					Text::text* menu_01_t;

					// SEARCHES
					mouse = Sprite::getOrCreate("MOUSE");

					// SETTING VARIABLES VALUES
					{

						menu_00 = Sprite::getOrCreate("L_MENU_00", true);
						menu_00->add("BAR_OFF");
						menu_00->add("BAR_ON");
						menu_00->set(Sprite::SCALEG, 0.6);
						menu_00->set(Sprite::COLLIDE, true);
						menu_00->set(Sprite::SHOWBOX, /*true*/false);
						menu_00->set(Sprite::LAYER, -81);
						menu_00->set(Sprite::SCALEY, 0.20);
						menu_00->set(Sprite::SCALEX, 1.5);
						menu_00->set(Sprite::POSX, 0.0);
						menu_00->set(Sprite::POSY, -0.15);

						menu_00_t = Text::getOrCreate("L_MENU_00", true);
						menu_00_t->set(Text::SETSTRING, "YOU FAILED");
						menu_00_t->set(Text::SETFOLLOW, "L_MENU_00");
						menu_00_t->set(Text::MODE, Text::ALIGN_CENTER);
						menu_00_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 1.0));
						menu_00_t->set(Text::SCALEG, 1.3);
						menu_00_t->set(Text::UPDATETIME, 0.5);
						menu_00_t->set(Text::POSY, -0.053);
						menu_00_t->set(Text::LAYER, -81);

						menu_01 = Sprite::getOrCreate("L_MENU_01", true);
						menu_01->add("BAR_OFF");
						menu_01->add("BAR_ON");
						menu_01->set(Sprite::SCALEG, 0.6);
						menu_01->set(Sprite::COLLIDE, true);
						menu_01->set(Sprite::SHOWBOX, /*true*/false);
						menu_01->set(Sprite::LAYER, -81);
						menu_01->set(Sprite::SCALEY, 0.20);
						menu_01->set(Sprite::SCALEX, 1.5);
						menu_01->set(Sprite::POSX, 0.0);
						menu_01->set(Sprite::POSY, 0.15);

						menu_01_t = Text::getOrCreate("L_MENU_01", true);
						menu_01_t->set(Text::SETSTRING, "Back to Menu");
						menu_01_t->set(Text::SETFOLLOW, "L_MENU_01");
						menu_01_t->set(Text::MODE, Text::ALIGN_CENTER);
						menu_01_t->set(Text::COLOR, al_map_rgb_f(1.0, 1.0, 1.0));
						menu_01_t->set(Text::SCALEG, 1.3);
						menu_01_t->set(Text::UPDATETIME, 0.5);
						menu_01_t->set(Text::POSY, -0.053);
						menu_01_t->set(Text::LAYER, -81);
					}



					// THE WHILE
					for (bool keep = true; keep;)
					{
						// INPUT
						bool b0, b1;
						menu_00->get(Sprite::_IS_COLLIDING, b0);
						menu_01->get(Sprite::_IS_COLLIDING, b1);

						if (b0) menu_00->set(Sprite::ANIMATIONTIME, -1);
						else menu_00->set(Sprite::ANIMATIONTIME, 0);
						if (b1) menu_01->set(Sprite::ANIMATIONTIME, -1);
						else menu_01->set(Sprite::ANIMATIONTIME, 0);

						if (data->bev.g().getKey(Events::MOUSE_0)) {
							if (b1) {
								keep = false;
								continue;
							}
						}
						if (data->bev.g().getKey(Events::KEY_ESCAPE))
						{
							keep = false;
							continue;
						}

						// FX
						mouse->set(Sprite::ROTATION, data->bev.g().getFunctionValNow(0));
						mouse->set(Sprite::SCALEG, 0.07 * data->bev.g().getFunctionValNow(1));
						lines.draw();

						// FLIP AND STUFF
						if (!__internal_task_level_common()) return false;
					}

					Sprite::easyRemove("L_MENU_00");
					Sprite::easyRemove("L_MENU_01");
					Text::easyRemove("L_MENU_00");
					Text::easyRemove("L_MENU_01");

					data->playing = MENU;
				}
				break;
				}

				return true;
			}


			/* --------------- INTERNAL ONES (not meant to be used anywhere than inside threads from the main class) --------------- */
			void main::__internal_thr_once_load()
			{
				if (!check("__thr_once_load")) return;

				Sprite::sprite*   ws = nullptr;
				Image::image_low* wi = nullptr;
				Text::text*       wt = nullptr;
				Sound::track*	  wm = nullptr;

				float o = 0.00;
				actual_perc = 0.00;

				// download file

				if (!data->setup.skip_download_flag) {

					Safer::safe_string path_d = Defaults::default_onefile_path;
					Tools::interpret_path(path_d);

					std::thread thr0(Downloader::easyDownload, Defaults::call_url_file.g().c_str(), path_d.g().c_str(), &o);

					while (o != 1.0 && data->disp.isOpen()) {
						actual_perc = o * 0.90;
					}

					o = 0.0;
					thr0.join();

					Stacker::extractor ext;
					std::thread thr3(__xtract_dis, (void*)&ext, path_d, Defaults::default_root_path);

					while ((o = ext.getPerc()) != 1.0 && data->disp.isOpen()) {
						actual_perc = o * 0.05 + 0.90;
					}
					thr3.join();

					std::experimental::filesystem::remove(path_d.g());
				}

				// big chunks

				std::thread thr1(Image::multipleLoad, "PAUSE", "pause/pause_", 29, 2, ".png", &o, true);
				while (o != 1.0 && data->disp.isOpen()) {
					actual_perc = 0.95 + o * 0.005;
				}

				o = 0.0;
				thr1.join();

				std::thread thr2(Image::multipleLoad, "LOGO", "logo/frame", 115, 2, ".png", &o, true);
				while (o != 1.0 && data->disp.isOpen()) {
					actual_perc = 0.955 + o * 0.005;
				}

				thr2.join();
				actual_perc = 0.96;

				// smaller chunks

				wi = Image::getOrCreate("MOUSE", true);
				wi->set(Image::PATH, "mouse.png");
				wi->set(Image::LOAD_LATER, true);

				actual_perc = 0.961;

				wi = Image::getOrCreate("BLAST_LOGO", true);
				wi->set(Image::PATH, "the_storm.png");
				wi->set(Image::LOAD_LATER, true);

				actual_perc = 0.962;

				wi = Image::getOrCreate("BAR_ON", true);
				wi->set(Image::PATH, "bar_single_one_on.png");
				wi->set(Image::LOAD_LATER, true);

				actual_perc = 0.963;

				wi = Image::getOrCreate("BAR_OFF", true);
				wi->set(Image::PATH, "bar_single_one.png");
				wi->set(Image::LOAD_LATER, true);

				actual_perc = 0.964;

				wi = Image::getOrCreate("BG_INTRO", true);
				wi->set(Image::PATH, "background_gameplay_start.png");
				wi->set(Image::LOAD_LATER, true);

				actual_perc = 0.965;

				std::thread thr4(Image::multipleLoad, Defaults::map_default_start, "anim/bloco", 10, Defaults::map_default_len_name_int, ".png", &o, false);
				while (o != 1.0 && data->disp.isOpen()) {
					actual_perc = 0.965 + o * 0.005;
				}

				thr4.join();

				actual_perc = 0.97;

				/* LOADING MUSICS...? */

				wm = Sound::getOrCreate("MUSIC_0", true);
				wm->set(Sound::PATH, "musics/music_01.ogg");
				wm->load();
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::LOOP);

				actual_perc = 0.972;

				wm = Sound::getOrCreate("MUSIC_1", true);
				wm->set(Sound::PATH, "musics/music_02.ogg");
				wm->load();
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::LOOP);

				actual_perc = 0.974;

				wm = Sound::getOrCreate("MUSIC_2", true);
				wm->set(Sound::PATH, "musics/music_03.ogg");
				wm->load();
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::LOOP);

				actual_perc = 0.976;

				wm = Sound::getOrCreate("JUMP_FX", true);
				wm->set(Sound::PATH, "musics/jump_01.wav");
				wm->load();
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::ONCE);

				actual_perc = 0.978;

				wm = Sound::getOrCreate("WALK_FX", true);
				wm->set(Sound::PATH, "musics/walk_01.wav");
				wm->load();
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::ONCE);

				actual_perc = 0.98;

				/* CREATING SPRITES...? */

				ws = Sprite::getOrCreate("BLAST_LOGO", true);
				ws->add("BLAST_LOGO");
				ws->set(Sprite::POSX, 0.0);
				ws->set(Sprite::POSY, -0.45);
				ws->set(Sprite::SCALEG, 1.0);
				ws->set(Sprite::SCALEY, 0.6);
				ws->set(Sprite::LAYER, 1);

				actual_perc = 0.981;

				ws = Sprite::getOrCreate("BAR_00", true);
				ws->add("BAR_OFF");
				ws->add("BAR_ON");
				ws->set(Sprite::POSX, 0.0);
				ws->set(Sprite::POSY, 0.4);
				ws->set(Sprite::SCALEG, 0.6);
				ws->set(Sprite::SCALEY, 0.22);
				ws->set(Sprite::COLLIDE, true);
				ws->set(Sprite::SHOWBOX, /*true*/false);
				ws->set(Sprite::LAYER, 0);

				actual_perc = 0.982;

				ws = Sprite::getOrCreate("BAR_01", true);
				ws->add("BAR_OFF");
				ws->add("BAR_ON");
				ws->set(Sprite::POSX, 0.0);
				ws->set(Sprite::POSY, 0.60);
				ws->set(Sprite::SCALEG, 0.6);
				ws->set(Sprite::SCALEY, 0.22);
				ws->set(Sprite::COLLIDE, true);
				ws->set(Sprite::SHOWBOX, /*true*/false);
				ws->set(Sprite::LAYER, 0);

				actual_perc = 0.983;

				ws = Sprite::getOrCreate("BAR_02", true);
				ws->add("BAR_OFF");
				ws->add("BAR_ON");
				ws->set(Sprite::POSX, 0.0);
				ws->set(Sprite::POSY, 0.8);
				ws->set(Sprite::SCALEG, 0.6);
				ws->set(Sprite::SCALEY, 0.22);
				ws->set(Sprite::COLLIDE, true);
				ws->set(Sprite::SHOWBOX, /*true*/false);
				ws->set(Sprite::LAYER, 0);

				actual_perc = 0.984;

				ws = Sprite::getOrCreate("PAUSE", true);
				ws->add("PAUSE", 29);
				ws->set(Sprite::ANIMATIONTIME, 24.0);
				ws->set(Sprite::POSX, 0.0);
				ws->set(Sprite::POSY, -0.34);
				ws->set(Sprite::SCALEG, 0.55);
				ws->set(Sprite::LAYER, Defaults::default_pauseonly_layer);

				actual_perc = 0.989;

				ws = Sprite::getOrCreate("INTRO", true);
				ws->add("LOGO", 115);
				ws->set(Sprite::ANIMATIONTIME, 0.0);
				ws->set(Sprite::SCALEX, 1.6);
				ws->set(Sprite::SCALEY, 0.7);
				ws->set(Sprite::SCALEG, 0.6);
				ws->set(Sprite::LAYER, -1);
				ws->set(Sprite::LOOPING_IMAGES, false);
				ws->set(Sprite::USE_TINTED_DRAWING, true);

				actual_perc = 0.994;

				ws = Sprite::getOrCreate("MOUSE", true);
				ws->add("MOUSE");
				ws->set(Sprite::LAYER, 100);
				ws->set(Sprite::SCALEX, 0.60);
				ws->set(Sprite::SCALEG, 0.07);
				ws->set(Sprite::FOLLOWMOUSE, true);
				ws->set(Sprite::COLLIDE, true);

				actual_perc = 0.995;

				/* CREATING A TEXT...? */

				wt = Text::getOrCreate("BAR_00_TEXT", true);
				wt->setMainDisplay(&data->disp);
				wt->set(Text::SETSTRING, "< TEST 1 >");
				wt->set(Text::MODE, Text::ALIGN_CENTER);
				wt->set(Text::LAYER, 99);
				wt->set(Text::SETFOLLOW, "BAR_00");
				wt->set(Text::UPDATETIME, 0.02);
				wt->set(Text::SCALEG, 1.35);
				wt->set(Text::POSY, -0.053);
				wt->set(Text::POSX, 0.002);

				actual_perc = 0.9957;

				wt = Text::getOrCreate("BAR_01_TEXT", true);
				wt->set(Text::SETSTRING, "< TEST 2 >");
				wt->set(Text::MODE, Text::ALIGN_CENTER);
				wt->set(Text::LAYER, 99);
				wt->set(Text::SETFOLLOW, "BAR_01");
				wt->set(Text::UPDATETIME, 0.02);
				wt->set(Text::SCALEG, 1.35);
				wt->set(Text::POSY, -0.053);
				wt->set(Text::POSX, 0.002);

				actual_perc = 0.9963;

				wt = Text::getOrCreate("BAR_02_TEXT", true);
				wt->set(Text::SETSTRING, "< EXIT >");
				wt->set(Text::MODE, Text::ALIGN_CENTER);
				wt->set(Text::LAYER, 99);
				wt->set(Text::SETFOLLOW, "BAR_02");
				wt->set(Text::UPDATETIME, 1.0);
				wt->set(Text::SCALEG, 1.35);
				wt->set(Text::POSY, -0.053);
				wt->set(Text::POSX, 0.002);

				actual_perc = 0.9970;

				wt = Text::getOrCreate("FOLLOW_0", true);
				wt->set(Text::SETSTRING, "0 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
				wt->set(Text::MODE, Text::ALIGN_CENTER);
				wt->set(Text::LAYER, 2);
				wt->set(Text::SETFOLLOW, "DEFAULT");

				actual_perc = 0.9977;

				wt = Text::getOrCreate("FOLLOW_1", true);
				wt->set(Text::SETSTRING, "1 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
				wt->set(Text::MODE, Text::ALIGN_CENTER);
				wt->set(Text::LAYER, 2);
				wt->set(Text::SETFOLLOW, "DEFAULT2");

				actual_perc = 0.9983;

				wt = Text::getOrCreate("FOLLOW_2", true);
				wt->set(Text::SETSTRING, "2 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
				wt->set(Text::MODE, Text::ALIGN_CENTER);
				wt->set(Text::LAYER, 2);
				wt->set(Text::SETFOLLOW, "DEFAULT3");

				actual_perc = 0.9989;

				wt = Text::getOrCreate("OSD_0", true);
				wt->set(Text::SETSTRING, "%fps%/%tps% | CAM: %cam_x% : %cam_y% @ %cam_zoom% | %curr_string% | %mouse_x%x%mouse_y%");
				wt->set(Text::POSX, -1.0);
				wt->set(Text::POSY, -1.0);
				wt->set(Text::SCALEG, 0.8);
				wt->set(Text::MODE, Text::ALIGN_LEFT);
				wt->set(Text::LAYER, Defaults::default_font_foreground_layer);
				wt->set(Text::UPDATETIME, 0.20);
				wt->set(Text::AFFECTED_BY_CAM, false);

				actual_perc = 0.9995;

				wt = Text::getOrCreate("OSD_1", true);
				wt->set(Text::SETSTRING,
					"{COL=[%tps_col%/" + std::to_string((int)(1.0 / Defaults::collision_timer)) +
					"];FUNC=[%tps_funcs%/" + std::to_string((int)(1.0 / Defaults::functions_timer)) +
					"];SEC=[%tps_second%/" + std::to_string((int)(1.0 / Defaults::calcLoops_timer)) +
					"];POSUPD=[%tps_posupd%/" + std::to_string((int)(1.0 / Defaults::updatepos_timer)) + "]}");
				wt->set(Text::POSX, -1.0);
				wt->set(Text::POSY, -0.97);
				wt->set(Text::SCALEG, 0.7);
				wt->set(Text::MODE, Text::ALIGN_LEFT);
				wt->set(Text::LAYER, Defaults::default_font_foreground_layer);
				wt->set(Text::UPDATETIME, 0.50);
				wt->set(Text::AFFECTED_BY_CAM, false);

				actual_perc = 0.9997;

				wt = Text::getOrCreate("OSD_2", true);
				wt->set(Text::SETSTRING,
					"{IMG=%num_images%;SPR=%num_sprites%;TXT=%num_texts%;TCK=%num_tracks%;E=%num_entities%}");
				wt->set(Text::POSX, -1.0);
				wt->set(Text::POSY, -0.95);
				wt->set(Text::SCALEG, 0.7);
				wt->set(Text::MODE, Text::ALIGN_LEFT);
				wt->set(Text::LAYER, Defaults::default_font_foreground_layer);
				wt->set(Text::UPDATETIME, 1.0);
				wt->set(Text::AFFECTED_BY_CAM, false);

				actual_perc = 1.0;
			}

			const bool main::__internal_task_level_common()
			{
				data->img_data.draw(); // check unload of textures and stuff
				data->spr_data.draw(); // draw sprites
				data->txt_data.draw(); // update and draw texts

				if (!data->disp.flip()) return false; // flip

				__internal_check_hotkeys();

				return true;
			}

			void main::__internal_check_hotkeys()
			{
				// toggle fullscreen with F11
				if (data->bev.g().getKey(Events::KEY_F11, true)) data->disp.toggleFS();
				// toggle OSD
				if (data->bev.g().getKey(Events::KEY_F3, true)) {
					int a = 0;
					if (al_get_time() - lastF3switch >= Defaults::display_osd_toggle_min_time) {
						lastF3switch = al_get_time();

						Text::text* txt = Text::getOrCreate("OSD_2");
						bool allas;
						txt->get(Text::SHOW, allas);
						Text::getOrCreate("OSD_0")->set(Text::SHOW, !allas);
						Text::getOrCreate("OSD_1")->set(Text::SHOW, !allas);
						Text::getOrCreate("OSD_2")->set(Text::SHOW, !allas);
					}
				}
			}

			const initialization interpret_console_entry(const int argc, char *argv[])
			{
				initialization init;

				if (argc > 1)
				{
					for (int u = 0; u < argc; u++)
					{
						Safer::safe_string wrk = argv[u];

						if (u + 1 < argc) {
							if (wrk == "-w")
							{
								init.res[0] = atoi(argv[++u]);
								continue;
							}
							if (wrk == "-h")
							{
								init.res[1] = atoi(argv[++u]);
								continue;
							}
						}
						if (wrk == "-fullscreen")
						{
							init.mode = Defaults::default_display_settings;
							continue;
						}
						if (wrk == "-windowed")
						{
							init.mode = Defaults::default_windowed_display_settings;
							continue;
						}
						if (wrk == "-skipdownload")
						{
							init.skip_download_flag = true;
							continue;
						}
						if (wrk == "-fixedmemory")
						{
							init.fixed_memory_flag = true;
							continue;
						}
						if (wrk == "-dev")
						{
							init.enable_console = true;
							continue;
						}
						if (wrk == "-fulllog")
						{
							init.full_log = true;
							continue;
						}
						if (wrk == "-skipintro")
						{
							init.skip_intro_flag = true;
							continue;
						}
						if (wrk == "-skippreload")
						{
							init.preload_textures = false;
							continue;
						}
					}
				}
				return init;
			}
			void __xtract_dis(void* oxtract, Safer::safe_string interp, Safer::safe_string saveon)
			{
				if (oxtract) {
					Stacker::extractor* xtract = (Stacker::extractor*)oxtract;
					xtract->interpret(interp.g());
					xtract->extractAll(saveon.g());
				}
				else throw "__XTRACT_DIS - NULL POINTER AT START!";
			}
			void __load(main* m)
			{
				if (m) m->__internal_thr_once_load();
			}
			void __random_map_load(Map::map* map)
			{
				if (!map) throw "__RANDOM_MAP_LOAD - NULL POINTER AT START!";
				int temprand;

				srand(time(0));
				while ((temprand = rand()) == 0);
				map->setSeed(temprand);
				map->start_cpu_thr();
			}
			Safer::safe_string interpret_color(const colors c)
			{
				switch (c){
				case WHITE:
					return "WHITE";
				case BLACK:
					return "BLACK";
				case RED:
					return "RED";
				case GREEN:
					return "GREEN";
				case BLUE:
					return "BLUE";
				case CYAN:
					return "CYAN";
				case MAGENTA:
					return "MAGENTA";
				case YELLOW:
					return "YELLOW";
				}
				return "UNDEF";
			}

			ALLEGRO_COLOR interpret_to_color(const colors c)
			{
				switch (c) {
				case WHITE:
					return al_map_rgb(255, 255, 255);
				case BLACK:
					return al_map_rgb(0, 0, 0);
				case RED:
					return al_map_rgb(255, 0, 0);
				case GREEN:
					return al_map_rgb(0, 255, 0);
				case BLUE:
					return al_map_rgb(32, 164, 255);
				case CYAN:
					return al_map_rgb(35, 230, 255);
				case MAGENTA:
					return al_map_rgb(204, 0, 255);
				case YELLOW:
					return al_map_rgb(255, 209, 0);
				}
				return al_map_rgb(0, 0, 0);
			}
		}
	}
}