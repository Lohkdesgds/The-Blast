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
#include "..\logger\logger.h"

#define BLACK	al_map_rgb(0,0,0)
#define WHITE	al_map_rgb(255,255,255)
#define GREEN	al_map_rgb(0,255,0)
#define RED		al_map_rgb(255,0,0)
#define BLUE	al_map_rgb(0,0,255)


namespace LSW {
	namespace v4 {

		namespace Constants {

			template<typename H> const auto lambda_null_load = [](const char* p, H*& r) -> bool { return false; };
			template<typename H> const auto lambda_null_unload = [](H*& b) -> void { return; };
			
			template<typename K> const auto lambda_default_load = [](const char* p,K*& r) -> bool { return (r = new K()); };
			template<typename K> const auto lambda_default_unload = [](K*& b) -> void { if (b) delete b; b = nullptr; };
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
				stop();
				for (size_t p = 0; p < num_args; p++) al_destroy_timer(timers_p[p]);
				al_destroy_timer(eachsec);
				al_destroy_event_queue(queue);
			}
			void start()
			{
				for (size_t p = 0; p < num_args; p++) al_start_timer(timers_p[p]);
				al_start_timer(eachsec);
			}
			void stop()
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
			void hasEventWait()
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

						gfile logg;
						logg << L::SLL << fsr(__FUNCSIG__, E::WARN) << "Can't keep up! Somewhere is having some trouble keeping the loops! Running " << (diff - 1.0) << " second(s) behind." << L::BLL;
						//printf_s("[WARN] Can't keep up! Somewhere is having some trouble keeping the loops!\n");
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

							gfile logg;

							eachsec_doubleverif = timee - 1.0;
							logg << L::SLL << fsr(__FUNCSIG__, E::WARN) << "Can't keep up! Somewhere is having some trouble keeping the loops! Running " << (diff - 1.0) << " second(s) behind." << L::BLL;
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
			ALLEGRO_EVENT getEventRaw()
			{
				return lastev;
			}
		};



		/*************************************************************************************

			# Advanced vector template:
			> Access to a database of T, T being anything, handled as you wish (custom std::function if needed)
			> Declaration doesn't need to set load/unload std::function s, so it can be accessed everywhere no problem

		**************************************************************************************/
		

		template<typename T>
		class __template_static_vector {
			template<typename S = T> struct _d {
				S* self = nullptr;
				std::string id;
			};
			template<typename Q = T> struct _i {
				std::vector< _d<Q>* >					db;
				std::mutex                              dbm;
				std::function<bool(const char*, Q*&)>	load; // cast void if different
				std::function<void(Q*&)>				unload;
			};

			static _i<T> data;
		public:
			//~__template_static_vector() { clear(); }

			void set(const std::function <bool(const char*, T*&)> hl, const std::function <void(T*&)> hd) {
				assert(hl);
				assert(hd);
				data.load = hl;
				data.unload = hd;
			}
			bool try_lock() {
				return data.dbm.try_lock();
			}
			size_t size() {
				return data.db.size();
			}
			auto begin() {
				return data.db.begin();
			}
			auto end() {
				return data.db.end();
			}
			void unlock() {
				data.dbm.unlock();
			}
			T* create(const std::string a, const std::string b = "") { return load(a, b); }
			T* load(const std::string id, const std::string path = "")	{
				T* b = nullptr;
				bool r = get(id, b);

				if (!r) {
					_d<T>* dt = new _d<T>();
					dt->id = id;

					r = data.load(path.c_str(), dt->self);
					assert(r);
					assert(dt->self);
					b = dt->self;

					data.dbm.lock();
					data.db.push_back(dt);
					data.dbm.unlock();
				}
				return b;
			}
			void create(std::vector<std::string> n, std::vector<std::string> q) { load(n, q); }
			void load(std::vector<std::string> n, std::vector<std::string> q, float* perc = nullptr)
			{
				assert(n.size() == q.size());

				for (size_t p = 0; p < n.size(); p++)
				{
					if (perc) *perc = 1.0f * p / n.size();
					load(n[p], q[p]);
				}
				if (perc) *perc = 1.00;
			}
			bool get(const std::string id, T*& p) {
				data.dbm.lock();
				for (auto& i : data.db) {
					if (i->id == id) {
						p = i->self;
						data.dbm.unlock();
						return true;
					}
				}
				data.dbm.unlock();
				return false;
			}
			void remove(const std::string id) {
				for (size_t p = 0; p < data.db.size(); p++)
				{
					if (data.db[p]->id == id) {
						data.dbm.lock();
						data.unload(data.db[p]->self);
						data.db.erase(data.db.begin() + p);
						data.dbm.unlock();
					}
				}
			}
			void clear() {
				data.dbm.lock();
				for (auto& i : data.db)
				{
					data.unload(i->self);
				}
				data.db.clear();
				data.dbm.unlock();
			}
		};

		template<typename T> __template_static_vector<T>::_i<T> __template_static_vector<T>::data;
	}
}