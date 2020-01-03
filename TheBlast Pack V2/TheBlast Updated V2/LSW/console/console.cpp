#include "console.h"

namespace LSW {
	namespace v4 {

		void Console::__l_thr_md()
		{
			try {
				Database conf;
				gfile logg;

				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing..." << L::ELF;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				size_t last_loop_had_error = 0;

				logg << L::SLF << fsr(__FUNCSIG__) << "Creating display..." << L::ELF;

				md = new Display();
				Sprites sprites;
				Texts texts;

				al_set_target_backbuffer(md->getDisplay());

				al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR/* | ALLEGRO_MIPMAP*/); // MIPMAP not working well? lmao
				//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGBA_8888);

				thread_maindisplay.thread_arguments = new __display_routines();
				thread_maindisplay.thread_arguments->insert(al_get_display_event_source(md->getDisplay()));
				thread_maindisplay.thread_arguments->insert(&evsrc);
				thread_maindisplay.thread_arguments->insert(logg.getEvent());


				// verify if there's something to load before anything
				__l_thr_md_run(Assistance::io__threads_taskid::START);


				thread_maindisplay.thread_arguments->start();

				{
					ALLEGRO_EVENT ev;
					ev.type = +Assistance::ro__my_events::THRKBM_DISPLAY_SIZE;
					ev.user.data1 = al_get_display_width(md->getDisplay());
					ev.user.data2 = al_get_display_height(md->getDisplay());
					al_emit_user_event(&evsrc, &ev, NULL);
				}

				logg << L::SLF << fsr(__FUNCSIG__) << "Creating local stuff..." << L::ELF;


				Text* const mtt = texts.create("lastlogtext");
				mtt->set(Assistance::io__text_string::FONT, "DEFAULT");
				mtt->set(Assistance::io__text_string::STRING, "Syncronization in progress... (LOG)");
				mtt->set(Assistance::io__text_string::ID, "lastlogtext");
				mtt->set(Assistance::io__text_boolean::SHOW, true);
				mtt->set(Assistance::io__text_double::SCALEG, 0.03);
				mtt->set(Assistance::io__text_double::POSY, 0.965);
				mtt->set(Assistance::io__text_double::POSX, -1.0);
				mtt->set(Assistance::io__text_integer::MODE, +Assistance::io__alignment_text::ALIGN_LEFT);
				mtt->set(Assistance::io__text_double::UPDATETIME, 1.0 / 5);
				mtt->set(Assistance::io__text_boolean::AFFECTED_BY_CAM, false);
				mtt->set(Assistance::io__text_integer::LAYER, -10);

				Text* const osd = texts.create("osd_stuff0");
				osd->set(Assistance::io__text_string::FONT, "DEFAULT");
				osd->set(Assistance::io__text_string::STRING, "FPS:%fps%,TPS:%tps%,UPS:%ups% | M:%mouse_x%,%mouse_y% | CAM: %cam_x%,%cam_y%@~%cam_zoom_combined% | DISPLAY: %curr_res_x%:%curr_res_y%@%curr_refresh_rate% | I:%num_images%,S:%num_sprites%,T:%num_texts%,A:%num_tracks% | STR: %curr_string%[%last_string%]");
				osd->set(Assistance::io__text_string::ID, "osd_stuff0");
				osd->set(Assistance::io__text_boolean::SHOW, true);
				osd->set(Assistance::io__text_double::SCALEG, 0.025);
				osd->set(Assistance::io__text_double::POSY, 0.935);
				osd->set(Assistance::io__text_double::POSX, -1.0);
				osd->set(Assistance::io__text_integer::MODE, +Assistance::io__alignment_text::ALIGN_LEFT);
				osd->set(Assistance::io__text_double::UPDATETIME, 1.0 / 2);
				osd->set(Assistance::io__text_boolean::AFFECTED_BY_CAM, false);
				osd->set(Assistance::io__text_integer::LAYER, -10);

				std::string mtt_s = "No new information";



				logg << L::SLF << fsr(__FUNCSIG__) << "In the loop!" << L::ELF;
				thread_maindisplay.am_i_running = true;

				for (bool localb = true; localb;)
				{
					while (thread_maindisplay.pause_thread) Sleep(10);

					if (thread_maindisplay.thread_arguments->hasEvent()) {

						if (thread_maindisplay.thread_arguments->isThisThis(+Assistance::ro__thread_display_routines_timers::FUNCTIONALITY))
						{
							__l_thr_md_run(Assistance::io__threads_taskid::ONCE);
						}
						else if (thread_maindisplay.thread_arguments->isThisThis(+Assistance::ro__thread_display_routines_timers::LOOPTRACK))
						{
							conf.set(Assistance::io__db_statistics_sizet::FRAMESPERSECOND, thread_maindisplay.thread_arguments->getNumCalls());
						}
						else if (thread_maindisplay.thread_arguments->isThisThis(+Assistance::ro__thread_display_routines_timers::CHECKKEEP))
						{
							localb = !thr_shared_arg.should_exit;
							//logg.debug("[THR_DRW] CHECKKEEP called\n");
						}
						else if (thread_maindisplay.thread_arguments->isThisThis(+Assistance::ro__thread_display_routines_timers::CHECKMEMORYBITMAP))
						{
							//logg.debug("[THR_DRW] CHECKMEMORYBITMAP called\n");
							al_convert_bitmaps();
						}
						else if (thread_maindisplay.thread_arguments->isThisThis(+Assistance::ro__thread_display_routines_timers::UPDATELOGONSCREEN))
						{
							mtt->set(Assistance::io__text_string::STRING, mtt_s);
						}


						else { // not a timer
							auto ev = thread_maindisplay.thread_arguments->getEventRaw();

							switch (ev.type) {
							case ALLEGRO_EVENT_DISPLAY_CLOSE:
								thr_shared_arg.should_exit = true;
								logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "DISPLAYCLOSE event got. Set to turn off soon." << L::EL;
								break;

							case +Assistance::ro__my_events::LOG_CLOUDLAUNCH_RAW:
							{
								mtt_s = (char*)ev.user.data1;
								ALLEGRO_COLOR* colp = (ALLEGRO_COLOR*)ev.user.data2;
								mtt->set(Assistance::io__text_color::COLOR, *colp);
							}
							break;

							case +Assistance::ro__my_events::THRDRW_GOT_FORCED_RESIZE:
								al_set_display_flag(md->getDisplay(), ALLEGRO_FULLSCREEN_WINDOW, (bool)ev.user.data1);
								// yes, merge
							case ALLEGRO_EVENT_DISPLAY_RESIZE:
								al_acknowledge_resize(md->getDisplay());

								gcam.apply();
								logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "DISPLAYRESIZE got, acknowledged, done." << L::EL;

								{
									ALLEGRO_EVENT ev;
									ev.type = +Assistance::ro__my_events::THRKBM_DISPLAY_SIZE;
									ev.user.data1 = al_get_display_width(md->getDisplay());
									ev.user.data2 = al_get_display_height(md->getDisplay());
									al_emit_user_event(&evsrc, &ev, NULL);
								}

								conf.set(Assistance::io__conf_integer::SCREEN_X, al_get_display_width(md->getDisplay()));
								conf.set(Assistance::io__conf_integer::SCREEN_Y, al_get_display_height(md->getDisplay()));
								conf.set(Assistance::io__conf_integer::SCREEN_FLAGS, al_get_display_flags(md->getDisplay()));
								conf.flush();

								break;
							}
						}
					}

					// draw screen at least once

					md->clearTo(BLACK);


					while (!sprites.tryLock()) Sleep(10);

					// locally check if dealable error
					try {
						gcam.apply();
						camera_preset ww = gcam.get();

						for (auto& k : ww) {
							for (auto& i : sprites) i->self->draw(k);
							for (auto& i : texts)  i->self->draw(k);
						}

						last_loop_had_error = 0;
					}
					catch (Abort::abort err)
					{

						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got draw exception! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg.flush();

						alert("EXCEPTION!", "There was a exception!", ("This exception is skippable, but keep in mind that this is not good. Please report:\n\n" + ("Got draw exception! {" + err.from() + "," + err.details() + ",#" + std::to_string(err.getErrN()) + "}")).c_str());

						if (err.getErrN() == 1 && (last_loop_had_error < 10)) {
							//md->restart();
							al_convert_bitmaps();
							last_loop_had_error++;
						}
						//else throw err;
					}


					sprites.unlock();


					if (conf.isEq(Assistance::io__conf_boolean::WAS_OSD_ON, true)) matrix_draw_help();

					md->flip();

					//Sleep(50);
				}

				logg << L::SLF << fsr(__FUNCSIG__) << "Closing stuff!" << L::ELF;

				__l_thr_md_run(Assistance::io__threads_taskid::END);

				delete md;
				md = nullptr;


				// should not stop before the others (it is safer to close later)
				while (thread_collision.thread_arguments) Sleep(50);
				while (thread_kbmouse.thread_arguments) Sleep(50);

				delete thread_maindisplay.thread_arguments;

				thread_maindisplay.thread_arguments = nullptr;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				//textures.clear();

				thread_maindisplay.am_i_running = false;
				logg << L::SLF << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::ELF;
			}
			catch (Abort::abort a)
			{
				forceExit("Something went wrong at MDTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
		}

		void Console::__l_thr_cl()
		{
			try {
				gfile logg;
				Sprites sprites;
				Database conf;

				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing..." << L::ELF;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				size_t last_loop_had_error = 0;

				thread_collision.thread_arguments = new __collision_routines();
				thread_collision.thread_arguments->insert(&evsrc);

				// verify if there's something to load before anything
				__l_thr_cl_run(Assistance::io__threads_taskid::START);

				thread_collision.thread_arguments->start();


				logg << L::SLF << fsr(__FUNCSIG__) << "In the loop!" << L::ELF;

				thread_collision.am_i_running = true;

				for (bool localb = true; localb;)
				{
					while (thread_collision.pause_thread) Sleep(10);

					thread_collision.thread_arguments->hasEventWait();

					if (thread_collision.thread_arguments->isThisThis(+Assistance::ro__thread_collision_routines_timers::FUNCTIONALITY))
					{
						__l_thr_cl_run(Assistance::io__threads_taskid::ONCE);
					}
					else if (thread_collision.thread_arguments->isThisThis(+Assistance::ro__thread_collision_routines_timers::LOOPTRACK))
					{
						conf.set(Assistance::io__db_statistics_sizet::COLLISIONSPERSECOND, thread_collision.thread_arguments->getNumCalls());
					}
					else if (thread_collision.thread_arguments->isThisThis(+Assistance::ro__thread_collision_routines_timers::CHECKKEEP))
					{
						localb = !thr_shared_arg.should_exit;
						//printf_s("[THR_COL] CHECKKEEP called\n");
					}
					else if (thread_collision.thread_arguments->isThisThis(+Assistance::ro__thread_collision_routines_timers::COLLISIONWORK))
					{
						try {
							gcam.apply();
							camera_preset ww = gcam.get();
							//float rr = ww.get(Assistance::io__camera_float::ROTATION_RAD); // adjustment because canvas has been rotated so mouse will be at normal and stuff offset by this

							for (auto& i : sprites) i->self->clearUp(); // set as no collision

							for (auto& k : ww) {
								for (auto& i : sprites) {
									i->self->process(k, ww); // process positioning

									for (auto& j : sprites)
									{
										i->self->collideWith(k, j->self); // see collisions
									}
								}
							}

							for (auto& i : sprites) i->self->applyCollideData(ww); // apply collision consequences

							last_loop_had_error = 0;
						}
						catch (Abort::abort err)
						{
							logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got updating pos exception! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;

							if (err.getErrN() == 1 && (last_loop_had_error < 10)) {
								last_loop_had_error++;
							}
						}
					}
				}
				logg << L::SLF << fsr(__FUNCSIG__) << "Closing stuff!" << L::ELF;

				__l_thr_cl_run(Assistance::io__threads_taskid::END);

				delete thread_collision.thread_arguments;

				thread_collision.thread_arguments = nullptr;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				thread_collision.am_i_running = false;
				logg << L::SLF << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::ELF;
			}
			catch (Abort::abort a)
			{
				forceExit("Something went wrong at CLTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
		}

		void Console::__l_thr_kb()
		{
			try {
				gfile logg;
				Sprites sprites;
				Database conf;

				int display_x = 1280;
				int display_y = 720;
				bool isscreenfullscreen = false;

				{
					int __s;
					conf.get(Assistance::io__conf_integer::SCREEN_FLAGS, __s, 0);
					isscreenfullscreen = (__s & ALLEGRO_FULLSCREEN_WINDOW) || (__s & ALLEGRO_FULLSCREEN);
				}

				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing..." << L::ELF;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
				setlocale(LC_ALL, "");

				thread_kbmouse.thread_arguments = new __keyboardmouse_routines();
				thread_kbmouse.thread_arguments->insert(al_get_keyboard_event_source());
				thread_kbmouse.thread_arguments->insert(al_get_mouse_event_source());
				thread_kbmouse.thread_arguments->insert(&evsrc);

				// verify if there's something to load before anything
				__l_thr_kb_run(Assistance::io__threads_taskid::START);

				thread_kbmouse.thread_arguments->start();


				logg << L::SLF << fsr(__FUNCSIG__) << "In the loop!" << L::ELF;

				thread_kbmouse.am_i_running = true;

				for (bool localb = true; localb;)
				{
					while (thread_kbmouse.pause_thread) Sleep(10);

					thread_kbmouse.thread_arguments->hasEventWait();

					if (thread_kbmouse.thread_arguments->isThisThis(+Assistance::ro__thread_keyboardm_routines_timers::FUNCTIONALITY))
					{
						__l_thr_kb_run(Assistance::io__threads_taskid::ONCE);
					}
					else if (thread_kbmouse.thread_arguments->isThisThis(+Assistance::ro__thread_keyboardm_routines_timers::LOOPTRACK))
					{
						conf.set(Assistance::io__db_statistics_sizet::USEREVENTSPERSECOND, thread_kbmouse.thread_arguments->getNumCalls());
					}
					else if (thread_kbmouse.thread_arguments->isThisThis(+Assistance::ro__thread_keyboardm_routines_timers::CHECKKEEP))
					{
						localb = !thr_shared_arg.should_exit;
					}
					else if (thread_kbmouse.thread_arguments->isThisThis(+Assistance::ro__thread_keyboardm_routines_timers::UPDATEMOUSE))
					{
						float m[2];
						conf.get(Assistance::io__db_mouse_float::RAW_MOUSE_X, m[0]);
						conf.get(Assistance::io__db_mouse_float::RAW_MOUSE_Y, m[1]);

						// transform based on cam
						Camera gcam;
						camera_preset psf = gcam.get(); // copy latest

						ALLEGRO_TRANSFORM untransf = psf.quick();
						al_invert_transform(&untransf);
						al_transform_coordinates(&untransf, &m[0], &m[1]);

						conf.set(Assistance::io__db_mouse_float::MOUSE_X, m[0]);
						conf.set(Assistance::io__db_mouse_float::MOUSE_Y, m[1]);


						for (auto& i : sprites) {
							if (i->self->isEq(Assistance::io__sprite_boolean::FOLLOWMOUSE, true)) {
								i->self->set(Assistance::io__sprite_double::POSX, m[0]);
								i->self->set(Assistance::io__sprite_double::POSY, m[1]);
							}
						}
					}

					else { // not a timer ///DEBUG NOW
						auto ev = thread_kbmouse.thread_arguments->getEventRaw();

						if (ev.type == +Assistance::ro__my_events::THRKBM_DISPLAY_SIZE)
						{
							display_x = ev.user.data1;
							display_y = ev.user.data2;
						}
						if (ev.type == ALLEGRO_EVENT_KEY_DOWN) { // USE IN GAME
							conf.set(ev.keyboard.keycode, true);
						}
						if (ev.type == ALLEGRO_EVENT_KEY_UP) { // USE IN GAME
							conf.set(ev.keyboard.keycode, false);

							switch (ev.keyboard.keycode) {
							case ALLEGRO_KEY_F11:
							{
								ALLEGRO_EVENT ev;
								ev.type = +Assistance::ro__my_events::THRDRW_GOT_FORCED_RESIZE;
								ev.user.data1 = (isscreenfullscreen = !isscreenfullscreen);
								al_emit_user_event(&evsrc, &ev, NULL);
							}
							break;
							case ALLEGRO_KEY_F2:
							{
								md->print();
							}
							break;
							}
						}

						if (ev.type == ALLEGRO_EVENT_KEY_CHAR) { // keyboard input
							//const char* actch = al_keycode_to_name(ev.keyboard.keycode); // prints literally key name KEY26 A B C
							//if (ev.keyboard.unichar > 32) logg.debug("[THR_KBM] KEYCHAR= %d ~= %c", ev.keyboard.keycode, ev.keyboard.unichar);
							//else                          logg.debug("[THR_KBM] KEYCHAR= %d", ev.keyboard.keycode);
						}
						if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
							//conf.set(Assistance::io__db_mouse_float::MOUSE_X, (ev.mouse.x * 2.0f / display_x) - 1.0);
							//conf.set(Assistance::io__db_mouse_float::MOUSE_Y, (ev.mouse.y * 2.0f / display_y) - 1.0);

							conf.set(Assistance::io__db_mouse_float::RAW_MOUSE_X, ev.mouse.x);
							conf.set(Assistance::io__db_mouse_float::RAW_MOUSE_Y, ev.mouse.y);

						}
						if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
							conf.set(((Assistance::io__db_mouse_boolean)ev.mouse.button), true);
						}
						if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
							conf.set(((Assistance::io__db_mouse_boolean)ev.mouse.button), false);
						}
					}
				}
				logg << L::SLF << fsr(__FUNCSIG__) << "Closing stuff!" << L::ELF;

				__l_thr_cl_run(Assistance::io__threads_taskid::END);

				delete thread_kbmouse.thread_arguments;

				thread_kbmouse.thread_arguments = nullptr;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				thread_kbmouse.am_i_running = false;
				logg << L::SLF << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::ELF;
			}
			catch (Abort::abort a)
			{
				forceExit("Something went wrong at KBTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
		}

		void Console::__l_thr_fc()
		{
			try {
				gfile logg;
				//std::map<ALLEGRO_TIMER*, size_t> quickmap;

				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing..." << L::ELF;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
				setlocale(LC_ALL, "");

				thread_functional.thread_arguments = new __functional_routines();

				// verify if there's something to load before anything
				__l_thr_fc_run(Assistance::io__threads_taskid::START);

				thread_functional.thread_arguments->start();


				logg << L::SLF << fsr(__FUNCSIG__) << "In the loop!" << L::ELF;

				thread_functional.am_i_running = true;

				while (!thr_shared_arg.should_exit)
				{
					while (thread_functional.pause_thread) Sleep(10);

					thread_functional.thread_arguments->hasEventWait();

					if (thread_functional.thread_arguments->isThisThis(+Assistance::ro__thread_functional_routines_timers::FUNCTIONALITY_AND_CHECKNEWFUNCS))
					{
						__l_thr_kb_run(Assistance::io__threads_taskid::ONCE);


						for (size_t k = 0; k < func_list_super.size(); k++)
						{
							auto& vv = func_list_super[k];

							if (!vv.should_run) {
								thread_functional.thread_arguments->remove(al_get_timer_event_source(vv.the_timer));
								func_list_super.erase(k);
								if (k > 0) k--;
							}
							if (!vv.has_timer_set_on_queue) {
								thread_functional.thread_arguments->insert(al_get_timer_event_source(vv.the_timer));
								al_start_timer(vv.the_timer);

								logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Timed function P=" << cast_pointer(vv.the_timer) << ";ID=" << vv.id << " created" << L::EL;
								vv.has_timer_set_on_queue = true;
							}
						}

					}
					else { // TIMER EVENT (default)

						ALLEGRO_EVENT cat = thread_functional.thread_arguments->getEventRaw();

						try {
							if (cat.type == ALLEGRO_EVENT_TIMER)
							{
								//logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Got event to timer P=" << cast_pointer(cat.timer.source) << "." << L::EL;

								auto& a = func_list_super[cat.timer.source];

								/*for (auto& a : f_dis)
								{
									if (has_run_once = (a.the_timer == cat.timer.source)) {*/

								if (a.should_run && a.func) {
									if (++a.calls > 0) a.calls += a.func(); // run only if a.calls >= 0
								}
									/*}
								}*/
							}
						}
						catch (Abort::abort err)
						{
							logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got function exception! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						}
						catch (const std::out_of_range & oor)
						{
							logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal function exception! {" << oor.what() << "}" << L::ELF;
						}
					}
				}

				logg << L::SLF << fsr(__FUNCSIG__) << "Closing stuff!" << L::ELF;

				__l_thr_fc_run(Assistance::io__threads_taskid::END);
				//quickmap.clear();

				delete thread_functional.thread_arguments;

				thread_functional.thread_arguments = nullptr;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				thread_functional.am_i_running = false;
				logg << L::SLF << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::ELF;
			}
			catch (Abort::abort a)
			{
				forceExit("Something went wrong at FCTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
		}




		void Console::__l_thr_md_run(const Assistance::io__threads_taskid tid)
		{
			if (thread_maindisplay.functions[+tid]) {
				if (thread_maindisplay.has_called_once[+tid]) return;
				thread_maindisplay.functions[+tid]();
				Sleep(10);
			}
		}

		void Console::__l_thr_cl_run(const Assistance::io__threads_taskid tid)
		{
			if (thread_collision.functions[+tid]) {
				if (thread_collision.has_called_once[+tid]) return;
				thread_collision.functions[+tid]();
				Sleep(10);
			}
		}

		void Console::__l_thr_kb_run(const Assistance::io__threads_taskid tid)
		{
			if (thread_kbmouse.functions[+tid]) {
				if (thread_kbmouse.has_called_once[+tid]) return;
				thread_kbmouse.functions[+tid]();
				Sleep(10);
			}
		}

		void Console::__l_thr_fc_run(const Assistance::io__threads_taskid tid)
		{
			if (thread_functional.functions[+tid]) {
				if (thread_functional.has_called_once[+tid]) return;
				thread_functional.functions[+tid]();
				Sleep(10);
			}
		}

		Console::Console()
		{
			al_init_user_event_source(&evsrc);
		}

		Console::~Console()
		{
			stop();
			al_destroy_user_event_source(&evsrc);
		}

		void Console::start()
		{
			///launch([](void* v) {return; }, [](void* v) {return; }); // do nothing, just go

			thr_shared_arg.should_exit = false;

			// start threads
			thread_maindisplay.thr_itself = new std::thread([=] {__l_thr_md(); });
			thread_collision.thr_itself = new std::thread([=] {__l_thr_cl(); });
			thread_kbmouse.thr_itself = new std::thread([=] {__l_thr_kb(); });
			thread_functional.thr_itself = new std::thread([=] {__l_thr_fc(); });

			al_set_target_bitmap(NULL);
		}

		/*void Console::launch(const std::function <void(void*)> f_disp, const std::function <void(void*)> f_cols)
		{
			thr_shared_arg.should_exit = false;

			// functions
			thr_md_func = f_disp;
			thr_cl_func = f_cols;

			// start threads
			thr_md = new std::thread([=] {__l_thr_md(); });
			///thr_cl = new std::thread([=] {__l_thr_cl(); }); // not ready yet
		}*/

		void Console::stop()
		{
			thr_shared_arg.should_exit = true;

			while (thr_shared_arg.threadcount > 0) Sleep(50);

			if (thread_maindisplay.thr_itself) thread_maindisplay.thr_itself->join();
			if (thread_collision.thr_itself) thread_collision.thr_itself->join();
			if (thread_kbmouse.thr_itself) thread_kbmouse.thr_itself->join();
			if (thread_functional.thr_itself) thread_functional.thr_itself->join();

			thread_maindisplay.thr_itself = nullptr;
			thread_collision.thr_itself = nullptr;
			thread_kbmouse.thr_itself = nullptr;
			thread_functional.thr_itself = nullptr;
		}
		void Console::shouldStop()
		{
			thr_shared_arg.should_exit = true;
		}

		bool Console::isOpen()
		{
			return (md != nullptr && thread_maindisplay.am_i_running && thread_collision.am_i_running && thread_kbmouse.am_i_running && thread_collision.am_i_running);
		}

		bool Console::isRunning()
		{
			bool stillrunning = !thr_shared_arg.should_exit;
			if (!stillrunning) {
				stop();
			}
			return stillrunning;
		}

		void Console::resetSimpleOnceTask(const Assistance::io__thread_ids which)
		{
			setSimpleTask(which, Assistance::io__threads_taskid::ONCE, std::function<void(void)>());
		}

		void Console::setSimpleTask(const Assistance::io__thread_ids which, const Assistance::io__threads_taskid tid, std::function<void(void)> f)
		{
			switch (which) {
			case Assistance::io__thread_ids::DRAWING:
				if (tid != Assistance::io__threads_taskid::size) {
					if (f) thread_maindisplay.functions[+tid] = f;
					thread_maindisplay.has_called_once[+tid] = false;
				}
				break;
			case Assistance::io__thread_ids::COLLIDING:
				if (tid != Assistance::io__threads_taskid::size) {
					if (f) thread_collision.functions[+tid] = f;
					thread_collision.has_called_once[+tid] = false;
				}
				break;
			case Assistance::io__thread_ids::USERINPUT:
				if (tid != Assistance::io__threads_taskid::size) {
					if (f) thread_kbmouse.functions[+tid] = f;
					thread_kbmouse.has_called_once[+tid] = false;
				}
				break;
			case Assistance::io__thread_ids::FUNCTIONAL:
				if (tid != Assistance::io__threads_taskid::size) {
					if (f) thread_functional.functions[+tid] = f;
					thread_functional.has_called_once[+tid] = false;
				}
				break;
			}
		}
		void Console::unsetSimpleTask(const Assistance::io__thread_ids which, const Assistance::io__threads_taskid tid)
		{
			switch (which) {
			case Assistance::io__thread_ids::DRAWING:
				if (tid != Assistance::io__threads_taskid::size) thread_maindisplay.functions[+tid] = std::function<void(void)>();
				break;
			case Assistance::io__thread_ids::COLLIDING:
				if (tid != Assistance::io__threads_taskid::size) thread_collision.functions[+tid] = std::function<void(void)>();
				break;
			case Assistance::io__thread_ids::USERINPUT:
				if (tid != Assistance::io__threads_taskid::size) thread_kbmouse.functions[+tid] = std::function<void(void)>();
				break;
			case Assistance::io__thread_ids::FUNCTIONAL:
				if (tid != Assistance::io__threads_taskid::size) thread_functional.functions[+tid] = std::function<void(void)>();
				break;
			}
		}

		bool Console::hasTasked(const Assistance::io__thread_ids which, const Assistance::io__threads_taskid tid)
		{
			switch (which) {
			case Assistance::io__thread_ids::DRAWING:
				return thread_maindisplay.has_called_once[+tid];
			case Assistance::io__thread_ids::COLLIDING:
				return thread_collision.has_called_once[+tid];
			case Assistance::io__thread_ids::USERINPUT:
				return thread_kbmouse.has_called_once[+tid];
			case Assistance::io__thread_ids::FUNCTIONAL:
				return thread_functional.has_called_once[+tid];
			}
			return 0;
		}

		size_t Console::getCallsPerSecondOnThread(const Assistance::io__thread_ids o)
		{
			switch (o) {
			case Assistance::io__thread_ids::DRAWING:
				return thread_maindisplay.thread_arguments->getNumCalls();
			case Assistance::io__thread_ids::COLLIDING:
				return thread_collision.thread_arguments->getNumCalls();
			case Assistance::io__thread_ids::USERINPUT:
				return thread_kbmouse.thread_arguments->getNumCalls();
			case Assistance::io__thread_ids::FUNCTIONAL:
				return thread_functional.thread_arguments->getNumCalls();
			}
			return 0;
		}

		void Console::pauseThread(const Assistance::io__thread_ids o)
		{
			switch (o) {
			case Assistance::io__thread_ids::ALL:
				thread_maindisplay.pause_thread = thread_collision.pause_thread = thread_kbmouse.pause_thread = thread_functional.pause_thread = true;
				break;
			case Assistance::io__thread_ids::DRAWING:
				thread_maindisplay.pause_thread = true;
				break;
			case Assistance::io__thread_ids::COLLIDING:
				thread_collision.pause_thread = true;
				break;
			case Assistance::io__thread_ids::USERINPUT:
				thread_kbmouse.pause_thread = true;
				break;
			case Assistance::io__thread_ids::FUNCTIONAL:
				thread_functional.pause_thread = true;
				break;
			}
		}

		void Console::resumeThread(const Assistance::io__thread_ids o)
		{
			switch (o) {
			case Assistance::io__thread_ids::ALL:
				thread_maindisplay.pause_thread = thread_collision.pause_thread = thread_kbmouse.pause_thread = thread_functional.pause_thread = false;
				break;
			case Assistance::io__thread_ids::DRAWING:
				thread_maindisplay.pause_thread = false;
				break;
			case Assistance::io__thread_ids::COLLIDING:
				thread_collision.pause_thread = false;
				break;
			case Assistance::io__thread_ids::USERINPUT:
				thread_kbmouse.pause_thread = false;
				break;
			case Assistance::io__thread_ids::FUNCTIONAL:
				thread_functional.pause_thread = false;
				break;
			}
		}

		void Console::addCustomTask(std::function<int(void)> f, const int id, const double t)
		{
			if (!f) {
				throw Abort::abort(__FUNCSIG__, "Can't add funtion to Console because it's not set properly!");
				return;
			}

			ALLEGRO_TIMER* tt = al_create_timer(t);
			if (!tt) {
				throw Abort::abort(__FUNCSIG__, "Can't create timer!");
				return;
			}

			gfile logg;
			logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Timed function P=" << cast_pointer(tt) << ";ID=" << id << " created" << L::EL;

			__functional_functions& ff = func_list_super.create(tt, id);

			ff.the_timer = tt;
			ff.id = id;
			ff.calls = 0;
			ff.func = f;
			ff.has_timer_set_on_queue = false;
			ff.should_run = true;
			//f_dis.push_back(ff);
		}

		void Console::delayCustomTaskTicksBy(const int p, const LONGLONG tt)
		{
			func_list_super[p].calls = -tt;
		}

		void Console::removeCustomTask(const int p)
		{
			func_list_super[p].should_run = false;
		}



	}
}
