#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <mutex>
#include <Windows.h>
#include <memory>

#include <allegro5/allegro.h>

#include "..\tools\tools.h"
#include "..\shared_constants\constants.h"
#include "..\big_templates\small_templates.h"

namespace LSW {
	namespace v4 {


		enum class L { EL, SL, SLF, ELF };
		enum class E { INFO, WARN, ERRR, DEBUG };
		
		struct __c_entry {
			std::string str;
			ALLEGRO_COLOR c = al_map_rgb(255,255,255);
		};
		
		template<typename T>
		struct cast_pointer {
			T* a = nullptr;
		public:
			cast_pointer(T* b) { a = b; }
			void* get() { return (void*)a; }
		};

		class gfile {
			struct _log {
				FILE* f = nullptr;
				std::string path = "log.log"; // temp
				std::mutex f_m, eachcall, evsrc_m;
				bool store_log = true;
				bool do_save_on_file = false;
				bool eachcall_safety_locked = false;
				std::string now;
				//std::vector<std::string> lines;
				ALLEGRO_EVENT_SOURCE evsrc = ALLEGRO_EVENT_SOURCE();
				bool started_evsrc = false;
				struct lines {
					char line[Constants::each_line_stored_by_memlog] = { 0 };
					ALLEGRO_COLOR event_color = al_map_rgb(255, 255, 255);
				} memline[Constants::max_lines_stored_by_memlog];
				size_t memlinecount = 0;
			};

			static _log g;

			const bool start(const std::string&, const char* = "wb", const bool = true);
			void printClock(const bool = true);
		public:
			void close();
			void push(const std::string&, const bool = Constants::_is_on_debug_mode);

			void flush();
			ALLEGRO_EVENT_SOURCE* getEvent();
			//void saveOnMemory(const bool);

			gfile& operator<<(const __c_entry&);
			gfile& operator<<(const std::string&);
			gfile& operator<<(const L&);
			gfile& operator<<(const int&);
			gfile& operator<<(const float&);
			gfile& operator<<(const double&);
			gfile& operator<<(const unsigned&);
			gfile& operator<<(const long&);
			gfile& operator<<(const size_t&);
			template<typename T> gfile& operator<<(cast_pointer<T>);
		};


		/*
			IMPLEMENTATION
		*/

		inline gfile::_log gfile::g;

		
		inline const bool gfile::start(const std::string& orig, const char* mode, const bool autopath) // easier
		{
			if (!g.started_evsrc) {
				if (!al_is_system_installed()) al_init();
				al_init_user_event_source(&g.evsrc);
				g.started_evsrc = true;
			}

			std::string s = orig;

			Tools::interpretPath(s);

			g.f_m.lock();
			if (g.f) {
				fclose(g.f);
				g.f = nullptr;
			}
			g.path = s;
			if (autopath) Tools::createPath(s);
			errno_t err = fopen_s(&g.f, s.c_str(), mode);
			g.f_m.unlock();
			return (err == 0);
		}
		inline void gfile::printClock(const bool denyconsole)
		{
			char temp[24];
			SYSTEMTIME t;
			GetLocalTime(&t);
			sprintf_s(temp, "[%02d%02d%02d%02d%02d%02d]", t.wYear % 100, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
			
			push(temp, !denyconsole);
		}

		
		inline void gfile::close()
		{
			g.f_m.lock();
			if (g.f) {
				fclose(g.f);
				g.f = nullptr;
			}
			g.f_m.unlock();
		}

		inline void gfile::push(const std::string& s, const bool showconsole)
		{
			if (showconsole) {
				for (auto& i : s) {
					putchar(i);
				}
			}
			if (g.started_evsrc) {
				for (auto& i : s) {

					if (i == '\n') {
						ALLEGRO_EVENT evv;

						memset(g.memline[g.memlinecount].line, '\0', Constants::each_line_stored_by_memlog);
						size_t pp = 0;
						for (auto& i : g.now) {
							g.memline[g.memlinecount].line[pp++] = i;
							if (pp >= Constants::each_line_stored_by_memlog) break;
						}
						
						evv.user.data1 = (intptr_t)g.memline[g.memlinecount].line;
						evv.user.data2 = (intptr_t)&g.memline[g.memlinecount].event_color;
						evv.type = +Constants::ro__my_events::LOG_CLOUDLAUNCH_RAW;
						al_emit_user_event(&g.evsrc, &evv, NULL);
						g.now.clear();

						if (++g.memlinecount >= Constants::max_lines_stored_by_memlog) g.memlinecount = 0;
					}
					else g.now += i;
				}
			}

			if (!g.do_save_on_file) return;

			if (!g.f)
				if (!start(Constants::default_file_global_path)) return;

			g.f_m.lock();
			//fprintf_s(g.f, "%s", temp.c_str());
			for (auto& i : s)
			{
				if (g.f) putc(i, g.f);
			}
			g.f_m.unlock();
		}

		
		
		inline void gfile::flush()
		{
			g.f_m.lock();
			fflush(g.f);
			g.f_m.unlock();
		}
		inline ALLEGRO_EVENT_SOURCE* gfile::getEvent()
		{
			if (g.started_evsrc) return &g.evsrc;
			return nullptr;
		}


		
		
		inline gfile& gfile::operator<<(const __c_entry& u)
		{
			g.memline[g.memlinecount].event_color = u.c;
			push(u.str);
			return *this;
		}
		inline gfile& gfile::operator<<(const std::string& u)
		{
			push(u);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const L& u)
		{
			switch (u)
			{
			case L::SL: // START LINE
				g.eachcall.lock();
				g.eachcall_safety_locked = true;
				printClock();
				break;
			case L::EL: // END LINE
				push("\n");
				if (!g.eachcall_safety_locked) {
					push("\n---------- FATAL WARN ----------\nMUTEX WAS NOT LOCKED! THE CODE HAS ERRORS!");	flush();
					throw Abort::abort(__FUNCSIG__, "FATAL ERROR MUTEX SHOULDN'T BE UNLOCKED IF IT WASN'T PREVIOUSLY!");
				}
				g.eachcall_safety_locked = false;
				g.eachcall.unlock();
				break;
			case L::SLF: // START LINE AND SAVE ON FILE
				g.eachcall.lock();
				g.eachcall_safety_locked = true;
				g.do_save_on_file = true;
				printClock();
				break;
			case L::ELF: // END LINE AND SAVE ON FILE
				push("\n");
				g.do_save_on_file = false;
				if (!g.eachcall_safety_locked) {
					push("\n---------- FATAL WARN ----------\nMUTEX WAS NOT LOCKED! THE CODE HAS ERRORS!");	flush();
					throw Abort::abort(__FUNCSIG__, "FATAL ERROR MUTEX SHOULDN'T BE UNLOCKED IF IT WASN'T PREVIOUSLY!");
				}
				g.eachcall_safety_locked = false;
				g.eachcall.unlock();
				flush();
				break;
			}
			return *this;
		}
		
		inline gfile& gfile::operator<<(const int& u)
		{
			char temp[32];
			if (fabs(u) < 1e15) sprintf_s(temp, "%d", u);
			else sprintf_s(temp, u > 0 ? "+++" : "---");
			push(temp);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const float& u)
		{
			char temp[32];
			if (fabs(u) < 1e15) sprintf_s(temp, "%.3f", u);
			else sprintf_s(temp, u > 0 ? "+++" : "---");
			push(temp);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const double& u)
		{
			char temp[32];
			if (fabs(u) < 1e15) sprintf_s(temp, "%.5lf", u);
			else sprintf_s(temp, u > 0 ? "+++" : "---");
			push(temp);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const unsigned& u)
		{
			char temp[32];
			if (u < 1e15) sprintf_s(temp, "%u", u);
			else sprintf_s(temp, u > 0 ? "+++" : "---");
			push(temp);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const long& u)
		{
			char temp[32];
			if (fabs(u) < 1e15) sprintf_s(temp, "%ld", u);
			else sprintf_s(temp, u > 0 ? "+++" : "---");
			push(temp);
			return *this;
		}

		inline gfile& gfile::operator<<(const size_t& u)
		{
			char temp[48];
			if (u < 1e15) sprintf_s(temp, "%zu", u);
			else sprintf_s(temp, u > 0 ? "+++" : "---");
			push(temp);
			return *this;
		}

		template <typename T> inline gfile& gfile::operator<<(cast_pointer<T> u)
		{
			char temp[48];
			void* pp = u.get();
			sprintf_s(temp, "%p", pp);
			push(temp);
			return *this;
		}

		inline const __c_entry fsr(std::string fname_pretty, const E situation = E::INFO) // Function class, function name, type
		{
			if (fname_pretty.length() < Constants::len_class) {
				for (size_t p = fname_pretty.length(); p < Constants::len_class; p++)
				{
					fname_pretty += '_';
				}
			}
			else {
				for (size_t p = fname_pretty.length(); p > Constants::len_class; p--)
				{
					fname_pretty.pop_back();
				}
			}

			for (auto& i : fname_pretty) i = ::toupper(i);

			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			__c_entry ce;

			switch (situation) {
			case E::INFO:
				ce.str = "[INFO]";
				ce.c = al_map_rgb(255, 255, 255);
				SetConsoleTextAttribute(hConsole, 0x07);
				break;
			case E::WARN:
				ce.str = "[WARN]";
				ce.c = al_map_rgb(255, 128, 0);
				SetConsoleTextAttribute(hConsole, 0x0E);
				break;
			case E::ERRR:
				ce.str = "[ERRR]";
				ce.c = al_map_rgb(255, 0, 0);
				SetConsoleTextAttribute(hConsole, 0x0C);
				break;
			case E::DEBUG:
				ce.str = "[DEBG]";
				ce.c = al_map_rgb(255, 0, 255);
				SetConsoleTextAttribute(hConsole, 0x08);
				break;
			}

			ce.str += "[" + fname_pretty + "] ";

			return ce;
		}

		//inline const std::string freg(std::string where, std::string what/*, const size_t ptrval = 0*/, const E situation = E::INFO) // Function class, function name, type
		//{
		//	if (where.length() < Constants::len_class) {
		//		for (size_t p = where.length(); p < Constants::len_class; p++)
		//		{
		//			where += '_';
		//		}
		//	}
		//	else {
		//		for (size_t p = where.length(); p > Constants::len_class; p--)
		//		{
		//			where.pop_back();
		//		}
		//	}

		//	if (what.length() < Constants::len_class) {
		//		for (size_t p = what.length(); p < Constants::len_class; p++)
		//		{
		//			what += '_';
		//		}
		//	}
		//	else {
		//		for (size_t p = what.length(); p > Constants::len_class; p--)
		//		{
		//			what.pop_back();
		//		}
		//	}

		//	for (auto& i : where) i = ::toupper(i);
		//	for (auto& i : what)  i = ::toupper(i);
		//				
		//	std::string sttr = "[" + where + ":" + what + "]";

		//	switch (situation) {
		//	case E::INFO:
		//		sttr += "[INFO] ";
		//		break;
		//	case E::WARN:
		//		sttr += "[WARN] ";
		//		break;
		//	case E::ERRR:
		//		sttr += "[ERRR] ";
		//		break;
		//	}

		//	return sttr;
		//}
	}
}