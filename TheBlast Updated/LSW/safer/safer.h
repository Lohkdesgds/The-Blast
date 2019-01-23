#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <cassert>

namespace LSW {
	namespace v2 {
		namespace Safer {

			// multiple read/writes can be hard.
			template <typename T>
			class safe_vector {
				std::vector<T> v;
				std::mutex m;
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
				void clearSomehow();

				void lock();
				const std::vector<T>& work();
				void unlock();

				const T& operator[](const size_t&) const;
				T& operator[](const size_t&);
				//safe_vector& operator=(const safe_vector&);
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

			template<typename T>
			inline safe_vector<T>::safe_vector()
			{
				// none needed
			}

			/*template<typename T>
			inline safe_vector<T>::safe_vector(safe_vector& sv)
			{
				sv.lock();
				v.clear();
				v = sv.work();
				sv.unlock();
			}*/

			template <typename T>
			inline void safe_vector<T>::push(T u, const bool skip)
			{
				if (!skip) m.lock();
				v.push_back(u);
				if (!skip) m.unlock();
			}

			template <typename T>
			inline const T& safe_vector<T>::get(const size_t s, const bool skip)
			{
				if (skip)
				{
					bool locked = m.try_lock();
					assert(s < getMax());
					T& i = v[s];
					if (locked) m.unlock();
					return i;
				}
				m.lock();
				if (s >= getMax()) {
					m.unlock();
					if (s == 0) {
						throw "SAFE_VECTOR::GET - S WAS NOT INTO VECTOR! Tried to fix, but it seems to be null/empty!";
					}
					return get(s - 1, skip);
				}
				T& i = v[s];
				m.unlock();
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
				m.lock();
				assert(p < getMax());
				v.erase(v.begin() + p);
				m.unlock();
			}
			template <typename T>
			inline T safe_vector<T>::pop()
			{
				m.lock();
				assert(!v.empty());
				T t = v.back();
				v.pop_back();
				m.unlock();
				return t;
			}
			template <typename T>
			inline T safe_vector<T>::fpop()
			{
				m.lock();
				assert(!v.empty());
				T t = v.front();
				v.erase(v.begin());
				m.unlock();
				return t;
			}
			template <typename T>
			inline void safe_vector<T>::clear()
			{
				m.lock();
				v.clear();
				m.unlock();
			}
			template <typename T>
			inline void safe_vector<T>::clearSomehow()
			{
				bool b = m.try_lock();
				v.clear();
				if (b) m.unlock();
			}
			template <typename T>
			inline void safe_vector<T>::lock()
			{
				m.lock();
				locked_work = true;
			}

			template <typename T>
			inline const std::vector<T>& safe_vector<T>::work()
			{
				assert(locked_work);
				return v;
			}

			template <typename T>
			inline void safe_vector<T>::unlock()
			{
				locked_work = false;
				m.unlock();
			}

			template<typename T>
			inline const T & safe_vector<T>::operator[](const size_t & u) const
			{
				return get(u, true);
			}

			template<typename T>
			inline T & safe_vector<T>::operator[](const size_t& s)
			{
				bool locked = m.try_lock();
				assert(s < getMax());
				T& i = v[s];
				if (locked) m.unlock();
				return i;
			}

			/*template<typename T>
			inline safe_vector<T> & safe_vector<T>::operator=(const safe_vector<T>& sv)
			{
				sv.lock();
				v.clear();
				v = sv.work();
				sv.unlock();
				return *this;
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