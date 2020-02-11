#pragma once

#include <string>
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

/*
Useful to remember:
__FUNCSIG__ = returns as string the name of the func like a charm
__DATE__
__TIME__
*/


#define WAY ALLEGRO_OPENGL
//#define FORCEDEBUG


namespace LSW {
	namespace v4 {
		namespace Constants {
			// this have to be global, come on																																* 100 eq 1.0							      * 1000 eq 1.0
			enum class ro__my_events { CUSTOM_EVENT_LOG_STRING = 512, CUSTOM_EVENT_SHARING_NEW_DISPLAY_SIZE, CUSTOM_EVENT_CALL_FULLSCREEN, CUSTOM_EVENT_EXTERNAL_EXIT_CALL, CUSTOM_EVENT_DISPLAY_UPDATE_RESOLUTION_SCALE, CUSTOM_EVENT_DISPLAY_CHROMA_FX /* <-- TO BE DONE */ }; 

			enum class directions {
				NORTH = 1 << 0,
				SOUTH = 1 << 1,
				EAST = 1 << 2,
				WEST = 1 << 3
			};

			/* VERSIONING */
			inline const std::string __interpret_date();
			inline const std::string version_app = std::string("B") + __interpret_date();

		
#ifdef _DEBUG
			inline const bool _is_on_debug_mode = true;
#elif defined FORCEDEBUG
			inline const bool _is_on_debug_mode = true;
#else
			inline const bool _is_on_debug_mode = false;
#endif

			// system.h stuff mostly
			inline const int start_audio_samples_max = 8;
			inline const int start_logdisp_default_mode = ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE;
			inline const int start_display_default_mode = ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | WAY;
			inline const int start_display_obligatory_flag_mode = ALLEGRO_RESIZABLE | WAY;
			inline const int start_bitmap_default_mode = ALLEGRO_VIDEO_BITMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR | ALLEGRO_MIPMAP;
			inline const bool start_force_720p = true;

			inline const std::string __match_unmatch = "_DATA";
			inline const std::string _match_extract = std::string("%LSW") + __match_unmatch;

			inline const std::string default_print_path = "%win_photos_path%/Lohk's Studios Screenshots/"; // + YYYY_MM_DD-HH_MM.jpg
			
			inline const std::string conf_string_default_txt = "config";
			inline const int conf_default_screen_siz[2] = { 1280,720 }; // not sure

			inline const double default_time_verification_limit_fps_config = 5.0; // sec

			// loggger.h stuff
			inline const size_t len_class = 45;
			inline const size_t max_lines_stored_by_memlog = 10;
			inline const size_t each_line_stored_by_memlog = 140;
			inline const int log_debug_screen_size[2] = { 750,350 };

			// drawing.h stuff
			inline const double text_default_sharpness_font = 75.0; // 2500
			inline const double text_timeout_interpret = 0.5;
			inline const double camera_default_slipperiness = 0.95;
			inline const double sprite_default_smoothness = 0.85; // alias: slipperines
			inline const double sprite_default_multiplier_global_div = 1.0e-1;
			inline const double sprite_default_power_global_div = 4.0;

			//const double sprite_default_limit_colision_speed_any = 0.15;
			inline const double intensity_player_run_max = 0.020; // limit: 0.025
			inline const double intensity_default_player_walk = 0.002;

			inline const double sprite_default_min_movement = 0.01; // adjustment because collision might be colliding forever
			// some fx
			inline const double amountofblur_bubbles = 85.0;
			inline const unsigned amountof_linesperline_draw = 20;
			inline const double precision_rotation_draw = 0.6 * ALLEGRO_PI / 360.0;
					   

			// sound.h stuff
			inline const double start_default_global_volume = 0.5;

			// button stuff
			inline const double default_delay_click = 0.7;
			



			inline const size_t __i_col_pos_t_update = 20; /*20*/
			inline const size_t __i_func_t_once = 5;  // can run on-the-fly functions up to 5 times per sec
			inline const size_t __i_thr_loop_timer_0 = 40,						// DISPLAY THREAD, may be smooth
						 __i_thr_loop_timer_1 = __i_col_pos_t_update,	// COLLISION THREAD may be timed
						 __i_thr_loop_timer_2 = 3,						// EVENTS THREAD don't need to be smooth at all
						 __i_thr_loop_timer_3 = 5;						// FUNCTIONS THREAD just need to check new functions and old ones not fast, but not slow

			inline const unsigned d_dbg_t_avg = 25; // to functional thread


			// map
			inline const size_t map_size_default_x = 32;
			inline const size_t map_size_default_y = 18;
			inline const double scale_g_map_default = 0.1251;

			// entity
			inline const double heal_default =    0.2  / __i_col_pos_t_update; // X per sec
			inline const double damage_default = -0.15 / __i_col_pos_t_update; // X per sec
			inline const double min_health = 0.0;
			inline const double max_health = 1.0;
			inline const double gravity = 0.981 / __i_col_pos_t_update;



			/* PRETTY USEFUL LAMBDA STUFF FOR BIG_TEMPLATES */
			template<typename H>
			inline const auto lambda_null_load = [](std::string& p, H*& r) -> bool { return false; };

			template<typename H>
			inline const auto lambda_null_unload = [](H*& b) -> void { b = nullptr;  return; };


			template<typename K>
			inline const auto lambda_default_load = [](std::string& p, K*& r) -> bool { return (r = new K()); };

			template<typename K>
			inline const auto lambda_default_unload = [](K*& b) -> void { if (b) delete b; b = nullptr; };


			inline const auto lambda_bitmap_load = [](std::string& p, ALLEGRO_BITMAP*& b) -> bool {
				return ((b = al_load_bitmap(p.c_str())));
			};
			inline const auto lambda_bitmap_unload = [](ALLEGRO_BITMAP*& b) -> void {
				if (al_is_system_installed() && b) { al_destroy_bitmap(b); b = nullptr; }
			};

			inline const auto lambda_font_load = [](std::string& p, ALLEGRO_FONT*& b) -> bool {
				return ((b = al_load_ttf_font(p.c_str(), text_default_sharpness_font, 0)));
			};
			inline const auto lambda_font_unload = [](ALLEGRO_FONT*& b) -> void {
				if (al_is_system_installed() && b) { al_destroy_font(b); b = nullptr; }
			};
			
			inline const auto lambda_sample_load = [](std::string& p, ALLEGRO_SAMPLE*& b) -> bool {
				return ((b = al_load_sample(p.c_str())));
			};
			inline const auto lambda_sample_unload = [](ALLEGRO_SAMPLE*& b) -> void {
				if (al_is_system_installed() && b) { al_destroy_sample(b); b = nullptr; }
			};





			// not verified

			/*
			inline const std::string default_root_path = "%appdata%/Lohk's Studios/TheBlast/";
			inline const std::string default_global_path = "%appdata%/Lohk's Studios/TheBlast/logs/latest.log";
			inline const std::string default_data_path = "%appdata%/Lohk's Studios/TheBlast/data/";
			inline const std::string default_onefile_path = "%appdata%/Lohk's Studios/TheBlast/down.lohkf";

			namespace Image {
				inline const bool no_optimization = false;
				inline const double timing_optimization = 3.0;
			}

			namespace Sound {
				inline const float global_volume = 0.50;
			}

			namespace Database {
				inline const std::string config_default_file = "%appdata%/Lohk's Studios/TheBlast/config/config.lohk";
				inline const std::string conf_string_default_txt = "config";
				inline const int conf_default_screen_siz[2] = { 1280,720 };
			}









			inline const size_t max_lines_stored_by_memlog = 20;

			inline const size_t possible_download_size = 41754703;

			inline const int default_resolution_base[2] = { 1920,1080 };
			//const double timeout_image_unload = 3.0;
			//const int max_images_reload_per_sec = 75;
			inline const double sharpness_font = 2500.0;

			inline const double mouse_size_fixed_val = 0.001;

			inline const int default_layer_colliding_first = -9999;
			inline const int default_layer_backup = -9999;
			inline const int default_map_layer_backup = -9998;
			inline const int default_font_foreground_layer = -9997;
			inline const int default_settings_layer = -23;
			inline const int default_pauseonly_layer = -47;

			inline const bool debug = false;
			inline const double texts_timeout_interpret = 0.1;

			inline const bool exp_veryfast = false;

			inline const int max_events_stack = 10;
			inline const size_t max_string_length = 256;

			inline const unsigned max_loops_for_tps = 480;

			// thr
			inline const double collision_timer = 1.0 / 240;
			inline const int max_delayed_times = 90;
			inline const double functions_timer = 1.0 / 240;
			//const double checkEnd_timer = 1.0 / 2;
			inline const double calcLoops_timer = 1.0;
			inline const double updatepos_timer = 1.0 / 240;

			// fx
			inline const float amountofblur_bubbles = 85.0;
			inline const unsigned amountof_linesperline_draw = 20;
			inline const double precision_rotation_draw = 0.6 * ALLEGRO_PI / 360.0;

			inline const float blur_buf_display = 0.3; // 0 < x <= 1.0 // draw to display ratio
			inline const float blur_buf_frame = 0.4; // 0 < x <= 1.0 // copying this from display
			//const float blur_buf_frame_droppin = 0.1; // 0 < x <= 1.0 // al_clear frame
			inline const double fx_blur_timing = 1.0 / 32;

			inline const double display_fullscreen_toggle_min_time = 2.0; // sec
			inline const double display_osd_toggle_min_time = 1.0; // sec

			inline const std::string font_default_name = "font.ttf";
			inline const std::string font_altern_name_full = "C:\\Windows\\Fonts\\calibri.ttf";

			inline const int default_display_settings = ALLEGRO_FULLSCREEN_WINDOW | ALLEGRO_OPENGL | ALLEGRO_RESIZABLE;
			inline const int default_windowed_display_settings = ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE;

			//const double verify_imager_timer_created = 2.0;

			inline const std::string map_default_start = "BLOCK_";
			inline const unsigned map_default_len_name_int = 2;

			inline const double diff_time_show_last_resetCollision = 0.5;

			// game control
			inline const double user_default_size = 0.08;
			inline const int user_default_layer = 70;
			inline const int badboys_default_layer = 71;
			inline const int map_default_layer = 50;

			inline const double acceleration_by_user_normal = 0.004;
			inline const double acceleration_by_bot_normal = 0.002;

			inline const double entity_default_proportion_speed = 0.09; // proportion on add
			inline const double entity_default_jump_proportion_speed = 0.15; // proportion on add
			inline const double sprite_bot_default_proportion_speed = 0.11; // proportion on add

			inline const double max_acceleration_possible = 0.005;
			inline const double user_scale_compared_to_map = 0.7;
			inline const double sprite_default_collision_proportion_speed = 0.09; // proportion on collision acceleration add

			inline const double smoothness_keep_sprite_movement = 0.987;
			inline const double collision_acel_multiplier = 0.05;
			inline const double min_collision_acel = 0.0001;
			inline const double gravity_default = 0.020;
			inline const double entity_default_gravity_speed = 0.005; // proportion on add

			inline const size_t max_string_input_size = 1024;

			inline const double map_load_max_time = 0.20;
			inline const double map_resolve_max_time = 0.15;

			inline const double corrosion_default_time = 0.8;
			inline const double corrosion_default_time_start = 5.0;

			inline const double music_prop_time = 1.0 / 75;
			inline const double music_maxspeed = 1.25;
			*/



			inline const std::string __interpret_date()
			{
				std::string date = __DATE__;
				std::string time = __TIME__;

				char monthr[16] = { '\0' };
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