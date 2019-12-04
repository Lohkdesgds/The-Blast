#include "console.h"

namespace LSW {
	namespace v4 {

		void Console::__l_thr_md()
		{
			gfile logg;

			logg << L::SLL << fsr(__FUNCSIG__) << "Initializing..." << L::BLL;

			thr_shared_arg.threadcountm.lock();
			thr_shared_arg.threadcount++;
			thr_shared_arg.threadcountm.unlock();

			size_t last_loop_had_error = 0;

			logg << L::SLL << fsr(__FUNCSIG__) << "Creating display..." << L::BLL;

			md = new single_display();
			Sprites sprites;
			Texts texts;
			//Textures textures;

			al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);

			//gcam.apply();

			thr_md_arg = new __display_routines();
			thr_md_arg->insert(al_get_display_event_source(md->_getD()));
			thr_md_arg->insert(&evsrc);
			thr_md_arg->start_timers();

			{
				ALLEGRO_EVENT ev;
				ev.type = +Assistance::__my_events::THRKBM_DISPLAY_SIZE;
				ev.user.data1 = al_get_display_width(md->_getD());
				ev.user.data2 = al_get_display_height(md->_getD());
				al_emit_user_event(&evsrc, &ev, NULL);
			}

			logg << L::SLL << fsr(__FUNCSIG__) << "In the loop!" << L::BLL;

			al_set_target_backbuffer(md->_getD());
			al_convert_bitmaps();
			thr_md_upnrunnin = true;

			for(bool localb = true; localb;)
			{
				if (thr_md_arg->hasEvent()) {
					
					if (thr_md_arg->isThisThis(+Assistance::__display_routines_timers::LOOPTRACK))
					{
						logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "LOOPSCHECK: " << thr_md_arg->getNumCalls() << " frames per second" << L::BL;
					}
					else if (thr_md_arg->isThisThis(+Assistance::__display_routines_timers::CHECKKEEP))
					{
						localb = !thr_shared_arg.should_exit;
						//logg.debug("[THR_DRW] CHECKKEEP called\n");
					}
					else if (thr_md_arg->isThisThis(+Assistance::__display_routines_timers::CHECKMEMORYBITMAP))
					{
						//logg.debug("[THR_DRW] CHECKMEMORYBITMAP called\n");
						al_convert_bitmaps();
					}


					else { // not a timer
						auto ev = thr_md_arg->soWhat();

						switch (ev.type) {
						case ALLEGRO_EVENT_DISPLAY_CLOSE:
							thr_shared_arg.should_exit = true;
							logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "DISPLAYCLOSE event got. Set to turn off soon." << L::BL;
							break;
						case +Assistance::__my_events::THRDRW_GOT_FORCED_RESIZE:
							al_set_display_flag(md->_getD(), ALLEGRO_FULLSCREEN_WINDOW, (bool)ev.user.data1);
							// yes, merge
						case ALLEGRO_EVENT_DISPLAY_RESIZE:
							al_acknowledge_resize(md->_getD());
							
							gcam.apply();
							logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "DISPLAYRESIZE got, acknowledged, done." << L::BL;

							Config config;
							config.set(Assistance::conf_i::SCREEN_X, al_get_display_width(md->_getD()));
							config.set(Assistance::conf_i::SCREEN_Y, al_get_display_height(md->_getD()));

							{
								ALLEGRO_EVENT ev;
								ev.type = +Assistance::__my_events::THRKBM_DISPLAY_SIZE;
								ev.user.data1 = al_get_display_width(md->_getD());
								ev.user.data2 = al_get_display_height(md->_getD());
								al_emit_user_event(&evsrc, &ev, NULL);
							}
							break;
						}
					}
				}

				// draw screen at least once

				md->clear_to(BLACK);

				
				while (!sprites.try_lock()) Sleep(10);

				// locally check if dealable error
				try {
					//al_set_target_backbuffer(md->_getD());
					gcam.apply();
					//al_draw_filled_rectangle(____temporary_mouse_pos[0] - 0.5, ____temporary_mouse_pos[1] - 0.05, ____temporary_mouse_pos[0], ____temporary_mouse_pos[1] + 0.1, al_map_rgb(0, 0, 255));
					//al_draw_filled_rectangle(____temporary_mouse_pos[0], ____temporary_mouse_pos[1] - 0.1, ____temporary_mouse_pos[0] + 0.5, ____temporary_mouse_pos[1] + 0.05, al_map_rgb(0, 255, 0));
					for (auto& i : sprites) i->self->draw(0);
					for (auto& i : texts)  i->self->draw(0);
					last_loop_had_error = 0;
				}
				catch (Abort::abort err)
				{

					logg << L::SLL << fsr(__FUNCSIG__, E::WARN) << "Got draw exception! {" << err.function() << "," << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::BLL;

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

		void Console::__l_thr_cl()
		{
			gfile logg;

			logg << L::SLL << fsr(__FUNCSIG__) << "Initializing..." << L::BLL;

			thr_shared_arg.threadcountm.lock();
			thr_shared_arg.threadcount++;
			thr_shared_arg.threadcountm.unlock();

			thr_cl_arg = new __collision_routines();
			thr_cl_arg->insert(&evsrc);
			thr_cl_arg->start_timers();


			logg << L::SLL << fsr(__FUNCSIG__) << "In the loop!" << L::BLL;

			thr_cl_upnrunnin = true;

			for (bool localb = true; localb;)
			{
				thr_cl_arg->hasWaitEvent();

				if (thr_cl_arg->isThisThis(+Assistance::__collision_routines_timers::LOOPTRACK))
				{
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "LOOPSCHECK: " << thr_cl_arg->getNumCalls() << " collisions and verifications per sec" << L::BL;
				}
				else if (thr_cl_arg->isThisThis(+Assistance::__collision_routines_timers::CHECKKEEP))
				{
					localb = !thr_shared_arg.should_exit;
					//printf_s("[THR_COL] CHECKKEEP called\n");
				}
				else if (thr_cl_arg->isThisThis(+Assistance::__collision_routines_timers::COLLISIONWORK))
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

		void Console::__l_thr_kb()
		{
			gfile logg;

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
			float mouse_pos[2] = { 0.0,0.0 };
			bool isscreenfullscreen = false;

			thr_kb_arg->start_timers();


			logg << L::SLL << fsr(__FUNCSIG__) << "In the loop!" << L::BLL;

			thr_kb_upnrunnin = true;

			for (bool localb = true; localb;)
			{
				thr_kb_arg->hasWaitEvent();

				if (thr_kb_arg->isThisThis(+Assistance::__keyboardm_routines_timers::LOOPTRACK))
				{
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "LOOPSCHECK: " << thr_kb_arg->getNumCalls() << " events per sec" << L::BL;
				}
				else if (thr_kb_arg->isThisThis(+Assistance::__keyboardm_routines_timers::CHECKKEEP))
				{
					localb = !thr_shared_arg.should_exit;
					//printf_s("[THR_KBM] CHECKKEEP called\n");
				}
				else if (thr_kb_arg->isThisThis(+Assistance::__keyboardm_routines_timers::CHECKDISPLAYRESIZE))
				{
					/*if (md) {
						display_x = al_get_display_width(md->_getD());
						display_y = al_get_display_height(md->_getD());
					}*/
				}

				else { // not a timer ///DEBUG NOW
					auto ev = thr_kb_arg->soWhat();

					if (ev.type == +Assistance::__my_events::THRKBM_DISPLAY_SIZE)
					{
						display_x = ev.user.data1;
						display_y = ev.user.data2;
					}
					if (ev.type == ALLEGRO_EVENT_KEY_DOWN) { // USE IN GAME

						//printf_s("[THR_KBM] KEYDOWN= %d\n", ev.keyboard.keycode);

						camera_preset cp;

						switch (ev.keyboard.keycode) {
						case ALLEGRO_KEY_1:
							cp.set(Assistance::io___float_camera::ROTATION, 0.0 * ALLEGRO_PI);
							gcam.set(cp, 0);
							gcam.apply(0);
							break;
						case ALLEGRO_KEY_2:
							cp.set(Assistance::io___float_camera::ROTATION, 0.5 * ALLEGRO_PI);
							gcam.set(cp, 0);
							gcam.apply(0);
							break;
						case ALLEGRO_KEY_3:
							cp.set(Assistance::io___float_camera::ROTATION, 1.0 * ALLEGRO_PI);
							gcam.set(cp, 0);
							gcam.apply(0);
							break;
						case ALLEGRO_KEY_4:
							cp.set(Assistance::io___float_camera::ROTATION, 1.5 * ALLEGRO_PI);
							gcam.set(cp, 0);
							gcam.apply(0);
							break;
						}
					}
					if (ev.type == ALLEGRO_EVENT_KEY_UP) { // USE IN GAME

						switch (ev.keyboard.keycode) {
						case ALLEGRO_KEY_F11:
							//al_set_display_flag(md->_getD(), ALLEGRO_FULLSCREEN_WINDOW, (isscreenfullscreen = !isscreenfullscreen));
							// cast an event on the queue of everybody
							ALLEGRO_EVENT ev;
							ev.type = +Assistance::__my_events::THRDRW_GOT_FORCED_RESIZE;
							ev.user.data1 = (isscreenfullscreen = !isscreenfullscreen);
							al_emit_user_event(&evsrc, &ev, NULL);
							break;
						case ALLEGRO_KEY_F2:
							md->print();
							break;
						}
					}

					if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
						//const char* actch = al_keycode_to_name(ev.keyboard.keycode); // prints literally key name KEY26 A B C
						//if (ev.keyboard.unichar > 32) logg.debug("[THR_KBM] KEYCHAR= %d ~= %c", ev.keyboard.keycode, ev.keyboard.unichar);
						//else                          logg.debug("[THR_KBM] KEYCHAR= %d", ev.keyboard.keycode);
					}
					if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
						____temporary_mouse_pos[0] = mouse_pos[0] = (ev.mouse.x * 2.0f / display_x) - 1.0;
						____temporary_mouse_pos[1] = mouse_pos[1] = (ev.mouse.y * 2.0f / display_y) - 1.0;

						//logg.debug("[THR_KBM] MOUSEAXIS dp={%d,%d} pos={%d,%d} rel={%.3f,%.3f}", ev.mouse.dx, ev.mouse.dy, ev.mouse.x, ev.mouse.y, mouse_pos[0], mouse_pos[1]);
					}
					if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
						//logg.debug("[THR_KBM] MOUSEDOWN= %d", ev.mouse.button);
					}
					if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
						//logg.debug("[THR_KBM] MOUSEDOWN= %d", ev.mouse.button);
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

		Console::Console()
		{
			al_init_user_event_source(&evsrc);
		}

		Console::~Console()
		{
			close();
			al_destroy_user_event_source(&evsrc);
		}

		void Console::launch()
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

		void Console::close()
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

		bool Console::awakened()
		{
			return (md != nullptr && thr_md_upnrunnin /* TODO: add collision thread and keyboard */);
		}

		bool Console::running()
		{
			bool stillrunning = !thr_shared_arg.should_exit;
			if (!stillrunning) {
				close();
			}
			return stillrunning;
		}

	}
}
