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

#include "..\custom_abort\abort.h"
#include "..\tools\tools.h"

namespace LSW {
	namespace v4 {

		namespace Constants {

			const int start_audio_samples_max = 8;
			const int start_display_default_mode = ALLEGRO_WINDOWED | ALLEGRO_DIRECT3D_INTERNAL;
			const bool start_force_720p = true;

			const std::string __match_unmatch = "_DATA";
			const std::string _match_extract = std::string("%LSW") + __match_unmatch;

			const std::string start_zip_default_extract_path = "%appdata%/Lohk's Studios/TheBlast/data/data.zip";
			const std::string start_zip_warning_file_txt = "%appdata%/Lohk's Studios/TheBlast/data/README.txt";
			const std::string start_zip_warning_default_text = "Hey,\nThe data.zip file will be replaced everytime you open the game. Just don't change it, because it won't be saved in this version, ok?\nThanks.";

			const std::string default_print_path = "%win_photos_path%/Lohk's Studios/TheBlast/Screenshots/"; // + YYYY_MM_DD-HH_MM.jpg
		}

		namespace Assistance {

			class __submode {
			public:
				int x = 0, y = 0, hz = 0;
				bool operator==(__submode sm) { return (sm.x == x && sm.y == y && sm.hz == hz); }
			};
			struct modes {
				std::vector<__submode> options;
				int lastmode = Constants::start_display_default_mode;
			};

		}
		

		class __systematic {
			Assistance::modes* l = nullptr;
			std::string extracted_zip_at;
			bool initialized = false;

			void __extract_package(float* = nullptr);
			void __ensure_warn_package();
			bool __loadPackage();
		public:
			~__systematic();

			void init_system();

			void __set_new_display_mode(const int);
			bool __check_resolution_existance(const int, const int, const int);
			const auto __get_available_res();
		};


		static __systematic __g_sys; // startup

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> some raw stuff

		class __raw_display {
			ALLEGRO_DISPLAY* d = nullptr;
			int x, y, f, h;
			bool printing = false;

			std::thread* printthr = nullptr;

			void _init(const int, const int, const int, int); // x, y, flags, hz
			void _print();
			void __print_thr_autodel(ALLEGRO_BITMAP*, const std::string);
		public:
			__raw_display();
			__raw_display(const int, const int, const int = Constants::start_display_default_mode, int = 0); // x, y, flags, hz
			~__raw_display();

			void restart();
			void flip();
			void clear_to(const ALLEGRO_COLOR);
			void close();
			bool exist();

			void print();
		};

		class __raw_image {
		public:
			ALLEGRO_BITMAP* bmp = nullptr;
			std::string path;
			std::string id;

			~__raw_image();
		};

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> not raw stuff

		class Textures {
			static std::vector<std::shared_ptr<__raw_image>> imgs;
		public:
			void load(const std::string, const std::string);
			bool get(const std::string, std::weak_ptr<__raw_image>&);
			void del(const std::string);
		};




		void lsw_init(); // init everything
	}
}