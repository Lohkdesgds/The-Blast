#pragma once

#include <string>
#include <allegro5/allegro.h>

/*
Useful to remember:
__FUNCSIG__ = returns as string the name of the func like a charm
__DATE__
__TIME__
*/


#define WAY ALLEGRO_OPENGL


namespace LSW {
	namespace v4 {
		namespace Constants {

			const std::string __interpret_date();

			const std::string version_app = std::string("V2.0.0#") + __interpret_date();

			// system.h stuff mostly
			const int start_audio_samples_max = 8;
			const int start_display_default_mode = ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | WAY;
			const int start_bitmap_default_mode = ALLEGRO_VIDEO_BITMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR;
			const bool start_force_720p = true;

			const std::string __match_unmatch = "_DATA";
			const std::string _match_extract = std::string("%LSW") + __match_unmatch;

			const std::string start_zip_default_extract_path = "%appdata%/Lohk's Studios/TheBlast/data/data.zip";
			const std::string start_zip_warning_file_txt = "%appdata%/Lohk's Studios/TheBlast/data/README.txt";
			const std::string start_zip_warning_default_text = "Hey,\nThe data.zip file will be replaced everytime you open the game. Just don't change it, because it won't be saved in this version, ok?\nThanks.";
			const std::string default_print_path = "%win_photos_path%/Lohk's Studios/TheBlast/Screenshots/"; // + YYYY_MM_DD-HH_MM.jpg
			const std::string config_default_file = "%appdata%/Lohk's Studios/TheBlast/config/config.lohk";
			
			const std::string conf_string_default_txt = "config";
			const int conf_default_screen_siz[2] = { 1280,720 }; // not sure

			// loggger.h stuff
			const std::string default_file_global_path = "%appdata%/Lohk's Studios/TheBlast/logs/latest.log";
			//const size_t len_func = 15;
			const size_t len_class = 45;
			const size_t max_lines_stored_by_memlog = 20;




			// not verified

			/*
			const std::string default_root_path = "%appdata%/Lohk's Studios/TheBlast/";
			const std::string default_global_path = "%appdata%/Lohk's Studios/TheBlast/logs/latest.log";
			const std::string default_data_path = "%appdata%/Lohk's Studios/TheBlast/data/";
			const std::string default_onefile_path = "%appdata%/Lohk's Studios/TheBlast/down.lohkf";

			namespace Image {
				const bool no_optimization = false;
				const double timing_optimization = 3.0;
			}

			namespace Sound {
				const float global_volume = 0.50;
			}

			namespace Config {
				const std::string config_default_file = "%appdata%/Lohk's Studios/TheBlast/config/config.lohk";
				const std::string conf_string_default_txt = "config";
				const int conf_default_screen_siz[2] = { 1280,720 };
			}









			const size_t max_lines_stored_by_memlog = 20;

			const size_t possible_download_size = 41754703;

			const int default_resolution_base[2] = { 1920,1080 };
			//const double timeout_image_unload = 3.0;
			//const int max_images_reload_per_sec = 75;
			const double sharpness_font = 2500.0;

			const double mouse_size_fixed_val = 0.001;

			const int default_layer_colliding_first = -9999;
			const int default_layer_backup = -9999;
			const int default_map_layer_backup = -9998;
			const int default_font_foreground_layer = -9997;
			const int default_settings_layer = -23;
			const int default_pauseonly_layer = -47;

			const bool debug = false;
			const double texts_timeout_interpret = 0.1;

			const bool exp_veryfast = false;

			const int max_events_stack = 10;
			const size_t max_string_length = 256;

			const unsigned max_loops_for_tps = 480;

			// thr
			const double collision_timer = 1.0 / 240;
			const int max_delayed_times = 90;
			const double functions_timer = 1.0 / 240;
			//const double checkEnd_timer = 1.0 / 2;
			const double calcLoops_timer = 1.0;
			const double updatepos_timer = 1.0 / 240;

			// fx
			const float amountofblur_bubbles = 85.0;
			const unsigned amountof_linesperline_draw = 20;
			const double precision_rotation_draw = 0.6 * ALLEGRO_PI / 360.0;

			const float blur_buf_display = 0.3; // 0 < x <= 1.0 // draw to display ratio
			const float blur_buf_frame = 0.4; // 0 < x <= 1.0 // copying this from display
			//const float blur_buf_frame_droppin = 0.1; // 0 < x <= 1.0 // al_clear frame
			const double fx_blur_timing = 1.0 / 32;

			const double display_fullscreen_toggle_min_time = 2.0; // sec
			const double display_osd_toggle_min_time = 1.0; // sec

			const std::string font_default_name = "font.ttf";
			const std::string font_altern_name_full = "C:\\Windows\\Fonts\\calibri.ttf";

			const int default_display_settings = ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_OPENGL | ALLEGRO_RESIZABLE;
			const int default_windowed_display_settings = ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE;

			//const double verify_imager_timer_created = 2.0;

			const std::string map_default_start = "BLOCK_";
			const unsigned map_default_len_name_int = 2;
			const int map_size_default_x = 32;
			const int map_size_default_y = 18;

			const double diff_time_show_last_resetCollision = 0.5;

			// game control
			const double user_default_size = 0.08;
			const int user_default_layer = 70;
			const int badboys_default_layer = 71;
			const int map_default_layer = 50;

			const double acceleration_by_user_normal = 0.004;
			const double acceleration_by_bot_normal = 0.002;

			const double entity_default_proportion_speed = 0.09; // proportion on add
			const double entity_default_jump_proportion_speed = 0.15; // proportion on add
			const double sprite_bot_default_proportion_speed = 0.11; // proportion on add

			const double max_acceleration_possible = 0.005;
			const double user_scale_compared_to_map = 0.7;
			const double sprite_default_collision_proportion_speed = 0.09; // proportion on collision acceleration add

			const double smoothness_keep_sprite_movement = 0.987;
			const double collision_acel_multiplier = 0.05;
			const double min_collision_acel = 0.0001;
			const double gravity_default = 0.020;
			const double entity_default_gravity_speed = 0.005; // proportion on add

			const size_t max_string_input_size = 1024;

			const double map_load_max_time = 0.20;
			const double map_resolve_max_time = 0.15;

			const double corrosion_default_time = 0.8;
			const double corrosion_default_time_start = 5.0;

			const double music_prop_time = 1.0 / 75;
			const double music_maxspeed = 1.25;
			*/



			inline const std::string __interpret_date()
			{
				std::string date = __DATE__;
				std::string time = __TIME__;

				char monthr[16];
				int year, monthn, day, hour, min, sec;

				sscanf_s(date.c_str(), "%s %d %d", monthr, 16, &day, &year);
				sscanf_s(time.c_str(), "%d:%d:%d", &hour, &min, &sec);

				std::string month = monthr;

				if (month == "Jan") {
					monthn = 1;
				}
				else if (month == "Feb") {
					monthn = 2;
				}
				else if (month == "Mar") {
					monthn = 3;
				}
				else if (month == "Apr") {
					monthn = 4;
				}
				else if (month == "May") {
					monthn = 5;
				}
				else if (month == "Jun") {
					monthn = 6;
				}
				else if (month == "Jul") {
					monthn = 7;
				}
				else if (month == "Aug") {
					monthn = 8;
				}
				else if (month == "Sep") {
					monthn = 9;
				}
				else if (month == "Oct") {
					monthn = 10;
				}
				else if (month == "Nov") {
					monthn = 11;
				}
				else if (month == "Dec") {
					monthn = 12;
				}
				else {
					monthn = 0;
				}

				char outtt[64];
				sprintf_s(outtt, "%04d%02d%02d%02d%02d", year, monthn, day, min, hour);

				return outtt;//std::to_string(year) + std::to_string(monthn) + std::to_string(day) + std::to_string(hour) + std::to_string(min);
			}
		}
	}
}