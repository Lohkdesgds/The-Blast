#pragma once

#include "..\safer\safer.h"
#include <allegro5/allegro.h>

namespace LSW {
	namespace v2 {
		namespace Defaults {

			const Safer::safe_string version_app = "V0.1.0A";

			const Safer::safe_string default_root_path = "%appdata%/Lohk's Studios/TheBlast/";
			const Safer::safe_string default_global_path = "%appdata%/Lohk's Studios/TheBlast/logs/latest.log";
			const Safer::safe_string default_data_path = "%appdata%/Lohk's Studios/TheBlast/data/";
			const Safer::safe_string default_onefile_path = "%appdata%/Lohk's Studios/TheBlast/down.lohkf";

			//const Safer::safe_string initial_call_url = "https://tutoriais-de-prog.000webhostapp.com/files_apps/"; // change later?
			const Safer::safe_string call_url_file = "https://www.dropbox.com/s/er912k5u5is8h42/package.lohkf?dl=1";//"https://tutoriais-de-prog.000webhostapp.com/files_apps/package.lohkf";

			const size_t max_lines_stored_by_memlog = 20;

			const size_t possible_download_size = 41754703;

			const int default_resolution_base[2] = { 1920,1080 };
			const double timeout_image_unload = 3.0;
			//const int max_images_reload_per_sec = 75;
			const double sharpness_font = 2500.0;

			const double mouse_size_fixed_val = 0.001;

			const int default_layer_colliding_first = -9999;
			const int default_layer_backup = -9999;
			const int default_font_foreground_layer = -9999;

			const bool debug = false;

			const bool exp_veryfast = false;

			const int max_events_stack = 10;
			const size_t max_string_length = 256;

			const unsigned max_loops_for_tps = 480;

			const double smoothness_keep_sprite_movement_rotational = 0.99;
			const double smoothness_keep_sprite_movement = 0.98;
			const double collision_acel_multiplier = 0.001;
			const double min_collision_acel = 0.0005;

			// thr
			const double collision_timer = 1.0 / 240;
			const int max_delayed_times = 90;
			const double functions_timer = 1.0 / 240;
			const double checkEnd_timer = 1.0 / 2;
			const double calcLoops_timer = 1.0;
			const double updatepos_timer = 1.0 / 240;

			// fx
			const float amountofblur_bubbles = 85.0;
			const unsigned amountof_linesperline_draw = 20;
			const double precision_rotation_draw = 0.6 * ALLEGRO_PI / 360.0;

			const float blur_buf_display = 0.3; // 0 < x <= 1.0 // draw to display ratio
			const float blur_buf_frame = 0.4; // 0 < x <= 1.0 // copying this from display
			//const float blur_buf_frame_droppin = 0.1; // 0 < x <= 1.0 // al_clear frame
			const double fx_blur_timing = 1.0/32;

			const double display_fullscreen_toggle_min_time = 2.0; // sec

			const Safer::safe_string font_default_name = "font.ttf";
			const Safer::safe_string font_altern_name_full = "C:\\Windows\\Fonts\\calibri.ttf";

			const int default_display_settings = 516;//ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_OPENGL;

			const double verify_imager_timer_created = 2.0;

			const Safer::safe_string map_default_start = "BLOCK_";
			const unsigned map_default_len_name_int = 2;
			const int map_size_default_x = 32;
			const int map_size_default_y = 18;

			const double diff_time_show_last_resetCollision = 0.5;

			// game control
			const double acceleration_by_user_normal = 0.003;
			const double user_scale_compared_to_map = 0.7;

			const size_t max_string_input_size = 1024;
		}
	}
}