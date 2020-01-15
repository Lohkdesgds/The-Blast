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
#include <vector>
#include <memory>
#include <Windows.h>
#include <thread>
#include <mutex>

#include "..\shared_constants\constants.h"
#include "..\tools\tools.h"
#include "..\custom_abort\abort.h"
#include "..\logger\logger.h"

namespace LSW {
	namespace v4 {

		// constants now at shared_constants\constants.h

		namespace Assistance {

			// actual config (no sense on using size)
			enum class io__conf_boolean { HAD_ERROR, WAS_OSD_ON, ULTRADEBUG };
			enum class io__conf_float { LAST_VOLUME };
			enum class io__conf_integer { SCREEN_X, SCREEN_Y, SCREEN_FLAGS, SCREEN_PREF_HZ };
			enum class io__conf_longlong { _TIMES_LIT };
			enum class io__conf_string { LAST_VERSION, LAST_PLAYERNAME, LAST_COLOR };

			// internal configuration not from file but not automatically set or functional
			enum class io__db_boolean { SAVING_STRING_INPUT, size };
			enum class ro__db_string { LAST_STRING, CURRENT_STRING };

			// on memory while running
			enum class ro__db_mouse_boolean { MOUSE_0, MOUSE_1, MOUSE_2, MOUSE_3, MOUSE_4, MOUSE_5, MOUSE_6, MOUSE_7, size, IS_ANY_PRESSED }; // Constants::max_mouse_set_buttons
			enum class ro__db_mouse_float { MOUSE_X, MOUSE_Y, RAW_MOUSE_X, RAW_MOUSE_Y, size };
			enum class ro__db_statistics_sizet { FRAMESPERSECOND, COLLISIONSPERSECOND, USEREVENTSPERSECOND, ADVANCEDFUNCSPERSECOND, size};
			enum class ro__db_statistics_double { INSTANT_FRAMESPERSECOND, INSTANT_COLLISIONSPERSECOND, INSTANT_USEREVENTSPERSECOND, INSTANT_ADVANCEDFUNCSPERSECOND, size };
			enum class ro__db_thread_string { KEY_ADD, KEY_ADD_SET_LEN, KEY_ERASE, KEY_SET };

			// functional
			enum class io__db_functional_opt {MOUSE_KEY,KEYBOARD_KEY};

			const std::string ro__conf_boolean_str[] = { "had_error", "was_osd_on", "ultradebug" };
			const std::string ro__conf_float_str[] = { "last_volume" };
			const std::string ro__conf_integer_str[] = { "screen_width","screen_height", "last_display_flags","pref_refresh_rate" };
			const std::string ro__conf_longlong_str[] = { "times_open" };
			const std::string ro__conf_string_str[] = { "last_version","playername","playercolor" };

			const std::string ro__conf_truefalse_str[] = { "false", "true" };

			class display_mode {
			public:
				int x = 0, y = 0, hz = 0;
				bool operator==(display_mode sm) { return (sm.x == x && sm.y == y && sm.hz == hz); }
			};
		}
		

		class __systematic {
			struct modes {
				std::vector<Assistance::display_mode> options;
				int lastmode = Constants::start_display_default_mode;
			} *l = nullptr;

			std::string extracted_zip_at;
			bool initialized = false;
			bool already_set_physfs_root = false;

			void __extract_package(float* = nullptr);
			void __nointernalzip_extract_package();
			void __ensure_warn_package();
			bool __loadPackage();
			bool __nointernalzip_loadPackage();
		public:
			~__systematic();

			void initSystem();
			void forceInitWithNoZipAnyway();

			void setInterface();

			void setNewDisplayMode(const int);
			bool checkResolutionExistance(const int, const int, const int);
			const auto getAvailableResolutions();
		};


		static __systematic __g_sys; // startup

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> some raw stuff

		class Display {
			ALLEGRO_DISPLAY* d = nullptr;
			std::mutex d_try;
			int x, y, f, h;
			bool printing = false;

			std::thread* printthr = nullptr;
			bool printthrdone = true;

			void _init(const int, const int, const int, int); // x, y, flags, hz
			void _print();
			void __print_thr_autodel(ALLEGRO_BITMAP*, const std::string);
		public:
			Display();
			Display(const int, const int, const int = Constants::start_display_default_mode, int = 0); // x, y, flags, hz
			~Display();

			void restart();
			void flip();
			void clearTo(const ALLEGRO_COLOR);
			void close();
			bool exist();

			void print();

			ALLEGRO_DISPLAY*& getDisplay();
		};


		

		class Database {

			struct custom_data {
				ALLEGRO_CONFIG* c = nullptr;
				std::mutex m;
				bool keys[ALLEGRO_KEY_MAX] = { false };
				bool mouse[+Assistance::ro__db_mouse_boolean::size] = { false };

				// memory only
				float db_mouse_axes[4] = { 0.0 };
				size_t db_statistics_sizet[+Assistance::ro__db_statistics_sizet::size] = { 0 };
				double db_statistics_double[+Assistance::ro__db_statistics_double::size] = { 0 };

				// memory but not automatically, I mean, it's like the Sprite stuff you know
				bool b[+Assistance::io__db_boolean::size] = { false };
				std::string curr_string;
				std::vector<char> curr_string_keylen;
				char curr_string_keylen_val = 1;
				std::string last_string;

				// purely automatic handleing
				std::function<void(void)>			func_mb[+Assistance::ro__db_mouse_boolean::size];	// each mouse key has a function
				std::function<void(void)>			func_kb[ALLEGRO_KEY_MAX];							// each key can have its custom function

				
				std::thread* savethr = nullptr;
				bool savethrdone = true;
			};

			static custom_data data;
			gfile logg;
		public:
			Database();

			void flush();

			void set(const std::string, const std::string);
			void set(const Assistance::io__conf_boolean, const bool);
			void set(const Assistance::io__conf_float, const float);
			void set(const Assistance::io__conf_integer, const int);
			void set(const Assistance::io__conf_longlong, const long long);
			void set(const Assistance::io__conf_string, const std::string);
			void set(const Assistance::io__db_boolean, const bool);

			void set(const int, const bool); // Allegro key
			void set(const Assistance::ro__db_mouse_boolean, const bool);
			void set(const Assistance::ro__db_mouse_float, const float);
			void set(const Assistance::ro__db_statistics_sizet, const size_t);
			void set(const Assistance::ro__db_statistics_double, const double);
			void set(const Assistance::ro__db_thread_string, const char = 0);

			void set(const Assistance::io__db_functional_opt, const int, const std::function<void(void)> = std::function<void(void)>());
			

			void get(const std::string, std::string&, const std::string);
			void get(const Assistance::io__conf_boolean, bool&, const bool);
			void get(const Assistance::io__conf_float, float&, const float);
			void get(const Assistance::io__conf_integer, int&, const int);
			void get(const Assistance::io__conf_longlong, long long&, const long long);
			void get(const Assistance::io__conf_string, std::string&, const std::string);
			void get(const Assistance::io__db_boolean, bool&, const bool);
			void get(const Assistance::ro__db_string, std::string&);

			void get(const int, bool&, const bool); // Allegro key
			void get(const Assistance::ro__db_mouse_boolean, bool&);
			void get(const Assistance::ro__db_mouse_float, float&);
			void get(const Assistance::ro__db_statistics_sizet, size_t&);
			void get(const Assistance::ro__db_statistics_double, double&);


			bool isEq(const std::string, const std::string);
			bool isEq(const Assistance::io__conf_boolean, const bool);
			bool isEq(const Assistance::io__conf_float, const float);
			bool isEq(const Assistance::io__conf_integer, const int);
			bool isEq(const Assistance::io__conf_longlong, const long long);
			bool isEq(const Assistance::io__conf_string, const std::string);
			bool isEq(const Assistance::io__db_boolean, const bool);
			bool isEq(const Assistance::ro__db_string, const std::string);

			bool isEq(const int, const bool); // Allegro key
			bool isEq(const Assistance::ro__db_mouse_boolean, const bool);
			bool isEq(const Assistance::ro__db_mouse_float, const float);
			bool isEq(const Assistance::ro__db_statistics_sizet, const size_t);
			bool isEq(const Assistance::ro__db_statistics_double, const double);
		};
		




		void lswInit(); // init everything
		void forceExit(const char* = nullptr, const char* = nullptr, const char* = nullptr);
		void askForceExit(const char*, const char*, const char*);
		void alert(const char*, const char*, const char*);
	}
}