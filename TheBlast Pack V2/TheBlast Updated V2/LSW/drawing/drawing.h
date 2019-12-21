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
#include <algorithm>    // std::sort

#include <string>
#include <map>
#include <Windows.h>
#include <vector>
#include <memory>
#include <math.h>

#include "..\custom_abort\abort.h"
#include "..\big_templates\small_templates.h"
#include "..\big_templates\big_templates.h"
#include "..\logger\logger.h"
#include "..\system\system.h"
#include "..\sound\sound.h"

namespace LSW {
	namespace v4 {

		namespace Assistance {

			enum class io__camera_float			{ SCALE_X, SCALE_Y, SCALE_G, OFFSET_X, OFFSET_Y, ROTATION_RAD, SLIPPERINESS, size };
			
			enum class io__sprite_string_vector	{ ADDMULTIPLE, REMOVEMULTIPLE };
			enum class io__sprite_string		{ ADD, REMOVE, ID };
			enum class io__sprite_double		{ POSX, POSY, SCALEX, SCALEY, CENTERX, CENTERY, SCALEG, ROTATION /*DEGREES*/, SPEEDX, SPEEDY, SPEEDROT, SMOOTHNESS_X, SMOOTHNESS_Y, RO_LAST_MOUSE_COLLISION, RO_LAST_OTHERS_COLLISION, RO_MOUSE_DISTANCE_X, RO_MOUSE_DISTANCE_Y, RO_OTHERS_DISTANCE_X, RO_OTHERS_DISTANCE_Y, size, /* since here they are not Sprite exactly stuff*/ ANIMATION_FPS };
			enum class ro__sprite_target_double { TARG_POSX, TARG_POSY, TARG_ROTATION, INTERN_LASTDRAW, size};
			enum class io__sprite_boolean		{ DRAW, COLLIDE_OTHERS, COLLIDE_MOUSE, AFFECTED_BY_COLLISION, AFFECTED_BY_CAM, SHOWDOT, SHOWBOX, FOLLOWMOUSE, FOLLOWKEYBOARD, USE_TINTED_DRAWING, RO_IS_MOUSE_COLLIDING, RO_IS_OTHERS_COLLIDING, size, /* since here they are not Sprite exactly stuff*/ LOOPFRAMES, HAS_DONE_LOOPONCE };
			enum class io__sprite_integer		{ LAYER, size };
			enum class io__sprite_sizet			{ SIZE, FRAME, size };
			enum class io__sprite_color			{ TINT, size };

			enum class cl__sprite_direction		{ NORTH, SOUTH, WEST, EAST };

			enum class io__text_string  { STRING, PROCESSED_STRING, ID, FOLLOW_SPRITE, FONT, size };
			enum class io__text_boolean { SHOW, AFFECTED_BY_CAM, DEBUG_ITSELF, size };
			enum class io__text_double  { POSX, POSY, LAST_FOLLOW_POSX, LAST_FOLLOW_POSY, LAST_INTERPRET, SCALEG, ROTATION, UPDATETIME, size };
			enum class io__text_integer { MODE, LAYER, size };
			enum class io__text_color   { COLOR };

			enum class io__alignment_text { ALIGN_LEFT = ALLEGRO_ALIGN_LEFT, ALIGN_CENTER = ALLEGRO_ALIGN_CENTER, ALIGN_RIGHT = ALLEGRO_ALIGN_RIGHT };
			// 2

			class __slice {
				std::string n;
				int p;
			public:
				__slice(const std::string w, const int v) { n = w; p = v; }
				const std::string s() const { return n; }
				const int i() const { return p; }
			};

			enum class tags_e {T_POSX,T_POSY,T_SCREEN_POSX,T_SCREEN_POSY,T_ISFOLLOWING,T_COLOR_R,T_COLOR_G,T_COLOR_B,T_COLOR_A,T_MODE,T_TIME,T_ISUSINGBUF,T_GB_RESX,T_GB_RESY,T_REFRESHRATE,T_FPS,T_TPS,T_UPS,T_SPRITE_FRAME,T_CAM_X,T_CAM_Y,T_CAM_ZOOM,T_CAM_ZOOMG,T_CAM_ZOOMX,T_CAM_ZOOMY,
				T_CURRSTRING,T_LASTSTRING,T_MOUSE_X,T_MOUSE_Y,T_SPRITE_SPEEDX,T_SPRITE_SPEEDY,T_SPRITE_NAME,T_SPRITE_ENTITY_NAME,T_SPRITE_ENTITY_HEALTH,T_TEXTURES_LOADED,T_FONTS_LOADED,T_SAMPLES_LOADED,T_SPRITES_LOADED,T_TEXTS_LOADED,T_TRACKS_LOADED, size };

			const __slice tags[] = { __slice("%pos_x%", +tags_e::T_POSX),  __slice("%pos_y%", +tags_e::T_POSY), __slice("%screen_pos_x%", +tags_e::T_SCREEN_POSX), __slice("%screen_pos_y%", +tags_e::T_SCREEN_POSY), __slice("%is_following%", +tags_e::T_ISFOLLOWING),
__slice("%color_r%", +tags_e::T_COLOR_R), __slice("%color_g%", +tags_e::T_COLOR_G), __slice("%color_b%", +tags_e::T_COLOR_B), __slice("%color_a%", +tags_e::T_COLOR_A), __slice("%mode%", +tags_e::T_MODE), __slice("%time%", +tags_e::T_TIME), __slice("%is_using_buf%", +tags_e::T_ISUSINGBUF),
__slice("%curr_res_x%", +tags_e::T_GB_RESX), __slice("%curr_res_y%", +tags_e::T_GB_RESY), __slice("%curr_refresh_rate%", +tags_e::T_REFRESHRATE), __slice("%fps%", +tags_e::T_FPS), __slice("%tps%", +tags_e::T_TPS), __slice("%ups%", +tags_e::T_UPS),
__slice("%sprite_frame%", +tags_e::T_SPRITE_FRAME), __slice("%cam_x%", +tags_e::T_CAM_X), __slice("%cam_y%", +tags_e::T_CAM_Y), __slice("%cam_zoom_combined%", +tags_e::T_CAM_ZOOM), __slice("%cam_zoom_g%", +tags_e::T_CAM_ZOOMG),
__slice("%cam_zoom_x%", +tags_e::T_CAM_ZOOMX), __slice("%cam_zoom_y%", +tags_e::T_CAM_ZOOMY),__slice("%curr_string%", +tags_e::T_CURRSTRING), __slice("%last_string%", +tags_e::T_LASTSTRING), __slice("%mouse_x%", +tags_e::T_MOUSE_X), __slice("%mouse_y%", +tags_e::T_MOUSE_Y),
__slice("%sprite_speed_x%", +tags_e::T_SPRITE_SPEEDX),__slice("%sprite_speed_y%", +tags_e::T_SPRITE_SPEEDY), __slice("%sprite_name%", +tags_e::T_SPRITE_NAME), __slice("%entity_name%", +tags_e::T_SPRITE_ENTITY_NAME), __slice("%entity_health%", +tags_e::T_SPRITE_ENTITY_HEALTH),
__slice("%num_images%", +tags_e::T_TEXTURES_LOADED),__slice("%num_fonts%", +tags_e::T_FONTS_LOADED),__slice("%num_samples%", +tags_e::T_SAMPLES_LOADED), __slice("%num_sprites%", +tags_e::T_SPRITES_LOADED), __slice("%num_texts%", +tags_e::T_TEXTS_LOADED),
__slice("%num_tracks%", +tags_e::T_TRACKS_LOADED) };
			/*
			
		typedef __template_static_vector<ALLEGRO_BITMAP>  Textures;
		typedef __template_static_vector<ALLEGRO_FONT>    Fonts;
		typedef __template_static_vector<ALLEGRO_SAMPLE>  Samples;

		typedef __template_static_vector<Sprite>		  Sprites;
		typedef __template_static_vector<Text>			  Texts;
		typedef __template_static_vector<Track>			  Tracks;
			*/
		}

		void draw_simple_bar(const float, const ALLEGRO_COLOR = al_map_rgb(0, 0, 0), const float = 0.98, const float = 0.95); // w, h
		void draw_confuse_rectangle(const float, const float, const float, const float, const ALLEGRO_COLOR, const ALLEGRO_COLOR, const ALLEGRO_COLOR, const ALLEGRO_COLOR);
		void draw_simple_txt(ALLEGRO_FONT*, const std::string, ALLEGRO_COLOR = al_map_rgb(255,255,255), const int = ALLEGRO_ALIGN_CENTER, const float = 0.2);
		ALLEGRO_TRANSFORM easyTransform(ALLEGRO_DISPLAY* const, const float, const float, const float, const float, const float);
		void matrix_draw_help();

		class camera_preset {
			float p[+Assistance::io__camera_float::size] = { 1.0,1.0,1.0,0.0,0.0,0.0,1.0 };
			ALLEGRO_TRANSFORM latest = ALLEGRO_TRANSFORM();
			std::vector<int> layers; // layers enabled

		public:
			void refresh(); // guaranteed ALLEGRO_TRANSFORM ready to use (less thinking when already set up)
			void reset();
			void set(const Assistance::io__camera_float, const float);
			void merge(const Assistance::io__camera_float, const float);
			float get(const Assistance::io__camera_float) const;
			void setLayer(const int, const bool);
			
			// for(auto& i : this)
			std::vector<int>::iterator begin();
			std::vector<int>::iterator end();

			// quicker get
			ALLEGRO_TRANSFORM& quick();
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
			ALLEGRO_TRANSFORM applyNoSave(camera_preset);
			void applyNoSave(ALLEGRO_TRANSFORM*);
			camera_preset& get(const int);
			camera_preset& get();
		};
		

		class Sprite {
			
			class __sprite_smart_data {
			public:
				double dval[+Assistance::io__sprite_double::size] = { 0.0 };
				double dtarg[+Assistance::ro__sprite_target_double::size] = { 0.0 };
				bool bval[+Assistance::io__sprite_boolean::size] = { false };
				ALLEGRO_COLOR tint = al_map_rgb(255, 255, 255);

				__sprite_smart_data();
			};


			class __sprite_smart_images {
				double difftimeanim = 1.0 / 60;
				double lastcall = 0;
				size_t actual = 0;
				bool loopin = true;
				bool hasdoneloop = false;

				struct  __custom_data { // local yes
					ALLEGRO_BITMAP* bmp = nullptr;
					std::string idc;
				};

				std::vector<__custom_data*> copies;
			public:
				auto get();
				size_t lastFrame();
				ALLEGRO_BITMAP* load(const std::string); // id
				void remove(const std::string);
				void setFPS(const double = 0); // 0 = no cycle, < 0 = static one (-1 == first aka 0), x > 0 = x changes per sec
				void loop(const bool = true);
				bool hasloopended();
				void reset(); // default settings
				void clear(); // copies.clear();
			};

			__sprite_smart_images bmps;
			__sprite_smart_data data;

			std::string sprite_id;
			int layer = 0;

			bool fastIsColliding(camera_preset);
		public:
			~Sprite();

			void set(const Assistance::io__sprite_string_vector, const std::vector<std::string>, float* = nullptr);
			void set(const Assistance::io__sprite_string, const std::string);
			void set(const Assistance::io__sprite_double, const double);
			void set(const Assistance::io__sprite_boolean, const bool);
			void set(const Assistance::io__sprite_integer, const int);
			void set(const Assistance::io__sprite_sizet, const size_t);
			void set(const Assistance::io__sprite_color, const ALLEGRO_COLOR);

			bool get(const Assistance::io__sprite_string, std::string&);
			bool get(const Assistance::io__sprite_double, double&);
			bool get(const Assistance::ro__sprite_target_double, double&);
			bool get(const Assistance::io__sprite_boolean, bool&);
			bool get(const Assistance::io__sprite_integer, int&);
			bool get(const Assistance::io__sprite_sizet, size_t&);
			bool get(const Assistance::io__sprite_color, ALLEGRO_COLOR&);

			bool isEq(const Assistance::io__sprite_string, const std::string);
			bool isEq(const Assistance::io__sprite_double, const double);
			bool isEq(const Assistance::io__sprite_boolean, const bool);
			bool isEq(const Assistance::io__sprite_integer, const int);
			bool isEq(const Assistance::io__sprite_sizet, const size_t);
			bool isEq(const Assistance::io__sprite_color, const ALLEGRO_COLOR);

			void draw(const int);

			void clearUp();
			void process(const int, camera_preset); // movement
			void collideWith(const int, Sprite* const);
		};



		class Text {
			Sprite* follow = nullptr;

			struct __custom_data {
				std::string str[+Assistance::io__text_string::size];
				bool b[+Assistance::io__text_boolean::size] = { false };
				double d[+Assistance::io__text_double::size] = { 0 };
				int i[+Assistance::io__text_integer::size] = { 0 };
				ALLEGRO_COLOR c;
				ALLEGRO_FONT* font = nullptr;
				std::string __internal_debug_flag;
			};

			__custom_data data;		
			
			void _draw(const double[2]);
			void _interpretTags(std::string&);

			void setFollow(const std::string);
			void setFollow(Sprite*);
		public:
			Text();
			//~Text();
			
			void set(const Assistance::io__text_string, const std::string);
			void set(const Assistance::io__text_boolean, const bool);
			void set(const Assistance::io__text_double, const double);
			void set(const Assistance::io__text_color, const ALLEGRO_COLOR);
			void set(const Assistance::io__text_integer, const int);

			void get(const Assistance::io__text_string, std::string&);
			void get(const Assistance::io__text_boolean, bool&);
			void get(const Assistance::io__text_double, double&);
			void get(const Assistance::io__text_color, ALLEGRO_COLOR&);
			void get(const Assistance::io__text_integer, int&);

			const bool isEq(const Assistance::io__text_string, const std::string);
			const bool isEq(const Assistance::io__text_boolean, const bool);
			const bool isEq(const Assistance::io__text_double, const double);
			const bool isEq(const Assistance::io__text_color, const ALLEGRO_COLOR);
			const bool isEq(const Assistance::io__text_integer, const int);

			void draw(const int);
		};
	}
}