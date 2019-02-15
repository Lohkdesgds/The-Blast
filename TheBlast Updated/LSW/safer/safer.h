#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <cassert>
#include <Windows.h>
#include <memory>

//#define NOT_SAFE_RIGHT_NOW
#define NOT_SAFE_RIGHT_NOW_STRING
#define TIMER_KILL_POINTERS 5000 // ms

namespace LSW {
	namespace v2 {
		namespace Safer {

			/*template<typename T>
			class __memory_alloc {
				T* obj = nullptr;
				bool must_kill = true;
				std::string id;
			public:
				__memory_alloc(T*&); // set this as its value. Naturally not responsible to delete it because it doesn't own it.
				__memory_alloc(); // create its value
				~__memory_alloc();

				void setKillAuto(const bool);

				T*& get();
				std::string& getS();
			};

			template<typename T>
			inline __memory_alloc<T>::__memory_alloc(T*& t)
			{
				if (!t) throw "at __memory_alloc::__memory_alloc: pointer should NEVER be NULL!";
				obj = t;
				must_kill = false;
			}
			template<typename T>
			inline __memory_alloc<T>::__memory_alloc()
			{
				if (!(obj = new T())) throw "at __memory_alloc::__memory_alloc: cannot alloc memory!";
				must_kill = true;
			}
			template<typename T>
			inline __memory_alloc<T>::~__memory_alloc()
			{
				if (must_kill) {
					if (obj) delete obj;
					obj = nullptr;
				}
			}
			template<typename T>
			inline void __memory_alloc<T>::setKillAuto(const bool b)
			{
				must_kill = b;
			}
			template<typename T>
			inline T*& __memory_alloc<T>::get()
			{
				return obj;
			}
			template<typename T>
			inline std::string& __memory_alloc<T>::getS()
			{
				return id;
			}*/

			
			template <typename T>
			using safe_pointer = std::shared_ptr<T>;
			template <typename T>
			using weak_pointer = std::weak_ptr<T>;
			template <typename T>
			using safer_vector = std::vector<safe_pointer<T>>;
			template <typename T>
			using weak_vector = std::vector<weak_pointer<T>>;

			/*template <typename T>
			struct __memory_autounref {
				safe_pointer<T> p;
				ULONGLONG now = 0;
			};*/

			template<typename T> // TODO
			class memory_controller {
				safer_vector<T> vec;
				//std::vector<__memory_autounref<T>> vec_dying;
				weak_vector<T> weak_count;
				std::mutex m;
			public:
				~memory_controller() {} // do nothing

				safe_pointer<T> create();

				template<typename V>
				safe_pointer<V> try_create_as();
				//void push(Safer::safe_pointer<T>);
				void erase(const size_t); // id

				safe_pointer<T> get(const size_t);

				void clear(); // all

				size_t check_weak_count();
				size_t check_weak_count_total_ptrs();

				safer_vector<T>& work();
				
				const safe_pointer<T> operator[](const size_t&) const;
				safe_pointer<T> operator[](const size_t&);
			};

			template<typename T>
			inline safe_pointer<T> memory_controller<T>::create()
			{
				safe_pointer<T> ptr = std::make_shared<T>();
				weak_pointer<T> wptr;
				wptr = ptr;

				m.lock();
				vec.push_back(ptr);
				weak_count.push_back(wptr);
				m.unlock();

				return ptr;
			}

			/*template<typename T>
			inline void memory_controller<T>::push(Safer::safe_pointer<T> rptr)
			{
				Safer::safe_pointer<T> ptr = rptr;
				weak_pointer<T> wptr;
				wptr = ptr;

				vec.push_back(ptr);
				weak_count.push_back(wptr);
			}*/

			template<typename T>
			inline void memory_controller<T>::erase(const size_t p)
			{
				if (p >= vec.size()) return;
				/*__memory_autounref<T> mem;
				mem.now = GetTickCount64();
				mem.p = vec[p];
				vec_dying.push_back(mem);*/
				m.lock();
				vec[p].reset();
				vec.erase(vec.begin() + p);
				m.unlock();
			}
			
			template<typename T>
			template<typename V>
			inline safe_pointer<V> memory_controller<T>::try_create_as()
			{
				safe_pointer<V> ptr_r = std::make_shared<V>();
				safe_pointer<T> ptr = std::dynamic_pointer_cast<T>(ptr_r);
				weak_pointer<T> wptr;
				wptr = ptr;

				m.lock();
				vec.push_back(ptr);
				weak_count.push_back(wptr);
				m.unlock();

				return ptr_r;
			}

			template<typename T>
			inline safe_pointer<T> memory_controller<T>::get(const size_t v)
			{
				return (*this)[v];
			}

			template<typename T>
			inline void memory_controller<T>::clear()
			{
				/*for (auto& i : vec) {
					__memory_autounref<T> mem;
					mem.now = GetTickCount64();
					mem.p = i;
					vec_dying.push_back(mem);
				}*/
				m.lock();
				vec.clear();
				m.unlock();
			}

			template<typename T>
			inline size_t memory_controller<T>::check_weak_count()
			{
				for (bool keep = true; keep;) {
					keep = false;
					size_t p = 0;

					for (auto& i : weak_count) {
						if (i.expired()) {
							keep = true;
							break;
						}
						p++;
					}

					if (keep) weak_count.erase(weak_count.begin() + p);
				}

				return weak_count.size();
			}

			template<typename T>
			inline size_t memory_controller<T>::check_weak_count_total_ptrs()
			{
				size_t total = 0;

				for (bool keep = true; keep;) {
					keep = false;
					size_t p = 0;

					for (auto& i : weak_count) {
						if (i.expired()) {
							keep = true;
							break;
						}
						p++;
					}

					if (keep) weak_count.erase(weak_count.begin() + p);
				}

				for (auto& i : weak_count) {
					total += i.use_count();
				}

				return total;
			}

			template<typename T>
			inline safer_vector<T>& memory_controller<T>::work()
			{
				return vec;
			}

			template<typename T>
			inline const safe_pointer<T> memory_controller<T>::operator[](const size_t& u) const
			{
				m.lock();
				if (u < vec.size()) {
					auto o = vec[u];
					m.unlock();
					return o;
				}
				m.unlock();
				throw "at memory_controller<T>::operator[]const: Could not find " + std::to_string(u);
				return safe_pointer<T>();
			}

			template<typename T>
			inline safe_pointer<T> memory_controller<T>::operator[](const size_t& u)
			{
				m.lock();
				if (u < vec.size()) {
					auto o = vec[u];
					m.unlock();
					return o;
				}
				m.unlock();
				throw "at memory_controller<T>::operator[]: Could not find " + std::to_string(u);
				return safe_pointer<T>();
			}




			template<typename T>
			inline const bool _check_pointer_existance(safe_pointer<T>& i)
			{
				weak_pointer<T> weak = i;
				auto result = weak.lock();
				if (result) return true;
				return false;
			}













			/*template<typename T> struct __is_pointer { static const bool is_pointer = false; };
			template<typename T> struct __is_pointer<T*> { static const bool is_pointer = true; };

			template<typename T>
			struct __safe_erasing {
				ULONGLONG timer = 0;
				T pointer;
				__is_pointer<T> isptr;
			};

			// multiple read/writes can be hard.
			template <typename T>
			class safer_vector {
				std::vector<T> v;
				static std::vector<__safe_erasing<T>> v_drop;
#ifndef NOT_SAFE_RIGHT_NOW
				std::mutex m;
#endif
				bool locked_work = false;
			public:
				safer_vector();

				void push(T, const bool = false);
				const T& get(const size_t, const bool = false);
				const size_t getMax();

				void erase(size_t);
				T pop();
				T fpop();
				void clear();
				size_t flush();
				size_t to_flush_size();

				void lock();
				const std::vector<T>& work();
				void unlock();

				const T& operator[](const size_t&) const;
				T& operator[](const size_t&);
			};
			*/



			class safe_string {
				std::string s;
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				std::mutex m;
#endif
				bool locked_work = false;
			public:
				safe_string();
				safe_string(const char*);
				safe_string(const std::string&);
				safe_string(const safe_string&);

				/*safe_string(std::string&);
				safe_string(safe_string&);*/
				~safe_string();

				void add(const char*);
				void add(const std::string&);
				void add(const safe_string&);
				void add(const char&);

				void set(const char*);
				void set(const std::string&);
				void set(const safe_string&);
				void set(const char&);

				const std::string& get() const;
				const std::string& g() const;
				std::string getCopy(); // example: drawing must have a valid string, not a changing on another thread one.
				std::string gC();

				const bool isEq(const char*);
				const bool isEq(const std::string&);
				const bool isEq(const safe_string&);
				const bool isEq(const char&);

				void clear();
				void pop_back();

				void lock();
				const std::string& work();
				void unlock();

				safe_string& operator=(const char*);
				safe_string& operator=(const std::string&);
				safe_string& operator=(const safe_string&);
				safe_string& operator=(const char&);

				const bool operator==(const char*);
				const bool operator==(const std::string&);
				const bool operator==(const safe_string&);
				const bool operator==(const char&);

				const bool operator!=(const char*);
				const bool operator!=(const std::string&);
				const bool operator!=(const safe_string&);
				const bool operator!=(const char&);

				safe_string& operator+=(const char*);
				safe_string& operator+=(const std::string&);
				safe_string& operator+=(const safe_string&);
				safe_string& operator+=(const char&);
			};

			/*
				IMPLEMENTATION
			*/

			/*template <typename T> std::vector<__safe_erasing<T>> safer_vector<T>::v_drop;

			template<typename T>
			inline safer_vector<T>::safer_vector()
			{
				// none needed
			}

			template <typename T>
			inline void safer_vector<T>::push(T u, const bool skip)
			{
#ifndef NOT_SAFE_RIGHT_NOW
				if (!skip) m.lock();
#endif
				v.push_back(u);
#ifndef NOT_SAFE_RIGHT_NOW
				if (!skip) m.unlock();
#endif
			}

			template <typename T>
			inline const T& safer_vector<T>::get(const size_t s, const bool skip)
			{
				if (skip)
				{
					assert(s < getMax());
					T& i = v[s];
					return i;
				}
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				if (s >= getMax()) {
#ifndef NOT_SAFE_RIGHT_NOW
					m.unlock();
#endif
					if (s == 0) {
						throw "safer_vector::GET - S WAS NOT INTO VECTOR! Tried to fix, but it seems to be null/empty!";
					}
					return get(s - 1, skip);
				}
				T& i = v[s];
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
				return i;
			}

			template <typename T>
			inline const size_t safer_vector<T>::getMax()
			{
				return v.size();
			}

			template <typename T>
			inline void safer_vector<T>::erase(size_t p)
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				assert(p < getMax());
				{
					__safe_erasing<T> se;
					if (se.isptr.is_pointer) {
						se.pointer = v[p];
						se.timer = GetTickCount64();
						v_drop.push_back(se);
					}
				}
				v.erase(v.begin() + p);
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
			}
			template <typename T>
			inline T safer_vector<T>::pop()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				assert(!v.empty());
				T t = v.back();
				{
					__safe_erasing<T> se;
					if (se.isptr.is_pointer) {
						se.pointer = v.back();
						se.timer = GetTickCount64();
						v_drop.push_back(se);
					}
				}
				v.pop_back();
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
				return t;
			}
			template <typename T>
			inline T safer_vector<T>::fpop()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				assert(!v.empty());
				T t = v.front();
				{
					__safe_erasing<T> se;
					if (se.isptr.is_pointer) {
						se.pointer = v.front();
						se.timer = GetTickCount64();
						v_drop.push_back(se);
					}
				}
				v.erase(v.begin());
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
				return t;
			}
			template <typename T>
			inline void safer_vector<T>::clear()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				for(auto& i : v)
				{
					__safe_erasing<T> se;
					if (se.isptr.is_pointer) {
						se.pointer = i;
						se.timer = GetTickCount64();
						v_drop.push_back(se);
					}
				}
				v.clear();
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
			}
			template <typename T>
			inline size_t safer_vector<T>::flush()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				size_t howmany = 0;

				for (size_t p = 0; v_drop.size() > p;)
				{
					__safe_erasing<T>& t = v_drop[p];
					if (GetTickCount64() - t.timer > TIMER_KILL_POINTERS) {
						//delete t.pointer;
						v_drop.erase(v_drop.begin() + p);
						howmany++;
					}
					else {
						p++;
					}
				}
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
				return howmany;
			}
			template<typename T>
			inline size_t safer_vector<T>::to_flush_size()
			{
				return v_drop.size();
			}
			template <typename T>
			inline void safer_vector<T>::lock()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
				locked_work = true;
#endif
			}

			template <typename T>
			inline const std::vector<T>& safer_vector<T>::work()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				assert(locked_work);
#endif
				return v;
			}

			template <typename T>
			inline void safer_vector<T>::unlock()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				locked_work = false;
				m.unlock();
#endif
			}

			template<typename T>
			inline const T & safer_vector<T>::operator[](const size_t & u) const
			{
				return get(u, true);
			}

			template<typename T>
			inline T & safer_vector<T>::operator[](const size_t& s)
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				//assert(s < getMax());
				T& i = v[s];
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
				return i;
			}*/















			inline safe_string::safe_string()
			{
				set("");
			}
			inline safe_string::safe_string(const char* u)
			{
				set(u);
			}
			inline safe_string::safe_string(const std::string& u)
			{
				set(u);
			}
			inline safe_string::safe_string(const safe_string& u)
			{
				set(u.g());
			}
			/*inline safe_string::safe_string(std::string& u)
			{
				set(u);
			}
			inline safe_string::safe_string(safe_string& u)
			{
				set(u);
			}*/

			inline safe_string::~safe_string()
			{
				//clear(); // not needed '-'
			}
			inline void safe_string::add(const char* a)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s += a;
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline void safe_string::add(const std::string& a)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s += a;
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline void safe_string::add(const safe_string & a)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s += a.get();
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline void safe_string::add(const char& a)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s += a;
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline void safe_string::set(const char* a)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s = a;
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline void safe_string::set(const std::string & a)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s = a;
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline void safe_string::set(const safe_string & a)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s = a.get();
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline void safe_string::set(const char& a)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s = a;
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline const std::string& safe_string::get() const
			{
				return s;
			}
			inline const std::string& safe_string::g() const
			{
				return get();
			}
			inline std::string safe_string::getCopy() // example: drawing must have a valid string, not a changing on another thread one.
			{
				std::string copy = s;
				return copy;
			}
			inline std::string safe_string::gC()
			{
				return getCopy();
			}

			inline const bool safe_string::isEq(const char* e)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				bool i = (s == e);
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
				return i;
			}
			inline const bool safe_string::isEq(const std::string& e)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				bool i = (s == e);
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
				return i;
			}
			inline const bool safe_string::isEq(const safe_string& e)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				bool i = (s == e.g());
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
				return i;
			}
			inline const bool safe_string::isEq(const char& e)
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				char temp[2] = { e, '\0' };
				bool i = (s == temp);
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
				return i;
			}

			inline void safe_string::clear()
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				s.clear();
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}
			inline void safe_string::pop_back()
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				if (s.length() > 0) s.pop_back();
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}

			inline void safe_string::lock()
			{
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.lock();
#endif
				locked_work = true;
			}
			inline const std::string& safe_string::work()
			{
				assert(locked_work);
				return s;
			}
			inline void safe_string::unlock()
			{
				locked_work = false;
#ifndef NOT_SAFE_RIGHT_NOW_STRING
				m.unlock();
#endif
			}

			inline safe_string& safe_string::operator=(const char* u)
			{
				set(u);
				return *this;
			}
			inline safe_string& safe_string::operator=(const std::string& u)
			{
				set(u);
				return *this;
			}
			inline safe_string& safe_string::operator=(const safe_string& e)
			{
				set(e);
				return *this;
			}
			inline safe_string& safe_string::operator=(const char& u)
			{
				set(u);
				return *this;
			}

			inline const bool safe_string::operator==(const char* u)
			{
				return isEq(u);
			}
			inline const bool safe_string::operator==(const std::string& u)
			{
				return isEq(u);
			}
			inline const bool safe_string::operator==(const safe_string& u)
			{
				return isEq(u);
			}
			inline const bool safe_string::operator==(const char& u)
			{
				return isEq(u);
			}

			inline const bool safe_string::operator!=(const char* u)
			{
				return !isEq(u);
			}
			inline const bool safe_string::operator!=(const std::string& u)
			{
				return !isEq(u);
			}
			inline const bool safe_string::operator!=(const safe_string& u)
			{
				return !isEq(u);
			}
			inline const bool safe_string::operator!=(const char& u)
			{
				return !isEq(u);
			}

			inline safe_string& safe_string::operator+=(const char* u)
			{
				add(u);
				return *this;
			}
			inline safe_string& safe_string::operator+=(const std::string& u)
			{
				add(u);
				return *this;
			}
			inline safe_string& safe_string::operator+=(const safe_string& u)
			{
				add(u.g());
				return *this;
			}
			inline safe_string& safe_string::operator+=(const char& u)
			{
				add(u);
				return *this;
			}
}
	}
}