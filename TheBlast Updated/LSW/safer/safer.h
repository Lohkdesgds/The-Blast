#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <cassert>
#include <Windows.h>

#define NOT_SAFE_RIGHT_NOW
#define TIMER_KILL_POINTERS 2000 // ms

namespace LSW {
	namespace v2 {
		namespace Safer {

			template<typename T> struct __is_pointer { static const bool is_pointer = false; };
			template<typename T> struct __is_pointer<T*> { static const bool is_pointer = true; };

			template<typename T>
			struct __safe_erasing {
				ULONGLONG timer = 0;
				T* pointer = nullptr;
				__is_pointer<T> isptr;
			};

			// multiple read/writes can be hard.
			template <typename T>
			class safe_vector {
				std::vector<T> v;
				static std::vector<__safe_erasing<T>> v_drop;
#ifndef NOT_SAFE_RIGHT_NOW
				std::mutex m;
#endif
				bool locked_work = false;
			public:
				safe_vector();

				void push(T, const bool = false);
				const T& get(const size_t, const bool = false);
				const size_t getMax();

				void erase(size_t);
				T pop();
				T fpop();
				void clear();
				size_t flush();

				void lock();
				const std::vector<T>& work();
				void unlock();

				const T& operator[](const size_t&) const;
				T& operator[](const size_t&);
			};

			class safe_string {
				std::string s;
				std::mutex m;
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

			template <typename T> std::vector<__safe_erasing<T>> safe_vector<T>::v_drop;

			template<typename T>
			inline safe_vector<T>::safe_vector()
			{
				// none needed
			}

			template <typename T>
			inline void safe_vector<T>::push(T u, const bool skip)
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
			inline const T& safe_vector<T>::get(const size_t s, const bool skip)
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
						throw "SAFE_VECTOR::GET - S WAS NOT INTO VECTOR! Tried to fix, but it seems to be null/empty!";
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
			inline const size_t safe_vector<T>::getMax()
			{
				return v.size();
			}

			template <typename T>
			inline void safe_vector<T>::erase(size_t p)
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				assert(p < getMax());
				{
					__safe_erasing<T> se;
					se.pointer = &v[p];
					se.timer = GetTickCount64() + TIMER_KILL_POINTERS;
					v_drop.push_back(se);
				}
				v.erase(v.begin() + p);
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
			}
			template <typename T>
			inline T safe_vector<T>::pop()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				assert(!v.empty());
				T t = v.back();
				{
					__safe_erasing<T> se;
					se.pointer = &v.back();
					se.timer = GetTickCount64() + TIMER_KILL_POINTERS;
					v_drop.push_back(se);
				}
				v.pop_back();
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
				return t;
			}
			template <typename T>
			inline T safe_vector<T>::fpop()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				assert(!v.empty());
				T t = v.front();
				{
					__safe_erasing<T> se;
					se.pointer = &v.front();
					se.timer = GetTickCount64() + TIMER_KILL_POINTERS;
					v_drop.push_back(se);
				}
				v.erase(v.begin());
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
				return t;
			}
			template <typename T>
			inline void safe_vector<T>::clear()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				for(auto& i : v)
				{
					__safe_erasing<T> se;
					se.pointer = &i;
					se.timer = GetTickCount64() + TIMER_KILL_POINTERS;
					v_drop.push_back(se);
				}
				v.clear();
#ifndef NOT_SAFE_RIGHT_NOW
				m.unlock();
#endif
			}
			template <typename T>
			inline size_t safe_vector<T>::flush()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
#endif
				size_t howmany = 0;

				for (size_t p = 0; v_drop.size() > p;)
				{
					__safe_erasing<T>& t = v_drop[p];
					if (t.timer - GetTickCount64() < 0) {
						if (t.isptr.is_pointer) delete t.pointer;
						v_drop.erase(v_drop.begin());
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
			template <typename T>
			inline void safe_vector<T>::lock()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				m.lock();
				locked_work = true;
#endif
			}

			template <typename T>
			inline const std::vector<T>& safe_vector<T>::work()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				assert(locked_work);
#endif
				return v;
			}

			template <typename T>
			inline void safe_vector<T>::unlock()
			{
#ifndef NOT_SAFE_RIGHT_NOW
				locked_work = false;
				m.unlock();
#endif
			}

			template<typename T>
			inline const T & safe_vector<T>::operator[](const size_t & u) const
			{
				return get(u, true);
			}

			template<typename T>
			inline T & safe_vector<T>::operator[](const size_t& s)
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
			}




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
				clear();
			}
			inline void safe_string::add(const char* a)
			{
				m.lock();
				s += a;
				m.unlock();
			}
			inline void safe_string::add(const std::string& a)
			{
				m.lock();
				s += a;
				m.unlock();
			}
			inline void safe_string::add(const safe_string & a)
			{
				m.lock();
				s += a.get();
				m.unlock();
			}
			inline void safe_string::add(const char& a)
			{
				m.lock();
				s += a;
				m.unlock();
			}
			inline void safe_string::set(const char* a)
			{
				m.lock();
				s = a;
				m.unlock();
			}
			inline void safe_string::set(const std::string & a)
			{
				m.lock();
				s = a;
				m.unlock();
			}
			inline void safe_string::set(const safe_string & a)
			{
				m.lock();
				s = a.get();
				m.unlock();
			}
			inline void safe_string::set(const char& a)
			{
				m.lock();
				s = a;
				m.unlock();
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
				m.lock();
				bool i = (s == e);
				m.unlock();
				return i;
			}
			inline const bool safe_string::isEq(const std::string& e)
			{
				m.lock();
				bool i = (s == e);
				m.unlock();
				return i;
			}
			inline const bool safe_string::isEq(const safe_string& e)
			{
				m.lock();
				bool i = (s == e.g());
				m.unlock();
				return i;
			}
			inline const bool safe_string::isEq(const char& e)
			{
				m.lock();
				char temp[2] = { e, '\0' };
				bool i = (s == temp);
				m.unlock();
				return i;
			}

			inline void safe_string::clear()
			{
				m.lock();
				s.clear();
				m.unlock();
			}
			inline void safe_string::pop_back()
			{
				m.lock();
				if (s.length() > 0) s.pop_back();
				m.unlock();
			}

			inline void safe_string::lock()
			{
				m.lock();
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
				m.unlock();
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