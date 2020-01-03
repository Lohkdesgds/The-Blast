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
#include <mutex>

#include "..\custom_abort\abort.h"
#include "small_templates.h"
#include "..\logger\logger.h"


namespace LSW {
	namespace v4 {

		/*************************************************************************************

			# Two key map:
			> Two keys to a same position

		**************************************************************************************/

		template <typename A, typename B, typename C> // what is being allocated, key 1, key 2
		class __map {
			std::vector<A*> aaa;
			std::vector<B> bbb;
			std::vector<C> ccc;
		public:
			A& create(const B b, const C c) {
				A* a = new A();
				aaa.push_back(a);
				bbb.push_back(b);
				ccc.push_back(c);
				return *a;
			}
			A& get(const B key) {
				for (size_t p = 0; p < bbb.size(); p++) {
					if (bbb[p] == key) return *aaa[p];
				}
				throw Abort::abort(__FUNCSIG__, "OUT OF RANGE");
				return *aaa[0];
			}
			A& get(const C key) {
				for (size_t p = 0; p < ccc.size(); p++) {
					if (ccc[p] == key) return *aaa[p];
				}
				throw Abort::abort(__FUNCSIG__, "OUT OF RANGE");
				return *aaa[0];
			}
			A& get(const size_t key) {
				if (key < aaa.size()) return *aaa[key];
				throw Abort::abort(__FUNCSIG__, "OUT OF RANGE");
				return *aaa[0];
			}

			bool getFirstKey(A& val, B& sav) {
				for (size_t p = 0; p < aaa.size(); p++) {
					if (aaa[p] == &val) {
						sav = bbb[p];
						return true;
					}
				}
				return false;
			}
			bool getFirstKey(size_t key, B& sav) {
				if (key < bbb.size()) {
					sav = bbb[key];
					return true;
				}
				return false;
			}
			bool getSecondKey(A& val, C& sav) {
				for (size_t p = 0; p < aaa.size(); p++) {
					if (aaa[p] == &val) {
						sav = ccc[p];
						return true;
					}
				}
				return false;
			}
			bool getSecondKey(size_t key, C& sav) {
				if (key < ccc.size()) {
					sav = ccc[key];
					return true;
				}
				return false;
			}

			void erase(const B key)
			{
				for (size_t p = 0; p < bbb.size(); p++) {
					if (bbb[p] == key) {
						aaa.erase(aaa.begin() + p);
						bbb.erase(bbb.begin() + p);
						ccc.erase(ccc.begin() + p);
						return;
					}
				}
			}
			void erase(const C key)
			{
				for (size_t p = 0; p < ccc.size(); p++) {
					if (ccc[p] == key) {
						aaa.erase(aaa.begin() + p);
						bbb.erase(bbb.begin() + p);
						ccc.erase(ccc.begin() + p);
						return;
					}
				}
			}
			void erase(const size_t p)
			{
				if (p >= aaa.size()) {
					throw Abort::abort(__FUNCSIG__, "OUT OF RANGE");
					return;
				}
				aaa.erase(aaa.begin() + p);
				bbb.erase(bbb.begin() + p);
				ccc.erase(ccc.begin() + p);
			}
			void clear()
			{
				while (aaa.size() > 0) erase(0);
			}

			A& operator[](const B p)
			{
				return get(p);
			}
			A& operator[](const C p)
			{
				return get(p);
			}
			A& operator[](const size_t p)
			{
				return get(p);
			}

			size_t size() {
				return aaa.size();
			}
		};

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
			ALLEGRO_EVENT lastev = ALLEGRO_EVENT();
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
				for (bool can_leave = false; !can_leave;) {
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
							logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Can't keep up! Somewhere is having some trouble keeping the loops! Running " << (diff - 1.0) << " second(s) behind." << L::ELF;
							//printf_s("[WARN] Can't keep up! Somewhere is having some trouble keeping the loops!\n");
						}

						eachsec_doubleverif += 1.0;

						for (size_t u = 0; u < num_args + 1; u++) { // + 1 because calls_per_sec_pos is one ahead
							benchmark[u] = rawcount[u];
							rawcount[u] = 0;
						}
					}
					else can_leave = true;
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
							logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Can't keep up! Somewhere is having some trouble keeping the loops! Running " << (diff - 1.0) << " second(s) behind." << L::ELF;
						}

						eachsec_doubleverif += 1.0;

						for (size_t u = 0; u < num_args + 1; u++) { // + 1 because calls_per_sec_pos is one ahead
							benchmark[u] = rawcount[u];
							rawcount[u] = 0;
						}

						return false;
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
				bool                                    dbmv = false; // verifier
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
			bool tryLock() {
				return (data.dbmv = data.dbm.try_lock());
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
				if (!data.dbmv) throw Abort::abort(__FUNCSIG__, "MUTEX UNLOCKED WHEN TRYING TO UNLOCK!");
				else data.dbm.unlock();
				data.dbmv = false;
			}
			T* customLoad(const std::string id, std::function<bool(T*&)> f) {
				T* b = nullptr;
				bool r = get(id, b);

				if (!r) {
					_d<T>* dt = new _d<T>();
					dt->id = id;

					r = f(dt->self);
					if (!r || !dt->self) throw Abort::abort(__FUNCSIG__, "Can't load a resource! id=" + id);
					b = dt->self;

					data.dbm.lock();
					data.db.push_back(dt);
					data.dbm.unlock();
				}
				return b;
			}
			T* create(const std::string id, const std::string path = "") { return load(id, path); }
			T* load(const std::string id, const std::string path = "")	{
				T* b = nullptr;
				bool r = get(id, b);

				if (!r) {
					_d<T>* dt = new _d<T>();
					dt->id = id;

					r = data.load(path.c_str(), dt->self);
					if (!r || !dt->self) throw Abort::abort(__FUNCSIG__, "Can't load a resource! id=" + id + ";path=" + path);
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