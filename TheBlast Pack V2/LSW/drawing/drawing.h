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
#include "..\tools\tools.h"
#include "..\big_templates\small_templates.h"
#include "..\big_templates\big_templates.h"
#include "..\logger\logger.h"
#include "..\system\system.h"
#include "..\sound\sound.h"

namespace LSW {
	namespace v4 {

		namespace Constants {

			enum class io__camera_double		 { SCALE_X, SCALE_Y, SCALE_G, OFFSET_X, OFFSET_Y, ROTATION_RAD, SLIPPERINESS, LIMIT_MIN_X, LIMIT_MIN_Y, LIMIT_MAX_X, LIMIT_MAX_Y, size };
			enum class io__camera_boolean	 	 { RESPECT_LIMITS, READONLY_NOW, size };
			
			enum class io__sprite_string_vector	 { ADDMULTIPLE, REMOVEMULTIPLE };
			enum class io__sprite_string		 { ADD, REMOVE, ID };
			enum class io__sprite_double		 { POSX, POSY, SCALEX, SCALEY, CENTERX, CENTERY, SCALEG, ROTATION /*DEGREES*/, ACCELERATION_X, ACCELERATION_Y, SPEEDROT, SPEEDXY_LIMIT, SMOOTHNESS_X, SMOOTHNESS_Y, RO_SPEEDX, RO_SPEEDY, RO_LAST_MOUSE_COLLISION, RO_LAST_OTHERS_COLLISION, RO_MOUSE_DISTANCE_X, RO_MOUSE_DISTANCE_Y, RO_OTHERS_DISTANCE_X, RO_OTHERS_DISTANCE_Y, RO_HAPPENED_RESIZE_DISPLAY, size, /* since here they are not Sprite exactly stuff*/ ANIMATION_FPS };
			enum class ro__sprite_target_double  { TARG_POSX, TARG_POSY, TARG_ROTATION, INTERN_LASTDRAW, size};
			/* FOLLOWMOUSE disable RESPECT_CAMERA_LIMITS automatically */
			// removed from io__sprite_boolean: COLLIDE_OTHERS, COLLIDE_BE_IN_ALL_LAYERS, AFFECTED_BY_COLLISION_ELASTIC, AFFECTED_BY_COLLISION_ROUGH, COLLIDE_MOUSE
			enum class io__sprite_collision_mode {
				COLLISION_TRANSPARENT, COLLISION_MOUSEONLY, 
				COLLISION_SAMELAYER_HOLD_STATIC, COLLISION_ANY_HOLD_STATIC,
				COLLISION_SAMELAYER_ROUGH, COLLISION_SAMELAYER_ELASTIC, COLLISION_ANY_ELASTIC, COLLISION_ANY_ROUGH,

				__COLLISION_TRANSPARENT_MAXVAL = COLLISION_MOUSEONLY,
				__COLLISION_HOLD_OR_TRANSPARENT_MAXVAL = COLLISION_ANY_HOLD_STATIC,
				__COLLISION_ANYLAYER_MINVAL = COLLISION_SAMELAYER_ROUGH };

			enum class io__sprite_boolean		 { DRAW, TIE_SIZE_TO_DISPLAY, USE_STATE_AS_BITMAP, AFFECTED_BY_CAM, SHOWDOT, SHOWBOX, RESPECT_CAMERA_LIMITS, FOLLOWMOUSE, FOLLOWKEYBOARD, USE_TINTED_DRAWING, RO_IS_MOUSE_COLLIDING, RO_IS_OTHERS_COLLIDING_FROM_X, RO_IS_OTHERS_COLLIDING_FROM_Y, RO_IS_OTHERS_ABOUT_TO_COLLIDE, size, /* since here they are not Sprite exactly stuff*/ LOOPFRAMES, HAS_DONE_LOOP_ONCE, IS_IT_ON_LAST_FRAME, ZERO_RESETS_POSITION_INSTEAD_OF_FREEZING };
			enum class io__sprite_integer		 { LAYER, size, ADD_ALT_LAYER, REMOVE_ALT_LAYER };
			enum class io__sprite_sizet			 { SIZE, FRAME, size };
			enum class ro__sprite_state			 { STATE }; // no size
			enum class io__sprite_color			 { TINT, size };
			enum class io__sprite_tie_func_to_state{ COLLISION_MOUSE_ON,COLLISION_MOUSE_CLICK,COLLISION_MOUSE_UNCLICK,COLLISION_COLLIDED_OTHER,COLLISION_NONE,ON_DRAW,WHEN_BITMAPS_RESIZED_AUTO,size };

			enum class cl__sprite_direction_mult {
				GO_NORTH, GO_SOUTH, GO_EAST, GO_WEST, STOP_NORTH, STOP_SOUTH, STOP_EAST, STOP_WEST
			};

			enum class io__entity_string { NICKNAME, TEXTURE, size };
			enum class io__entity_color  { COLOR, size };
			enum class io__entity_double { HEALTH, size };

			enum class io__text_tie_func_to_state { SPRITE_COLLISION_MOUSE_ON, SPRITE_COLLISION_MOUSE_CLICK, SPRITE_COLLISION_MOUSE_UNCLICK, SPRITE_COLLISION_COLLIDED_OTHER, SPRITE_COLLISION_NONE, 
				ON_UPDATE, ON_DRAW, size,
				
				__SPRITE_TIED_MAX = SPRITE_COLLISION_NONE
			};

			enum class io__text_string  { STRING, PROCESSED_STRING, ID, FOLLOW_SPRITE, FONT, size };
			enum class io__text_boolean { SHOW, AFFECTED_BY_CAM, USE_SPRITE_TINT_INSTEAD, DEBUG_ITSELF, size };
			enum class io__text_double  { POSX, POSY, LAST_FOLLOW_POSX, LAST_FOLLOW_POSY, LAST_INTERPRET, SCALEX, SCALEY, SCALEG, ROTATION, UPDATETIME, size };
			enum class io__text_integer { MODE, LAYER, size, ADD_ALT_LAYER, REMOVE_ALT_LAYER };
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
			enum class tags_e {T_POSX,T_POSY,T_SCREEN_POSX,T_SCREEN_POSY,T_ISFOLLOWING,T_COLOR_R,T_COLOR_G,T_COLOR_B,T_COLOR_A,T_MODE,T_TIME,T_ISUSINGBUF,T_GB_RESX,T_GB_RESY,T_REFRESHRATE,T_FPS,T_TPS,T_UPS,T_APS,T_I_FPS,T_I_TPS,T_I_UPS,T_I_APS,T_MS_FPS,T_MS_TPS,T_MS_UPS,T_MS_APS,
				T_SPRITE_FRAME,T_CAM_X,T_CAM_Y,T_CAM_ZOOM, T_CAM_ZOOMG,T_CAM_ZOOMX,T_CAM_ZOOMY, T_CURRSTRING,T_LASTSTRING,T_MOUSE_X,T_MOUSE_Y,T_SPRITE_SPEEDX,T_SPRITE_SPEEDY,T_SPRITE_NAME,T_SPRITE_ENTITY_NAME,T_SPRITE_ENTITY_HEALTH,_T_SPRITE_DEBUG,T_TEXTURES_LOADED,T_FONTS_LOADED,
				T_SAMPLES_LOADED,T_SPRITES_LOADED,T_TEXTS_LOADED, T_TRACKS_LOADED,T_SPRITE_STATE,T_VOLUME,T_VERSION, size };
			const __slice tags[] = { __slice("%pos_x%", +tags_e::T_POSX),  __slice("%pos_y%", +tags_e::T_POSY), __slice("%screen_pos_x%", +tags_e::T_SCREEN_POSX), __slice("%screen_pos_y%", +tags_e::T_SCREEN_POSY), __slice("%is_following%", +tags_e::T_ISFOLLOWING),
__slice("%color_r%", +tags_e::T_COLOR_R), __slice("%color_g%", +tags_e::T_COLOR_G), __slice("%color_b%", +tags_e::T_COLOR_B), __slice("%color_a%", +tags_e::T_COLOR_A), __slice("%mode%", +tags_e::T_MODE), __slice("%time%", +tags_e::T_TIME), __slice("%is_using_buf%", +tags_e::T_ISUSINGBUF),
__slice("%curr_res_x%", +tags_e::T_GB_RESX), __slice("%curr_res_y%", +tags_e::T_GB_RESY), __slice("%curr_refresh_rate%", +tags_e::T_REFRESHRATE), __slice("%int_fps%", +tags_e::T_FPS), __slice("%int_tps%", +tags_e::T_TPS), __slice("%int_ups%", +tags_e::T_UPS),
__slice("%int_aps%", +tags_e::T_APS), __slice("%instant_fps%", +tags_e::T_I_FPS), __slice("%instant_tps%", +tags_e::T_I_TPS), __slice("%instant_ups%", +tags_e::T_I_UPS), __slice("%instant_aps%", +tags_e::T_I_APS), __slice("%ms_fps%", +tags_e::T_MS_FPS),
__slice("%ms_tps%", +tags_e::T_MS_TPS), __slice("%ms_ups%", +tags_e::T_MS_UPS), __slice("%ms_aps%", +tags_e::T_MS_APS), __slice("%sprite_frame%", +tags_e::T_SPRITE_FRAME), __slice("%cam_x%", +tags_e::T_CAM_X), __slice("%cam_y%", +tags_e::T_CAM_Y),
__slice("%cam_zoom_combined%", +tags_e::T_CAM_ZOOM), __slice("%cam_zoom_g%", +tags_e::T_CAM_ZOOMG), __slice("%cam_zoom_x%", +tags_e::T_CAM_ZOOMX), __slice("%cam_zoom_y%", +tags_e::T_CAM_ZOOMY),__slice("%curr_string%", +tags_e::T_CURRSTRING),
__slice("%last_string%", +tags_e::T_LASTSTRING), __slice("%mouse_x%", +tags_e::T_MOUSE_X), __slice("%mouse_y%", +tags_e::T_MOUSE_Y), __slice("%sprite_speed_x%", +tags_e::T_SPRITE_SPEEDX),__slice("%sprite_speed_y%", +tags_e::T_SPRITE_SPEEDY),
__slice("%sprite_name%", +tags_e::T_SPRITE_NAME), __slice("%entity_name%", +tags_e::T_SPRITE_ENTITY_NAME), __slice("%entity_health%", +tags_e::T_SPRITE_ENTITY_HEALTH), __slice("%sprite_debug%", +tags_e::_T_SPRITE_DEBUG), __slice("%num_images%", +tags_e::T_TEXTURES_LOADED),
__slice("%num_fonts%", +tags_e::T_FONTS_LOADED),__slice("%num_samples%", +tags_e::T_SAMPLES_LOADED), __slice("%num_sprites%", +tags_e::T_SPRITES_LOADED), __slice("%num_texts%", +tags_e::T_TEXTS_LOADED), __slice("%num_tracks%", +tags_e::T_TRACKS_LOADED),
__slice("%sprite_state%", +tags_e::T_SPRITE_STATE),__slice("%volume_perc%", +tags_e::T_VOLUME),__slice("%version%", +tags_e::T_VERSION) };
		}


		void draw_simple_bar(const double, const ALLEGRO_COLOR = al_map_rgb(0, 0, 0), const double = 0.98, const double = 0.95); // w, h
		void draw_confuse_rectangle(const float, const float, const float, const float, const ALLEGRO_COLOR, const ALLEGRO_COLOR, const ALLEGRO_COLOR, const ALLEGRO_COLOR);
		void draw_simple_txt(ALLEGRO_FONT*, const std::string, ALLEGRO_COLOR = al_map_rgb(255,255,255), const int = ALLEGRO_ALIGN_CENTER, const double = 0.2);
		ALLEGRO_TRANSFORM easyTransform(ALLEGRO_DISPLAY* const, const float, const float, const float, const float, const float);
		ALLEGRO_TRANSFORM easyTransform(ALLEGRO_BITMAP* const, const float, const float, const float, const float, const float);
		void matrix_draw_help();

		class camera_preset {
			double p[+Constants::io__camera_double::size] = { 1.0,1.0,1.0,0.0,0.0,0.0,Constants::camera_default_slipperiness, -1.0e308, -1.0e308, 1.0e308, 1.0e308 };
			bool b[+Constants::io__camera_boolean::size] = { false };
			ALLEGRO_TRANSFORM latest = ALLEGRO_TRANSFORM();
			std::vector<int> layers; // layers enabled
			std::string internal_id = "unknown";

			bool canWrite();	
		public:
			void refresh(); // guaranteed ALLEGRO_TRANSFORM ready to use (less thinking when already set up)
			void reset();

			void set(const Constants::io__camera_double, const double);
			void set(const Constants::io__camera_boolean, const bool);

			void merge(const Constants::io__camera_double, const double);

			double get(const Constants::io__camera_double) const;
			bool get(const Constants::io__camera_boolean) const;

			void setInternalID(const std::string);
			std::string getInternalID(); // mostly debugging (to be implemented)

			void setLayer(const int, const bool);
			
			// for(auto& i : this)
			std::vector<int>::iterator begin();
			std::vector<int>::iterator end();

			// quicker get
			ALLEGRO_TRANSFORM& quick();
		};
		

		class Camera {
			ALLEGRO_TRANSFORM g_t;

			struct paired {
				camera_preset preset;
				std::function<void(camera_preset&)> func;
			};

			static std::map<int, paired> presets;
			static int lastapply;
		public:
			void reset();
			void set(const camera_preset&, const int = 0, const std::function<void(camera_preset&)> = std::function<void(camera_preset&)>());
			void apply(const int);
			void apply();
			ALLEGRO_TRANSFORM applyNoSave(ALLEGRO_BITMAP* const, camera_preset = camera_preset());
			ALLEGRO_TRANSFORM applyNoSave(camera_preset = camera_preset());
			void applyNoSave(ALLEGRO_TRANSFORM*);
			camera_preset& get(const int);
			camera_preset& get();
		};
		
			   
		class Sprite {

			struct ___collision_data {
				int diretion_by_centers_interpreted = 0; // Constants::directions
				double expected_walk_distances[2] = { 0.0,0.0 };
			};
			
			struct __sprite_smart_data {

				double dval[+Constants::io__sprite_double::size] = { 0.0 };
				double dtarg[+Constants::ro__sprite_target_double::size] = { 0.0 };
				bool bval[+Constants::io__sprite_boolean::size] = { false };
				ALLEGRO_COLOR tint = al_map_rgb(255, 255, 255);
				std::vector<int> layers_colliding;
				std::mutex layers_colliding_m;
				Constants::io__sprite_tie_func_to_state last_state = Constants::io__sprite_tie_func_to_state::size;
				Constants::io__sprite_tie_func_to_state new_state = Constants::io__sprite_tie_func_to_state::size;

				std::function<void(void)> function_pair[+Constants::io__sprite_tie_func_to_state::size];
				bool function_pair_looping[+Constants::io__sprite_tie_func_to_state::size] = { false };

				Constants::io__sprite_collision_mode collision_mode = Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_HOLD_STATIC;
				/*int direction_col = 0; // where is the block I stepped on?
				int direction_predict = 0;
				size_t countdown_col = 0;*/

				//Constants::cl__sprite_direction_mult where_i_wanna_go = Constants::cl__sprite_direction_mult::STAY;
				int combined_direction_i_wanna_go = 0;

				std::vector<___collision_data> col_data;

				//bool tempblock_dirs[+Constants::cl__sprite_direction_mult::size] = { false };

				__sprite_smart_data();
			};


			class __sprite_smart_images {
				double difftimeanim = 1.0 / 60;
				double lastcall = 0;
				size_t actual = 0;
				bool loopin = true;
				bool has_done_loop_once = false;

				struct  __custom_data { // local yes
					ALLEGRO_BITMAP* bmp = nullptr;
					std::string idc;
				};

				std::vector<__custom_data*> copies;
				bool reset_instead_of_pause = false;

				size_t pair[+Constants::io__sprite_tie_func_to_state::size] = { std::string::npos };
			public:
				ALLEGRO_BITMAP* get(const Constants::io__sprite_tie_func_to_state = Constants::io__sprite_tie_func_to_state::size);
				size_t lastFrame();
				ALLEGRO_BITMAP* load(const std::string); // id
				ALLEGRO_BITMAP* load(ALLEGRO_BITMAP*, const std::string);
				void remove(const std::string);
				void setFPS(const double = 0); // 0 = no cycle, < 0 = static one (-1 == first aka 0), x > 0 = x changes per sec
				void setState(const Constants::io__sprite_tie_func_to_state, const size_t = std::string::npos);
				size_t getState(const Constants::io__sprite_tie_func_to_state);
				void loop(const bool = true);
				bool hasLoopEndedOnce();
				bool isOnLastFrame();
				void reset(); // default settings
				void clear(); // copies.clear();
				void resetInsteadOfPause(const bool);
				void checkAllReferences();
				bool resizeAllTo(const int, const int);
			};

			__sprite_smart_images bmps;
			__sprite_smart_data data;

			std::string sprite_id;
			int layer = 0;
			std::string __debug_s;

			bool fastIsColliding(camera_preset);
			void checkTied();

			bool shouldThisCalcCollisionIn(const int); // related to: isThisCollidableIn

		protected:

			bool is_this_sprite = true;

		public:
			~Sprite();

			void hook(const Constants::io__sprite_tie_func_to_state, std::function<void(void)>, const bool = false);
			void unhook(const Constants::io__sprite_tie_func_to_state);

			void set(const Constants::io__sprite_string_vector, const std::vector<std::string>, float* = nullptr);
			void set(const Constants::io__sprite_string_vector, const std::function<bool(const std::string)>);
			void set(const Constants::io__sprite_string, const std::string);
			void set(const Constants::io__sprite_double, const double);
			void set(const Constants::io__sprite_boolean, const bool);
			void set(const Constants::io__sprite_integer, const int);
			void set(const Constants::io__sprite_sizet, const size_t);
			void set(const Constants::io__sprite_color, const ALLEGRO_COLOR);
			void set(const Constants::io__sprite_tie_func_to_state, const size_t = std::string::npos);
			void set(const Constants::io__sprite_collision_mode);
			void set(const Constants::cl__sprite_direction_mult); // set dir to go (GO_NORTH...)

			bool get(const Constants::io__sprite_string, std::string&);
			bool get(const Constants::io__sprite_double, double&);
			bool get(const Constants::ro__sprite_target_double, double&);
			bool get(const Constants::io__sprite_boolean, bool&);
			bool get(const Constants::io__sprite_integer, int&);
			bool get(const Constants::io__sprite_sizet, size_t&);
			bool get(const Constants::ro__sprite_state, Constants::io__sprite_tie_func_to_state&);
			bool get(const Constants::io__sprite_color, ALLEGRO_COLOR&);
			void get(const Constants::io__sprite_tie_func_to_state, size_t&);
			void get(ALLEGRO_BITMAP*&);
			void get(Constants::io__sprite_collision_mode&);

			bool isEq(const Constants::io__sprite_string, const std::string);
			bool isEq(const Constants::io__sprite_double, const double);
			bool isEq(const Constants::io__sprite_boolean, const bool = true);
			bool isEq(const Constants::io__sprite_integer, const int);
			bool isEq(const Constants::io__sprite_sizet, const size_t);
			bool isEq(const Constants::ro__sprite_state, const Constants::io__sprite_tie_func_to_state);
			bool isEq(const Constants::io__sprite_color, const ALLEGRO_COLOR);
			bool isEq(const Constants::io__sprite_tie_func_to_state, const size_t);
			bool isEq(const Constants::io__sprite_collision_mode);

			void draw(const int);
			void rebindReferences();

			void clearUp();
			void process(const int, camera_preset); // movement and task()

			void collideWith(const int, Sprite* const); // test collision
			void applyCollideData(camera_preset); // apply collision, check limits and run possible functions hooked to states

			bool isThisCollidableIn(const int); // related to: shouldThisCalcCollisionIn
			bool doesThisReflect(); // calls shouldThisCalcCollisionIn
			
			std::string __debug_str();

			bool isThisEntity();
		};

		class Entity : public Sprite {
			struct __e_custom_data {
				std::string s[+Constants::io__entity_string::size];
				ALLEGRO_COLOR c[+Constants::io__entity_color::size];
				double d[+Constants::io__entity_color::size];
			};
			
			__e_custom_data edata;
		public:
			Entity();

			bool isThisEntity();

			using Sprite::set;
			using Sprite::get;
			using Sprite::isEq;

			void set(Constants::io__entity_string, const std::string);
			void set(Constants::io__entity_color, const ALLEGRO_COLOR);
			void set(Constants::io__entity_double, const double);

			void get(Constants::io__entity_string, std::string&);
			void get(Constants::io__entity_color, ALLEGRO_COLOR&);
			void get(Constants::io__entity_double, double&);

			bool isEq(Constants::io__entity_string, const std::string);
			bool isEq(Constants::io__entity_color, const ALLEGRO_COLOR);
			bool isEq(Constants::io__entity_double, const double);
		};



		class Text {
			Sprite* follow = nullptr;

			struct __custom_data {
				std::string str[+Constants::io__text_string::size];
				bool b[+Constants::io__text_boolean::size] = { false };
				double d[+Constants::io__text_double::size] = { 0 };
				int i[+Constants::io__text_integer::size] = { 0 };
				ALLEGRO_COLOR c;
				ALLEGRO_FONT* font = nullptr;
				std::string __internal_debug_flag;

				std::vector<int> layers_colliding;
				std::mutex layers_colliding_m;

				std::function<void(std::string&)> pair_tied[+Constants::io__text_tie_func_to_state::size]; // if SPRITE has this flag, then TEXT has it too
			};

			__custom_data data;		
			
			void _draw(const double[2]);
			void _interpretTags(std::string&);

			void setFollow(const std::string);
			void setFollow(Sprite*);
		public:
			Text();
			//~Text();
			
			void set(const Constants::io__text_string, const std::string);
			void set(const Constants::io__text_boolean, const bool);
			void set(const Constants::io__text_double, const double);
			void set(const Constants::io__text_color, const ALLEGRO_COLOR);
			void set(const Constants::io__text_integer, const int);

			void hook(const Constants::io__text_tie_func_to_state, const std::function<void(std::string&)>); // set and unset if no arg
			void unhook(const Constants::io__text_tie_func_to_state);

			void get(const Constants::io__text_string, std::string&);
			void get(const Constants::io__text_boolean, bool&);
			void get(const Constants::io__text_double, double&);
			void get(const Constants::io__text_color, ALLEGRO_COLOR&);
			void get(const Constants::io__text_integer, int&);

			bool isEq(const Constants::io__text_string, const std::string);
			bool isEq(const Constants::io__text_boolean, const bool);
			bool isEq(const Constants::io__text_double, const double);
			bool isEq(const Constants::io__text_color, const ALLEGRO_COLOR);
			bool isEq(const Constants::io__text_integer, const int);

			void draw(const int);
		};



		class BubblesFX {
			struct particle {
				double posx = 0.0, posy = 0.0;
				double lastsize = 1.0;
				double lastpositionscalculated[2] = {0.0, 0.0};
			};

			std::vector<particle> positions;
			unsigned amountofentities = 2;
			double fps = 30;
			double lastdraw = 0;
			bool alreadyreset = false;

			bool firstcall = true;


			ALLEGRO_BITMAP* imglw = nullptr;
			Sprite* disguy = nullptr;

			int siz[2] = { 0, 0 };

			const double maxone(double, const double = 1.0);
		public:
			// amount of entities running forever :3, fps and layer

			void init(const unsigned, const double, const int = 0);
			bool draw(const int);
			bool think(const int);
		};

		class LiningFX {
			ALLEGRO_BITMAP* imglw = nullptr;
			Sprite* disguy = nullptr;

			int siz[2] = { 0, 0 };

			const double maxone(double, const double = 1.0);
		public:
			void init(const int = 0);

			bool draw(const int);
		};
	}
}