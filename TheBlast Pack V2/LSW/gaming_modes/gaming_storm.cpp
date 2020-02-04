#include "gaming_storm.h"


namespace LSW {
	namespace v4 {



		void GamingStorm::updateBlock(const POINT i, Sprite* s)
		{

			if (!wd) return;
			if (!s) {
				std::string thus = "GAMEBOX_" + std::to_string(i.y) + "_" + std::to_string(i.x);
				if (!sprites.get(thus, s)) {
					logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Failed to find " << thus << " in the wild (map)" << L::ELF;
					return;
				}
			}
			if (s) {
				Constants::io__sprite_collision_mode col_mode = Constants::io__sprite_collision_mode::COLLISION_TRANSPARENT;
				s->set(Constants::io__sprite_string_vector::REMOVEMULTIPLE, [&](const std::string s)->bool {return true; }); // clean up all
				std::string to_add;
				switch (wd->readPos((int)i.x, (int)i.y)) {
				case +blocks::BLOCK:
					col_mode = Constants::io__sprite_collision_mode::COLLISION_ANY_HOLD_STATIC;
					to_add = "BLOCK_00";	// BLOCK
					break;
				case +blocks::START_POINT:
				case +blocks::EMPTY:
					to_add = "BLOCK_01";	// DIRT
					break;
				case +blocks::LIFE:
					to_add = "BLOCK_02";	// LIFE
					break;
				case +blocks::EXIT:
				case +blocks::FAKE_EXIT:
					to_add = "BLOCK_03";	// ?
					break;
				case +blocks::FAILEDEXIT:
					to_add = "BLOCK_05";	// X
					break;
				case +blocks::BRICKING_0:
					col_mode = Constants::io__sprite_collision_mode::COLLISION_ANY_HOLD_STATIC;
					to_add = "BLOCK_06";	// 0
					break;
				case +blocks::BRICKING_1:
					col_mode = Constants::io__sprite_collision_mode::COLLISION_ANY_HOLD_STATIC;
					to_add = "BLOCK_07";	// 1
					break;
				case +blocks::BRICKING_2:
					col_mode = Constants::io__sprite_collision_mode::COLLISION_ANY_HOLD_STATIC;
					to_add = "BLOCK_08";	// 2
					break;
				case +blocks::BRICKING_3:
					col_mode = Constants::io__sprite_collision_mode::COLLISION_ANY_HOLD_STATIC;
					to_add = "BLOCK_09";	// 3
					break;
				}
				s->set(Constants::io__sprite_string::ADD, to_add);
				s->set(col_mode);

				refresing_m.lock();
				refreshing.push_back(s);
				refresing_m.unlock();
			}
		}

		void GamingStorm::regenMap()
		{
			/*int level_dif_max = level_count / 5;
			if (level_dif_max > 3) level_dif_max = 3;
			int mult = (level_dif_max ? (rand() % level_dif_max) : 0) + 1;*/

			int map_y = rand()%((level_count > 41 ? 41 : level_count) + 1) + 6;
			int map_x = 4 + map_y + rand() % (3);

			double mult = map_x * 1.0 / 16;
			double scale_y_map = map_x * 1.0 / map_y;

			consol->pauseThread();
			while (!consol->hasThreadsButThisOnePaused(Constants::io__thread_ids::FUNCTIONAL)) Sleep(10);

			if (wd) delete wd;

			wd = new World(map_x, map_y);

			wd->setDelta(current_delta = pow(2.7 / (level_count + 1), 2.0) * 50);

			this_is_the_player->set(Constants::io__sprite_double::SCALEG, 0.065 / +mult);
			this_is_the_player->set(Constants::io__sprite_double::ACCELERATION_X, Constants::intensity_player_run_max * 5.3 / map_x);
			this_is_the_player->set(Constants::io__sprite_double::ACCELERATION_Y, Constants::intensity_player_run_max * 2.7 / map_y);

			sprites.rename([](const std::string a)->bool {return a.find("GAMEBOX_") == 0; }, [](const std::string a)->std::string {return "GAMEBOX_DIEING_NOW_" + a.substr(8); });

			// moved to be loaded before, lmao

			double off_x, off_y;
			off_x = 0.5 * Constants::scale_g_map_default / +mult;
			off_y = 0.5 * scale_y_map * Constants::scale_g_map_default / +mult;

			POINT player_start_point = wd->generate(al_get_time() * 20.0);

			for (int y = 0; y < wd->getLen(1); y++) {
				for (int x = 0; x < wd->getLen(0); x++)
				{
					double pss[2];

					pss[0] = off_x + 2.0 * (1.0 * x / (wd->getLen(0)) - 0.5);
					pss[1] = off_y + 2.0 * (1.0 * y / (wd->getLen(1)) - 0.5);

					Sprite* s = sprites.create("GAMEBOX_DIEING_NOW_" + std::to_string(y) + "_" + std::to_string(x)); // recatch
					sprites.rename("GAMEBOX_DIEING_NOW_" + std::to_string(y) + "_" + std::to_string(x), "GAMEBOX_" + std::to_string(y) + "_" + std::to_string(x)); // rename

					updateBlock({ x,y }, s); // set texture and collision

					//if (wd->readPos((int)x, (int)y) == +blocks::BLOCK) {
					s->set(Constants::io__sprite_string::ID, "GAMEBOX_" + std::to_string(y) + "_" + std::to_string(x));
					s->set(Constants::io__sprite_boolean::DRAW, false); // no draw for now
					s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
					s->set(Constants::io__sprite_boolean::SHOWDOT, false);
					s->set(Constants::io__sprite_boolean::SHOWBOX, false);
					s->set(Constants::io__sprite_double::SCALEG, Constants::scale_g_map_default / +mult);
					s->set(Constants::io__sprite_double::SCALEY, scale_y_map);
					s->set(Constants::io__sprite_integer::LAYER, 0);
					s->set(Constants::io__sprite_double::POSX, pss[0]);
					s->set(Constants::io__sprite_double::POSY, pss[1]);


					//}
				}
			}
			{
				double pss[2];
				pss[0] = off_x + 2.0 * (1.0 * player_start_point.x / (wd->getLen(0)) - 0.5);
				pss[1] = off_y + 2.0 * (1.0 * player_start_point.y / (wd->getLen(1)) - 0.5);

				this_is_the_player->set(Constants::io__sprite_double::POSX, pss[0]);
				this_is_the_player->set(Constants::io__sprite_double::POSY, pss[1]);
			}

			{
				textures.customLoad("GAMEBOXMAIN_I", [](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_bitmap(1600, 900)); });

				Sprite* s = sprites.create("GAMEBOXMAIN");
				s->set(Constants::io__sprite_string::ID, "GAMEBOX_MAIN");
				s->set(Constants::io__sprite_boolean::DRAW, true); // no draw for now
				s->set(Constants::io__sprite_collision_mode::COLLISION_TRANSPARENT);
				s->set(Constants::io__sprite_boolean::SHOWDOT, false);
				s->set(Constants::io__sprite_boolean::SHOWBOX, false);
				s->set(Constants::io__sprite_double::SCALEG, 2.0);
				s->set(Constants::io__sprite_boolean::TIE_SIZE_TO_DISPLAY, true);
				s->set(Constants::io__sprite_string::ADD, "GAMEBOXMAIN_I");
				s->set(Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY, al_get_time() + 0.1);
				s->set(Constants::io__sprite_integer::LAYER, 0);
				s->hook(Constants::io__sprite_tie_func_to_state::WHEN_BITMAPS_RESIZED_AUTO, [&]() {
					need_refresh = true;
					});
				s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [&, s]() {
					if (need_refresh) {
						//s->set(Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY, al_get_time() + 0.1);
						refresing_m.lock();
						auto vct = refreshing; //sprites.getList([](const std::string a)->bool {return a.find("GAMEBOX_") == 0; });
						refreshing.clear();
						refresing_m.unlock();
						ALLEGRO_BITMAP* trg = nullptr;
						s->get(trg);
						if (!trg) {
							logg << L::SL << fsr(__FUNCSIG__, E::WARN) << "Cannot refresh map sprite! NULL!" << L::EL;
							return;
						}
						ALLEGRO_BITMAP* bff = al_get_target_bitmap();
						al_set_target_bitmap(trg);
						//al_clear_to_color(al_map_rgb(0, 0, 0)); // if needed (not anymore) to test only the changes in map
						

						for (auto& i : vct) {
							i->set(Constants::io__sprite_boolean::DRAW, true);
							i->draw(0);
							i->set(Constants::io__sprite_boolean::DRAW, false);
						}
						need_refresh = false;
						if (bff) al_set_target_bitmap(bff);
					}
					});
			}

			logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Cleaned up " << sprites.remove([](const std::string a)->bool {return a.find("GAMEBOX_DIEING_NOW_") == 0; }) << " sprites" << L::EL;
			wd->setTime();

			consol->resumeThread();
			has_map_gen = true;
		}

		void GamingStorm::refreshMap()
		{
			need_refresh = true;
		}

		void GamingStorm::threadTasking()
		{
			level_count = 0;

			refresing_m.lock();
			refreshing.clear();
			refresing_m.unlock();

			dynamic_speed_difft_m = 1.0;

			auto dynamic_speed_track = [&](bool ignore = false) {
				if (!ignore) {
					double __t = 0.2 * (al_get_time() - level_fx_text - 5.0) / (1e-50 + 15.0 * fabs(current_delta));
					if (__t < 1e-50) __t = 1e-50;
					double diff_t = 1.0 + pow(__t, 1.6);
					if (diff_t < 1.0) diff_t = 1.0;
					if (diff_t > 1.55) diff_t = 1.55;

					dynamic_speed_difft_m = (19.0 * dynamic_speed_difft_m + diff_t) / 20.0;

					playtrack->set(Constants::io__track_double::SPEED, dynamic_speed_difft_m);
				}
				else {
					dynamic_speed_difft_m = (19.0 * dynamic_speed_difft_m + 1.0) / 20.0;
					playtrack->set(Constants::io__track_double::SPEED, dynamic_speed_difft_m);
				}
			};


			ALLEGRO_EVENT_QUEUE* ev_qu = al_create_event_queue();
			ALLEGRO_TIMER* timer_sound = al_create_timer(1.0 / 18);
			ALLEGRO_TIMER* timer_think = al_create_timer(1.0 / 90);

			al_register_event_source(ev_qu, al_get_timer_event_source(timer_sound));
			al_register_event_source(ev_qu, al_get_timer_event_source(timer_think));

			al_start_timer(timer_sound);
			al_start_timer(timer_think);

			level_fx_text = al_get_time();

			while (keep_game_going && !has_player_died) {

				ALLEGRO_EVENT ev;
				al_wait_for_event(ev_qu, &ev);

				if (ev.type != ALLEGRO_EVENT_TIMER) continue;

				if (ev.timer.source == timer_sound) {
					dynamic_speed_track(is_paused);
				}
				if (ev.timer.source == timer_think) {
					if (!has_map_gen)
					{
						consol->pauseThread();
						while (!consol->hasThreadsButThisOnePaused(Constants::io__thread_ids::FUNCTIONAL)) Sleep(10);

						level_fx_text = al_get_time();

						this_is_the_player->set(Constants::io__entity_double::HEALTH, 1.0);
						level_text = texts.create("GAMEBOX_TEXT");
						level_text->set(Constants::io__text_string::FONT, "DEFAULT");
						level_text->set(Constants::io__text_string::STRING, "LEVEL UNKNOWN");
						level_text->set(Constants::io__text_string::ID, "GAMEBOX_TEXT");
						level_text->set(Constants::io__text_boolean::SHOW, true);
						level_text->set(Constants::io__text_double::SCALEG, 0.24);
						level_text->set(Constants::io__text_double::SCALEX, 0.5);
						level_text->set(Constants::io__text_integer::LAYER, 1);
						level_text->set(Constants::io__text_double::UPDATETIME, 1.0 / 5);
						level_text->set(Constants::io__text_color::COLOR, al_map_rgba_f(0.75, 0.75, 0.75, 0.80));
						level_text->hook(Constants::io__text_tie_func_to_state::ON_DRAW, [&](std::string& str)->void {
							double coef = 1.0 - 0.2 * (al_get_time() - (level_fx_text + 3.0));
							if (coef > 1.0) coef = 1.0;
							if (coef < 0.0) coef = 0.0;
							level_text->set(Constants::io__text_color::COLOR, al_map_rgba_f(0.75 * pow(coef, 3.0), 0.75 * pow(coef, 3.0), 0.75 * pow(coef, 3.0), 0.80 * pow(coef, 3.0)));
							level_text->set(Constants::io__text_double::SCALEX, 0.5 / (0.5 + 0.5 * pow(coef, 1.5)));
							level_text->set(Constants::io__text_double::SCALEY, 1.0 * (0.7 + 0.3 * pow(coef, 2.0)));
							str = "< LEVEL " + std::to_string(level_count + 1) + " >";
							});

						// even if regenMap() call and call again pauseThread and resumeThread(), it is safe to call again outside (just to guarantee no freezing)
						regenMap();
						this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true);

						{
							const auto supercheck_entity_on_block = [&]()->int {
								if (!wd) return 0;
								double act_pos[2];
								this_is_the_player->get(Constants::ro__sprite_target_double::TARG_POSX, act_pos[0]);
								this_is_the_player->get(Constants::ro__sprite_target_double::TARG_POSY, act_pos[1]);
								int xy[2];
								xy[0] = (((act_pos[0] /*+ szs[0]*/) * 1.0 / 2) + 0.5) * (wd->getLen(0));
								xy[1] = (((act_pos[1] /*+ szs[1]*/) * 1.0 / 2) + 0.5) * (wd->getLen(1));

								int blockid = wd->readPos(xy[0], xy[1]);
								switch (blockid) {
								case +blocks::LIFE:
								{
									double heal = 1.0;
									this_is_the_player->get(Constants::io__entity_double::HEALTH, heal);
									heal += Constants::heal_default;
									if (heal > Constants::max_health) heal = Constants::max_health;
									this_is_the_player->set(Constants::io__entity_double::HEALTH, heal);
								}
								break;
								case +blocks::EXIT:
									regenMap();
									level_count++;
									level_fx_text = al_get_time();
									break;
								case +blocks::FAKE_EXIT:
									wd->setPos(xy[0], xy[1], +blocks::FAILEDEXIT);
									updateBlock({ xy[0], xy[1] }, nullptr);
									refreshMap();
									break;
								case +blocks::FAILEDEXIT:
								{
									double heal = 1.0;
									this_is_the_player->get(Constants::io__entity_double::HEALTH, heal);
									heal += Constants::damage_default;
									if (heal < Constants::min_health) {
										has_player_died = true;
										return 0;
									}
									this_is_the_player->set(Constants::io__entity_double::HEALTH, heal);
								}
								break;
								}
								return 0;
							};

							consol->addCustomTask(supercheck_entity_on_block, task_id, 1.0 / 10);
						}

						consol->resumeThread();
					}
					else if (is_paused) {
						this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, false);

						if (!has_sleeping_sprites)
							sprites.setEnabled([](const std::string a)->bool {return a.find("GAMEBOX_") == 0; }, [](const std::string a)->bool {return false; });
						has_sleeping_sprites = true;

						wd->pauseCorrupt();
					}
					else {  // map is up n running
						if (has_sleeping_sprites) sprites.setEnabled([](const std::string a)->bool {return a.find("GAMEBOX_") == 0; }, [](const std::string a)->bool {return true; });
						has_sleeping_sprites = false;

						this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true);
						std::vector<POINT> points;
						if (level_count / 5) {
							if (wd->corruptWorld(points)) {
								logg << L::SLF << fsr(__FUNCSIG__) << "World task has run." << L::ELF;

								for (auto& i : points) {
									updateBlock(i, nullptr);
								}

								refreshMap();
							}
						}
						else {
							wd->pauseCorrupt();
						}
					}
				}
			}

			refresing_m.lock();
			refreshing.clear();
			refresing_m.unlock();

			al_destroy_event_queue(ev_qu);
			al_destroy_timer(timer_sound);
			al_destroy_timer(timer_think);

			if (has_map_gen) {
				consol->pauseThread();
				while (!consol->hasThreadPaused()) Sleep(10);

				consol->removeCustomTask(task_id);

				delete wd;
				wd = nullptr;
				sprites.remove([](const std::string a)->bool {return a.find("GAMEBOX_") == 0; });
				sprites.remove("GAMEBOXMAIN");
				textures.remove("GAMEBOXMAIN_I");
				texts.remove("GAMEBOX_TEXT");

				consol->resumeThread();
				has_map_gen = false;
			}
		}


		GamingStorm::GamingStorm(Manager* mng, Entity* plr, Track* trk, const int tid)
		{
			if (!mng) throw Abort::abort(__FUNCSIG__, "Invalid Manager! Cannot start without it!");
			if (!plr) throw Abort::abort(__FUNCSIG__, "Invalid Entity! Cannot start without it!");
			if (!trk) throw Abort::abort(__FUNCSIG__, "Invalid Track! Cannot start without it!");
			task_id = tid;
			consol = mng;
			this_is_the_player = plr;
			playtrack = trk;
		}

		GamingStorm::~GamingStorm()
		{
			stopNow();
		}

		void GamingStorm::startAutomatically()
		{
			if (!thr_taskin) {
				keep_game_going = true;
				thr_taskin = new std::thread([&]() {threadTasking(); });
			}
		}

		void GamingStorm::pauseTask(const bool is_it_paused)
		{
			is_paused = is_it_paused;
		}

		void GamingStorm::stopNow()
		{
			if (thr_taskin) {
				keep_game_going = false;
				thr_taskin->join();
				delete thr_taskin;
				thr_taskin = nullptr;
			}
		}

		bool GamingStorm::hasEnded()
		{
			return has_player_died;
		}

	}
}