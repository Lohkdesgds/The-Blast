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

#include "..\custom_abort\abort.h"
#include "..\system\system.h"

#define BLACK	al_map_rgb(0,0,0)
#define WHITE	al_map_rgb(255,255,255)
#define GREEN	al_map_rgb(0,255,0)
#define RED		al_map_rgb(255,0,0)
#define BLUE	al_map_rgb(0,0,255)


namespace LSW {
	namespace v4 {

		namespace Assistance {

			template<typename N>
			struct __template_static_each_control {
				N* ptr;
				std::string id;
				std::string path;
			};

			template<typename H> const auto lambda_null_load = [](const char* p, H*& r) -> bool { return false; };//[](char* p, T*& r) -> bool { return (r = new T()); };
			template<typename H> const auto lambda_null_unload = [](H*& b) -> void { return; };//[](T*& b) -> void { delete b; b = nullptr; };
			
			template<typename K> const auto lambda_default_load = [](const char* p,K*& r) -> bool { return (r = new K()); };
			template<typename K> const auto lambda_default_unload = [](K*& b) -> void { if (b) delete b; b = nullptr; };
					   
			template<typename T>
			struct __template_static_vector_control {
				std::vector<__template_static_each_control<T>> data;

				std::function <bool(const char*,T*&)> load = lambda_null_load<T>; // cast void if different
				std::function <void(T*&)> unload = lambda_null_unload<T>;

				std::mutex hugedeal;
			};


		}


		/*************************************************************************************

			# Advanced timer template:
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



		/*************************************************************************************

			# Advanced vector template:
			> Access to a database of T, T being anything, handled as you wish (custom std::function if needed)
			> Declaration doesn't need to set load/unload std::function s, so it can be accessed everywhere no problem

		**************************************************************************************/
			   		 

		template <typename T>
		class __template_static_vector {
			static Assistance::__template_static_vector_control<T> data;
		public:
			~__template_static_vector() {
				clear();
			}
			__template_static_vector() {}
			__template_static_vector(const std::function <bool(const char*,T*&)> howtoload, const std::function <void(T*&)> howtounload)
			{
				if (howtoload)		data.load =		howtoload;
				if (howtounload)	data.unload =	howtounload;
			}
			void load(const std::string id, const char* path/*, const std::function <T * (char*)> howtoload = nullptr, const std::function <void(T*&)> howtounload = nullptr*/)
			{
				//setNewDel(howtoload, howtounload);

				T* i = nullptr;

				if (!get(id, i)) {
					Assistance::__template_static_each_control<T> jj;

					jj.path = path;
					jj.id = id;

					if (!(data.load(path, jj.ptr))) throw Abort::abort("lambda load", "__template_static_vector<>::load", "Couldn't load '" + id + "' @ path '" + path + "'");

					data.hugedeal.lock();
					data.data.push_back(jj);
					data.hugedeal.unlock();
				}
			}
			void load(std::vector<std::string> names, std::vector<std::string> paths) {

				if (names.size() != paths.size()) {
					throw Abort::abort("std::function<std::vector<std::string>()>", "__template_static_vector<>::load", "Names generated doesn't match paths generated size! (they have to be the same size!)");
					return;
				}
				for (size_t p = 0; p < names.size(); p++)
				{
					load(names[p], paths[p].c_str());
				}
			}
			bool get(const std::string id, T*& wptr)
			{
				for (Assistance::__template_static_each_control<T>& i : data.data) {
					if (id == i.id) {
						wptr = i.ptr;
						return true;
					}
				}
				return false;
			}
			void del(const std::string id)
			{
				for (size_t p = 0; p < data.data.size(); p++)
				{
					if (data.data[p].id == id) {
						data.unload(data.data[p]);
						data.hugedeal.lock();
						data.data.erase(data.data.begin() + p);
						data.hugedeal.unlock();
						return;
					}
				}
			}
			void clear()
			{
				data.hugedeal.lock();
				if (data.data.size() == 0) { data.hugedeal.unlock();  return; }

				for (auto& i : data.data) {
					data.unload(i.ptr);
				}
				data.data.clear();
				data.hugedeal.unlock();
			}
		};


		// init?
		template<typename T> Assistance::__template_static_vector_control<T> __template_static_vector<T>::data;
	}
}