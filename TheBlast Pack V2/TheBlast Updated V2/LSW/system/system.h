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

			enum class conf_b { HAD_ERROR, WAS_OSD_ON, WAS_FULLSCREEN };
			enum class conf_f { LAST_VOLUME };
			enum class conf_i { SCREEN_X, SCREEN_Y };
			enum class conf_ll { _TIMES_LIT };
			enum class conf_s { LAST_VERSION, LAST_PLAYERNAME, LAST_COLOR };

			const std::string conf_b_str[] = { "had_error", "was_osd_on", "fullscreen" };
			const std::string conf_f_str[] = { "last_volume" };
			const std::string conf_i_str[] = { "screen_width","screen_height" };
			const std::string conf_ll_str[] = { "times_open" };
			const std::string conf_s_str[] = { "last_version","playername","playercolor" };

			const std::string conf_bool_s[] = { "false", "true" };

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


		

		class Config {
			static ALLEGRO_CONFIG* c;
			static std::mutex m;
			gfile logg;
		public:
			Config();
			~Config();

			void set(const std::string, const std::string);
			void set(const Assistance::conf_b, const bool);
			void set(const Assistance::conf_f, const float);
			void set(const Assistance::conf_i, const int);
			void set(const Assistance::conf_ll, const long long);
			void set(const Assistance::conf_s, const std::string);

			void get(const std::string, std::string&, const std::string);
			void get(const Assistance::conf_b, bool&, const bool);
			void get(const Assistance::conf_f, float&, const float);
			void get(const Assistance::conf_i, int&, const int);
			void get(const Assistance::conf_ll, long long&, const long long);
			void get(const Assistance::conf_s, std::string&, const std::string);

			const bool isEq(const std::string, const std::string);
			const bool isEq(const Assistance::conf_b, const bool);
			const bool isEq(const Assistance::conf_f, const float);
			const bool isEq(const Assistance::conf_i, const int);
			const bool isEq(const Assistance::conf_ll, const long long);
			const bool isEq(const Assistance::conf_s, const std::string);
		};
		




		void lswInit(); // init everything
		void forceExit(const char* = nullptr, const char* = "", const char* = "");
	}
}