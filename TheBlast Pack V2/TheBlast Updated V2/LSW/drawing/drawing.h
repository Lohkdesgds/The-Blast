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
			enum class _cam_v {
				OFFX,
				OFFY,
				ZOOM,
				ZOOMX,
				ZOOMY,
				ROTATION
			};

			enum class _cam_opt { SCALE_X, SCALE_Y, SCALE_G, OFFSET_X, OFFSET_Y, ROTATION, _MAX_CAM_OPT };
			
			enum class in___vecstring_sprite{ ADDMULTIPLE, REMOVEMULTIPLE };
			enum class in___string_sprite	{ ADD, REMOVE, SPRITE_ID };
			enum class in___double_sprite	{ POSX, POSY, SCALEX, SCALEY, CENTERX, CENTERY, SCALEG, ROTATION /*DEGREES*/, SPEEDX, SPEEDY, SPEEDROT, SMOOTHNESS_X, SMOOTHNESS_Y, size, /* since here they are not Sprite exactly stuff*/ ANIMATION_FPS };
			enum class in___boolean_sprite	{ DRAW, COLLIDE, IS_COLLIDING, AFFECTED_BY_COLLISION, AFFECTED_BY_CAM, SHOWDOT, SHOWBOX, FOLLOWMOUSE, FOLLOWKEYBOARD, USE_TINTED_DRAWING, size, /* since here they are not Sprite exactly stuff*/ LOOPFRAMES };
			enum class in___integer_sprite	{ LAYER, size };
			enum class in___size_sprite		{ SIZE, FRAME, size };
			enum class in___color_sprite	{ TINT, size };

			enum class _sprite_coll_way { NORTH, SOUTH, WEST, EAST };
		}

		class camera_preset {
		public:
			float p[+Assistance::_cam_opt::_MAX_CAM_OPT] = { 1.0,1.0,1.0,0.0,0.0,0.0 };
			std::vector<int> layers; // layers enabled

			void set(const Assistance::_cam_opt, const float); // set a value to a property
			float get(const Assistance::_cam_opt);             // get a value for a property
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
				double dval[+Assistance::in___double_sprite::size] = { 0.0 };
				bool bval[+Assistance::in___boolean_sprite::size] = { false };
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

			void apply(const Assistance::in___vecstring_sprite, const std::vector<std::string>);
			void apply(const Assistance::in___string_sprite, const std::string);
			void apply(const Assistance::in___double_sprite, const double);
			void apply(const Assistance::in___boolean_sprite, const bool);
			void apply(const Assistance::in___integer_sprite, const int);
			void apply(const Assistance::in___size_sprite, const size_t);
			void apply(const Assistance::in___color_sprite, const ALLEGRO_COLOR);

			bool get(const Assistance::in___string_sprite, std::string&);
			bool get(const Assistance::in___double_sprite, double&);
			bool get(const Assistance::in___boolean_sprite, bool&);
			bool get(const Assistance::in___integer_sprite, int&);
			bool get(const Assistance::in___size_sprite, size_t&);
			bool get(const Assistance::in___color_sprite, ALLEGRO_COLOR&);

			void draw(const int);
		};





		namespace Assistance {

			enum class _text_opt_str { SETSTRING, SETID, SETFOLLOW, SETFONT };
			enum class _text_opt_bool { SHOW, AFFECTED_BY_CAM };
			enum class _text_opt_db { POSX, POSY, SCALEG, ROTATION, UPDATETIME };
			enum class _text_opt_int { MODE, LAYER };
			enum class _text_opt_color { COLOR };

			enum class mode_int { ALIGN_LEFT = ALLEGRO_ALIGN_LEFT, ALIGN_CENTER = ALLEGRO_ALIGN_CENTER, ALIGN_RIGHT = ALLEGRO_ALIGN_RIGHT };
			// 2
			const std::string tags[] = { "%pos_x%", "%pos_y%", "%screen_pos_x%", "%screen_pos_y%","%is_following%", "%color_r%", "%color_g%", "%color_b%", "%color_a%", "%mode%", "%time%", "%is_using_buf%", "%g_b_res_x%", "%g_b_res_y%", "%base_refresh_rate%", "%fps%", "%tps%", "%tps_col%", "%tps_funcs%", "%tps_second%", "%tps_posupd%", "%sprite_frame%", "%cam_x%", "%cam_y%", "%cam_zoom%", "%cam_zoom_x%", "%cam_zoom_y%", "%curr_string%", "%last_string%", "%mouse_x%", "%mouse_y%", "%sprite_speed_x%", "%sprite_speed_y%", "%sprite_name%", "%entity_name%", "%entity_health%", "%num_images%", "%num_sprites%", "%num_texts%", "%num_tracks%", "%num_entities%", "%garbage_total%", "%garbage_images%", "%garbage_sprites%", "%garbage_texts%", "%garbage_tracks%", "%garbage_entities%" };
			enum class tags_e { T_POSX, T_POSY, T_SCREEN_POSX, T_SCREEN_POSY, T_ISFOLLOWING, T_COLOR_R, T_COLOR_G, T_COLOR_B, T_COLOR_A, T_MODE, T_TIME, T_ISUSINGBUF, T_GB_RESX, T_GB_RESY, T_REFRESHRATE, T_FPS, T_TPS_COUNT, T_TPS_COLLISION, T_TPS_FUNCTIONS, T_TPS_SECOND_TAKEN, T_TPS_POSUPDATE, T_SPRITE_FRAME, T_CAM_X, T_CAM_Y, T_CAM_ZOOM, T_CAM_ZOOMX, T_CAM_ZOOMY, T_CURRSTRING, T_LASTSTRING, T_MOUSE_X, T_MOUSE_Y, T_SPRITE_SPEEDX, T_SPRITE_SPEEDY, T_SPRITE_NAME, T_SPRITE_ENTITY_NAME, T_SPRITE_ENTITY_HEALTH, T_IMAGES_LOADED, T_SPRITES_LOADED, T_TEXTS_LOADED, T_TRACKS_LOADED, T_ENTITIES_LOADED, T_GARBAGE_TOTAL, T_IMAGES_GARBAGE, T_SPRITES_GARBAGE, T_TEXTS_GARBAGE, T_TRACKS_GARBAGE, T_ENTITIES_GARBAGE };
		}

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