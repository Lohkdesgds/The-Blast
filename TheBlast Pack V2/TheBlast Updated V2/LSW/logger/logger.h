#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <mutex>

#include "..\tools\tools.h"

namespace LSW {
	namespace v4 {

		namespace Constants {

			const std::string default_file_global_path = "%appdata%/Lohk's Studios/TheBlast/logs/latest.log";

			const size_t len_func = 15;
			const size_t len_class = 12;
			const size_t max_lines_stored_by_memlog = 20;

		}


		enum class L { ENDL, START, ERRDV, NEEDED_START, NEEDED_ENDL };
		enum class E { INFO, WARN, ERRR };
		

		class gfile {
			struct _log {
				FILE* f = nullptr;
				std::string path = "log.log"; // temp
				std::mutex f_m, each_call;
				bool store_log = true;
				bool islonglog = false;
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
			void printclock();

			void showOnConsole(const bool);
			void longLog(const bool);

			void flush();
			void saveOnMemory(const bool); // global only
			void debug(const std::string&);

			gfile& operator<<(const std::string&);
			gfile& operator<<(const L&);
			gfile& operator<<(const int&);
			gfile& operator<<(const float&);
			gfile& operator<<(const double&);
			gfile& operator<<(const unsigned&);
			gfile& operator<<(const long&);
		};


		/*
			IMPLEMENTATION
		*/

		 inline gfile::_log gfile::g;
		 inline bool gfile::showconsole = false;

		
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

			if (!g.islonglog && !g.is_needed_lock) return;

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

		
		inline void gfile::printclock()
		{
			char temp[24];
			SYSTEMTIME t;
			GetLocalTime(&t);
			sprintf_s(temp, "[%02d-%02d-%02d_%02d-%02d-%02d]", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
			push(temp);
		}
		
		inline void gfile::showOnConsole(const bool u)
		{
			showconsole = u;
			if (u) {
				AllocConsole();
				FILE* temphandle = stdout;
				freopen_s(&temphandle, "CONOUT$", "w", stdout);

				printf("Now showing on console.\n");
				*this << L::START << "Console output enabled! It can cause lag and/or lower FPS in game! Use for testing only!" << L::ENDL;
			}
			else FreeConsole();
		}
		
		inline void gfile::longLog(const bool b)
		{
			g.islonglog = b;
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

		
		inline void gfile::debug(const std::string& s)
		{
			if (showconsole) {
				g.each_call.lock();

				printf("[DEBUG] %s\n", s.c_str());

				g.each_call.unlock();
			}
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
			case L::START:
				g.each_call.lock();
				printclock();
				break;
			case L::ENDL:
				push("\n");
				g.each_call.unlock();
				break;
			case L::ERRDV:
				push("\n---------- ERROR ----------\n");
				flush();
				break;
			case L::NEEDED_START:
				g.each_call.lock();
				g.is_needed_lock = true;
				printclock();
				break;
			case L::NEEDED_ENDL:
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

		inline const std::string freg(std::string where, std::string what, const E situation = E::INFO) // Function class, function name, type
		{
			if (where.length() < Constants::len_class) {
				for (size_t p = where.length(); p < Constants::len_class; p++)
				{
					where += '_';
				}
			}
			else {
				for (size_t p = where.length(); p > Constants::len_class; p--)
				{
					where.pop_back();
				}
			}

			if (what.length() < Constants::len_class) {
				for (size_t p = what.length(); p < Constants::len_class; p++)
				{
					what += '_';
				}
			}
			else {
				for (size_t p = what.length(); p > Constants::len_class; p--)
				{
					what.pop_back();
				}
			}

			for (auto& i : where) i = ::toupper(i);
			for (auto& i : what)  i = ::toupper(i);

			std::string sttr = "[" + where + ":" + what + "]";

			switch (situation) {
			case E::INFO:
				sttr += "[INFO]";
				break;
			case E::WARN:
				sttr += "[WARN]";
				break;
			case E::ERRR:
				sttr += "[ERRR]";
				break;
			}

			return sttr;
		}
	}
}