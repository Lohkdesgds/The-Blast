#pragma once

#include "..\shared_constants\constants.h"
#include "..\custom_abort\abort.h"
#include "..\big_templates\small_templates.h"
#include "..\logger\logger.h"
#include "..\big_templates\big_templates.h"
#include "..\system\system.h"
#include "..\sound\sound.h"
#include "..\drawing\drawing.h"

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

#define BLACK al_map_rgb(0,0,0)

/*
* What is console?
* This is the console, system, the one controlling everything.
* It will work everything together.
*/

namespace LSW {
	namespace v4 {
		namespace Constants {

			enum class ro__thread_display_routines_timers { FUNCTIONALITY_ONCE, FUNCTIONALITY_LOOP, LOOPTRACK, CHECKMEMORYBITMAP, UPDATELOGONSCREEN };
			enum class ro__thread_collision_routines_timers { FUNCTIONALITY_ONCE, FUNCTIONALITY_LOOP, LOOPTRACK, COLLISIONWORK };
			enum class ro__thread_keyboardm_routines_timers { FUNCTIONALITY_ONCE, FUNCTIONALITY_LOOP, LOOPTRACK, UPDATEMOUSE };
			enum class ro__thread_functional_routines_timers { FUNCTIONALITY_ONCE_AND_CHECKNEWFUNCS, FUNCTIONALITY_LOOP, LOOPTRACK };

			enum class io__thread_ids { ALL = -1, DRAWING, COLLIDING, USERINPUT, FUNCTIONAL,
										__THREADS_COUNT};
			enum class io__threads_taskid { START, ONCE, LOOP, END, size };

		}

		typedef __template_multiple_timers<Constants::__i_func_t_once, Constants::__i_thr_loop_timer_0, 1, 2, 5>								__display_routines;
		typedef __template_multiple_timers<Constants::__i_func_t_once, Constants::__i_thr_loop_timer_1, 1, Constants::__i_col_pos_t_update>		__collision_routines;
		typedef __template_multiple_timers<Constants::__i_func_t_once, Constants::__i_thr_loop_timer_2, 1, 60>								    __keyboardmouse_routines;
		typedef __template_multiple_timers<Constants::__i_func_t_once, Constants::__i_thr_loop_timer_3, 1>										__functional_routines;



		class Manager {
			Display* md = nullptr; // HANDLED INTERNALLY ON THREAD

			ResourceOf<ALLEGRO_BITMAP> gimg;
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
				std::function<void(void)> functions[+Constants::io__threads_taskid::size]; // init does on init, once runs once, running does all the time, deinit deinit the thread
				bool has_called_once[+Constants::io__threads_taskid::size] = { false };
				//std::mutex functions_m; // have I?
				bool tasking = false;
				std::string threadid_str = "unknown";
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
			class __linkdebug_timing {
				ULONGLONG last_start = 0;
				unsigned last_difference = 0;
				double average_difference = 0;
			public:
				void start();
				void end();

				unsigned getLastDifference();
				double getAverageDifference();
			};

			/// SHARED
			//std::function<void(void)> smth_to_load; // load textures here
			//int has_smth_to_load = -1; // has somethign to load
			//std::function<void(void)> howto_unload; // die how?
			//bool hasset_howto_unload = false;

			__each_routine<__display_routines>			thread_maindisplay;
			__each_routine<__collision_routines>		thread_collision;
			__each_routine<__keyboardmouse_routines>	thread_kbmouse;
			__each_routine<__functional_routines>		thread_functional;

			//std::map<intptr_t, __functional_functions> func_list;

			__map<__functional_functions, ALLEGRO_TIMER*, int> func_list_super;
			std::map<int, __linkdebug_timing> func_list_timing;
			//std::vector<__functional_functions> f_dis;

			ALLEGRO_EVENT_SOURCE evsrc = ALLEGRO_EVENT_SOURCE();

			void __l_thr_md();
			void __l_thr_cl();
			void __l_thr_kb();
			void __l_thr_fc();

			void __l_thr_md_run(const Constants::io__threads_taskid);
			void __l_thr_cl_run(const Constants::io__threads_taskid);
			void __l_thr_kb_run(const Constants::io__threads_taskid);
			void __l_thr_fc_run(const Constants::io__threads_taskid);

			bool someoneIsRunning();
		public:
			Manager();
			~Manager();

			void start();
			//void launch(const std::function <void(void*)>, const std::function <void(void*)>, const std::function <void(void*)>); // display, events, kb?
			void stop(); // will be called from isRunning() if shouldStop() was called somewhere
			void shouldStop(); // won't hold

			bool isOpen();
			bool isRunning();

			void resetSimpleOnceTask(const Constants::io__thread_ids);
			void setSimpleTask(const Constants::io__thread_ids, const Constants::io__threads_taskid, std::function<void(void)>);
			void unsetSimpleTask(const Constants::io__thread_ids, const Constants::io__threads_taskid);
			bool hasTasked(const Constants::io__thread_ids, const Constants::io__threads_taskid);

			size_t getCallsPerSecondOnThread(const Constants::io__thread_ids = Constants::io__thread_ids::ALL);
			double getInstantSCallsOnThread(const Constants::io__thread_ids = Constants::io__thread_ids::ALL);

			void pauseThread(const Constants::io__thread_ids = Constants::io__thread_ids::ALL);
			bool hasThreadsButThisOnePaused(const Constants::io__thread_ids = Constants::io__thread_ids::ALL);
			bool hasThreadPaused(const Constants::io__thread_ids = Constants::io__thread_ids::ALL);
			void resumeThread(const Constants::io__thread_ids = Constants::io__thread_ids::ALL);

			void addCustomTask(std::function<int(void)>, const int, const double); // t/sec
			void delayCustomTaskTicksBy(const int, const LONGLONG);
			void removeCustomTask(const int);

			void sendEvent(const Constants::ro__my_events, const intptr_t, const intptr_t = 0, const intptr_t = 0, const intptr_t = 0); // you MUST know what you're doing

			Display* const _display_handle();
		};

	}
}
// .h