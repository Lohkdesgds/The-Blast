#include "console.h"

namespace LSW {
	namespace v4 {
		namespace Assistance {
			/*__display_routines::__display_routines()
			{				
				eachsecbm = al_create_timer(1.0 / eachsecbm_t);
				checkdead = al_create_timer(1.0 / checkdead_t);
				checkmemb = al_create_timer(1.0 / checkmemb_t);
				acknwresz = al_create_timer(1.0 / acknwresz_t);

				queue = al_create_event_queue();

				al_register_event_source(queue, al_get_timer_event_source(eachsecbm));
				al_register_event_source(queue, al_get_timer_event_source(checkdead));
				al_register_event_source(queue, al_get_timer_event_source(checkmemb));
				al_register_event_source(queue, al_get_timer_event_source(acknwresz));
				
			}
			__display_routines::~__display_routines()
			{
				stop_timers();
				al_destroy_timer(eachsecbm);
				al_destroy_timer(checkdead);
				al_destroy_timer(checkmemb);
				al_destroy_timer(acknwresz);
				al_destroy_event_queue(queue);
			}
			void __display_routines::start_timers()
			{
				al_start_timer(eachsecbm);
				al_start_timer(checkdead);
				al_start_timer(checkmemb);
				al_start_timer(acknwresz);
			}
			void __display_routines::stop_timers()
			{
				al_stop_timer(eachsecbm);
				al_stop_timer(checkdead);
				al_stop_timer(checkmemb);
				al_stop_timer(acknwresz);
			}
			size_t __display_routines::getReport(const __display_routines_timers t)
			{
				switch (t) {
				case __display_routines_timers::UNLEASHEDREPORT:
					return benchmark_unleashed;
				case __display_routines_timers::CHECKKEEP:
					return benchmark_checkdead;
				case __display_routines_timers::CHECKMEMORYBITMAP:
					return benchmark_checkmemb;
				case __display_routines_timers::CHECKACKNOWLEDGE:
					return benchmark_acknwresz;
				}
				return 0;
			}
			size_t __display_routines::getReportExpected(const __display_routines_timers t)
			{
				switch (t) {
				case __display_routines_timers::CHECKKEEP:
					return checkdead_t;
				case __display_routines_timers::CHECKMEMORYBITMAP:
					return checkmemb_t;
				case __display_routines_timers::CHECKACKNOWLEDGE:
					return acknwresz_t;
				}
				return 0;
			}
			bool __display_routines::hasEvent()
			{
				bool b = al_get_next_event(queue,&lastev);

				if (b) { // task eachsecond joins the match
					if (isThisThis(__display_routines_timers::__EACHSECOND)) {
						benchmark_checkdead = rawcount_checkdead;
						benchmark_checkmemb = rawcount_checkmemb;
						benchmark_acknwresz = rawcount_acknwresz;
						benchmark_unleashed = rawcount_unleashed;

						rawcount_checkdead = rawcount_checkmemb = rawcount_acknwresz = rawcount_unleashed = 0;
					}
				}
				rawcount_unleashed++;

				return b;
			}
			bool __display_routines::isThisThis(const __display_routines_timers t)
			{
				if (!(lastev.type == ALLEGRO_EVENT_TIMER)) return false;

				switch (t) {
				case __display_routines_timers::__EACHSECOND:
					return (lastev.timer.source == eachsecbm);
				case __display_routines_timers::CHECKKEEP:
					return (lastev.timer.source == checkdead);
				case __display_routines_timers::CHECKMEMORYBITMAP:
					return (lastev.timer.source == checkmemb);
				case __display_routines_timers::CHECKACKNOWLEDGE:
					return (lastev.timer.source == acknwresz);
				}
				return false;
			}
			ALLEGRO_EVENT __display_routines::soWhat()
			{
				return lastev;
			}


			__collision_routines::__collision_routines()
			{
				eachsecbm = al_create_timer(1.0 / eachsecbm_t);
				checkdead = al_create_timer(1.0 / checkdead_t);
				colliding = al_create_timer(1.0 / colliding_t);

				queue = al_create_event_queue();

				al_register_event_source(queue, al_get_timer_event_source(eachsecbm));
				al_register_event_source(queue, al_get_timer_event_source(checkdead));
				al_register_event_source(queue, al_get_timer_event_source(colliding));
			}
			__collision_routines::~__collision_routines()
			{
				stop_timers();
				al_destroy_timer(eachsecbm);
				al_destroy_timer(checkdead);
				al_destroy_timer(colliding);
				al_destroy_event_queue(queue);
			}
			void __collision_routines::start_timers()
			{
				al_start_timer(eachsecbm);
				al_start_timer(checkdead);
				al_start_timer(colliding);
			}
			void __collision_routines::stop_timers()
			{
				al_stop_timer(eachsecbm);
				al_stop_timer(checkdead);
				al_stop_timer(colliding);
			}
			bool __collision_routines::hasEvent()
			{
				return al_get_next_event(queue, &lastev);
			}
			bool __collision_routines::isThisThis(const __collision_routines_timers t)
			{
				if (!(lastev.type == ALLEGRO_EVENT_TIMER)) return false;

				switch (t) {
				case __collision_routines_timers::__EACHSECOND:
					return (lastev.timer.source == eachsecbm);
				case __collision_routines_timers::CHECKKEEP:
					return (lastev.timer.source == checkdead);
				case __collision_routines_timers::COLLISIONWORK:
					return (lastev.timer.source == colliding);
				}
				return false;
			}
			ALLEGRO_EVENT __collision_routines::soWhat()
			{
				return lastev;
			}*/
		}


		void Console::__l_thr_md()
		{
			gfile logg;

			logg << L::START << freg("main", "main") << "[THR_DRW] Initializing..." << L::ENDL;

			thr_shared_arg.threadcountm.lock();
			thr_shared_arg.threadcount++;
			thr_shared_arg.threadcountm.unlock();

			size_t last_loop_had_error = 0;

			logg << L::START << freg("main", "main") << "[THR_DRW] Creating display..." << L::ENDL;

			md = new __raw_display();
			Sprites sprites;
			Texts texts;

			al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);

			gcam.apply();

			thr_md_arg = new __display_routines();
			thr_md_arg->insert(al_get_display_event_source(md->_getD()));
			thr_md_arg->insert(&evsrc);
			thr_md_arg->start_timers();


			logg << L::START << freg("main", "main") << "[THR_DRW] In the loop!" << L::ENDL;


			for(bool localb = true; localb;)
			{
				if (thr_md_arg->hasEvent()) {
					
					if (thr_md_arg->isThisThis(+Assistance::__display_routines_timers::LOOPTRACK))
					{
						printf_s("[INFO][THR_DRW] LOOPSCHECK: %zu\n", thr_md_arg->getNumCalls());
					}
					else if (thr_md_arg->isThisThis(+Assistance::__display_routines_timers::CHECKKEEP))
					{
						localb = !thr_shared_arg.should_exit;
						//printf_s("[THR_DRW] CHECKKEEP called\n");
					}
					else if (thr_md_arg->isThisThis(+Assistance::__display_routines_timers::CHECKMEMORYBITMAP))
					{
						//printf_s("[THR_DRW] CHECKMEMORYBITMAP called\n");
						al_convert_bitmaps();
					}


					else { // not a timer
						auto ev = thr_md_arg->soWhat();

						if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
							thr_shared_arg.should_exit = true;
							printf_s("[INFO][THR_DRW] DISPLAYCLOSE event got. Set to turn off soon.\n");
						}
						if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
						{
							al_acknowledge_resize(md->_getD());
							gcam.apply();
							printf_s("[INFO][THR_DRW] DISPLAYRESIZE got, acknowledged, done.\n");
						}
					}
				}

				// draw screen at least once

				md->clear_to(BLACK);

				
				while (!sprites.try_lock()) Sleep(10);

				// locally check if dealable error
				try {
					for (auto& i : sprites) i->self->draw(0);
					for (auto& i : texts)  i->self->draw(0);

					last_loop_had_error = 0;
				}
				catch (Abort::abort err)
				{

					printf_s("[WARN][THR_DRW] Got draw exception! {%s,%s,%s,#%d}", err.function().c_str(), err.from().c_str(), err.details().c_str(), err.getErrN());

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

			// should not stop before the others (it is safer to close later)
			while (thr_cl_arg) Sleep(50);
			while (thr_kb_arg) Sleep(50);


			delete thr_md_arg;
			delete md;

			thr_md_arg = nullptr;
			md = nullptr;

			printf_s("[INFO][THR_DRW] Byee!\n");
		}

		void Console::__l_thr_cl()
		{
			gfile logg;

			logg << L::START << freg("main", "main") << "[THR_COL] Initializing..." << L::ENDL;

			thr_shared_arg.threadcountm.lock();
			thr_shared_arg.threadcount++;
			thr_shared_arg.threadcountm.unlock();

			thr_cl_arg = new __collision_routines();
			thr_cl_arg->insert(&evsrc);
			thr_cl_arg->start_timers();


			logg << L::START << freg("main", "main") << "[THR_COL] In the loop!" << L::ENDL;


			for (bool localb = true; localb;)
			{
				thr_cl_arg->hasWaitEvent();

				if (thr_cl_arg->isThisThis(+Assistance::__collision_routines_timers::LOOPTRACK))
				{
					printf_s("[INFO][THR_COL] LOOPSCHECK: %zu\n", thr_cl_arg->getNumCalls());
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

			delete thr_cl_arg;

			thr_cl_arg = nullptr;

			printf_s("[INFO][THR_COL] Byee!\n");
		}

		void Console::__l_thr_kb()
		{
			gfile logg;

			logg << L::START << freg("main", "main") << "[THR_KBM] Initializing..." << L::ENDL;

			thr_shared_arg.threadcountm.lock();
			thr_shared_arg.threadcount++;
			thr_shared_arg.threadcountm.unlock();

			_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
			setlocale(LC_ALL, "");

			thr_kb_arg = new __keyboardmouse_routines();
			thr_kb_arg->insert(al_get_keyboard_event_source());
			thr_kb_arg->insert(al_get_mouse_event_source());
			thr_kb_arg->insert(&evsrc);



			logg << L::START << freg("main", "main") << "[THR_KBM] Waiting display to be created to get event source..." << L::ENDL;


			while (!md) Sleep(50); // it will use the display size to work with mouse relative positioning
			while (!md->_getD()) Sleep(50);

			int display_x = al_get_display_width(md->_getD());
			int display_y = al_get_display_height(md->_getD());
			float mouse_pos[2] = { 0.0,0.0 };
			bool isscreenfullscreen = (al_get_display_flags(md->_getD()) & ALLEGRO_FULLSCREEN) || (al_get_display_flags(md->_getD()) & ALLEGRO_FULLSCREEN_WINDOW);

			thr_kb_arg->start_timers();


			logg << L::START << freg("main", "main") << "[THR_KBM] In the loop!" << L::ENDL;


			for (bool localb = true; localb;)
			{
				thr_kb_arg->hasWaitEvent();

				if (thr_kb_arg->isThisThis(+Assistance::__keyboardm_routines_timers::LOOPTRACK))
				{
					printf_s("[INFO][THR_KBM] LOOPSCHECK: %zu\n", thr_kb_arg->getNumCalls());
				}
				else if (thr_kb_arg->isThisThis(+Assistance::__keyboardm_routines_timers::CHECKKEEP))
				{
					localb = !thr_shared_arg.should_exit;
					//printf_s("[THR_KBM] CHECKKEEP called\n");
				}
				else if (thr_kb_arg->isThisThis(+Assistance::__keyboardm_routines_timers::CHECKDISPLAYRESIZE))
				{
					display_x = al_get_display_width(md->_getD());
					display_y = al_get_display_height(md->_getD());
				}

				else { // not a timer ///DEBUG NOW
					auto ev = thr_kb_arg->soWhat();

					if (ev.type == ALLEGRO_EVENT_KEY_DOWN) { // USE IN GAME

						//printf_s("[THR_KBM] KEYDOWN= %d\n", ev.keyboard.keycode);
					}
					if (ev.type == ALLEGRO_EVENT_KEY_UP) { // USE IN GAME

						switch (ev.keyboard.keycode) {
						case ALLEGRO_KEY_F11:
							al_set_display_flag(md->_getD(), ALLEGRO_FULLSCREEN_WINDOW, (isscreenfullscreen = !isscreenfullscreen));
							// cast an event on the queue of everybody
							ALLEGRO_EVENT ev;
							ev.type = ALLEGRO_EVENT_DISPLAY_RESIZE;
							al_emit_user_event(&evsrc, &ev, NULL);
							break;
						case ALLEGRO_KEY_F2:
							md->print();
							break;
						}
					}

					if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
						//const char* actch = al_keycode_to_name(ev.keyboard.keycode); // prints literally key name KEY26 A B C
						if (ev.keyboard.unichar > 32) printf_s("[THR_KBM] KEYCHAR= %d ~= %c\n", ev.keyboard.keycode, ev.keyboard.unichar);
						else                          printf_s("[THR_KBM] KEYCHAR= %d\n", ev.keyboard.keycode);
					}
					if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
						mouse_pos[0] = (ev.mouse.x * 2.0f / display_x) - 1.0;
						mouse_pos[1] = (ev.mouse.y * 2.0f / display_y) - 1.0;

						printf_s("[THR_KBM] MOUSEAXIS dp={%d,%d} pos={%d,%d} rel={%.3f,%.3f}\n", ev.mouse.dx, ev.mouse.dy, ev.mouse.x, ev.mouse.y, mouse_pos[0], mouse_pos[1]);
					}
					if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
						printf_s("[THR_KBM] MOUSEDOWN= %d\n", ev.mouse.button);
					}
					if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
						printf_s("[THR_KBM] MOUSEDOWN= %d\n", ev.mouse.button);
					}
				}
			}
			
			delete thr_kb_arg;

			thr_kb_arg = nullptr;

			printf_s("[INFO][THR_KBM] Byee!\n");
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

			if (thr_md) thr_md->join();
			if (thr_cl) thr_cl->join();
			if (thr_kb) thr_kb->join();

			thr_md = nullptr;
			thr_cl = nullptr;
			thr_kb = nullptr;
		}

		bool Console::awakened()
		{
			return (md != nullptr);
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
