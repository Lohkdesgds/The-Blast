#include "..\..\all\enable_all.h"



namespace LSW {
	namespace v2 {
		namespace Events {

			events* big_event::eventr = nullptr;

			void _functionThr::setId(const int v)
			{
				id = v;
			}
			const int _functionThr::getId()
			{
				return id;
			}

			void _functionThr::setUpdateTime(const double tps)
			{
				pertime = tps;
			}
			void _functionThr::resetTo(const double v)
			{
				f = v;
			}
			const bool _functionThr::_gotWork()
			{
				if (al_get_time() - _lasttry > pertime)
				{
					_lasttry += pertime;
					return true;
				}
				return false;
			}
			double _functionThr::getVal()
			{
				return f;
			}
			void _functionThr::setF(void(*fuf) (void*, double&))
			{
				fu = fuf;
			}


			const char _string_work::translate(const int code)
			{
				char ch = 0;
				switch (code)
				{
				case ALLEGRO_KEY_A:
					ch = 'A';
					break;
				case ALLEGRO_KEY_B:
					ch = 'B';
					break;
				case ALLEGRO_KEY_C:
					ch = 'C';
					break;
				case ALLEGRO_KEY_D:
					ch = 'D';
					break;
				case ALLEGRO_KEY_E:
					ch = 'E';
					break;
				case ALLEGRO_KEY_F:
					ch = 'F';
					break;
				case ALLEGRO_KEY_G:
					ch = 'G';
					break;
				case ALLEGRO_KEY_H:
					ch = 'H';
					break;
				case ALLEGRO_KEY_I:
					ch = 'I';
					break;
				case ALLEGRO_KEY_J:
					ch = 'J';
					break;
				case ALLEGRO_KEY_K:
					ch = 'K';
					break;
				case ALLEGRO_KEY_L:
					ch = 'L';
					break;
				case ALLEGRO_KEY_M:
					ch = 'M';
					break;
				case ALLEGRO_KEY_N:
					ch = 'N';
					break;
				case ALLEGRO_KEY_O:
					ch = 'O';
					break;
				case ALLEGRO_KEY_P:
					ch = 'P';
					break;
				case ALLEGRO_KEY_Q:
					ch = 'Q';
					break;
				case ALLEGRO_KEY_R:
					ch = 'R';
					break;
				case ALLEGRO_KEY_S:
					ch = 'S';
					break;
				case ALLEGRO_KEY_T:
					ch = 'T';
					break;
				case ALLEGRO_KEY_U:
					ch = 'U';
					break;
				case ALLEGRO_KEY_V:
					ch = 'V';
					break;
				case ALLEGRO_KEY_W:
					ch = 'W';
					break;
				case ALLEGRO_KEY_X:
					ch = 'X';
					break;
				case ALLEGRO_KEY_Y:
					ch = 'Y';
					break;
				case ALLEGRO_KEY_Z:
					ch = 'Z';
					break;
				case ALLEGRO_KEY_0:
					ch = '0';
					break;
				case ALLEGRO_KEY_1:
					ch = '1';
					break;
				case ALLEGRO_KEY_2:
					ch = '2';
					break;
				case ALLEGRO_KEY_3:
					ch = '3';
					break;
				case ALLEGRO_KEY_4:
					ch = '4';
					break;
				case ALLEGRO_KEY_5:
					ch = '5';
					break;
				case ALLEGRO_KEY_6:
					ch = '6';
					break;
				case ALLEGRO_KEY_7:
					ch = '7';
					break;
				case ALLEGRO_KEY_8:
					ch = '8';
					break;
				case ALLEGRO_KEY_9:
					ch = '9';
					break;
				case ALLEGRO_KEY_SPACE:
					ch = ' ';
					break;
				case ALLEGRO_KEY_ENTER:
				case ALLEGRO_KEY_PAD_ENTER:
					ch = 1;
					break;
				case ALLEGRO_KEY_FULLSTOP:
					ch = '.';
					break;
				case ALLEGRO_KEY_BACKSPACE:
					ch = 2;
					break;
				case ALLEGRO_KEY_ESCAPE:
					ch = 3;
					break;
				}
				return ch;
			}

			void _string_work::interpret(int last_key, int allegro_code) // unichar, keycode
			{
				//ev_handlr evh;

				/*if (lastkey != last_key)
				{*/
					lastkey = last_key;

					if (last_key != 0)
					{
						char translated = translate(allegro_code);
						
						if (translated == 1)
						{
							if (ongoing.g().length() > 0) done = ongoing;
							ongoing.clear();
						}
						else if (translated == 2)
						{
							ongoing.pop_back();
						}
						else if (translated == 3)
						{
							ongoing.clear();
						}
						else
						{
							if (lastkey != '%' && ongoing.g().length() < Defaults::max_string_input_size) ongoing += last_key;
						}
					}
				//}
				return;
			}
			void _string_work::getLastString(Safer::safe_string& u)
			{
				u = done;
			}
			void _string_work::getOnGoingString(Safer::safe_string& u)
			{
				u = ongoing;
			}
			const bool _string_work::isHack(hack_strings & e)
			{
				if (ongoing == "SETMEIMMORTAL") { e = HACK_IMMORTAL; return true; }
				if (ongoing == "JUMPNEXTLEVEL") { e = HACK_NEXTLEVEL; return true; }
				// else
				e = HACK_NONE;
				return false;
			}
			void _string_work::clear()
			{
				done.clear();
			}
			void _string_work::clearall()
			{
				clear();
				ongoing.clear();
			}


			Safer::safe_pointer<_functionThr> events::_get(const int r)
			{
				for (auto& i : funcs)
				{
					if (i->getId() == r) {
						return i;
					}
				}
				throw "at events::_get: not found id " + std::to_string(r);
				return Safer::safe_pointer<_functionThr>();
			}
			const bool events::setup()
			{
				if (!targ) {
					glog << Log::START << Log::_func("events", "setup") << "Initializing event handler... (thread)" << Log::ENDL;

					targ = al_get_current_display();
					if (!targ) {
						glog << Log::START << Log::_func("events", "setup", Log::ERRR) << "Cannot detect display! Something is not great!" << Log::ENDL;
						glog.flush();
						return false;
					}

					if (!(ev_qu = al_create_event_queue())) {
						targ = nullptr;
						glog << Log::START << Log::_func("events", "setup", Log::ERRR) << "Could not create event queue." << Log::ENDL;
						return false;
					}

					al_register_event_source(ev_qu, al_get_keyboard_event_source());
					al_register_event_source(ev_qu, al_get_mouse_event_source());
					al_register_event_source(ev_qu, al_get_display_event_source(targ));

					glog << Log::START << Log::_func("events", "setup") << "Initializing thread right now..." << Log::ENDL;

					thr = new std::thread(_thread_ev, this);

					for (clock_t now = clock(); clock() - now < 20 * CLOCKS_PER_SEC && !alright;);
					if (!alright) {
						glog << Log::START << Log::_func("events", "setup", Log::ERRR) << "SOMETHING WENT WRONG, I guess. I have to wait for the thread, so..." << Log::ENDL;
						glog.flush();
						thr->join();
						alright = false;
						al_destroy_event_queue(ev_qu);
						ev_qu = nullptr;
						targ = nullptr;
						glog << Log::START << Log::_func("events", "setup", Log::WARN) << "Trying again..." << Log::ENDL;
						setup();
					}
				}
				return true;
			}

			void events::stop()
			{
				if (!targ) {
					glog << Log::START << Log::_func("events", "stop") << "Shutting down thread(s)..." << Log::ENDL;
					glog.flush();
					alright = false;
					thr->join();
					al_destroy_event_queue(ev_qu);
					ev_qu = nullptr;
					targ = nullptr;
				}
			}

			bool * events::getKeep()
			{
				return &alright;
			}

			void events::setMultiplierForUpdatingImg(const double v)
			{
				multiplier_hz_d = 1.0 / v;
			}

			const bool events::addFunction(Safer::safe_pointer<_functionThr> u, void(*f)(void *, double &)) //int(*f)(void*, bool), void* data)
			{
				for (auto& i : funcs)
				{
					if (i->getId() == u->getId()) {
						// a function is returning the same value for null as someone else already on functions!
						// change the default "nullptr" return for the new function for correct usage of this "hack" I made :3
						glog << Log::START << Log::_func("events", "addFunction", Log::WARN) << "A function added on functions vector has already been added or there's another one with the same signature! PLEASE VERIFY THIS SITUATION! Return value got: " << u->getId() << Log::ENDL;
						glog.flush();
						exit(EXIT_FAILURE);
					}
				}
				u->setF(f);
				funcs.push_back(u);
				//params_f.push(data);
				return true;
			}
			void events::delFunction(const int r)
			{
				for (size_t p = 0; p < funcs.size(); p++)
				{
					if (funcs[p]->getId() == r) {
						funcs.erase(funcs.begin() + p);
						return;
					}
				}
			}

			void events::delAllFunctions()
			{
				funcs.clear();
			}

			double events::getFunctionValNow(const int r)
			{
				Safer::safe_pointer<_functionThr> tf = _get(r);
				if (tf) {
					return tf->getVal();
				}
				return 0.0;
			}

			void* events::getFVoidArg(const int r)
			{
				Safer::safe_pointer<_functionThr> tf = _get(r);
				if (tf) {
					return tf->getArgAsVoid();
				}
				return nullptr;
			}

			void events::setTimerForF(const double u, const int r)
			{
				Safer::safe_pointer<_functionThr> tf = _get(r);
				if (tf) {
					assert(u >= Defaults::functions_timer); // cannot be faster than that
					tf->setUpdateTime(u);
				}
			}

			void events::resetFValTo(const double u, const int r)
			{
				Safer::safe_pointer<_functionThr> tf = _get(r);
				if (tf) {
					tf->resetTo(u);
				}
			}

			ALLEGRO_TIMER * events::getTimer(const int v, const double a)
			{
				ALLEGRO_TIMER * t = nullptr;
				std::map<int, ALLEGRO_TIMER*>::iterator it = timers.find(v);
				if (it == timers.end())
				{
					t = al_create_timer(a);
					al_register_event_source(ev_qu, al_get_timer_event_source(t));
					timers[v] = t;
					al_start_timer(t);
					return t;
				}
				else
				{
					return it->second;
				}
			}
			void events::setRunningTimer(const int v, const bool a)
			{
				ALLEGRO_TIMER * t = nullptr;
				std::map<int, ALLEGRO_TIMER*>::iterator it = timers.find(v);
				if (it != timers.end())
				{
					t = it->second;
					if (a) al_stop_timer(t);
					else al_start_timer(t);
				}
			}
			void events::delTimer(const int v)
			{
				if (v < 0)
				{
					for (auto& i : timers) {
						al_stop_timer(i.second);
						al_destroy_timer(i.second);
					}
					timers.clear();
					return;
				}

				ALLEGRO_TIMER * t = nullptr;
				std::map<int, ALLEGRO_TIMER*>::iterator it = timers.find(v);
				if (it != timers.end())
				{
					t = it->second;
					al_stop_timer(t);
					al_destroy_timer(t);
					timers.erase(v);
				}
			}

			const bool events::getKey(const events_keys v, const bool e)
			{
				bool r = Tools::GetWithDef(keys, v, false);
				if (e) keys[v] = false;
				return r;
			}
			void events::getMouse(float & v, const event_xy o)
			{
				v = axis[o];
			}
			const bool events::getLastString(Safer::safe_string& u, const bool clr)
			{
				if (lastString.g().length() > 0) {
					u = lastString;
					if (clr) lastString.clear();
					return true;
				}
				return false;
			}
			const bool events::getCurrentString(Safer::safe_string& u, const bool clr)
			{
				if (currString.g().length() > 0) {
					u = currString;
					if (clr) currString.clear();
					return true;
				}
				return false;
			}

			/*const unsigned events::getCurrentMaxTickCounted()
			{
				return last_log.loops_per_second;
			}*/

			const bool events::_nextEv(ALLEGRO_EVENT& e, const bool wait)
			{
				if (ev_qu) {
					if (!wait) return al_get_next_event(ev_qu, &e); // bugs
					else {
						al_wait_for_event(ev_qu, &e);
						return true;
					}
				}
				return false;
			}
			void events::_setKey(const events_keys v, const bool k)
			{
				keys[v] = k;
			}
			void events::_confirm_initialized_thr()
			{
				alright = true;
			}
			const bool events::_keep()
			{
				return alright;
			}
			void events::_update_functions()
			{
				for (auto& i : funcs)
				{					
					i->work();
				}
			}
			void events::_setMousePos(const float x, const float y)
			{
				axis[0] = x;
				axis[1] = y;
			}
			void events::_setLastString(const Safer::safe_string u)
			{
				lastString = u;
			}
			void events::_setCurrString(const Safer::safe_string u)
			{
				currString = u;
			}

			/*void events::_setMaxTickCount(const unsigned u)
			{
				ticks_one_sec = u;
			}*/

			const double events::_getMultiplierForUpdatingImg()
			{
				return multiplier_hz_d;
			}

			ALLEGRO_DISPLAY * events::_getTargD()
			{
				return targ;
			}

			void events::_setThrRunning(const bool b)
			{
				thr_running_yet = b;
			}

			_event_log& events::_getEventLog()
			{
				return last_log;
			}

			/*void events::_setEventLog(const _event_log & u)
			{
				last_log = u;
			}*/

			void events::_setShutDownNow()
			{
				alright = false;
			}

			const bool events::isSafeToQuit()
			{
				return !thr_running_yet;
			}




			void _thread_ev(events* e)
			{
				if (!e) return;
				e->_confirm_initialized_thr();

				d_sprite_database spr_data;
				_string_work wrk;
				Log::gfile glog;
				//unsigned count = 0;
				_event_log& tlogging = e->_getEventLog();
				double last = al_get_time();

				glog << Log::NEEDED_START << Log::_func("_thread_ev", "") << "Preparing smaller threads..." << Log::NEEDED_ENDL;

				// multithread start:
				std::thread* thrs[3];
				bool arethey[3];

				for (int p = 0; p < 3; p++) {
					thrs[p] = nullptr;
					arethey[p] = false;
				}

				thrs[0] = new std::thread(_i_thr_collisionTimed, &tlogging, e, Defaults::collision_timer, &arethey[0]);
				thrs[1] = new std::thread(_i_thr_functionsTimed, &tlogging, e, Defaults::functions_timer, &arethey[1]);
				thrs[2] = new std::thread(_i_thr_updatePosTimed, &tlogging, e, Defaults::updatepos_timer, &arethey[2]);

				glog << Log::START << Log::_func("_thread_ev", "") << "Waiting smaller threads..." << Log::ENDL;

				{
					bool isDone = false;
					while (!isDone) {
						isDone = true;
						for (auto& i : arethey) {
							if (!i) isDone = false;
						}
					}
				}

				glog << Log::START << Log::_func("_thread_ev", "") << "They seem ready. Let's go!" << Log::ENDL;


				/*double fixMultI = e->_getMultiplierForUpdatingImg();
				if (fixMultI > 1.0 / 60) fixMultI = 1.0 / 60;*/

				/*ALLEGRO_TIMER* collisionTimer = e->getTimer(0, Defaults::collision_timer);
				ALLEGRO_TIMER* functionsTimer = e->getTimer(1, Defaults::functions_timer);
				ALLEGRO_TIMER* updatePosTimer = e->getTimer(4, Defaults::updatepos_timer);*/

				//ALLEGRO_TIMER* checkEnd_Timer = e->getTimer(2, Defaults::checkEnd_timer);

				ALLEGRO_TIMER* calcLoopsTimer = e->getTimer(3, Defaults::calcLoops_timer);

				glog << Log::NEEDED_START << Log::_func("_thread_ev", "") << "Thread initialized successfully." << Log::NEEDED_ENDL;

				e->_setThrRunning(true);

				while (e->_keep())
				{
					ALLEGRO_EVENT ev;

					// simple tasks
					if (e->getKey(CUSTOMEVENT_RESETCURRSTRING)) {
						wrk.clearall();
					}

					if (e->_nextEv(ev, true))
					{
						tlogging._loops++;
						if (ev.type == ALLEGRO_EVENT_TIMER)
						{
							/*if (ev.timer.source == collisionTimer)
							{
								_i_thr_collisionTimed(&tlogging, e);
							}*/
							/*else if (ev.timer.source == functionsTimer)
							{
								_i_thr_functionsTimed(&tlogging, e);
							}*/
							/*else if (ev.timer.source == checkEnd_Timer)
							{
								continue;
							}*/
							/*else */
							if (ev.timer.source == calcLoopsTimer)
							{
								double tii = al_get_time();
								tlogging.calcLoopsTimer_tps = 1.0 / (tii - tlogging._calcLoopsTimer_last);
								tlogging._calcLoopsTimer_last = tii;

								tlogging.loops_per_second = tlogging._loops;
								tlogging._loops = 0;

							}
							/*else if (ev.timer.source == updatePosTimer)
							{
								_i_thr_updatePosTimed(&tlogging, e);
							}*/
						}
						else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
							glog << Log::START << Log::_func("_thread_ev", "") << "Display has been closed!" << Log::ENDL;
							glog.flush();							

							e->delTimer(-1); // all
							e->_setShutDownNow();

							//exit(0); // change later
							continue;
						}
						else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
						{
							e->_setKey(CUSTOMEVENT_DISPLAY_RESIZED, true);
							e->_setKey(CUSTOMEVENT_BITMAPS_RELOAD, true);
						}
						else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
						{
							float ax = ev.mouse.x;
							float ay = ev.mouse.y;
							int dx = al_get_display_width(ev.mouse.display);
							int dy = al_get_display_height(ev.mouse.display);

							ax /= 1.0 * dx;
							ay /= 1.0 * dy;
							ax = ax * 2.0 - 1.0;
							ay = ay * 2.0 - 1.0;

							e->_setMousePos(ax, ay);
						}
						else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
						{
							if (ev.mouse.button - 1 + MOUSE_0 <= MOUSE_9) {
								e->_setKey((events_keys)(ev.mouse.button + MOUSE_0 - 1), true);
								glog << Log::START << Log::_func("_thread_ev", "") << "Mouse key pressed: N" << ev.mouse.button << Log::ENDL;
							}
						}
						else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
						{
							if (ev.mouse.button - 1 + MOUSE_0 <= MOUSE_9) {
								e->_setKey((events_keys)(ev.mouse.button + MOUSE_0 - 1), false);
								glog << Log::START << Log::_func("_thread_ev", "") << "Mouse key released: N" << ev.mouse.button << Log::ENDL;
							}
						}

						else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
						{
							e->_setKey((events_keys)ev.keyboard.keycode, true);
							glog << Log::START << Log::_func("_thread_ev", "") << "Detected keyboard pressed: N" << ev.keyboard.keycode << Log::ENDL;

							{ // interpret
								Config::config conf;
								int t;

								conf.get(Config::KEY_FULLSCREEN, t, Defaults::Events::key_fullscreen);
								if (ev.keyboard.keycode == t) {
									e->_setKey(CUSTOMEVENT_DISPLAY_TOGGLED, true);
									e->_setKey(CUSTOMEVENT_BITMAPS_RELOAD, true);
								}
								conf.get(Config::KEY_OSD, t, Defaults::Events::key_osd);
								if (ev.keyboard.keycode == t) {
									e->_setKey(CUSTOMEVENT_TOGGLE_OSD, true);
								}
							}
						}
						else if (ev.type == ALLEGRO_EVENT_KEY_UP)
						{
							e->_setKey((events_keys)ev.keyboard.keycode, false);
							glog << Log::START << Log::_func("_thread_ev", "") << "Detected keyboard released: N" << ev.keyboard.keycode << Log::ENDL;
						}

						else if (ev.type == ALLEGRO_EVENT_KEY_UP || ev.type == ALLEGRO_EVENT_KEY_CHAR)
						{

							wrk.interpret(ev.keyboard.unichar, ev.keyboard.keycode);

							Safer::safe_string str, str2;
							wrk.getOnGoingString(str2);
							e->_setCurrString(str2);

							wrk.getLastString(str);
							if (str.g().length() > 1) {
								wrk.clear();
								glog << Log::START << Log::_func("_thread_ev", "") << "New string for last string: " << str << Log::ENDL;
								e->_setLastString(str);
							}

							hack_strings val;
							if (wrk.isHack(val))
							{
								switch (val)
								{
								case HACK_IMMORTAL:
									glog << Log::NEEDED_START << Log::_func("_thread_ev", "") << "GOT IMMORTAL HACK CODE!" << str << Log::NEEDED_ENDL;
									break;
								case HACK_NEXTLEVEL:
									glog << Log::NEEDED_START << Log::_func("_thread_ev", "") << "GOT NEXTLEVEL HACK CODE!" << str << Log::NEEDED_ENDL;
									break;
								}
							}
						}
					}
				}
				glog << Log::NEEDED_START << Log::_func("_thread_ev", "") << "Got call for end. Waiting smaller threads..." << Log::NEEDED_ENDL;

				{
					bool isDone = false;
					while (!isDone) {
						isDone = true;
						for (auto& i : arethey) {
							if (i) isDone = false;
						}
					}
				}

				glog << Log::NEEDED_START << Log::_func("_thread_ev", "") << "Done. Bye!" << Log::NEEDED_ENDL;

				e->_setThrRunning(false);
			}
			big_event::~big_event()
			{
				if (!eventr) {
					eventr->stop();
					delete eventr;
					eventr = nullptr;
				}
			}
			void big_event::initInstance()
			{
				if (!eventr) eventr = new events();
			}
			void big_event::initSetupAndRun()
			{
				if (eventr)
				{
					eventr->setup();
				}
			}
			events & big_event::g()
			{
				assert(eventr);
				return *eventr;
			}

			void defaultfunction_add(void* va, double& d)
			{				
				add_t_s* v = (add_t_s*)va;
				d += v->adding;
			}
			void defaultfunction_cos(void* va, double& d)
			{
				cos_t_s* v = (cos_t_s*)va;
				d = v->default_val + cos(al_get_time() * v->time_multiplier) * v->ampl;
			}



			void _i_thr_collisionTimed(_event_log* evl, events* e, double min_timer, bool* amIRunning)
			{
				Log::gfile logg;
				int error_count = 0;

				if (!evl || !e || !amIRunning) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::ERRR) << "FATAL ERROR AT _i_thr_collisionTimed: NULL POINTER AT START!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}

				ALLEGRO_EVENT_QUEUE* ev_qu_internal = nullptr;
				if (!(ev_qu_internal = al_create_event_queue())) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::ERRR) << "FATAL ERROR AT _i_thr_collisionTimed: Cannot setup Event queue!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}
				ALLEGRO_TIMER* ev_timer_internal = nullptr;
				if (!(ev_timer_internal = al_create_timer(min_timer))) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::ERRR) << "FATAL ERROR AT _i_thr_collisionTimed: Cannot setup Timer for thread!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}
				al_register_event_source(ev_qu_internal, al_get_timer_event_source(ev_timer_internal));
				al_start_timer(ev_timer_internal);

				logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "") << "Thread initialized successfully." << Log::NEEDED_ENDL;
				*amIRunning = true;

				while (e->_keep()) {
					ALLEGRO_EVENT ev;
					al_wait_for_event(ev_qu_internal, &ev); // it can be only the timer.

					double tii = al_get_time();
					evl->collisionTimer_tps = 1.0 / (tii - evl->_collisionTimer_last);
					evl->_collisionTimer_last = tii;

					Layer::layerer lyr;
					d_sprite_database spr_data;
					//Safer::safe_pointer<Sprite::sprite> spr, *spr2;

					switch (lyr.getNow().getMode())
					{
					case Layer::USEMAP:
						try
						{
							Map::map* map = ((Map::map*)lyr.getNow().get_package());
							if (map) {
								map->cpuTask();
							}
						}
						catch (...)
						{
							if (++error_count < 10) {
								logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::WARN) << " _i_thr_collisionTimed got an exception. Trying to leave it as is (skippable)" << Log::NEEDED_ENDL;
								logg.flush();
							}
						}
						break;
					case Layer::STANDARD:
						try {
							for (auto& i : lyr.getNow().work())
							{
								// START
								for (size_t pos = 0; pos < spr_data.work().work().size();)
								{
									auto spr = spr_data.work().get(pos);
									if (Safer::_check_pointer_existance<Sprite::sprite>(spr)) {
										{// ------------------------------------------------------
											bool skip;
											spr->get(Sprite::_SKIP_DEFAULT_COLLISION_METHOD, skip);
											if (skip) continue;

											int u, m;
											spr->get(Sprite::LAYER, u);

											if (u == i.first) {

												spr->_resetCollision();

												// START2
												for (size_t pos = 0; pos < spr_data.work().work().size();)
												{
													auto spr2 = spr_data.work().get(pos);
													if (Safer::_check_pointer_existance<Sprite::sprite>(spr2)) {
														// ------------------------------------------------------
														if (spr != spr2) {
															spr2->get(Sprite::_SKIP_DEFAULT_COLLISION_METHOD, skip);
															if (skip) continue;

															spr2->get(Sprite::LAYER, m);

															for (auto& a : i.second.collides_with)
															{
																if (a == m) {
																	spr->_verifyCollision(*spr2);
																	break;
																}
															}
														}
														// ------------------------------------------------------
														pos++;
													}
													else {
														spr_data.work().erase(pos);
													}
												}
											}
										}// ------------------------------------------------------
										pos++;
									}
									else {
										spr_data.work().erase(pos);
									}
								}
							}
						}
						catch (const Safer::safe_string& s)
						{
							logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::WARN) << " Caught throw at _i_thr_collisionTimed: " << s << Log::NEEDED_ENDL;
							logg.flush();
							if (!al_get_current_display()) throw 0;
							throw s;
						}
						catch (const std::string& s)
						{
							logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::WARN) << " Caught throw at _i_thr_collisionTimed: " << s << Log::NEEDED_ENDL;
							logg.flush();
							if (!al_get_current_display()) throw 0;
							throw s;
						}
						catch (const char* s)
						{
							logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::WARN) << " Caught throw at _i_thr_collisionTimed: " << s << Log::NEEDED_ENDL;
							logg.flush();
							if (!al_get_current_display()) throw 0;
							throw s;
						}
						catch (const int i)
						{
							logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::WARN) << " Caught throw at _i_thr_collisionTimed: Error code #" << i << Log::NEEDED_ENDL;
							logg.flush();
							if (!al_get_current_display()) throw 0;
							throw i;
						}
						catch (...)
						{
							logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "", Log::WARN) << " Caught throw at _i_thr_collisionTimed: Unknown error." << Log::NEEDED_ENDL;
							logg.flush();
							if (!al_get_current_display()) throw 0;
							throw 1;
						}
						break;
					}
				}

				logg << Log::NEEDED_START << Log::_func("_i_thr_collisionTimed", "") << "Thread has quit the usual way." << Log::NEEDED_ENDL;

				al_stop_timer(ev_timer_internal);
				al_unregister_event_source(ev_qu_internal, al_get_timer_event_source(ev_timer_internal));
				al_destroy_timer(ev_timer_internal);
				al_destroy_event_queue(ev_qu_internal);

				*amIRunning = false;
			}


			void _i_thr_functionsTimed(_event_log* evl, events* e, double min_timer, bool* amIRunning)
			{				
				Log::gfile logg;
				if (!evl || !e || !amIRunning) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_functionsTimed", "", Log::ERRR) << " FATAL ERROR AT _i_thr_collisionTimed: NULL POINTER AT START!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}

				ALLEGRO_EVENT_QUEUE* ev_qu_internal = nullptr;
				if (!(ev_qu_internal = al_create_event_queue())) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_functionsTimed", "", Log::ERRR) << "FATAL ERROR AT _i_thr_functionsTimed: Cannot setup Event queue!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}
				ALLEGRO_TIMER* ev_timer_internal = nullptr;
				if (!(ev_timer_internal = al_create_timer(min_timer))) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_functionsTimed", "", Log::ERRR) << "FATAL ERROR AT _i_thr_functionsTimed: Cannot setup Timer for thread!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}
				al_register_event_source(ev_qu_internal, al_get_timer_event_source(ev_timer_internal));
				al_start_timer(ev_timer_internal);

				logg << Log::NEEDED_START << Log::_func("_i_thr_functionsTimed", "") << "Thread initialized successfully." << Log::NEEDED_ENDL;

				*amIRunning = true;

				while (e->_keep()) {
					ALLEGRO_EVENT ev;
					al_wait_for_event(ev_qu_internal, &ev); // it can be only the timer.

					double tii = al_get_time();
					evl->functionsTimer_tps = 1.0 / (tii - evl->_functionsTimer_last);
					evl->_functionsTimer_last = tii;

					e->_update_functions();
				}

				logg << Log::NEEDED_START << Log::_func("_i_thr_functionsTimed", "") << "Thread has quit the usual way." << Log::NEEDED_ENDL;

				al_stop_timer(ev_timer_internal);
				al_unregister_event_source(ev_qu_internal, al_get_timer_event_source(ev_timer_internal));
				al_destroy_timer(ev_timer_internal);
				al_destroy_event_queue(ev_qu_internal);

				*amIRunning = false;
			}


			void _i_thr_updatePosTimed(_event_log* evl, events* e, double min_timer, bool* amIRunning)
			{
				Log::gfile logg;
				if (!evl || !e || !amIRunning) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_updatePosTimed", "", Log::ERRR) << "FATAL ERROR AT _i_thr_collisionTimed: NULL POINTER AT START!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}

				ALLEGRO_EVENT_QUEUE* ev_qu_internal = nullptr;
				if (!(ev_qu_internal = al_create_event_queue())) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_updatePosTimed", "", Log::ERRR) << "FATAL ERROR AT _i_thr_updatePosTimed: Cannot setup Event queue!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}
				ALLEGRO_TIMER* ev_timer_internal = nullptr;
				if (!(ev_timer_internal = al_create_timer(min_timer))) {
					logg << Log::NEEDED_START << Log::_func("_i_thr_updatePosTimed", "", Log::ERRR) << "FATAL ERROR AT _i_thr_updatePosTimed: Cannot setup Timer for thread!" << Log::NEEDED_ENDL;
					logg.flush();
					exit(EXIT_FAILURE);
				}
				al_register_event_source(ev_qu_internal, al_get_timer_event_source(ev_timer_internal));
				al_start_timer(ev_timer_internal);

				logg << Log::NEEDED_START << Log::_func("_i_thr_updatePosTimed", "") << "Thread initialized successfully." << Log::NEEDED_ENDL;

				*amIRunning = true;

				while (e->_keep()) {
					ALLEGRO_EVENT ev;
					al_wait_for_event(ev_qu_internal, &ev); // it can be only the timer.

					double tii = al_get_time();
					evl->updatePosTimer_tps = 1.0 / (tii - evl->_updatePosTimer_last);
					evl->_updatePosTimer_last = tii;

					d_sprite_database spr_data;

					for (size_t pos = 0; pos < spr_data.work().work().size();)
					{
						auto i = spr_data.work().get(pos);
						if (Safer::_check_pointer_existance<Sprite::sprite>(i)) {

							i->_updateAcceleration(/*fixMultI*/);

							pos++;
						}
						else {
							spr_data.work().erase(pos);
						}
					}


					Layer::layerer lyr;
					if (lyr.getNow().getMode() == Layer::USEMAP) {
						try {
							Map::map* map = ((Map::map*)lyr.getNow().get_package());
							if (map) {
								if (!map->isPaused())
								{
									d_entity_database ent_data;
									ent_data.draw();
								}
							}
						}
						catch (...)
						{
							logg << Log::NEEDED_START << Log::_func("_i_thr_updatePosTimed", "", Log::WARN) << "_i_thr_updatePosTimed got an exception. Trying to leave it as is (skippable)" << Log::NEEDED_ENDL;
						}
					}

					//Entities::entity* ent;

					/*for (size_t p = 0; p < ent_data.work().getMax(); p++)
					{
						ent_data.get(ent, p);
						switch (ent->getType())
						{
						case Entities::PLAYER:
							{
								Entities::player* pl = (Entities::player*)ent;
								pl->tick();
							}
							break;
						case Entities::BADBOY:
							{
								Entities::badboy* bb = (Entities::badboy*)ent;
								ent->tick();
							}
							break;
						}
					}*/
				}

				logg << Log::NEEDED_START << Log::_func("_i_thr_updatePosTimed", "") << "Thread has quit the usual way." << Log::NEEDED_ENDL;

				al_stop_timer(ev_timer_internal);
				al_unregister_event_source(ev_qu_internal, al_get_timer_event_source(ev_timer_internal));
				al_destroy_timer(ev_timer_internal);
				al_destroy_event_queue(ev_qu_internal);

				*amIRunning = false;
			}
		}
	}
}
