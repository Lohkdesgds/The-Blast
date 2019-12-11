#include "console.h"

namespace LSW {
	namespace v4 {

		void Console::__l_thr_md()
		{
			try {
				gfile logg;

				logg << L::SLL << fsr(__FUNCSIG__) << "Initializing..." << L::BLL;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				size_t last_loop_had_error = 0;

				logg << L::SLL << fsr(__FUNCSIG__) << "Creating display..." << L::BLL;

				md = new Display();
				Sprites sprites;
				Texts texts;
				//Textures textures;

				al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
				Text* const mtt = texts.create("lastlogtext");

				//gcam.apply();

				thr_md_arg = new __display_routines();
				thr_md_arg->insert(al_get_display_event_source(md->getDisplay()));
				thr_md_arg->insert(&evsrc);
				thr_md_arg->insert(logg.getEvent());
				thr_md_arg->start();

				{
					ALLEGRO_EVENT ev;
					ev.type = +Assistance::ro__my_events::THRKBM_DISPLAY_SIZE;
					ev.user.data1 = al_get_display_width(md->getDisplay());
					ev.user.data2 = al_get_display_height(md->getDisplay());
					al_emit_user_event(&evsrc, &ev, NULL);
				}

				logg << L::SLL << fsr(__FUNCSIG__) << "In the loop!" << L::BLL;

				al_set_target_backbuffer(md->getDisplay());
				al_convert_bitmaps();


				mtt->set(Assistance::io__text_string::FONT, "DEFAULT");
				mtt->set(Assistance::io__text_string::STRING, "Syncronization in progress... (LOG)");
				mtt->set(Assistance::io__text_string::ID, "lastlogtext");
				mtt->set(Assistance::io__text_boolean::SHOW, true);
				mtt->set(Assistance::io__text_double::SCALEG, 0.03);
				mtt->set(Assistance::io__text_double::POSY, 0.965);
				mtt->set(Assistance::io__text_double::POSX, -1.0);
				mtt->set(Assistance::io__text_integer::MODE, +Assistance::io___alignment_text::ALIGN_LEFT);
				mtt->set(Assistance::io__text_double::UPDATETIME, 1.0 / 5);
				mtt->set(Assistance::io__text_boolean::AFFECTED_BY_CAM, false);

				std::string mtt_s;


				thr_md_upnrunnin = true;

				for (bool localb = true; localb;)
				{
					if (thr_md_arg->hasEvent()) {

						if (thr_md_arg->isThisThis(+Assistance::ro__thread_display_routines_timers::LOOPTRACK))
						{
							logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "LOOPSCHECK: " << thr_md_arg->getNumCalls() << " frames per second" << L::BL;
						}
						else if (thr_md_arg->isThisThis(+Assistance::ro__thread_display_routines_timers::CHECKKEEP))
						{
							localb = !thr_shared_arg.should_exit;
							//logg.debug("[THR_DRW] CHECKKEEP called\n");
						}
						else if (thr_md_arg->isThisThis(+Assistance::ro__thread_display_routines_timers::CHECKMEMORYBITMAP))
						{
							//logg.debug("[THR_DRW] CHECKMEMORYBITMAP called\n");
							al_convert_bitmaps();
						}
						else if (thr_md_arg->isThisThis(+Assistance::ro__thread_display_routines_timers::UPDATELOGONSCREEN))
						{
							mtt->set(Assistance::io__text_string::STRING, mtt_s);
						}


						else { // not a timer
							auto ev = thr_md_arg->getEventRaw();

							switch (ev.type) {
							case ALLEGRO_EVENT_DISPLAY_CLOSE:
								thr_shared_arg.should_exit = true;
								logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "DISPLAYCLOSE event got. Set to turn off soon." << L::BL;
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
								logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "DISPLAYRESIZE got, acknowledged, done." << L::BL;

								Database config;
								config.set(Assistance::io__conf_integer::SCREEN_X, al_get_display_width(md->getDisplay()));
								config.set(Assistance::io__conf_integer::SCREEN_Y, al_get_display_height(md->getDisplay()));

								{
									ALLEGRO_EVENT ev;
									ev.type = +Assistance::ro__my_events::THRKBM_DISPLAY_SIZE;
									ev.user.data1 = al_get_display_width(md->getDisplay());
									ev.user.data2 = al_get_display_height(md->getDisplay());
									al_emit_user_event(&evsrc, &ev, NULL);
								}
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

						logg << L::SLL << fsr(__FUNCSIG__, E::WARN) << "Got draw exception! {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::BLL;

						if (err.getErrN() == 1 && (last_loop_had_error < 10)) {
							//md->restart();
							al_convert_bitmaps();
							last_loop_had_error++;
						}
						//else throw err;
					}


					sprites.unlock();

					md->flip();

					//Sleep(50);
				}

				logg << L::SLL << fsr(__FUNCSIG__) << "Closing stuff!" << L::BLL;

				delete md;
				md = nullptr;


				// should not stop before the others (it is safer to close later)
				while (thr_cl_arg) Sleep(50);
				while (thr_kb_arg) Sleep(50);

				delete thr_md_arg;

				thr_md_arg = nullptr;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				//textures.clear();

				thr_md_upnrunnin = false;
				logg << L::SLL << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::BLL;
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

				logg << L::SLL << fsr(__FUNCSIG__) << "Initializing..." << L::BLL;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				thr_cl_arg = new __collision_routines();
				thr_cl_arg->insert(&evsrc);
				thr_cl_arg->start();


				logg << L::SLL << fsr(__FUNCSIG__) << "In the loop!" << L::BLL;

				thr_cl_upnrunnin = true;

				for (bool localb = true; localb;)
				{
					thr_cl_arg->hasEventWait();

					if (thr_cl_arg->isThisThis(+Assistance::ro__thread_collision_routines_timers::LOOPTRACK))
					{
						logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "LOOPSCHECK: " << thr_cl_arg->getNumCalls() << " collisions and verifications per sec" << L::BL;
					}
					else if (thr_cl_arg->isThisThis(+Assistance::ro__thread_collision_routines_timers::CHECKKEEP))
					{
						localb = !thr_shared_arg.should_exit;
						//printf_s("[THR_COL] CHECKKEEP called\n");
					}
					else if (thr_cl_arg->isThisThis(+Assistance::ro__thread_collision_routines_timers::COLLISIONWORK))
					{
						//printf_s("[THR_COL] COLLISIONWORK called\n");

						// work on timed collision stuff


					}
				}
				logg << L::SLL << fsr(__FUNCSIG__) << "Closing stuff!" << L::BLL;

				delete thr_cl_arg;

				thr_cl_arg = nullptr;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				thr_cl_upnrunnin = false;
				logg << L::SLL << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::BLL;
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

				logg << L::SLL << fsr(__FUNCSIG__) << "Initializing..." << L::BLL;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount++;
				thr_shared_arg.threadcountm.unlock();

				_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
				setlocale(LC_ALL, "");

				thr_kb_arg = new __keyboardmouse_routines();
				thr_kb_arg->insert(al_get_keyboard_event_source());
				thr_kb_arg->insert(al_get_mouse_event_source());
				thr_kb_arg->insert(&evsrc);



				int display_x = 1280;
				int display_y = 720;
				bool isscreenfullscreen = false;

				thr_kb_arg->start();


				logg << L::SLL << fsr(__FUNCSIG__) << "In the loop!" << L::BLL;

				thr_kb_upnrunnin = true;

				for (bool localb = true; localb;)
				{
					thr_kb_arg->hasEventWait();

					if (thr_kb_arg->isThisThis(+Assistance::ro__thread_keyboardm_routines_timers::LOOPTRACK))
					{
						logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "LOOPSCHECK: " << thr_kb_arg->getNumCalls() << " events per sec" << L::BL;
					}
					else if (thr_kb_arg->isThisThis(+Assistance::ro__thread_keyboardm_routines_timers::CHECKKEEP))
					{
						localb = !thr_shared_arg.should_exit;
					}
					else if (thr_kb_arg->isThisThis(+Assistance::ro__thread_keyboardm_routines_timers::UPDATEMOUSE))
					{
						float _temp[2];
						conf.get(Assistance::io__conf_mouse_float::MOUSE_X, _temp[0]);
						conf.get(Assistance::io__conf_mouse_float::MOUSE_Y, _temp[1]);

						for (auto& i : sprites) {
							if (i->self->isEq(Assistance::io__sprite_boolean::FOLLOWMOUSE, true)) {
								i->self->set(Assistance::io__sprite_double::POSX, _temp[0]);
								i->self->set(Assistance::io__sprite_double::POSY, _temp[1]);
							}
						}
					}

					else { // not a timer ///DEBUG NOW
						auto ev = thr_kb_arg->getEventRaw();

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
							conf.set(Assistance::io__conf_mouse_float::MOUSE_X, (ev.mouse.x * 2.0f / display_x) - 1.0);
							conf.set(Assistance::io__conf_mouse_float::MOUSE_Y, (ev.mouse.y * 2.0f / display_y) - 1.0);
						}
						if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
							conf.set(+((Assistance::io__conf_mouse_boolean)ev.mouse.button), true);
						}
						if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
							conf.set(+((Assistance::io__conf_mouse_boolean)ev.mouse.button), false);
						}
					}
				}
				logg << L::SLL << fsr(__FUNCSIG__) << "Closing stuff!" << L::BLL;

				delete thr_kb_arg;

				thr_kb_arg = nullptr;

				thr_shared_arg.threadcountm.lock();
				thr_shared_arg.threadcount--;
				thr_shared_arg.threadcountm.unlock();

				thr_kb_upnrunnin = false;
				logg << L::SLL << fsr(__FUNCSIG__) << "Ended everything! Bye!" << L::BLL;
			}
			catch (Abort::abort a)
			{
				forceExit("Something went wrong at KBTHR!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
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
			thr_md = new std::thread([=] {__l_thr_md(); }); // working on
			///thr_cl = new std::thread([=] {__l_thr_cl(); }); // not ready yet
			thr_kb = new std::thread([=] {__l_thr_kb(); }); // working on

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

			if (thr_md) thr_md->join();
			if (thr_cl) thr_cl->join();
			if (thr_kb) thr_kb->join();

			thr_md = nullptr;
			thr_cl = nullptr;
			thr_kb = nullptr;
		}

		bool Console::isOpen()
		{
			return (md != nullptr && thr_md_upnrunnin /* TODO: add collision thread and keyboard */);
		}

		bool Console::isRunning()
		{
			bool stillrunning = !thr_shared_arg.should_exit;
			if (!stillrunning) {
				stop();
			}
			return stillrunning;
		}

	}
}
