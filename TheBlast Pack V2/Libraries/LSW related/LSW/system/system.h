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
#include <shellapi.h>

#include "..\defaults.h"

#include "..\shared_constants\constants.h"
#include "..\tools\tools.h"
#include "..\custom_abort\abort.h"
#include "..\logger\logger.h"

#include "..\download_manager\downloader.h"
#include "..\hash\sha256.h"
#include "..\pipe\pipe.h"

namespace LSW {
	namespace v4 {

		// constants now at shared_constants\constants.h

		namespace Constants {

			// actual config (no sense on using size) **** IF CHANGES, CHECK Database::internalCheck ****
			enum class io__conf_boolean { WAS_OSD_ON, ENABLE_SECOND_DEBUGGING_SCREEN, ULTRADEBUG, DOUBLEBUFFERING, HIDEMOUSE };
			enum class io__conf_double { LAST_VOLUME, RESOLUTION_BUFFER_PROPORTION, FX_AMOUNT };
			enum class io__conf_integer { SCREEN_X, SCREEN_Y, SCREEN_FLAGS, SCREEN_PREF_HZ, LIMIT_FPS };
			enum class io__conf_longlong { _TIMES_LIT };
			enum class io__conf_string { LAST_VERSION, LAST_PLAYERNAME, PRINT_PATH };
			enum class io__conf_color{ LAST_COLOR_TRANSLATE };

			// internal configuration not from file but not automatically set or functional
			enum class io__db_boolean { SAVING_STRING_INPUT, size };
			enum class io__db_sizet   { MAXIMUM_STRING_INPUT_LEN, size};
			enum class ro__db_string  { LAST_STRING, CURRENT_STRING };

			// on memory while running
			enum class ro__db_mouse_boolean { MOUSE_0, MOUSE_1, MOUSE_2, MOUSE_3, MOUSE_4, MOUSE_5, MOUSE_6, MOUSE_7, size, IS_ANY_PRESSED }; // Constants::max_mouse_set_buttons
			enum class ro__db_mouse_double { MOUSE_X, MOUSE_Y, RAW_MOUSE_X, RAW_MOUSE_Y, size };
			enum class ro__db_statistics_sizet { FRAMESPERSECOND, COLLISIONSPERSECOND, USEREVENTSPERSECOND, ADVANCEDFUNCSPERSECOND, size};
			enum class ro__db_statistics_double { INSTANT_FRAMESPERSECOND, INSTANT_COLLISIONSPERSECOND, INSTANT_USEREVENTSPERSECOND, INSTANT_ADVANCEDFUNCSPERSECOND, size };
			enum class ro__db_thread_string { KEY_ADD, KEY_ADD_SET_LEN, KEY_ERASE, KEY_SET };

			// functional
			enum class io__db_functional_opt {MOUSE_KEY,KEYBOARD_KEY,MOUSE_LEFT,KEYBOARD_LEFT};

			inline const std::string ro__conf_boolean_str[] = { "was_osd_on", "second_screen_debug", "ultradebug", "double_buffering_screen", "hidemouse" };
			inline const std::string ro__conf_double_str[] = { "last_volume", "resolution_proportion", "fx_amount" };
			inline const std::string ro__conf_integer_str[] = { "screen_width","screen_height", "last_display_flags","pref_refresh_rate","limit_framerate_to" };
			inline const std::string ro__conf_longlong_str[] = { "times_open" };
			inline const std::string ro__conf_string_str[] = { "last_version","playername", "prints_path" };
			inline const std::string ro__conf_color_str[] = { "playercolor" };

			inline const std::string ro__conf_truefalse_str[] = { "false", "true" };

			class display_mode {
			public:
				int x = 0, y = 0, hz = 0;
				bool operator==(display_mode sm) { return (sm.x == x && sm.y == y && sm.hz == hz); }
			};
		}
		

		class __systematic {
			struct modes {
				std::vector<Constants::display_mode> options;
				int lastmode = Constants::start_display_default_mode;
			};

			struct custom_systematic_data {
				modes *l = nullptr;

				std::string extracted_zip_at,
					expect_zip_at;
				bool initialized = false;
				bool already_set_physfs_root = false;

#ifdef AUTOMATIC_LAUNCH
				bool has_once_launched = false;
#endif
				bool has_init_allegro_once = false;

				std::vector<std::pair<std::string, std::string>> hashupdate_stuff;
#ifdef USE_DISCORD_API_EXTERNAL
				Pipe discord_pipe;
#endif
			};

			static custom_systematic_data d;

			void __extract_package(float* = nullptr);
			void __nointernalzip_extract_package();
			bool __loadPackage();
			bool __nointernalzip_loadPackage();
#ifdef USE_DISCORD_API_EXTERNAL
			void __extract_discord_external_module_proj(); // exe and dll
#endif
		public:
#ifdef AUTOMATIC_LAUNCH
			__systematic();
#endif
			void deinitSystem();

			void setZipLocation(const std::string);
			void setupPhysfs();

			void initAllegro();

			void initSystemNoZip(); // no zip verification or setup
			void initSystem(const bool = true);
			void posInit_forceWithNoZipAnyway();

			void setInterface(); // enable PHYSFS interface
			void unsetInterface(); // undo PHYSFS interface

			void addPathFile(const char*);
			void delPathFile(const char*);

			void setNewDisplayMode(const int);
			bool checkResolutionExistance(const int, const int, const int) const;
			const auto getAvailableResolutions() const;
		};

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> some raw stuff


		

		class Database {

			struct custom_data {
				ALLEGRO_CONFIG* c = nullptr;
				std::mutex m = std::mutex();
				bool keys[ALLEGRO_KEY_MAX] = { false };
				bool mouse[+Constants::ro__db_mouse_boolean::size] = { false };

				// memory only
				float db_mouse_axes[4] = { 0.0 };
				size_t db_statistics_sizet[+Constants::ro__db_statistics_sizet::size] = { 0 };
				double db_statistics_double[+Constants::ro__db_statistics_double::size] = { 0 };

				// memory but not automatically, I mean, it's like the Sprite stuff you know
				bool b[+Constants::io__db_boolean::size] = { false };
				size_t sz[+Constants::io__db_sizet::size] = { 0 };

				ALLEGRO_COLOR color = al_map_rgb(0, 0, 0);

				std::string curr_string;
				std::vector<char> curr_string_keylen;
				char curr_string_keylen_val = 1;
				size_t real_string_size = 0, real_string_size_delayed = 0;
				std::string last_string;

				// purely automatic handleing
				std::function<void(void)>			func_mb[+Constants::ro__db_mouse_boolean::size];	// each mouse key has a function
				std::function<void(void)>			func_kb[ALLEGRO_KEY_MAX];							// each key can have its custom function
				std::function<void(void)>			func_mb_u[+Constants::ro__db_mouse_boolean::size];	// unhold
				std::function<void(void)>			func_kb_u[ALLEGRO_KEY_MAX];							// unhold

				

				std::thread* constantly_saving = nullptr;
				bool still_running = false;

				std::string config_raw_path;
			};

			static custom_data data;
			gfile logg;

			void keepSavingThr();

			void internalCheck();
			void __skipAndLoad(std::string); // lalala
		public:
#ifdef AUTOMATIC_LAUNCH
			Database();
#endif

			void load(std::string); // please set a path ONCE
			void unload();

			void set(const std::string, const std::string);
			void set(const Constants::io__conf_boolean, const bool);
			void set(const Constants::io__conf_double, const double);
			void set(const Constants::io__conf_integer, const int);
			void set(const Constants::io__conf_longlong, const long long);
			void set(const Constants::io__conf_string, const std::string);
			void set(const Constants::io__conf_color, const ALLEGRO_COLOR);

			void set(const Constants::io__db_boolean, const bool);
			void set(const Constants::io__db_sizet, const size_t);

			void set(const int, const bool); // Allegro key
			void set(const Constants::ro__db_mouse_boolean, const bool);
			void set(const Constants::ro__db_mouse_double, const double);
			void set(const Constants::ro__db_statistics_sizet, const size_t);
			void set(const Constants::ro__db_statistics_double, const double);
			void set(const Constants::ro__db_thread_string, const char = 0);

			void set(const Constants::io__db_functional_opt, const int, const std::function<void(void)> = std::function<void(void)>());

			bool check(const Constants::io__conf_boolean);
			bool check(const Constants::io__conf_double);
			bool check(const Constants::io__conf_integer);
			bool check(const Constants::io__conf_longlong);
			bool check(const Constants::io__conf_string);
			bool check(const Constants::io__conf_color);


			bool get(const std::string, std::string&);
			bool get(const Constants::io__conf_boolean, bool&);
			bool get(const Constants::io__conf_double, double&);
			bool get(const Constants::io__conf_integer, int&);
			bool get(const Constants::io__conf_longlong, long long&);
			bool get(const Constants::io__conf_string, std::string&);
			bool get(const Constants::io__conf_color, ALLEGRO_COLOR&);

			bool get(const Constants::io__db_boolean, bool&);
			bool get(const Constants::io__db_sizet, size_t&);
			bool get(const Constants::ro__db_string, std::string&);

			bool get(const int, bool&, const bool); // Allegro key
			bool get(const Constants::ro__db_mouse_boolean, bool&);
			bool get(const Constants::ro__db_mouse_double, double&);
			bool get(const Constants::ro__db_statistics_sizet, size_t&);
			bool get(const Constants::ro__db_statistics_double, double&);


			bool isEq(const std::string, const std::string);
			bool isEq(const Constants::io__conf_boolean, const bool);
			bool isEq(const Constants::io__conf_double, const double);
			bool isEq(const Constants::io__conf_integer, const int);
			bool isEq(const Constants::io__conf_longlong, const long long);
			bool isEq(const Constants::io__conf_string, const std::string);
			bool isEq(const Constants::io__conf_color, const ALLEGRO_COLOR);

			bool isEq(const Constants::io__db_boolean, const bool);
			bool isEq(const Constants::io__db_sizet, const size_t);
			bool isEq(const Constants::ro__db_string, const std::string);

			bool isEq(const int, const bool); // Allegro key
			bool isEq(const Constants::ro__db_mouse_boolean, const bool);
			bool isEq(const Constants::ro__db_mouse_double, const double);
			bool isEq(const Constants::ro__db_statistics_sizet, const size_t);
			bool isEq(const Constants::ro__db_statistics_double, const double);
		};



		class Display {
			ALLEGRO_DISPLAY* d = nullptr;
			ALLEGRO_BITMAP* buf_ = nullptr;
			double quick_fx = 0.0;
			double should_check_acknowledge_and_prop_buf = 0.0;

			double last_draw = 0;
			int limit_fps = 0;

			double last_config_check = 0;

			std::mutex d_try;
			int x, y, f, h;
			bool printing = false;

			std::thread* printthr = nullptr;
			bool printthrdone = true;

			void _init(const int, const int, const int, int); // x, y, flags, hz
			void _print();
			void __print_thr_autodel(ALLEGRO_BITMAP*, const std::string);
			void __check_acknowledge_n_buf();
		public:
			Display();
			Display(const int, const int, const int = Constants::start_display_default_mode, int = 0); // x, y, flags, hz
			~Display();

			void restart();
			void flip();
			void clearTo(ALLEGRO_COLOR);
			void close();
			bool exist();

			void setToCheckBufferResConfig();
			double getAppliedProportion();

			void print();

			ALLEGRO_DISPLAY*& getDisplay();
			void acknowledgeResize();
		};


		void forceExit(const char* = nullptr, const char* = nullptr, const char* = nullptr);
		void askForceExit(const char*, const char*, const char*);
		void alert(const char*, const char*, const char*);
	}
}