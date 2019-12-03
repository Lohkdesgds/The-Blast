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
#include <map>
#include <Windows.h>
#include <vector>
#include <memory>

#include "..\custom_abort\abort.h"
#include "..\system\system.h"
#include "..\big_templates\big_templates.h"

namespace LSW {
	namespace v4 {

		namespace Assistance {

			enum class io___float_camera    { SCALE_X, SCALE_Y, SCALE_G, OFFSET_X, OFFSET_Y, ROTATION, _MAX_CAM_OPT };
			
			enum class io___vecstring_sprite{ ADDMULTIPLE, REMOVEMULTIPLE };
			enum class io___string_sprite	{ ADD, REMOVE, SPRITE_ID };
			enum class io___double_sprite	{ POSX, POSY, SCALEX, SCALEY, CENTERX, CENTERY, SCALEG, ROTATION /*DEGREES*/, SPEEDX, SPEEDY, SPEEDROT, SMOOTHNESS_X, SMOOTHNESS_Y, size, /* since here they are not Sprite exactly stuff*/ ANIMATION_FPS };
			enum class io___boolean_sprite	{ DRAW, COLLIDE, IS_COLLIDING, AFFECTED_BY_COLLISION, AFFECTED_BY_CAM, SHOWDOT, SHOWBOX, FOLLOWMOUSE, FOLLOWKEYBOARD, USE_TINTED_DRAWING, size, /* since here they are not Sprite exactly stuff*/ LOOPFRAMES };
			enum class io___integer_sprite	{ LAYER, size };
			enum class io___size_sprite		{ SIZE, FRAME, size };
			enum class io___color_sprite	{ TINT, size };

			enum class cl___sprite_nswe		{ NORTH, SOUTH, WEST, EAST };

			enum class _text_opt_str { SETSTRING, SETID, SETFOLLOW, SETFONT };
			enum class _text_opt_bool { SHOW, AFFECTED_BY_CAM };
			enum class _text_opt_db { POSX, POSY, SCALEG, ROTATION, UPDATETIME };
			enum class _text_opt_int { MODE, LAYER };
			enum class _text_opt_color { COLOR };

			enum class io___alignment_text { ALIGN_LEFT = ALLEGRO_ALIGN_LEFT, ALIGN_CENTER = ALLEGRO_ALIGN_CENTER, ALIGN_RIGHT = ALLEGRO_ALIGN_RIGHT };
			// 2

			class __t {
				std::string n;
				int p;
			public:
				__t(const std::string w, const int v) { n = w; p = v; }
				const std::string s() const { return n; }
				const int i() const { return p; }
			};

			enum class tags_e {
				T_POSX,
				T_POSY,
				T_SCREEN_POSX,
				T_SCREEN_POSY,
				T_ISFOLLOWING,
				T_COLOR_R,
				T_COLOR_G,
				T_COLOR_B,
				T_COLOR_A,
				T_MODE,
				T_TIME,
				T_ISUSINGBUF,
				T_GB_RESX,
				T_GB_RESY,
				T_REFRESHRATE,
				T_FPS,
				T_TPS_COUNT,
				T_TPS_COLLISION,
				T_TPS_FUNCTIONS,
				T_TPS_SECOND_TAKEN,
				T_TPS_POSUPDATE,
				T_SPRITE_FRAME,
				T_CAM_X,
				T_CAM_Y,
				T_CAM_ZOOM,
				T_CAM_ZOOMX,
				T_CAM_ZOOMY,
				T_CURRSTRING,
				T_LASTSTRING,
				T_MOUSE_X,
				T_MOUSE_Y,
				T_SPRITE_SPEEDX,
				T_SPRITE_SPEEDY,
				T_SPRITE_NAME,
				T_SPRITE_ENTITY_NAME,
				T_SPRITE_ENTITY_HEALTH,
				T_IMAGES_LOADED,
				T_SPRITES_LOADED,
				T_TEXTS_LOADED,
				T_TRACKS_LOADED,
				T_ENTITIES_LOADED,
				T_GARBAGE_TOTAL,
				T_IMAGES_GARBAGE,
				T_SPRITES_GARBAGE,
				T_TEXTS_GARBAGE,
				T_TRACKS_GARBAGE,
				T_ENTITIES_GARBAGE
			};
			const __t tags[] = { __t("%pos_x%",				+tags_e::T_POSX),
									 __t("%pos_y%",				+tags_e::T_POSY),
									 __t("%screen_pos_x%",		+tags_e::T_SCREEN_POSX),
									 __t("%screen_pos_y%",		+tags_e::T_SCREEN_POSY),
									 __t("%is_following%",		+tags_e::T_ISFOLLOWING),
									 __t("%color_r%",			+tags_e::T_COLOR_R),
									 __t("%color_g%",			+tags_e::T_COLOR_G),
									 __t("%color_b%",			+tags_e::T_COLOR_B),
									 __t("%color_a%",			+tags_e::T_COLOR_A),
									 __t("%mode%",				+tags_e::T_MODE),
									 __t("%time%",				+tags_e::T_TIME),
									 __t("%is_using_buf%",		+tags_e::T_ISUSINGBUF),
									 __t("%g_b_res_x%",			+tags_e::T_GB_RESX),
									 __t("%g_b_res_y%",			+tags_e::T_GB_RESY),
									 __t("%base_refresh_rate%", +tags_e::T_REFRESHRATE),
									 __t("%fps%",				+tags_e::T_FPS),
									 __t("%tps%",				+tags_e::T_TPS_COUNT),
									 __t("%tps_col%",			+tags_e::T_TPS_COLLISION),
									 __t("%tps_funcs%",			+tags_e::T_TPS_FUNCTIONS),
									 __t("%tps_second%",		+tags_e::T_TPS_SECOND_TAKEN),
									 __t("%tps_posupd%",		+tags_e::T_TPS_POSUPDATE),
									 __t("%sprite_frame%",		+tags_e::T_SPRITE_FRAME),
									 __t("%cam_x%",				+tags_e::T_CAM_X),
									 __t("%cam_y%",				+tags_e::T_CAM_Y),
									 __t("%cam_zoom%",			+tags_e::T_CAM_ZOOM),
									 __t("%cam_zoom_x%",		+tags_e::T_CAM_ZOOMX),
									 __t("%cam_zoom_y%",		+tags_e::T_CAM_ZOOMY),
									 __t("%curr_string%",		+tags_e::T_CURRSTRING),
									 __t("%last_string%",		+tags_e::T_LASTSTRING),
									 __t("%mouse_x%",			+tags_e::T_MOUSE_X),
									 __t("%mouse_y%",			+tags_e::T_MOUSE_Y),
									 __t("%sprite_speed_x%",	+tags_e::T_SPRITE_SPEEDX),
									 __t("%sprite_speed_y%",	+tags_e::T_SPRITE_SPEEDY),
									 __t("%sprite_name%",		+tags_e::T_SPRITE_NAME),
									 __t("%entity_name%",		+tags_e::T_SPRITE_ENTITY_NAME),
									 __t("%entity_health%",		+tags_e::T_SPRITE_ENTITY_HEALTH),
									 __t("%num_images%",		+tags_e::T_IMAGES_LOADED),
									 __t("%num_sprites%",		+tags_e::T_SPRITES_LOADED),
									 __t("%num_texts%",			+tags_e::T_TEXTS_LOADED),
									 __t("%num_tracks%",		+tags_e::T_TRACKS_LOADED),
									 __t("%num_entities%",		+tags_e::T_ENTITIES_LOADED),
									 __t("%garbage_total%",		+tags_e::T_GARBAGE_TOTAL),
									 __t("%garbage_images%",	+tags_e::T_IMAGES_GARBAGE),
									 __t("%garbage_sprites%",	+tags_e::T_SPRITES_GARBAGE),
									 __t("%garbage_texts%",		+tags_e::T_TEXTS_GARBAGE),
									 __t("%garbage_tracks%",	+tags_e::T_TRACKS_GARBAGE),
									 __t("%garbage_entities%",	+tags_e::T_ENTITIES_GARBAGE) };

			//const std::string tags[] = { "%pos_x%", "%pos_y%", "%screen_pos_x%", "%screen_pos_y%","%is_following%", "%color_r%", "%color_g%", "%color_b%", "%color_a%", "%mode%", "%time%", "%is_using_buf%", "%g_b_res_x%", "%g_b_res_y%", "%base_refresh_rate%", "%fps%", "%tps%", "%tps_col%", "%tps_funcs%", "%tps_second%", "%tps_posupd%", "%sprite_frame%", "%cam_x%", "%cam_y%", "%cam_zoom%", "%cam_zoom_x%", "%cam_zoom_y%", "%curr_string%", "%last_string%", "%mouse_x%", "%mouse_y%", "%sprite_speed_x%", "%sprite_speed_y%", "%sprite_name%", "%entity_name%", "%entity_health%", "%num_images%", "%num_sprites%", "%num_texts%", "%num_tracks%", "%num_entities%", "%garbage_total%", "%garbage_images%", "%garbage_sprites%", "%garbage_texts%", "%garbage_tracks%", "%garbage_entities%" };
		}

		void draw_simple_bar(const float, const ALLEGRO_COLOR = al_map_rgb(0, 0, 0), const ALLEGRO_COLOR = al_map_rgb(0, 200, 0));
		void draw_simple_txt(ALLEGRO_FONT*, const std::string, ALLEGRO_COLOR = al_map_rgb(255,255,255), const int = ALLEGRO_ALIGN_CENTER);

		class camera_preset {
		public:
			float p[+Assistance::io___float_camera::_MAX_CAM_OPT] = { 1.0,1.0,1.0,0.0,0.0,0.0 };
			std::vector<int> layers; // layers enabled

			void set(const Assistance::io___float_camera, const float); // set a value to a property
			float get(const Assistance::io___float_camera);             // get a value for a property
			void setLayer(const int, const bool);  // enables or disables a layer from drawing methods (sprites have to have layers)
		};

		// transformator has scale, pos and rotation transformations
		// cam should become a kind of transformator.


		class Camera {
			ALLEGRO_TRANSFORM g_t;

			static std::map<int, camera_preset> presets;
			static int lastapply;
		public:
			void reset();
			void set(const camera_preset&, const int = 0);
			void apply(const int);
			void apply();
			void applyNoSave(camera_preset);
			camera_preset& get(const int);
			camera_preset& get();
		};


		/*
		TODO:
		work on a way to transform the bitmaps like a camera transform with a camera transform combined? Maybe...?
		*/

		class Sprite {

			class __sprite_smart_data {
			public:
				double dval[+Assistance::io___double_sprite::size] = { 0.0 };
				bool bval[+Assistance::io___boolean_sprite::size] = { false };
				ALLEGRO_COLOR tint = al_map_rgb(255, 255, 255);

				__sprite_smart_data();
			};


			class __sprite_smart_images {
				double difftimeanim = 1.0 / 60;
				double lastcall = 0;
				size_t actual = 0;
				bool loopin = true;

				struct  __custom_data { // local yes
					ALLEGRO_BITMAP* bmp = nullptr;
					std::string idc;
				};

				std::vector<__custom_data*> copies;
			public:
				auto get();
				ALLEGRO_BITMAP* add(const std::string); // id
				void remove(const std::string);
				void setFPS(const double = 0); // 0 = no cycle, < 0 = static one (-1 == first aka 0), x > 0 = x changes per sec
				void loop(const bool = true);
				void reset(); // default settings
				void clear(); // copies.clear();
			};

			__sprite_smart_images bmps;
			__sprite_smart_data data;

			std::string sprite_id;
			int layer = 0;
		public:
			~Sprite();

			void apply(const Assistance::io___vecstring_sprite, const std::vector<std::string>, float* = nullptr);
			void apply(const Assistance::io___string_sprite, const std::string);
			void apply(const Assistance::io___double_sprite, const double);
			void apply(const Assistance::io___boolean_sprite, const bool);
			void apply(const Assistance::io___integer_sprite, const int);
			void apply(const Assistance::io___size_sprite, const size_t);
			void apply(const Assistance::io___color_sprite, const ALLEGRO_COLOR);

			bool get(const Assistance::io___string_sprite, std::string&);
			bool get(const Assistance::io___double_sprite, double&);
			bool get(const Assistance::io___boolean_sprite, bool&);
			bool get(const Assistance::io___integer_sprite, int&);
			bool get(const Assistance::io___size_sprite, size_t&);
			bool get(const Assistance::io___color_sprite, ALLEGRO_COLOR&);

			void draw(const int);
		};






		namespace Constants {
			const double text_default_sharpness_font = 2500.0;
			const double text_timeout_interpret = 0.1;
		}


		class Text {
			Sprite* follow = nullptr;
			double off_plr[2] = { 0.0,0.0 };
			double ofr_plr = 0.0;

			//Events::big_event bev;

			int layer = 0;

			double scale = 1.0;
			double final_scale = 1.0;
			double last_final_scale = 1.0;
			double rot = 0.0;

			double pos[2] = { 0,0 };
			int mode = ALLEGRO_ALIGN_CENTER;
			bool affected_by_camera = true;

			ALLEGRO_FONT* ffont = nullptr;
			//bool lastwasraw = false;

			ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);
			std::string orig_str;
			std::string string;
			bool str_upd = false;

			bool show = true;

			double lastinterpret = 0;
			double update_time = 1.0/24; // sec
			
			std::string id;//, path;// , _bpath;
			//static std::string gpath;
			//static bool is_gpath_raw;
			//bool using_gpath = true;

			void _draw(const double[2]);
			void _interpretTags(std::string&);

			void setFollow(const std::string);
			void setFollow(Sprite*);
			///const bool load(std::string = "font.ttf", const double = 1.0);
			///void reload();
			//void loadInternalBMP();
			///void _verify();
		public:
			Text();
			~Text();
			///void verify(const bool = false); // reload
			/*const bool loadFromDatabase(const std::string = Defaults::font_default_name);
			const bool loadFromURL(const std::string, const std::string); // url, name it as (THE FILE! NOT ID)*/

			void unload();

			void set(const Assistance::_text_opt_str, const std::string);
			void set(const Assistance::_text_opt_bool, const bool);
			void set(const Assistance::_text_opt_db, const double);
			void set(const Assistance::_text_opt_color, const ALLEGRO_COLOR);
			void set(const Assistance::_text_opt_int, const int);

			void get(const Assistance::_text_opt_str, std::string&);
			void get(const Assistance::_text_opt_bool, bool&);
			void get(const Assistance::_text_opt_db, double&);
			void get(const Assistance::_text_opt_color, ALLEGRO_COLOR&);
			void get(const Assistance::_text_opt_int, int&);

			const bool isEq(const Assistance::_text_opt_str, const std::string);
			const bool isEq(const Assistance::_text_opt_bool, const bool);
			const bool isEq(const Assistance::_text_opt_db, const double);
			const bool isEq(const Assistance::_text_opt_color, const ALLEGRO_COLOR);
			const bool isEq(const Assistance::_text_opt_int, const int);

			void draw(const int);
		};
	}
}