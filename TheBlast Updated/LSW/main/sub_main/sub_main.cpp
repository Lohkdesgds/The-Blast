#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Main {

			_main_data* main::data = nullptr;

			main::main()
			{
				prepare_commom_templates;
				data = new _main_data();
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

				// toggle fullscreen with F11
				if (data->bev.g().getKey(Events::KEY_F11, true)) data->disp.toggleFS();

				return true;
			}

			void main::init()
			{
				if (!check("init")) return;

				{
					Log::gfile logg;
					logg.showOnConsole(data->setup.enable_console);
					logg.longLog(data->setup.full_log);
				}

				// create display
				if (data->setup.res[0] > 0 && data->setup.res[1] > 0) {
					data->disp.custom_launch(data->setup.res[0],data->setup.res[1], data->setup.mode);
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
						cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer
						cam_p.cam_settings.layers_enabled.push_back(-2);
						cam_p.cam_settings.offset_y = 2.7;
						cam_p.cam_settings.scale_g = 1.2;

						__set_camera_quick(cam_p);
					}
					{   /* -------------------- PRESET #-1 -------------------- */
						Main::__fast_cam_preset cam_p;
						cam_p.config_number = -1;
						cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer
						cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);
						cam_p.cam_settings.layers_enabled.push_back(-1);
						cam_p.cam_settings.scale_g = 1.15;

						__set_camera_quick(cam_p);
					}
					{   /* -------------------- PRESET #0 -------------------- */
						Main::__fast_cam_preset cam_p;
						cam_p.config_number = 0;
						cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer
						cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);
						cam_p.cam_settings.layers_enabled.push_back(-20);
						cam_p.cam_settings.layers_enabled.push_back(0);
						cam_p.cam_settings.layers_enabled.push_back(99);
						cam_p.cam_settings.layers_enabled.push_back(100);

						__set_camera_quick(cam_p);
					}
					{   /* -------------------- PRESET #2 -------------------- */
						Main::__fast_cam_preset cam_p;
						cam_p.config_number = 2;
						cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer
						cam_p.cam_settings.layers_enabled.push_back(100);
						cam_p.cam_settings.layers_enabled.push_back(0);
						cam_p.cam_settings.layers_enabled.push_back(1);
						cam_p.cam_settings.layers_enabled.push_back(2);
						cam_p.cam_settings.layers_enabled.push_back(3);
						cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);

						__set_camera_quick(cam_p);
					}
					{   /* -------------------- PRESET #3 -------------------- */
						Main::__fast_cam_preset cam_p;
						cam_p.config_number = 3;
						cam_p.cam_settings.layers_enabled.push_back(-9999); // Early Access layer
						cam_p.cam_settings.layers_enabled.push_back(Defaults::default_font_foreground_layer);
						cam_p.cam_settings.layers_enabled.push_back(Defaults::map_default_layer);
						cam_p.cam_settings.layers_enabled.push_back(Defaults::user_default_layer);
						cam_p.cam_settings.layers_enabled.push_back(Defaults::badboys_default_layer);

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
						lyr_p.layer_mode = Layer::STANDARD;
						lyr_p.each_layer_settings[2].collides_with.push_back(100);

						__set_layer_quick(lyr_p);
					}
					{   /* -------------------- PRESET #3 -------------------- */
						Main::__fast_lyr_preset lyr_p;
						lyr_p.config_number = 3;
						lyr_p.layer_mode = Layer::USEMAP;

						__set_layer_quick(lyr_p);
					}
				}

				data->hasInit = true;
			}

			void main::load()
			{
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

							title->set(Text::SETSTRING, std::string("Checking [") + std::to_string(nnow) + "/" + std::to_string(sizz) + "]: " + i->whoAmI().g());
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
						nulp->_setKeepOnMemory(data->setup.fixed_memory_flag);
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

					// SEARCHES
					mouse = Sprite::getOrCreate("MOUSE");
					b00 = Sprite::getOrCreate("BAR_00");
					b01 = Sprite::getOrCreate("BAR_01");
					b02 = Sprite::getOrCreate("BAR_02");
					t00 = Sound::getOrCreate("MUSIC_0");
					t01 = Sound::getOrCreate("MUSIC_1");
					t02 = Sound::getOrCreate("MUSIC_2");

					// SETTING VARIABLES VALUES
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

						if (data->bev.g().getKey(Events::MOUSE_0, false)) {
							if (b0) { // test 1
								data->playing = PLAYING;
								keep = false;
							}
							if (b1) { // test 2
								data->playing = PLAYING;
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
					Text::text* txt;
					double zoom = 1.0;
					double pos_x = 0.0, pos_y = 0.0;
					bool is_following_player = false;
					int temprand = rand();

					// SEARCHES
					map = nullptr;
					mouse = Sprite::getOrCreate("MOUSE");
					player = nullptr; // started from map
					t00 = Sound::getOrCreate("MUSIC_0");
					t01 = Sound::getOrCreate("MUSIC_1");
					t02 = Sound::getOrCreate("MUSIC_2");

					// CAMERA (needed before)
					data->disp.capFPS();
					__apply_cam_number(3);
					
					// SETTING VARIABLES VALUES
					if (!(map = new Map::map())) {
						throw "MAIN::PLAY - COULD NOT CREATE MAP POINTER!";
						return false;
					}
					map->launch_player("BAR_ON");
					map->launch_badboys("BAR_OFF", 4);
					map->setSeed(temprand);

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

					data->lyr.getOne(3).save_package(map);
					t00->set(Sound::PLAYING, false);
					t01->set(Sound::PLAYING, false);
					t02->set(Sound::PLAYING, true);

					// LAYER CONFIGURATION
					__apply_layer_number(3);

					// THE WHILE
					while (!data->bev.g().getKey(Events::KEY_ESCAPE)) {

						// TASKS
						map->checkDraw();
						map->checkPositionChange();

						if (map->hasReachedEnd()) {
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

							map->setCPULock(false);
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
				wi->load("mouse.png");

				actual_perc = 0.961;

				wi = Image::getOrCreate("BLAST_LOGO", true);
				wi->load("the_storm.png");

				actual_perc = 0.962;

				wi = Image::getOrCreate("BAR_ON", true);
				wi->load("bar_single_one_on.png");

				actual_perc = 0.963;

				wi = Image::getOrCreate("BAR_OFF", true);
				wi->load("bar_single_one.png");

				actual_perc = 0.964;

				wi = Image::getOrCreate("BG_INTRO", true);
				wi->load("background_gameplay_start.png");

				actual_perc = 0.965;

				std::thread thr4(Image::multipleLoad, Defaults::map_default_start, "anim/bloco", 10, Defaults::map_default_len_name_int, ".png", &o, false);
				while (o != 1.0 && data->disp.isOpen()) {
					actual_perc = 0.965 + o * 0.005;
				}

				thr4.join();

				actual_perc = 0.97;

				/* LOADING MUSICS...? */

				wm = Sound::getOrCreate("MUSIC_0", true);
				wm->load("musics/music_01.ogg");
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::LOOP);

				actual_perc = 0.972;

				wm = Sound::getOrCreate("MUSIC_1", true);
				wm->load("musics/music_02.ogg");
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::LOOP);

				actual_perc = 0.974;

				wm = Sound::getOrCreate("MUSIC_2", true);
				wm->load("musics/music_03.ogg");
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::LOOP);

				actual_perc = 0.976;

				wm = Sound::getOrCreate("JUMP_FX", true);
				wm->load("musics/jump_01.wav");
				wm->set(Sound::PLAYING, false);
				wm->set(Sound::PLAYMODE, Sound::ONCE);

				actual_perc = 0.978;

				wm = Sound::getOrCreate("WALK_FX", true);
				wm->load("musics/walk_01.wav");
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
				ws->set(Sprite::LAYER, 0);

				actual_perc = 0.981;

				ws = Sprite::getOrCreate("BAR_00", true);
				ws->add("BAR_OFF");
				ws->add("BAR_ON");
				ws->set(Sprite::POSX, 0.0);
				ws->set(Sprite::POSY, 0.4);
				ws->set(Sprite::SCALEG, 0.6);
				ws->set(Sprite::SCALEY, 0.22);
				ws->set(Sprite::COLLIDE, true);
				ws->set(Sprite::SHOWBOX, true);
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
				ws->set(Sprite::SHOWBOX, true);
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
				ws->set(Sprite::SHOWBOX, true);
				ws->set(Sprite::LAYER, 0);

				actual_perc = 0.984;

				ws = Sprite::getOrCreate("DEFAULT", true);
				ws->add("PAUSE", 29);
				ws->set(Sprite::ANIMATIONTIME, 24.0);
				ws->set(Sprite::POSX, 0.5);
				ws->set(Sprite::POSY, 0.5);
				ws->set(Sprite::SCALEG, 0.2);
				ws->set(Sprite::COLLIDE, true);
				ws->set(Sprite::AFFECTED_BY_COLLISION, true);
				ws->set(Sprite::SHOWBOX, true);
				ws->set(Sprite::LAYER, 2);

				actual_perc = 0.986;

				ws = Sprite::getOrCreate("DEFAULT3", true);
				ws->add("PAUSE", 29);
				ws->set(Sprite::ANIMATIONTIME, 40.0);
				ws->set(Sprite::SCALEG, 0.4);
				ws->set(Sprite::POSX, 0.0);
				ws->set(Sprite::POSY, 0.7);
				ws->set(Sprite::COLLIDE, true);
				ws->set(Sprite::AFFECTED_BY_COLLISION, true);
				ws->set(Sprite::SHOWBOX, true);
				ws->set(Sprite::LAYER, 2);

				actual_perc = 0.988;

				ws = Sprite::getOrCreate("DEFAULT2", true);
				ws->add("PAUSE", 29);
				ws->set(Sprite::ANIMATIONTIME, 30.0);
				ws->set(Sprite::LAYER, 100);
				ws->set(Sprite::SCALEG, 0.6);
				ws->set(Sprite::POSX, 0.0);
				ws->set(Sprite::POSY, 0.0);
				ws->set(Sprite::COLLIDE, true);
				ws->set(Sprite::AFFECTED_BY_COLLISION, true);
				ws->set(Sprite::SHOWBOX, true);
				ws->set(Sprite::LAYER, 2);

				actual_perc = 0.990;

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
				wt->set(Text::UPDATETIME, 1.0);
				wt->set(Text::SCALEG, 1.35);
				wt->set(Text::POSY, -0.053);
				wt->set(Text::POSX, 0.002);

				actual_perc = 0.9957;

				wt = Text::getOrCreate("BAR_01_TEXT", true);
				wt->set(Text::SETSTRING, "< TEST 2 >");
				wt->set(Text::MODE, Text::ALIGN_CENTER);
				wt->set(Text::LAYER, 99);
				wt->set(Text::SETFOLLOW, "BAR_01");
				wt->set(Text::UPDATETIME, 1.0);
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

				actual_perc = 1.0;
			}

			const bool main::__internal_task_level_common()
			{
				data->img_data.draw(); // check unload of textures and stuff
				data->spr_data.draw(); // draw sprites
				data->txt_data.draw(); // update and draw texts

				if (!data->disp.flip()) return false; // flip

				// toggle fullscreen with F11
				if (data->bev.g().getKey(Events::KEY_F11, true)) data->disp.toggleFS();

				return true;
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
							init.mode = ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE;
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
		}
	}
}