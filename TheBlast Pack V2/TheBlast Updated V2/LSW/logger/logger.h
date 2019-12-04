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

#ifdef _DEBUG
#define ISDEBUG true
#else
#define ISDEBUG false
#endif

namespace LSW {
	namespace v4 {


		enum class L { BL, SL, ERRDV, PRTCLOCK, SLL, BLL };
		enum class E { INFO, WARN, ERRR, DEBUG };
		
		struct __c_entry {
			std::string str;
			ALLEGRO_COLOR c;
		};

		class gfile {
			struct _log {
				FILE* f = nullptr;
				std::string path = "log.log"; // temp
				std::mutex f_m, each_call, evsrc_m;
				bool store_log = true;
				bool is_needed_lock = false;
				std::string now;
				//std::vector<std::string> lines;
				ALLEGRO_EVENT_SOURCE evsrc;
				bool started_evsrc = false;
				struct lines {
					char line[Constants::each_line_stored_by_memlog] = { 0 };
					ALLEGRO_COLOR event_color = al_map_rgb(255, 255, 255);
				} memline[Constants::max_lines_stored_by_memlog];
				size_t memlinecount = 0;
			};

			static bool showconsole;
			static _log g;

			const bool start(const std::string&, const char* = "wb", const bool = true);
			void printclock(const bool = true);
			void showOnConsole(const bool);
		public:
			void close();
			void push(const std::string&);

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
		};


		/*
			IMPLEMENTATION
		*/

		inline gfile::_log gfile::g;
		inline bool gfile::showconsole = ISDEBUG;

		
		inline const bool gfile::start(const std::string& orig, const char* mode, const bool autopath) // easier
		{
			if (!g.started_evsrc) {
				if (!al_is_system_installed()) al_init();
				al_init_user_event_source(&g.evsrc);
				g.started_evsrc = true;
			}

			std::string s = orig;

			Tools::interpret_path(s);

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
		inline void gfile::printclock(const bool denyconsole)
		{
			char temp[24];
			SYSTEMTIME t;
			GetLocalTime(&t);
			sprintf_s(temp, "[%02d%02d%02d%02d%02d%02d]", t.wYear % 100, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
			bool bckp = showconsole;
			if (denyconsole) showconsole = false;
			push(temp);
			showconsole = bckp;
		}
		inline void gfile::showOnConsole(const bool u)
		{
			showconsole = u;
			if (u) {
				AllocConsole();
				FILE* temphandle = stdout;
				freopen_s(&temphandle, "CONOUT$", "w", stdout);

				printf("Now showing on console.\n");
				*this << L::SL << "Console output enabled! It can cause lag and/or lower FPS in game! Use for testing only!" << L::BL;
			}
			else FreeConsole();
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

		inline void gfile::push(const std::string& s)
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
						sprintf_s(g.memline[g.memlinecount].line, g.now.c_str());
						
						evv.user.data1 = (intptr_t)g.memline[g.memlinecount].line;
						evv.user.data2 = (intptr_t)&g.memline[g.memlinecount].event_color;
						evv.type = +Assistance::ro__my_events::LOG_CLOUDLAUNCH_RAW;
						al_emit_user_event(&g.evsrc, &evv, NULL);
						g.now.clear();

						if (++g.memlinecount >= Constants::max_lines_stored_by_memlog) g.memlinecount = 0;
					}
					else g.now += i;
				}
			}

			if (!g.is_needed_lock) return;

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
			case L::SL:
				g.each_call.lock();
				printclock();
				break;
			case L::BL:
				push("\n");
				g.each_call.unlock();
				break;
			case L::ERRDV:
				push("\n---------- ERROR ----------\n");
				flush();
				break;
			case L::PRTCLOCK:
				printclock(false);
				break;
			case L::SLL:
				g.each_call.lock();
				g.is_needed_lock = true;
				printclock();
				break;
			case L::BLL:
				push("\n");
				g.is_needed_lock = false;
				g.each_call.unlock();
				flush();
				break;
			}
			return *this;
		}
		
		inline gfile& gfile::operator<<(const int& u)
		{
			char temp[32];
			sprintf_s(temp, "%d", u);
			push(temp);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const float& u)
		{
			char temp[32];
			sprintf_s(temp, "%.3f", u);
			push(temp);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const double& u)
		{
			char temp[32];
			sprintf_s(temp, "%.5lf", u);
			push(temp);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const unsigned& u)
		{
			char temp[32];
			sprintf_s(temp, "%u", u);
			push(temp);
			return *this;
		}
		
		inline gfile& gfile::operator<<(const long& u)
		{
			char temp[32];
			sprintf_s(temp, "%ld", u);
			push(temp);
			return *this;
		}

		inline gfile& gfile::operator<<(const size_t& u)
		{
			char temp[48];
			sprintf_s(temp, "%zu", u);
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