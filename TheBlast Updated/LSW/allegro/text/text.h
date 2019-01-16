#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Text {

			enum _text_opt_str {SETSTRING, SETID, SETFOLLOW};
			enum _text_opt_bool{SHOW,USEBUFOPT,AFFECTED_BY_CAM};
			enum _text_opt_db  {POSX,POSY,UPDATETIME};
			enum _text_opt_int {MODE, LAYER};
			enum _text_opt_color {COLOR};
			enum _text_opt_sprite {SPRITE};

			enum mode_int{ALIGN_LEFT = ALLEGRO_ALIGN_LEFT,ALIGN_CENTER = ALLEGRO_ALIGN_CENTER,ALIGN_RIGHT = ALLEGRO_ALIGN_RIGHT};
			// 2
			const std::string tags[] = { "%pos_x%", "%pos_y%", "%screen_pos_x%", "%screen_pos_y%","%is_following%", "%color_r%", "%color_g%", "%color_b%", "%color_a%", "%mode%", "%time%", "%is_using_buf%", "%g_b_res_x%", "%g_b_res_y%", "%base_refresh_rate%", "%fps%", "%tps%", "%sprite_frame%", "%cam_x%", "%cam_y%", "%cam_zoom%", "%curr_string%", "%last_string%", "%mouse_x%", "%mouse_y%", "%sprite_speed_x%", "%sprite_speed_y%" };
			enum tags_e{T_POSX,T_POSY,T_SCREEN_POSX, T_SCREEN_POSY,T_ISFOLLOWING,T_COLOR_R, T_COLOR_G, T_COLOR_B, T_COLOR_A,T_MODE,T_TIME,T_ISUSINGBUF,T_GB_RESX, T_GB_RESY,T_REFRESHRATE,T_FPS,T_TPS,T_SPRITE_FRAME,T_CAM_X,T_CAM_Y,T_CAM_ZOOM,T_CURRSTRING,T_LASTSTRING,T_MOUSE_X,T_MOUSE_Y,T_SPRITE_SPEEDX,T_SPRITE_SPEEDY};
					   		

			class text {
				Sprite::sprite* follow = nullptr;
				double off_plr[2] = { 0.0,0.0 };

				Events::big_event bev;

				int layer = 0;

				double pos[2] = { 0,0 };
				int mode = ALLEGRO_ALIGN_CENTER;
				bool affected_by_camera = true;

				static ALLEGRO_FONT* default_font;

				ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);
				Safer::safe_string orig_str;
				Safer::safe_string string;
				bool str_upd = false;

				//static size_t counter;
				//Sprite::sprite* sbuf = nullptr;
				//ALLEGRO_BITMAP* buf = nullptr;
				bool usebuf = false;
				bool show = true;

				double lastinterpret = 0;
				double update_time = 0.05; // sec

				static Display::display* ref_disp;
				static unsigned text_count;

				Sprite::sprite* local_paint = nullptr;
				Image::image_low* local_paint_i = nullptr;

				Safer::safe_string id, path;

				void _draw();
				void _interpretTags(Safer::safe_string&);

				void setFollow(const Safer::safe_string);
				void setFollow(Sprite::sprite*);
				void loadInternalBMP();
			public:
				void verify(const bool = false); // reload
				const bool load(const Safer::safe_string = Defaults::font_default_name, const bool = false);
				/*const bool loadFromDatabase(const Safer::safe_string = Defaults::font_default_name);
				const bool loadFromURL(const Safer::safe_string, const Safer::safe_string); // url, name it as (THE FILE! NOT ID)*/

				void unload();

				void set(const _text_opt_str, const Safer::safe_string);
				void set(const _text_opt_bool, const bool);
				void set(const _text_opt_db, const double);
				void set(const _text_opt_color, const ALLEGRO_COLOR);
				void set(const _text_opt_int, const int);

				void get(const _text_opt_str, Safer::safe_string&);
				void get(const _text_opt_bool, bool&);
				void get(const _text_opt_db, double&);
				void get(const _text_opt_color, ALLEGRO_COLOR&);
				void get(const _text_opt_int, int&);
				void get(const _text_opt_sprite, Sprite::sprite*&);


				void setID(const Safer::safe_string);

				const bool amI(const Safer::safe_string);

				void setMainDisplay(Display::display*);

				void draw(const int);
			};

			size_t _find(const Safer::safe_string, Safer::safe_vector<text*>&, bool&);
			void _draw();

		}
	}
}