#pragma once

#include "..\..\all\enable_all.h"

/*
sub_main.h and sub_main.cpp
They were made for the game to be easier to work with. Each part of LSW app works by themselves, but this one makes them all into one thing for the game to look easier. Look at this like an example of game with the library.
*/


namespace LSW {
	namespace v2 {
		namespace Main {

			enum levels {
				VERYFIRST = -2,
				INTRO,
				MENU,
				PLAYING,
				SETTINGS,
				LOSER
			};
			enum colors {
				WHITE,
				BLACK,
				RED,
				GREEN,
				BLUE,
				CYAN,
				MAGENTA,
				YELLOW
			};

			struct initialization {
				int res[2] = { -1,-1 };
				//int hz = -1;
				int mode = Defaults::default_windowed_display_settings;
				bool fixed_memory_flag = false;
				bool preload_textures = true;
				bool skip_download_flag = false;
				bool skip_intro_flag = false;
				bool enable_console = false;
				bool full_log = false;
			};

			struct _main_data {
				d_sprite_database spr_data; // ya, does need to draw!
				d_images_database img_data; // does not need to draw.
				d_texts_database  txt_data; // ya, does need to draw!
				d_musics_database msk_data; // there's no way to draw.
				d_entity_database ent_data;

				Display::display disp;
				Camera::camera_g cam; // cam stuff
				Layer::layerer lyr; // layering stuff
				Events::big_event bev;

				initialization setup;
				bool hasInit = false;

				/* PLAYING */
				levels playing = VERYFIRST;
			};

			struct __fast_cam_preset {
				Camera::_transf_data cam_settings;
				int config_number = 0;
			};
			struct __fast_lyr_preset {
				std::map<int, Layer::_layer_down> each_layer_settings; // layer number X behavior with <any defined on _layer_down>
				Layer::mode layer_mode = Layer::STANDARD;
				int config_number;
			};

			struct __player_settings {
				Safer::safe_string nickname;
				ALLEGRO_COLOR color = al_map_rgb(0,255,0);
				colors color_interp = GREEN;
			};

			class main {
				static _main_data* data;
				float actual_perc = 0.0;
				double lastF3switch = 0;
				unsigned level_now = 0;
				bool lastOSDstats = true;
				__player_settings player_settings;
				//double rgb_start = 0;
			public:
				main();
				void unloadAll();

				const bool check(const Safer::safe_string); // function name

				initialization __getConfigCopy();
				void __saveConfig(const initialization);

				// configurations
				void __set_camera_quick(const __fast_cam_preset);
				void __set_layer_quick(const __fast_lyr_preset);

				// fast apply
				void __apply_cam_number(const int);
				void __apply_layer_number(const int);

				Display::display* __getDisplay();

				const bool _rgb_clear_screen_load();
				const levels _rgb_pause_screen();
				//void _dead_screen();

				void init();
				void load();

				const bool play();

				// load()
				void __internal_thr_once_load();
				// play()
				const bool __internal_task_level_common();
				// any draw
				void __internal_check_hotkeys();
			};

			void __xtract_dis(void*, Safer::safe_string, Safer::safe_string);
			void __load(main*);

			const initialization interpret_console_entry(const int, char*[]);

			void __random_map_load(Map::map*);

			Safer::safe_string interpret_color(const colors);
			colors interpret_from_color_name(const Safer::safe_string);
			ALLEGRO_COLOR interpret_to_color(const colors);
		}
	}
}