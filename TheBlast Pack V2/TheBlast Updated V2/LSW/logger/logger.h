#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <mutex>
#include <Windows.h>

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
		

		class gfile {
			struct _log {
				FILE* f = nullptr;
				std::string path = "log.log"; // temp
				std::mutex f_m, each_call;
				bool store_log = true;
				bool is_needed_lock = false;
				std::string now;
				std::vector<std::string> lines;
			};

			static bool showconsole;
			static _log g;
		public:
			const bool setPath(const std::string&, const char* = "wb", const bool = true);
			void close();
			void push(const std::string&);
			void printclock(const bool = true);

			void showOnConsole(const bool);

			void flush();
			void saveOnMemory(const bool); // global only
			//void debugP(const std::string&, const size_t = 0);
			//template<typename... Args> void debug(const std::string&, Args...);

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

		
		inline const bool gfile::setPath(const std::string& orig, const char* mode, const bool autopath) // easier
		{
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
				for (auto& i : s) putchar(i);
			}

			if (!g.is_needed_lock) return;

			if (!g.f)
				if (!setPath(Constants::default_file_global_path)) return;

			g.f_m.lock();
			//fprintf_s(g.f, "%s", temp.c_str());
			for (auto& i : s)
			{
				if (g.f) putc(i, g.f);

				if (g.store_log) {
					if (i == '\n') {
						g.lines.push_back(g.now);
						g.now.clear();
						if (g.lines.size() > Constants::max_lines_stored_by_memlog)
						{
							g.lines.erase(g.lines.begin());
						}
					}
					else g.now += i;
				}
			}
			g.f_m.unlock();
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
		
		inline void gfile::flush()
		{
			g.f_m.lock();
			fflush(g.f);
			g.f_m.unlock();
		}
		
		inline void gfile::saveOnMemory(const bool s)
		{
			g.store_log = s;
		}

		/*inline void gfile::debugP(const std::string& s, const size_t ptrval)
		{
			if (showconsole) {
				std::string addinf = "";
				if (ptrval != 0) {
					addinf = "[->" + std::to_string(ptrval) + "]";
				}

				g.each_call.lock();

				printf("%s[DEBUG]%s\n", addinf.c_str(), s.c_str());

				g.each_call.unlock();
			}
		}
		template<typename... Args>
		inline void gfile::debug(const std::string& s, Args... args)
		{
			if (showconsole) {
				
				g.each_call.lock();

				printf(("[DEBUG]" + s + "\n").c_str(), args...);

				g.each_call.unlock();
			}
		}*/

		
		
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

		inline const std::string fsr(std::string fname_pretty, const E situation = E::INFO) // Function class, function name, type
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

			std::string sttr;
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			switch (situation) {
			case E::INFO:
				sttr = "[INFO]";
				SetConsoleTextAttribute(hConsole, 0x07);
				break;
			case E::WARN:
				sttr = "[WARN]";
				SetConsoleTextAttribute(hConsole, 0x0E);
				break;
			case E::ERRR:
				sttr = "[ERRR]";
				SetConsoleTextAttribute(hConsole, 0x0C);
				break;
			case E::DEBUG:
				sttr = "[DEBG]";
				SetConsoleTextAttribute(hConsole, 0x08);
				break;
			}

			sttr += "[" + fname_pretty + "] ";

			return sttr;
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