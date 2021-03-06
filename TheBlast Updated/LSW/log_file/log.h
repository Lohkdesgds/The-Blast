#pragma once

#include "..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Log {

			enum mode{LOCAL,GLOBAL};
			enum bruk{ENDL,START,ERRDV,NEEDED_START,NEEDED_ENDL};

			struct _log {
				FILE* f = nullptr;
				Safer::safe_string path = "log.log"; // temp
				std::mutex f_m, each_call;
				bool store_log = true;
				bool islonglog = false;
				bool is_needed_lock = false;
				Safer::safe_string now;
				Safer::safe_vector<Safer::safe_string> lines;
			};

			template <mode N>
			class _file {
				static bool showconsole;
				static _log g;
				_log l;
			public:
				const bool setPath(const Safer::safe_string&, const char* = "wb", const bool = true);
				void close();
				void push(const Safer::safe_string&);
				void printclock();

				void showOnConsole(const bool);
				void longLog(const bool);

				void flush();
				void saveOnMemory(const bool); // global only
				void debug(const Safer::safe_string&);

				_file& operator<<(const Safer::safe_string&);
				_file& operator<<(const bruk&);
				_file& operator<<(const int&);
				_file& operator<<(const float&);
				_file& operator<<(const double&);
				_file& operator<<(const unsigned&);
				_file& operator<<(const long&);
			};

			// easier
			typedef _file<GLOBAL> gfile;
			typedef _file<LOCAL> lfile;

			/*
				IMPLEMENTATION
			*/

			template <mode N> _log _file<N>::g;
			template <mode N> bool _file<N>::showconsole = false;

			template <mode N>
			inline const bool _file<N>::setPath(const Safer::safe_string& orig, const char* mode, const bool autopath) // easier
			{
				Safer::safe_string s = orig;

				Tools::interpret_path(s);

				if (N == GLOBAL)
				{
					g.f_m.lock();
					if (g.f) {
						fclose(g.f);
						g.f = nullptr;
					}
					g.path.set(s);
					if (autopath) Tools::createPath(s);
					errno_t err = fopen_s(&g.f, s.g().c_str(), mode);
					g.f_m.unlock();
					return (err == 0);
				}
				else {
					l.f_m.lock();
					if (l.f) {
						fclose(l.f);
						l.f = nullptr;
					}
					l.path.set(s);
					if (autopath) Tools::createPath(s);
					errno_t err = fopen_s(&l.f, s.g().c_str(), mode);
					l.f_m.unlock();
					return (err == 0);
				}
			}

			template <mode N>
			inline void _file<N>::close()
			{
				if (N == GLOBAL)
				{
					g.f_m.lock();
					if (g.f) {
						fclose(g.f);
						g.f = nullptr;
					}
					g.f_m.unlock();
				}
				else {
					l.f_m.lock();
					if (l.f) {
						fclose(l.f);
						l.f = nullptr;
					}
					l.f_m.unlock();
				}
			}

			template<mode N>
			inline void _file<N>::push(const Safer::safe_string & s)
			{
				//const std::string temp = s.g();

				if (N == GLOBAL)
				{
					if (showconsole) {
						for (auto& i : s.g()) printf("%c", i);
					}

					if (!g.islonglog && !g.is_needed_lock) return;

					if (!g.f)
						if (!setPath(Defaults::default_global_path)) return;

					g.f_m.lock();
					//fprintf_s(g.f, "%s", temp.c_str());
					for (auto& i : s.g())
					{
						putc(i, g.f);

						if (g.store_log) {
							if (i == '\n') {
								g.lines.push(g.now);
								g.now.clear();
								if (g.lines.getMax() > Defaults::max_lines_stored_by_memlog)
								{
									g.lines.fpop();
								}
							}
							else g.now += i;
						}
					}
					g.f_m.unlock();
				}
				else{
					if (showconsole) {
						for (auto& i : s.g()) printf("%c", i);
					}

					if (!l.islonglog && !l.is_needed_lock) return;

					if (!l.f) return;
					l.f_m.lock();
					//fprintf_s(l.f, "%s", temp.c_str());
					for (auto& i : s.g())
					{
						putc(i, l.f);
					}
					l.f_m.unlock();
				}
			}

			template<mode N>
			inline void LSW::v2::Log::_file<N>::printclock()
			{
				char temp[24];
				SYSTEMTIME t;
				GetLocalTime(&t);
				sprintf_s(temp, "[%02d-%02d-%02d_%02d-%02d]", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute);
				push(temp);
			}
			template<mode N>
			inline void _file<N>::showOnConsole(const bool u)
			{
				showconsole = u;
				if (u) {
					AllocConsole();
					FILE* temphandle = stdout;
					freopen_s(&temphandle, "CONOUT$", "w", stdout);

					printf("Now showing on console.\n");
					*this << Log::START << "Console output enabled! It can cause lag and/or lower FPS in game! Use for testing only!" << Log::ENDL;
				}
				else FreeConsole();
			}
			template<mode N>
			inline void _file<N>::longLog(const bool b)
			{
				if (N == GLOBAL) g.islonglog = b;
				else l.islonglog = b;
			}
			template<mode N>
			inline void _file<N>::flush()
			{
				if (N == GLOBAL)
				{
					g.f_m.lock();
					fflush(g.f);
					g.f_m.unlock();
				}
				else {
					l.f_m.lock();
					fflush(l.f);
					l.f_m.unlock();
				}
			}
			template<mode N>
			inline void LSW::v2::Log::_file<N>::saveOnMemory(const bool s)
			{
				g.store_log = s;
			}

			template<mode N>
			inline void _file<N>::debug(const Safer::safe_string & s)
			{
				if (showconsole) {
					if (N == GLOBAL) g.each_call.lock();
					else l.each_call.lock();

					printf("[DEBUG][CONSOLE_ONLY] %s\n", s.g().c_str());

					if (N == GLOBAL) g.each_call.unlock();
					else l.each_call.unlock();
				}
			}

			template<mode N>
			inline _file<N>& _file<N>::operator<<(const Safer::safe_string& u)
			{
				push(u);
				return *this;
			}
			template<mode N>
			inline _file<N>& _file<N>::operator<<(const bruk& u)
			{
				switch (u)
				{
				case START:
					if (N == GLOBAL) g.each_call.lock();
					else l.each_call.lock();
					printclock();
					break;
				case ENDL:
					push("\n");
					if (N == GLOBAL) g.each_call.unlock();
					else l.each_call.unlock();
					break;
				case ERRDV:
					push("\n---------- ERROR ----------\n");
					break;
				case NEEDED_START:
					if (N == GLOBAL) {
						g.each_call.lock();
						g.is_needed_lock = true;
					}
					else {
						l.each_call.lock();
						l.is_needed_lock = true;
					}
					printclock();
					break;
				case NEEDED_ENDL:
					if (N == GLOBAL) {
						push("\n");
						g.is_needed_lock = false;
						g.each_call.unlock();
					}
					else {
						l.is_needed_lock = false;
						l.each_call.unlock();
					}
					break;
				}
				return *this;
			}
			template<mode N>
			inline _file<N>& _file<N>::operator<<(const int& u)
			{
				char temp[32];
				sprintf_s(temp, "%d", u);
				push(temp);
				return *this;
			}
			template<mode N>
			inline _file<N>& _file<N>::operator<<(const float& u)
			{
				char temp[32];
				sprintf_s(temp, "%.3f", u);
				push(temp);
				return *this;
			}
			template<mode N>
			inline _file<N>& _file<N>::operator<<(const double& u)
			{
				char temp[32];
				sprintf_s(temp, "%.5lf", u);
				push(temp);
				return *this;
			}
			template<mode N>
			inline _file<N>& _file<N>::operator<<(const unsigned& u)
			{
				char temp[32];
				sprintf_s(temp, "%u", u);
				push(temp);
				return *this;
			}
			template<mode N>
			inline _file<N>& _file<N>::operator<<(const long& u)
			{
				char temp[32];
				sprintf_s(temp, "%ld", u);
				push(temp);
				return *this;
			}

		}
	}
}