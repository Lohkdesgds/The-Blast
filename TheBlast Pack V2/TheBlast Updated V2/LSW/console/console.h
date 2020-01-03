#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_physfs.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <physfs.h>

#include <string>
#include <map>
#include <Windows.h>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include <locale>

#include "..\organizer\organizer.h"

#define BLACK al_map_rgb(0,0,0)

/*
* What is console?
* This is the console, system, the one controlling everything.
* It will work everything together.
*/

namespace LSW {
	namespace v4 {
		namespace Assistance {

			enum class ro__thread_display_routines_timers { FUNCTIONALITY, LOOPTRACK, CHECKKEEP, CHECKMEMORYBITMAP, UPDATELOGONSCREEN };
			enum class ro__thread_collision_routines_timers { FUNCTIONALITY, LOOPTRACK, CHECKKEEP, COLLISIONWORK };
			enum class ro__thread_keyboardm_routines_timers { FUNCTIONALITY, LOOPTRACK, CHECKKEEP, UPDATEMOUSE };
			enum class ro__thread_functional_routines_timers { FUNCTIONALITY_AND_CHECKNEWFUNCS };

			enum class io__thread_ids { ALL = -1, DRAWING, COLLIDING, USERINPUT, FUNCTIONAL };
			enum class io__threads_taskid { START, ONCE, END, size };

		}

		typedef __template_multiple_timers<Constants::internal_all_function_timing_run, 1, 2, 2, 5>													    __display_routines;
		typedef __template_multiple_timers<Constants::internal_all_function_timing_run, 1, 5, Constants::internal_collision_positioning_time_update>	__collision_routines;
		typedef __template_multiple_timers<Constants::internal_all_function_timing_run, 1, 5, 60>													    __keyboardmouse_routines;
		typedef __template_multiple_timers<Constants::internal_all_function_timing_run>																	__functional_routines;

		typedef __template_static_vector<ALLEGRO_BITMAP>  Textures;
		typedef __template_static_vector<ALLEGRO_FONT>    Fonts;
		typedef __template_static_vector<ALLEGRO_SAMPLE>  Samples;

		typedef __template_static_vector<Sprite>		  Sprites;
		typedef __template_static_vector<Text>			  Texts;
		typedef __template_static_vector<Track>			  Tracks;



		class Console {
			Display* md = nullptr; // HANDLED INTERNALLY ON THREAD

			Textures gimg;
			Camera gcam = Camera();

			struct __shared_routines {
				size_t threadcount = 0;
				std::mutex threadcountm;
				bool should_exit = true;
			} thr_shared_arg; // shared

			template <typename T>
			struct __each_routine {
				std::thread* thr_itself = nullptr; // unleash framerate
				bool am_i_running = false;
				T* thread_arguments = nullptr;
				bool pause_thread = false;
				std::function<void(void)> functions[+Assistance::io__threads_taskid::size]; // init does on init, once runs once, running does all the time, deinit deinit the thread
				bool has_called_once[+Assistance::io__threads_taskid::size] = { false };
				//std::mutex functions_m; // have I?
				bool tasking = false;
			};

			class __functional_functions {
			public:
				std::function<int(void)> func;
				int id = 0;
				LONGLONG calls = 0; // if negative, add and wait until 0 then run
				ALLEGRO_TIMER* the_timer = nullptr;
				bool has_timer_set_on_queue = false;
				bool should_run = true;

				bool operator==(const ALLEGRO_TIMER* t) const { return the_timer == t; }
				bool operator==(const int i) const { return i == id; }
				bool operator==(const __functional_functions& f) const { return *this == f; }
			};

			/// SHARED
			//std::function<void(void)> smth_to_load; // load textures here
			//int has_smth_to_load = -1; // has somethign to load
			//std::function<void(void)> howto_unload; // die how?
			//bool hasset_howto_unload = false;

			__each_routine<__display_routines>			thread_maindisplay;
			__each_routine<__collision_routines>		thread_collision;
			__each_routine<__keyboardmouse_routines>	thread_kbmouse;
			__each_routine< __functional_routines>		thread_functional;

			//std::map<intptr_t, __functional_functions> func_list;

			__map<__functional_functions, ALLEGRO_TIMER*, int> func_list_super;
			//std::vector<__functional_functions> f_dis;


			/// DISPLAY AND DRAW
			//std::thread* thr_md = nullptr; // unleash framerate
			//bool thr_md_upnrunnin = false;
			//__display_routines* thr_md_arg = nullptr; // already in here oops, HANDLED INTERNALLY ON THREAD
			//bool pause_thr_md = false;

			/// COLLISION AND SPRITES
			//std::thread* thr_cl = nullptr; // unleash collision work
			//bool thr_cl_upnrunnin = false;
			//__collision_routines* thr_cl_arg = nullptr; // already in here oops, HANDLED INTERNALLY ON THREAD
			//bool pause_thr_cl = false;

			/// KEYBOARD, MOUSE AND STUFF
			//std::thread* thr_kb = nullptr; // unleash collision work
			//bool thr_kb_upnrunnin = false;
			//__keyboardmouse_routines* thr_kb_arg = nullptr; // already in here oops, HANDLED INTERNALLY ON THREAD
			//bool pause_thr_kb = false;


			ALLEGRO_EVENT_SOURCE evsrc = ALLEGRO_EVENT_SOURCE();

			void __l_thr_md();
			void __l_thr_cl();
			void __l_thr_kb();
			void __l_thr_fc();

			void __l_thr_md_run(const Assistance::io__threads_taskid);
			void __l_thr_cl_run(const Assistance::io__threads_taskid);
			void __l_thr_kb_run(const Assistance::io__threads_taskid);
			void __l_thr_fc_run(const Assistance::io__threads_taskid);
		public:
			Console();
			~Console();

			void start();
			//void launch(const std::function <void(void*)>, const std::function <void(void*)>, const std::function <void(void*)>); // display, events, kb?
			void stop(); // will be called from isRunning() if shouldStop() was called somewhere
			void shouldStop(); // won't hold

			bool isOpen();
			bool isRunning();

			void resetSimpleOnceTask(const Assistance::io__thread_ids);
			void setSimpleTask(const Assistance::io__thread_ids, const Assistance::io__threads_taskid, std::function<void(void)>);
			void unsetSimpleTask(const Assistance::io__thread_ids, const Assistance::io__threads_taskid);
			bool hasTasked(const Assistance::io__thread_ids, const Assistance::io__threads_taskid);

			size_t getCallsPerSecondOnThread(const Assistance::io__thread_ids = Assistance::io__thread_ids::ALL);

			void pauseThread(const Assistance::io__thread_ids = Assistance::io__thread_ids::ALL);
			void resumeThread(const Assistance::io__thread_ids = Assistance::io__thread_ids::ALL);

			void addCustomTask(std::function<int(void)>, const int, const double); // t/sec
			void delayCustomTaskTicksBy(const int, const LONGLONG);
			void removeCustomTask(const int);
		};

	}
}
// .h