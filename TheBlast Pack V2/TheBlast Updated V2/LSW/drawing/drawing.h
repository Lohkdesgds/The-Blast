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

			enum class in___string_sprite	{ ADD, REMOVE, SPRITE_ID };
			enum class in___double_sprite	{ POSX, POSY, SCALEX, SCALEY, CENTERX, CENTERY, SCALEG, ROTATION /*DEGREES*/, SPEEDX, SPEEDY, SPEEDROT, SMOOTHNESS_X, SMOOTHNESS_Y, size, /* since here they are not Sprite exactly stuff*/ ANIMATION_FPS };
			enum class in___boolean_sprite	{ DRAW, COLLIDE, IS_COLLIDING, AFFECTED_BY_COLLISION, AFFECTED_BY_CAM, SHOWDOT, SHOWBOX, FOLLOWMOUSE, FOLLOWKEYBOARD, USE_TINTED_DRAWING, size, /* since here they are not Sprite exactly stuff*/ LOOPFRAMES };
			enum class in___integer_sprite	{ LAYER, size };
			enum class in___size_sprite		{ SIZE, FRAME, size };
			enum class in___color_sprite	{ TINT, size };

			enum class _sprite_coll_way { NORTH, SOUTH, WEST, EAST };

			class __sprite_smart_data {
			public:
				double dval[+in___double_sprite::size] = { 0.0 };
				bool bval[+in___boolean_sprite::size] = { false };
				ALLEGRO_COLOR tint = al_map_rgb(255, 255, 255);

				__sprite_smart_data();
			};
			

			class __sprite_smart_images {
				double difftimeanim = 1.0/60;
				double lastcall = 0;
				size_t actual = 0;
				bool loopin = true;

				struct  __custom_data { // local yes
					ALLEGRO_BITMAP* bmp = nullptr;
					std::string idc;
				};

				std::vector<__custom_data> copies;
			public:
				auto get();
				void add(const std::string); // id
				void remove(const std::string);
				void setFPS(const double = 0); // 0 = no cycle, < 0 = static one (-1 == first aka 0), x > 0 = x changes per sec
				void loop(const bool = true);
				void reset(); // default settings
				void clear(); // copies.clear();
			};
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
			camera_preset& get(const int);
			camera_preset& get();
		};


		/*
		TODO:
		work on a way to transform the bitmaps like a camera transform with a camera transform combined? Maybe...?
		*/

		class Sprite {
			Assistance::__sprite_smart_images bmps;
			Assistance::__sprite_smart_data data;

			std::string sprite_id;
			int layer = 0;
		public:
			~Sprite();

			void apply(const Assistance::in___string_sprite, const std::string);
			void apply(const Assistance::in___double_sprite, const double);
			void apply(const Assistance::in___boolean_sprite, const bool);
			void apply(const Assistance::in___integer_sprite, const int);
			void apply(const Assistance::in___size_sprite, const size_t);
			void apply(const Assistance::in___color_sprite, const ALLEGRO_COLOR);

			void draw();
		};
	}
}