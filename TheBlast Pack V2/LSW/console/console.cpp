#include "console.h"

namespace LSW {
	namespace v4 {

		
		void Manager::__linkdebug_timing::start()
		{
			last_start = GetTickCount64();
		}

		void Manager::__linkdebug_timing::end()
		{
			auto a = GetTickCount64();
			last_difference = (unsigned)(last_start - a);
			if (average_difference == 0) average_difference = last_difference;
			else {
				average_difference = ((Constants::d_dbg_t_avg - 1.0) / Constants::d_dbg_t_avg) * average_difference + (1.0 / Constants::d_dbg_t_avg) * last_difference;
			}
		}

		unsigned Manager::__linkdebug_timing::getLastDifference()
		{
			return last_difference;
		}

		double Manager::__linkdebug_timing::getAverageDifference()
		{
			return average_difference;
		}


		void Manager::__l_thr_md()
		{
			gfile logg;

			try {
				Database conf;
				std::string& thrid = thread_maindisplay.threadid_str;

				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing..." << L::ELF;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				size_t last_loop_had_error = 0;

				logg << L::SLF << fsr(__FUNCSIG__) << "Creating display..." << L::ELF;

				md = new Display();
				md->setToCheckBufferResConfig(); // update from config
				Sprites sprites;
				Texts texts;
				bool was_sprites_locked = false;

				al_set_target_backbuffer(md->getDisplay());

				al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR/* | ALLEGRO_MIPMAP*/); // MIPMAP not working well? lmao
				//al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGBA_8888);

				thread_maindisplay.thread_arguments = new __display_routines();
				thread_maindisplay.thread_arguments->insert(al_get_display_event_source(md->getDisplay()));
				thread_maindisplay.thread_arguments->insert(&evsrc);
				thread_maindisplay.thread_arguments->insert(logg.getEvent());


				// verify if there's something to load before anything
				__l_thr_md_run(Constants::io__threads_taskid::START);


				thread_maindisplay.thread_arguments->start();

				{
					ALLEGRO_EVENT evv;
					evv.type = +Constants::ro__my_events::CUSTOM_EVENT_SHARING_NEW_DISPLAY_SIZE;
					evv.user.data1 = al_get_display_width(md->getDisplay());
					evv.user.data2 = al_get_display_height(md->getDisplay());
					al_emit_user_event(&evsrc, &evv, NULL);
				}

				logg << L::SLF << fsr(__FUNCSIG__) << "Creating local stuff..." << L::ELF;


				Text* const mtt = texts.create("osd_stuff_lastlogtext");
				{
					bool is_osd_enabled = true;
					conf.get(Constants::io__conf_boolean::WAS_OSD_ON, is_osd_enabled);

					mtt->set(Constants::io__text_string::FONT, "DEFAULT");
					mtt->set(Constants::io__text_string::STRING, "Syncronization in progress... (LOG)");
					mtt->set(Constants::io__text_string::ID, "osd_stuff_lastlogtext");
					mtt->set(Constants::io__text_boolean::SHOW, is_osd_enabled);
					mtt->set(Constants::io__text_double::SCALEG, 0.03);
					mtt->set(Constants::io__text_double::SCALEX, 0.55);
					mtt->set(Constants::io__text_double::POSY, 0.965); // 0.935
					mtt->set(Constants::io__text_double::POSX, -1.0);
					mtt->set(Constants::io__text_integer::MODE, +Constants::io__text_alignment::ALIGN_LEFT);
					mtt->set(Constants::io__text_double::UPDATETIME, 1.0 / 5);
					mtt->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
					mtt->set(Constants::io__text_integer::LAYER, 100);
				}

				std::string mtt_s = "No new information";



				logg << L::SLF << fsr(__FUNCSIG__) << "In the loop!" << L::ELF;
				thread_maindisplay.am_i_running = true;

				while (!thread_collision.am_i_running) Sleep(20);
				while (!thread_kbmouse.am_i_running) Sleep(20);
				while (!thread_functional.am_i_running) Sleep(20);

				while (!thr_shared_arg.should_exit)
				{
					try {
						conf.set(Constants::ro__db_statistics_double::INSTANT_FRAMESPERSECOND, thread_maindisplay.thread_arguments->getNumInstantSCallsDefault());

						while (thread_maindisplay.pause_thread) {
							Sleep(10);
							thread_maindisplay.tasking = false;
						}
						thread_maindisplay.tasking = true;

						if (thread_maindisplay.thread_arguments->hasEvent()) {

							if (thread_maindisplay.thread_arguments->isThisThis(+Constants::ro__thread_display_routines_timers::FUNCTIONALITY_ONCE))
							{
								__l_thr_md_run(Constants::io__threads_taskid::ONCE);
							}
							else if (thread_maindisplay.thread_arguments->isThisThis(+Constants::ro__thread_display_routines_timers::FUNCTIONALITY_LOOP))
							{
								__l_thr_md_run(Constants::io__threads_taskid::LOOP);
							}
							else if (thread_maindisplay.thread_arguments->isThisThis(+Constants::ro__thread_display_routines_timers::LOOPTRACK))
							{
								conf.set(Constants::ro__db_statistics_sizet::FRAMESPERSECOND, thread_maindisplay.thread_arguments->getNumCallsDefault());
							}
							else if (thread_maindisplay.thread_arguments->isThisThis(+Constants::ro__thread_display_routines_timers::CHECKMEMORYBITMAP))
							{
								//logg.debug("[THR_DRW] CHECKMEMORYBITMAP called\n");
								al_convert_bitmaps();
							}
							else if (thread_maindisplay.thread_arguments->isThisThis(+Constants::ro__thread_display_routines_timers::UPDATELOGONSCREEN))
							{
								mtt->set(Constants::io__text_string::STRING, mtt_s);
							}


							else { // not a timer
								auto ev = thread_maindisplay.thread_arguments->getEventRaw();

								switch (ev.type) {
								case ALLEGRO_EVENT_DISPLAY_CLOSE:
									if (ev.display.source == md->getDisplay()) {
										thr_shared_arg.should_exit = true;
										logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "DISPLAYCLOSE event got. Set to turn off soon." << L::EL;
									}
									break;

								case +Constants::ro__my_events::CUSTOM_EVENT_LOG_STRING:
								{
									mtt_s = (char*)ev.user.data1;
									ALLEGRO_COLOR* colp = (ALLEGRO_COLOR*)ev.user.data2;
									mtt->set(Constants::io__text_color::COLOR, *colp);
								}
								break;

								case +Constants::ro__my_events::CUSTOM_EVENT_EXTERNAL_EXIT_CALL:
									thr_shared_arg.should_exit = true;
									break;

								case +Constants::ro__my_events::CUSTOM_EVENT_CALL_FULLSCREEN:

									al_set_display_flag(md->getDisplay(), ALLEGRO_FULLSCREEN_WINDOW, (bool)ev.user.data1);
									ev.display.source = md->getDisplay();

									// yes, merge
								case ALLEGRO_EVENT_DISPLAY_RESIZE:
									if (ev.display.source == md->getDisplay()) {
										//al_acknowledge_resize(md->getDisplay());
										md->acknowledgeResize();

										gcam.apply();
										logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "DISPLAYRESIZE got, acknowledged, done." << L::EL;

										{
											ALLEGRO_EVENT evv;
											evv.type = +Constants::ro__my_events::CUSTOM_EVENT_SHARING_NEW_DISPLAY_SIZE;
											evv.user.data1 = al_get_display_width(md->getDisplay());
											evv.user.data2 = al_get_display_height(md->getDisplay());
											al_emit_user_event(&evsrc, &evv, NULL);
										}

										conf.set(Constants::io__conf_integer::SCREEN_X, al_get_display_width(md->getDisplay()));
										conf.set(Constants::io__conf_integer::SCREEN_Y, al_get_display_height(md->getDisplay()));
										conf.set(Constants::io__conf_integer::SCREEN_FLAGS, al_get_display_flags(md->getDisplay()));
										conf.flush();

										for (auto& i : sprites) {
											i->self->set(Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY, al_get_time() + 1.5);
										}
									}
									break;
								}
							}
						}

						// draw screen at least once

						md->clearTo(BLACK);

						while (!sprites.tryLock()) Sleep(10);
						was_sprites_locked = true;

						gcam.apply();
						camera_preset ww = gcam.get();

						for (auto& k : ww) {
							for (auto& i : sprites) {
								if (!i->enabled) continue;
								i->self->draw(k);
							}
							for (auto& i : texts) {
								if (!i->enabled) continue;
								i->self->draw(k);
							}
						}

						last_loop_had_error = 0;

						sprites.unlock();
						was_sprites_locked = false;


						if (conf.isEq(Constants::io__conf_boolean::ULTRADEBUG, true)) matrix_draw_help();

						md->flip();

						//Sleep(50);
					}
					catch (Abort::warn err)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got warn at " << thrid << "! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg.flush();
						if (was_sprites_locked) sprites.unlock();
					}
					catch (Abort::abort err)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got exception at " << thrid << "! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg.flush();
						if (was_sprites_locked) sprites.unlock();
					}
					catch (const std::out_of_range & oor)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal exception at " << thrid << "! {" << oor.what() << "}" << L::ELF;
						logg.flush();
						pauseThread();
						for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::DRAWING) && al_get_time() - t < 3.0;) Sleep(10);
						askForceExit("ERROR!", ("There was an error at " + thrid + " thread!").c_str(), "You could try to continue if you really want to, but please, report this error! (share the log file located at %appdata%/Lohk's Studios/TheBlast/logs)");
						resumeThread();
						if (was_sprites_locked) sprites.unlock();
					}
					catch (...)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal UNKNOWN exception at " << thrid << "!" << L::ELF;
						logg.flush();
						pauseThread();
						for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::DRAWING) && al_get_time() - t < 3.0;) Sleep(10);
						askForceExit("FATAL ERROR!", ("There was a fatal error (maybe fixable) at " + thrid + " thread!").c_str(), "You could try to continue if you really want to, but please, report this error! (share the log file located at %appdata%/Lohk's Studios/TheBlast/logs)");
						resumeThread();
						if (was_sprites_locked) sprites.unlock();
					}
				}

				logg << L::SLF << fsr(__FUNCSIG__) << "Closing stuff!" << L::ELF;

				__l_thr_md_run(Constants::io__threads_taskid::END);

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				thread_maindisplay.am_i_running = false;

				while (someoneIsRunning()) Sleep(20);

				delete md;
				md = nullptr;

				delete thread_maindisplay.thread_arguments;
				thread_maindisplay.thread_arguments = nullptr;
				logg << L::SLF << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::ELF;
			}
			catch (Abort::warn a)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::DRAWING) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went bad at MDTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
			catch (Abort::abort a)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::DRAWING) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went wrong at MDTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
			catch (...)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::DRAWING) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went wrong at MDTHR!", "There was a unknown error! What a sad thing to happen!");
			}
		}

		void Manager::__l_thr_cl()
		{
			gfile logg;

			try {
				Sprites sprites;
				Database conf;
				std::string& thrid = thread_collision.threadid_str;

				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing..." << L::ELF;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				size_t last_loop_had_error = 0;

				thread_collision.thread_arguments = new __collision_routines();
				thread_collision.thread_arguments->insert(&evsrc);

				// verify if there's something to load before anything
				__l_thr_cl_run(Constants::io__threads_taskid::START);

				thread_collision.thread_arguments->start();


				logg << L::SLF << fsr(__FUNCSIG__) << "In the loop!" << L::ELF;

				thread_collision.am_i_running = true;

				while (!thr_shared_arg.should_exit)
				{
					try {
						conf.set(Constants::ro__db_statistics_double::INSTANT_COLLISIONSPERSECOND, thread_collision.thread_arguments->getNumInstantSCallsDefault());

						while (thread_collision.pause_thread) {
							Sleep(10);
							thread_collision.tasking = false;
						}
						thread_collision.tasking = true;

						thread_collision.thread_arguments->hasEventWait();

						if (thread_collision.thread_arguments->isThisThis(+Constants::ro__thread_collision_routines_timers::FUNCTIONALITY_ONCE))
						{
							__l_thr_cl_run(Constants::io__threads_taskid::ONCE);
						}
						else if (thread_collision.thread_arguments->isThisThis(+Constants::ro__thread_collision_routines_timers::FUNCTIONALITY_LOOP))
						{
							__l_thr_cl_run(Constants::io__threads_taskid::LOOP);
						}
						else if (thread_collision.thread_arguments->isThisThis(+Constants::ro__thread_collision_routines_timers::LOOPTRACK))
						{
							conf.set(Constants::ro__db_statistics_sizet::COLLISIONSPERSECOND, thread_collision.thread_arguments->getNumCallsDefault());
						}
						else if (thread_collision.thread_arguments->isThisThis(+Constants::ro__thread_collision_routines_timers::COLLISIONWORK))
						{
							gcam.apply();
							camera_preset ww = gcam.get();
							//float rr = ww.get(Constants::io__camera_double::ROTATION_RAD); // adjustment because canvas has been rotated so mouse will be at normal and stuff offset by this

							for (auto& i : sprites) {
								i->self->clearUp(); // set as no collision
							}

							for (auto& k : ww) {
								for (auto& i : sprites) {
									if (!i->enabled) continue;
									i->self->process(k, ww); // process positioning

									for (auto& j : sprites)
									{
										if (!j->enabled) continue;
										i->self->collideWith(k, j->self); // see collisions
									}
								}
							}

							for (auto& i : sprites) {
								if (!i->enabled) continue;
								i->self->applyCollideData(ww); // apply collision consequences
							}

							last_loop_had_error = 0;
						}

						else { // not a timer ///DEBUG NOW
							auto ev = thread_kbmouse.thread_arguments->getEventRaw();

							if (ev.type == +Constants::ro__my_events::CUSTOM_EVENT_EXTERNAL_EXIT_CALL) {
								thr_shared_arg.should_exit = true;
								continue;
							}
						}
					}
					catch (Abort::warn err)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got warn at " << thrid << "! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg.flush();
					}
					catch (Abort::abort err)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got exception at " << thrid << "! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg.flush();
					}
					catch (const std::out_of_range & oor)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal exception at " << thrid << "! {" << oor.what() << "}" << L::ELF;
						logg.flush();
						pauseThread();
						for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::COLLIDING) && al_get_time() - t < 3.0;) Sleep(10);
						askForceExit("ERROR!", ("There was an error at " + thrid + " thread!").c_str(), "You could try to continue if you really want to, but please, report this error! (share the log file located at %appdata%/Lohk's Studios/TheBlast/logs)");
						resumeThread();
					}
					catch (...)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal UNKNOWN exception at " << thrid << "!" << L::ELF;
						logg.flush();
						pauseThread();
						for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::COLLIDING) && al_get_time() - t < 3.0;) Sleep(10);
						askForceExit("FATAL ERROR!", ("There was a fatal error (maybe fixable) at " + thrid + " thread!").c_str(), "You could try to continue if you really want to, but please, report this error! (share the log file located at %appdata%/Lohk's Studios/TheBlast/logs)");
						resumeThread();
					}
				}
				logg << L::SLF << fsr(__FUNCSIG__) << "Closing stuff!" << L::ELF;

				__l_thr_cl_run(Constants::io__threads_taskid::END);

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				thread_collision.am_i_running = false;

				while (someoneIsRunning()) Sleep(20);

				delete thread_collision.thread_arguments;
				thread_collision.thread_arguments = nullptr;

				logg << L::SLF << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::ELF;
			}
			catch (Abort::warn a)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::COLLIDING) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went bad at CLTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
			catch (Abort::abort a)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::COLLIDING) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went wrong at CLTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
			catch (...)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::COLLIDING) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went wrong at CLTHR!", "There was a unknown error! What a sad thing to happen!");
			}
		}

		void Manager::__l_thr_kb()
		{
			gfile logg;

			try {
				Sprites sprites;
				Database conf;
				std::string& thrid = thread_kbmouse.threadid_str;

				int display_x = 1280;
				int display_y = 720;
				/*bool isscreenfullscreen = false;

				{
					int __s;
					conf.get(Constants::io__conf_integer::SCREEN_FLAGS, __s, 0);
					isscreenfullscreen = (__s & ALLEGRO_FULLSCREEN_WINDOW) || (__s & ALLEGRO_FULLSCREEN);
				}*/

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
				__l_thr_kb_run(Constants::io__threads_taskid::START);

				thread_kbmouse.thread_arguments->start();


				logg << L::SLF << fsr(__FUNCSIG__) << "In the loop!" << L::ELF;

				thread_kbmouse.am_i_running = true;

				while (!thr_shared_arg.should_exit)
				{
					try {
						conf.set(Constants::ro__db_statistics_double::INSTANT_USEREVENTSPERSECOND, thread_kbmouse.thread_arguments->getNumInstantSCallsDefault());

						while (thread_kbmouse.pause_thread) {
							Sleep(10);
							thread_kbmouse.tasking = false;
						}
						thread_kbmouse.tasking = true;

						thread_kbmouse.thread_arguments->hasEventWait();

						if (thread_kbmouse.thread_arguments->isThisThis(+Constants::ro__thread_keyboardm_routines_timers::FUNCTIONALITY_ONCE))
						{
							__l_thr_kb_run(Constants::io__threads_taskid::ONCE);
						}
						else if (thread_kbmouse.thread_arguments->isThisThis(+Constants::ro__thread_keyboardm_routines_timers::FUNCTIONALITY_LOOP))
						{
							__l_thr_kb_run(Constants::io__threads_taskid::LOOP);
						}
						else if (thread_kbmouse.thread_arguments->isThisThis(+Constants::ro__thread_keyboardm_routines_timers::LOOPTRACK))
						{
							conf.set(Constants::ro__db_statistics_sizet::USEREVENTSPERSECOND, thread_kbmouse.thread_arguments->getNumCallsDefault());
						}
						else if (thread_kbmouse.thread_arguments->isThisThis(+Constants::ro__thread_keyboardm_routines_timers::UPDATEMOUSE))
						{
							double md[2];
							conf.get(Constants::ro__db_mouse_double::RAW_MOUSE_X, md[0]);
							conf.get(Constants::ro__db_mouse_double::RAW_MOUSE_Y, md[1]);

							float m[2];
							for (short p = 0; p < 2; p++) m[p] = +md[p];

							// transform based on cam
							Camera gcam;
							camera_preset psf = gcam.get(); // copy latest

							ALLEGRO_TRANSFORM untransf = psf.quick();
							al_invert_transform(&untransf);
							al_transform_coordinates(&untransf, &m[0], &m[1]);

							for (short p = 0; p < 2; p++) {
								md[p] = +m[p];
							}

							conf.set(Constants::ro__db_mouse_double::MOUSE_X, md[0]);
							conf.set(Constants::ro__db_mouse_double::MOUSE_Y, md[1]);


							for (auto& i : sprites) {
								if (!i->enabled) continue;
								if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWMOUSE, true)) {
									i->self->set(Constants::io__sprite_double::POSX, md[0]);
									i->self->set(Constants::io__sprite_double::POSY, md[1]);
								}
							}
						}

						else { // not a timer ///DEBUG NOW
							auto ev = thread_kbmouse.thread_arguments->getEventRaw();

							if (ev.type == +Constants::ro__my_events::CUSTOM_EVENT_EXTERNAL_EXIT_CALL) {
								thr_shared_arg.should_exit = true;
								continue;
							}
							if (ev.type == +Constants::ro__my_events::CUSTOM_EVENT_SHARING_NEW_DISPLAY_SIZE)
							{
								display_x = ev.user.data1;
								display_y = ev.user.data2;
							}
							if (ev.type == +Constants::ro__my_events::CUSTOM_EVENT_DISPLAY_UPDATE_RESOLUTION_SCALE)
							{
								double scale = ev.user.data1 * 1.0 / 100;
								bool is_enabled = ev.user.data2;

								conf.set(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION, scale);
								conf.set(Constants::io__conf_boolean::DOUBLEBUFFERING, is_enabled);

								md->setToCheckBufferResConfig();
							}
							if (ev.type == +Constants::ro__my_events::CUSTOM_EVENT_DISPLAY_CHROMA_FX)
							{
								double fx_amount = ev.user.data1 * 1.0 / 1000;

								conf.set(Constants::io__conf_double::FX_AMOUNT, fx_amount);

								md->setToCheckBufferResConfig();
							}
							if (ev.type == ALLEGRO_EVENT_KEY_DOWN) { // USE IN GAME
								if (md && ev.keyboard.display == md->getDisplay()) {
									conf.set(ev.keyboard.keycode, true);
								}
							}
							if (ev.type == ALLEGRO_EVENT_KEY_UP) { // USE IN GAME
								if (md && ev.keyboard.display == md->getDisplay()) {
									conf.set(ev.keyboard.keycode, false);
								}
							}

							if (ev.type == ALLEGRO_EVENT_KEY_CHAR) { // keyboard input
								if (md && ev.keyboard.display == md->getDisplay()) {
									if (ev.keyboard.unichar >= 32)
									{
										char multibyte[5] = { 0, 0, 0, 0, 0 };

										al_utf8_encode(multibyte, ev.keyboard.unichar <= 32 ? ' ' : ev.keyboard.unichar);
										char v = (char)strlen(multibyte);
										if (v > 4) throw Abort::abort(__FUNCSIG__, "Got an exception on user input: invalid key code, couldn't translate to a valid string");
										conf.set(Constants::ro__db_thread_string::KEY_ADD_SET_LEN, v);

										for (auto& i : multibyte) {
											if (i) conf.set(Constants::ro__db_thread_string::KEY_ADD, i);
										}
									}
									else if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE)
									{
										conf.set(Constants::ro__db_thread_string::KEY_ERASE);
									}
									else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER || ev.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER)
									{
										conf.set(Constants::ro__db_thread_string::KEY_SET);
									}
								}
							}
							if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
								if (md && ev.keyboard.display == md->getDisplay()) {
									double prop = md->getAppliedProportion();
									conf.set(Constants::ro__db_mouse_double::RAW_MOUSE_X, ev.mouse.x * prop);
									conf.set(Constants::ro__db_mouse_double::RAW_MOUSE_Y, ev.mouse.y * prop);
								}

							}
							if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
								if (md && ev.keyboard.display == md->getDisplay()) {
									conf.set(((Constants::ro__db_mouse_boolean)ev.mouse.button), true);
								}
							}
							if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
								if (md && ev.keyboard.display == md->getDisplay()) {
									conf.set(((Constants::ro__db_mouse_boolean)ev.mouse.button), false);
								}
							}
						}
					}
					catch (Abort::warn err)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got warn at " << thrid << "! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg.flush();
					}
					catch (Abort::abort err)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got exception at " << thrid << "! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg.flush();
					}
					catch (const std::out_of_range & oor)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal exception at " << thrid << "! {" << oor.what() << "}" << L::ELF;
						logg.flush();
						pauseThread();
						for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::USERINPUT) && al_get_time() - t < 3.0;) Sleep(10);
						askForceExit("ERROR!", ("There was an error at " + thrid + " thread!").c_str(), "You could try to continue if you really want to, but please, report this error! (share the log file located at %appdata%/Lohk's Studios/TheBlast/logs)");
						resumeThread();
					}
					catch (...)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal UNKNOWN exception at " << thrid << "!" << L::ELF;
						logg.flush();
						pauseThread();
						for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::USERINPUT) && al_get_time() - t < 3.0;) Sleep(10);
						askForceExit("FATAL ERROR!", ("There was a fatal error (maybe fixable) at " + thrid + " thread!").c_str(), "You could try to continue if you really want to, but please, report this error! (share the log file located at %appdata%/Lohk's Studios/TheBlast/logs)");
						resumeThread();
					}
				}
				logg << L::SLF << fsr(__FUNCSIG__) << "Closing stuff!" << L::ELF;

				__l_thr_cl_run(Constants::io__threads_taskid::END);

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

			    thread_kbmouse.am_i_running = false;

				while (someoneIsRunning()) Sleep(20);			

				delete thread_kbmouse.thread_arguments;
				thread_kbmouse.thread_arguments = nullptr;

				logg << L::SLF << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::ELF;
			}
			catch (Abort::warn a)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::USERINPUT) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went bad at KBTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
			catch (Abort::abort a)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::USERINPUT) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went wrong at KBTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
			catch (...)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::USERINPUT) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went wrong at KBTHR!", "There was a unknown error! What a sad thing to happen!");
			}
		}

		void Manager::__l_thr_fc()
		{
			gfile logg;

			try {
				Database conf;
				std::string& thrid = thread_functional.threadid_str;
				//std::map<ALLEGRO_TIMER*, size_t> quickmap;

				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing..." << L::ELF;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
				setlocale(LC_ALL, "");

				thread_functional.thread_arguments = new __functional_routines();

				// verify if there's something to load before anything
				__l_thr_fc_run(Constants::io__threads_taskid::START);

				thread_functional.thread_arguments->start();


				logg << L::SLF << fsr(__FUNCSIG__) << "In the loop!" << L::ELF;

				thread_functional.am_i_running = true;

				while (!thr_shared_arg.should_exit)
				{
					try {
						conf.set(Constants::ro__db_statistics_double::INSTANT_ADVANCEDFUNCSPERSECOND, thread_functional.thread_arguments->getNumInstantSCallsDefault());

						while (thread_functional.pause_thread) {
							Sleep(10);
							thread_functional.tasking = false;
						}
						thread_functional.tasking = true;

						thread_functional.thread_arguments->hasEventWait();

						if (thread_functional.thread_arguments->isThisThis(+Constants::ro__thread_functional_routines_timers::LOOPTRACK))
						{
							conf.set(Constants::ro__db_statistics_sizet::ADVANCEDFUNCSPERSECOND, thread_functional.thread_arguments->getNumCallsDefault());
						}
						else if (thread_functional.thread_arguments->isThisThis(+Constants::ro__thread_functional_routines_timers::FUNCTIONALITY_LOOP))
						{
							__l_thr_kb_run(Constants::io__threads_taskid::LOOP);
						}
						else if (thread_functional.thread_arguments->isThisThis(+Constants::ro__thread_functional_routines_timers::FUNCTIONALITY_ONCE_AND_CHECKNEWFUNCS))
						{
							__l_thr_kb_run(Constants::io__threads_taskid::ONCE);

							for (size_t k = 0; k < func_list_super.size(); k++)
							{
								auto& vv = func_list_super[k];

								if (!vv.should_run) {
									func_list_timing.erase(vv.id);
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


							if (cat.type == +Constants::ro__my_events::CUSTOM_EVENT_EXTERNAL_EXIT_CALL) {
								thr_shared_arg.should_exit = true;
								continue;
							}

							if (cat.type == ALLEGRO_EVENT_TIMER)
							{
								//logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Got event to timer P=" << cast_pointer(cat.timer.source) << "." << L::EL;

								auto& a = func_list_super[cat.timer.source];
								

								if (a.should_run && a.func) {
									if (++a.calls > 0) {
										func_list_timing[a.id].start();

										auto v = a.func(); // run only if a.calls >= 0
										if (v < 0) a.calls = v;
										else a.calls += v;

										func_list_timing[a.id].end();
									}
								}
							}
						}
					}
					catch (Abort::warn err)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got warn at " << thrid << "! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Detailed function timing debug:" << L::ELF;
						for (auto& i : func_list_timing) {
							logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "- ID: " << i.first << "; AVG_T: " << i.second.getAverageDifference() << "; INST_T: " << i.second.getLastDifference() << " (ms)" << L::ELF;
						}
						logg.flush();
					}
					catch (Abort::abort err)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got exception at " << thrid << "! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Detailed function timing debug:" << L::ELF;
						for (auto& i : func_list_timing) {
							logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "- ID: " << i.first << "; AVG_T: " << i.second.getAverageDifference() << "; INST_T: " << i.second.getLastDifference() << " (ms)" << L::ELF;
						}
						logg.flush();
					}
					catch (const std::out_of_range & oor)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal exception at " << thrid << "! {" << oor.what() << "}" << L::ELF;
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Detailed function timing debug:" << L::ELF;
						for (auto& i : func_list_timing) {
							logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "- ID: " << i.first << "; AVG_T: " << i.second.getAverageDifference() << "; INST_T: " << i.second.getLastDifference() << " (ms)" << L::ELF;
						}
						logg.flush();
						pauseThread();
						for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::FUNCTIONAL) && al_get_time() - t < 3.0;) Sleep(10);
						askForceExit("ERROR!", ("There was a error at " + thrid + " thread!").c_str(), "You could try to continue if you really want to, but please, report this error! (share the log file located at %appdata%/Lohk's Studios/TheBlast/logs)");
						resumeThread();
					}
					catch (...)
					{
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Got fatal UNKNOWN exception at " << thrid << "!" << L::ELF;
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Detailed function timing debug:" << L::ELF;
						for (auto& i : func_list_timing) {
							logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "- ID: " << i.first << "; AVG_T: " << i.second.getAverageDifference() << "; INST_T: " << i.second.getLastDifference() << " (ms)" << L::ELF;
						}
						logg.flush();
						pauseThread();
						for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::FUNCTIONAL) && al_get_time() - t < 3.0;) Sleep(10);
						askForceExit("FATAL ERROR!", ("There was a fatal error (maybe fixable) at " + thrid + " thread!").c_str(), "You could try to continue if you really want to, but please, report this error! (share the log file located at %appdata%/Lohk's Studios/TheBlast/logs)");
						resumeThread();
					}

				} // endof while

				logg << L::SLF << fsr(__FUNCSIG__) << "Closing stuff!" << L::ELF;

				__l_thr_fc_run(Constants::io__threads_taskid::END);
				//quickmap.clear();

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				thread_functional.am_i_running = false;

				while (someoneIsRunning()) Sleep(20);

				delete thread_functional.thread_arguments;
				thread_functional.thread_arguments = nullptr;

				logg << L::SLF << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::ELF;
			}
			catch (Abort::warn a)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::FUNCTIONAL) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went bad at FCTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
			catch (Abort::abort a)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::FUNCTIONAL) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went wrong at FCTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
			}
			catch (...)
			{
				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();
				pauseThread();
				for (double t = al_get_time(); !hasThreadsButThisOnePaused(Constants::io__thread_ids::FUNCTIONAL) && al_get_time() - t < 3.0;) Sleep(10);
				forceExit("Something went wrong at FCTHR!", "There was a unknown error! What a sad thing to happen!");
			}
		}




		void Manager::__l_thr_md_run(const Constants::io__threads_taskid tid)
		{
			if (thread_maindisplay.functions[+tid]) {
				if (tid != Constants::io__threads_taskid::LOOP && thread_maindisplay.has_called_once[+tid]) return;
				thread_maindisplay.functions[+tid]();
				Sleep(10);
			}
		}

		void Manager::__l_thr_cl_run(const Constants::io__threads_taskid tid)
		{
			if (thread_collision.functions[+tid]) {
				if (tid != Constants::io__threads_taskid::LOOP && thread_collision.has_called_once[+tid]) return;
				thread_collision.functions[+tid]();
				Sleep(10);
			}
		}

		void Manager::__l_thr_kb_run(const Constants::io__threads_taskid tid)
		{
			if (thread_kbmouse.functions[+tid]) {
				if (tid != Constants::io__threads_taskid::LOOP && thread_kbmouse.has_called_once[+tid]) return;
				thread_kbmouse.functions[+tid]();
				Sleep(10);
			}
		}

		void Manager::__l_thr_fc_run(const Constants::io__threads_taskid tid)
		{
			if (thread_functional.functions[+tid]) {
				if (tid != Constants::io__threads_taskid::LOOP && thread_functional.has_called_once[+tid]) return;
				thread_functional.functions[+tid]();
				Sleep(10);
			}
		}

		bool Manager::someoneIsRunning()
		{
			return (thread_maindisplay.am_i_running || thread_collision.am_i_running || thread_kbmouse.am_i_running || thread_functional.am_i_running);
		}

		Manager::Manager()
		{
			al_init_user_event_source(&evsrc);
		}

		Manager::~Manager()
		{
			stop();
			al_destroy_user_event_source(&evsrc);
		}

		void Manager::start()
		{
			///launch([](void* v) {return; }, [](void* v) {return; }); // do nothing, just go

			thr_shared_arg.should_exit = false;

			thread_maindisplay.threadid_str = "Display Manager Thread";
			thread_collision.threadid_str = "Positioning and Collision Thread";
			thread_kbmouse.threadid_str = "Events and User Input Thread";
			thread_functional.threadid_str = "Advanced Functions Thread";

			// start threads
			thread_maindisplay.thr_itself = new std::thread([=] {__l_thr_md(); });
			thread_collision.thr_itself = new std::thread([=] {__l_thr_cl(); });
			thread_kbmouse.thr_itself = new std::thread([=] {__l_thr_kb(); });
			thread_functional.thr_itself = new std::thread([=] {__l_thr_fc(); });

			al_set_target_bitmap(NULL);
		}

		/*void Manager::launch(const std::function <void(void*)> f_disp, const std::function <void(void*)> f_cols)
		{
			thr_shared_arg.should_exit = false;

			// functions
			thr_md_func = f_disp;
			thr_cl_func = f_cols;

			// start threads
			thr_md = new std::thread([=] {__l_thr_md(); });
			///thr_cl = new std::thread([=] {__l_thr_cl(); }); // not ready yet
		}*/

		void Manager::stop()
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
		void Manager::shouldStop()
		{
			thr_shared_arg.should_exit = true;
		}

		bool Manager::isOpen()
		{
			return (thr_shared_arg.threadcount > 0 && md != nullptr && thread_maindisplay.am_i_running && thread_collision.am_i_running && thread_kbmouse.am_i_running && thread_collision.am_i_running);
		}

		bool Manager::isRunning()
		{
			bool stillrunning = !thr_shared_arg.should_exit && thr_shared_arg.threadcount > 0;
			if (!stillrunning) {
				stop();
			}
			return stillrunning;
		}

		void Manager::resetSimpleOnceTask(const Constants::io__thread_ids which)
		{
			setSimpleTask(which, Constants::io__threads_taskid::ONCE, std::function<void(void)>());
		}

		void Manager::setSimpleTask(const Constants::io__thread_ids which, const Constants::io__threads_taskid tid, std::function<void(void)> f)
		{
			switch (which) {
			case Constants::io__thread_ids::DRAWING:
				if (tid != Constants::io__threads_taskid::size) {
					if (f) thread_maindisplay.functions[+tid] = f;
					thread_maindisplay.has_called_once[+tid] = false;
				}
				break;
			case Constants::io__thread_ids::COLLIDING:
				if (tid != Constants::io__threads_taskid::size) {
					if (f) thread_collision.functions[+tid] = f;
					thread_collision.has_called_once[+tid] = false;
				}
				break;
			case Constants::io__thread_ids::USERINPUT:
				if (tid != Constants::io__threads_taskid::size) {
					if (f) thread_kbmouse.functions[+tid] = f;
					thread_kbmouse.has_called_once[+tid] = false;
				}
				break;
			case Constants::io__thread_ids::FUNCTIONAL:
				if (tid != Constants::io__threads_taskid::size) {
					if (f) thread_functional.functions[+tid] = f;
					thread_functional.has_called_once[+tid] = false;
				}
				break;
			}
		}
		void Manager::unsetSimpleTask(const Constants::io__thread_ids which, const Constants::io__threads_taskid tid)
		{
			switch (which) {
			case Constants::io__thread_ids::DRAWING:
				if (tid != Constants::io__threads_taskid::size) thread_maindisplay.functions[+tid] = std::function<void(void)>();
				break;
			case Constants::io__thread_ids::COLLIDING:
				if (tid != Constants::io__threads_taskid::size) thread_collision.functions[+tid] = std::function<void(void)>();
				break;
			case Constants::io__thread_ids::USERINPUT:
				if (tid != Constants::io__threads_taskid::size) thread_kbmouse.functions[+tid] = std::function<void(void)>();
				break;
			case Constants::io__thread_ids::FUNCTIONAL:
				if (tid != Constants::io__threads_taskid::size) thread_functional.functions[+tid] = std::function<void(void)>();
				break;
			}
		}

		bool Manager::hasTasked(const Constants::io__thread_ids which, const Constants::io__threads_taskid tid)
		{
			switch (which) {
			case Constants::io__thread_ids::DRAWING:
				return thread_maindisplay.has_called_once[+tid];
			case Constants::io__thread_ids::COLLIDING:
				return thread_collision.has_called_once[+tid];
			case Constants::io__thread_ids::USERINPUT:
				return thread_kbmouse.has_called_once[+tid];
			case Constants::io__thread_ids::FUNCTIONAL:
				return thread_functional.has_called_once[+tid];
			}
			return 0;
		}

		size_t Manager::getCallsPerSecondOnThread(const Constants::io__thread_ids o)
		{
			switch (o) {
			case Constants::io__thread_ids::DRAWING:
				return thread_maindisplay.thread_arguments->getNumFullCalls();
			case Constants::io__thread_ids::COLLIDING:
				return thread_collision.thread_arguments->getNumFullCalls();
			case Constants::io__thread_ids::USERINPUT:
				return thread_kbmouse.thread_arguments->getNumFullCalls();
			case Constants::io__thread_ids::FUNCTIONAL:
				return thread_functional.thread_arguments->getNumFullCalls();
			}
			return 0;
		}

		double Manager::getInstantSCallsOnThread(const Constants::io__thread_ids o)
		{
			switch (o) {
			case Constants::io__thread_ids::DRAWING:
				return thread_maindisplay.thread_arguments->getNumFullInstantSCalls();
			case Constants::io__thread_ids::COLLIDING:
				return thread_collision.thread_arguments->getNumFullInstantSCalls();
			case Constants::io__thread_ids::USERINPUT:
				return thread_kbmouse.thread_arguments->getNumFullInstantSCalls();
			case Constants::io__thread_ids::FUNCTIONAL:
				return thread_functional.thread_arguments->getNumFullInstantSCalls();
			}
			return 0;
		}

		void Manager::pauseThread(const Constants::io__thread_ids o)
		{
			switch (o) {
			case Constants::io__thread_ids::ALL:
				thread_maindisplay.pause_thread = thread_collision.pause_thread = thread_kbmouse.pause_thread = thread_functional.pause_thread = true;
				break;
			case Constants::io__thread_ids::DRAWING:
				thread_maindisplay.pause_thread = true;
				break;
			case Constants::io__thread_ids::COLLIDING:
				thread_collision.pause_thread = true;
				break;
			case Constants::io__thread_ids::USERINPUT:
				thread_kbmouse.pause_thread = true;
				break;
			case Constants::io__thread_ids::FUNCTIONAL:
				thread_functional.pause_thread = true;
				break;
			}
		}

		bool Manager::hasThreadsButThisOnePaused(const Constants::io__thread_ids o)
		{
			bool failed_pause = false;
			for (int a = 0; a < +Constants::io__thread_ids::__THREADS_COUNT; a++)
			{
				if (a != +o) failed_pause |= !hasThreadPaused((Constants::io__thread_ids)a);
			}
			return !failed_pause;
		}

		bool Manager::hasThreadPaused(const Constants::io__thread_ids o)
		{
			if (!isRunning()) throw Abort::abort(__FUNCSIG__, "Threads are dead, so pausing them is not possible (no sense)");

			switch (o) {
			case Constants::io__thread_ids::ALL:
				return !thread_maindisplay.tasking && !thread_collision.tasking && !thread_kbmouse.tasking && !thread_functional.tasking;
			case Constants::io__thread_ids::DRAWING:
				return !thread_maindisplay.tasking;
			case Constants::io__thread_ids::COLLIDING:
				return !thread_collision.tasking;
			case Constants::io__thread_ids::USERINPUT:
				return !thread_kbmouse.tasking;
			case Constants::io__thread_ids::FUNCTIONAL:
				return !thread_functional.tasking;
			}
			return false;
		}

		void Manager::resumeThread(const Constants::io__thread_ids o)
		{
			switch (o) {
			case Constants::io__thread_ids::ALL:
				thread_maindisplay.pause_thread = thread_collision.pause_thread = thread_kbmouse.pause_thread = thread_functional.pause_thread = false;
				break;
			case Constants::io__thread_ids::DRAWING:
				thread_maindisplay.pause_thread = false;
				break;
			case Constants::io__thread_ids::COLLIDING:
				thread_collision.pause_thread = false;
				break;
			case Constants::io__thread_ids::USERINPUT:
				thread_kbmouse.pause_thread = false;
				break;
			case Constants::io__thread_ids::FUNCTIONAL:
				thread_functional.pause_thread = false;
				break;
			}
		}

		void Manager::addCustomTask(std::function<int(void)> f, const int id, const double t)
		{
			if (!f) {
				throw Abort::abort(__FUNCSIG__, "Can't add funtion to Manager because it's not set properly!");
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

			func_list_timing[ff.id];

			//f_dis.push_back(ff);
		}

		void Manager::delayCustomTaskTicksBy(const int p, const LONGLONG tt)
		{
			func_list_super[p].calls = -tt;
		}

		void Manager::removeCustomTask(const int p)
		{
			func_list_super[p].should_run = false;
		}

		void Manager::sendEvent(const Constants::ro__my_events t, const intptr_t a, const intptr_t b, const intptr_t c, const intptr_t d)
		{
			ALLEGRO_EVENT ev;
			ev.type = +t;
			ev.user.data1 = a;
			ev.user.data2 = b;
			ev.user.data3 = c;
			ev.user.data4 = d;
			al_emit_user_event(&evsrc, &ev, NULL);
		}

		Display* const Manager::_display_handle()
		{
			return md;
		}
	}
}
