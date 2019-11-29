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

			struct __shared_routines {
				size_t threadcount = 0;
				std::mutex threadcountm;
				bool should_exit = true;
			};
			
			/*************************************************************************************
			   
			   # Advanced template:
			   > Multiple times at once, automatic creation and fast use

			**************************************************************************************/

			template <size_t... Args>
			class __template_multiple_timers {
				const size_t num_args = sizeof...(Args);
				const size_t calls_per_sec_pos = sizeof...(Args);

				size_t timers_t[sizeof...(Args)] = { Args... };
				ALLEGRO_TIMER* timers_p[sizeof...(Args)] = { nullptr };

				ALLEGRO_TIMER* eachsec = nullptr;
				double eachsec_doubleverif = 0;

				size_t benchmark[sizeof...(Args) + 1] = { 0 };
				size_t rawcount[sizeof...(Args) + 1] = { 0 };

				ALLEGRO_EVENT_QUEUE* queue = nullptr;
				ALLEGRO_EVENT lastev;
			public:
				__template_multiple_timers()
				{
					queue = al_create_event_queue();

					eachsec = al_create_timer(1.0);
					al_register_event_source(queue, al_get_timer_event_source(eachsec));

					for (size_t p = 0; p < num_args; p++) {
						timers_p[p] = al_create_timer(1.0 / timers_t[p]);
						al_register_event_source(queue, al_get_timer_event_source(timers_p[p]));
					}
				}
				~__template_multiple_timers()
				{
					stop_timers();
					for (size_t p = 0; p < num_args; p++) al_destroy_timer(timers_p[p]);
					al_destroy_timer(eachsec);
					al_destroy_event_queue(queue);
				}
				void start_timers()
				{
					for (size_t p = 0; p < num_args; p++) al_start_timer(timers_p[p]);
					al_start_timer(eachsec);
				}
				void stop_timers()
				{
					for (size_t p = 0; p < num_args; p++) al_stop_timer(timers_p[p]);
					al_stop_timer(eachsec);
				}
				void insert(ALLEGRO_EVENT_SOURCE* evs)
				{
					al_register_event_source(queue, evs);
				}
				void remove(ALLEGRO_EVENT_SOURCE* evs)
				{
					al_unregister_event_source(queue, evs);
				}
				size_t getReport(const size_t t)
				{
					assert(t < num_args);
					return benchmark[t];
				}
				size_t getReportExpected(const size_t t)
				{
					assert(t < num_args);
					return timers_t[t];
				}
				size_t getNumCalls()
				{
					return benchmark[calls_per_sec_pos];
				}
				void hasWaitEvent()
				{
					rawcount[calls_per_sec_pos]++;

					al_wait_for_event(queue, &lastev);

					for (size_t u = 0; u < num_args; u++) {
						if (isThisThis(u)) rawcount[u]++;
					}

					if (lastev.type == ALLEGRO_EVENT_TIMER && lastev.timer.source == eachsec) {

						double timee = al_get_time();
						if (eachsec_doubleverif == 0) eachsec_doubleverif = timee - 1.0;
						double diff = timee - eachsec_doubleverif;

						if (diff > 5.0) {
							al_flush_event_queue(queue); // there's something lagging, so clear and refresh
							// cast warn on log?
							eachsec_doubleverif = timee - 1.0;
							printf_s("[WARN] Can't keep up! Somewhere is having some trouble keeping the loops!\n");
						}

						eachsec_doubleverif += 1.0;

						for (size_t u = 0; u < num_args + 1; u++) { // + 1 because calls_per_sec_pos is one ahead
							benchmark[u] = rawcount[u];
							rawcount[u] = 0;
						}
					}
				}
				bool hasEvent()
				{
					rawcount[calls_per_sec_pos]++;

					if (al_get_next_event(queue, &lastev)) {

						for (size_t u = 0; u < num_args; u++) {
							if (isThisThis(u)) rawcount[u]++;
						}

						if (lastev.type == ALLEGRO_EVENT_TIMER && lastev.timer.source == eachsec) {

							double timee = al_get_time();
							if (eachsec_doubleverif == 0) eachsec_doubleverif = timee - 1.0;
							double diff = timee - eachsec_doubleverif;

							if (diff > 5.0) {
								al_flush_event_queue(queue); // there's something lagging, so clear and refresh
								// cast warn on log?
								eachsec_doubleverif = timee - 1.0;
								printf_s("[WARN] Can't keep up! Somewhere is having some trouble keeping the loops!\n");
							}							

							eachsec_doubleverif += 1.0;

							for (size_t u = 0; u < num_args + 1; u++) { // + 1 because calls_per_sec_pos is one ahead
								benchmark[u] = rawcount[u];
								rawcount[u] = 0;
							}
						}

						return true;
					}

					return false;
				}
				bool isThisThis(const size_t t)
				{
					if (!(lastev.type == ALLEGRO_EVENT_TIMER)) return false;
					return (lastev.timer.source == timers_p[t]);
				}
				ALLEGRO_EVENT soWhat()
				{
					return lastev;
				}
			};


			enum class __display_routines_timers { LOOPTRACK, CHECKKEEP, CHECKMEMORYBITMAP, CHECKACKNOWLEDGE };
			enum class __collision_routines_timers { LOOPTRACK, CHECKKEEP, COLLISIONWORK };
			enum class __keyboardm_routines_timers { LOOPTRACK, CHECKKEEP, CHECKDISPLAYRESIZE };

			typedef __template_multiple_timers<1, 2, 2, 10> __display_routines;
			typedef __template_multiple_timers<1, 5, 30>	__collision_routines;
			typedef __template_multiple_timers<1, 5, 5>	__keyboardmouse_routines;




		}

		class Console {
			__raw_display* md = nullptr; // HANDLED INTERNALLY ON THREAD

			Textures gimg;
			Camera gcam;

			Assistance::__shared_routines thr_shared_arg; // shared

			/// DISPLAY AND DRAW
			std::thread* thr_md = nullptr; // unleash framerate
			//std::function <void(void*)> thr_md_func; // if user wants to run something before the actual init
			Assistance::__display_routines* thr_md_arg; // already in here oops, HANDLED INTERNALLY ON THREAD

			/// COLLISION AND SPRITES
			std::thread* thr_cl = nullptr; // unleash collision work
			//std::function <void(void*)> thr_cl_func; // if user wants to run something before the actual init
			Assistance::__collision_routines* thr_cl_arg; // already in here oops, HANDLED INTERNALLY ON THREAD
			
			/// KEYBOARD, MOUSE AND STUFF
			std::thread* thr_kb = nullptr; // unleash collision work
			//std::function <void(void*)> thr_kb_func; // if user wants to run something before the actual init
			Assistance::__keyboardmouse_routines* thr_kb_arg; // already in here oops, HANDLED INTERNALLY ON THREAD

			void __l_thr_md();
			void __l_thr_cl();
			void __l_thr_kb();
		public:
			~Console();

			void launch();
			//void launch(const std::function <void(void*)>, const std::function <void(void*)>, const std::function <void(void*)>); // display, events, kb?
			void close();

			bool running();
		};

	}
}