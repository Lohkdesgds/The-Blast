#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Sprite {

			/*class clssss {
			public:
				clssss(double a, double b) { x = a; y = b; }
				double x, y;
			};
			
#define QPolygonF std::vector<clssss>*/

			enum _sprite_dvals{POSX,POSY,SCALEX,SCALEY,CENTERX,CENTERY,SCALEG,ROTATION,ANIMATIONTIME, // rotation = degrees, ANIMATIONTIME = animation time (fps)
			SPEEDX,SPEEDY,SPEEDROT,SMOOTHNESS_X,SMOOTHNESS_Y,/*SMOOTHNESS_ROT // TO BE IMPLEMENTED LATER ,*/ // smoothness = speed *= smoothness
			//ACCELERATION_BY_KEYING,ACCELERATION_SCALE,ACCELERATION_GRAVITY_SCALE,JUMP_SPEED_PROPORTION/*if AFFECTED_BY_GRAVITY && FOLLOWKEYBOARD*/,JUMP_TIME_DELAY,
			DVAL_MAX};
			enum _sprite_bvals{DRAW,COLLIDE,_IS_COLLIDING,AFFECTED_BY_COLLISION,/*DOES_COLLISION_ROTATE_OBJECT// TO BE IMPLEMENTED LATER,*/AFFECTED_BY_CAM,LOOPING_IMAGES, SHOWDOT,SHOWBOX,FOLLOWMOUSE,USE_TINTED_DRAWING,_SKIP_DEFAULT_COLLISION_METHOD,
			//FOLLOWKEYBOARD,//AFFECTED_BY_GRAVITY,
			BVAL_MAX};
			enum _sprite_ivals{LAYER};
			enum _sprite_zvals{SIZE,FRAME};
			enum _sprite_cvals{TINT};
			enum _sprite_vvals{ENTITY};
			enum _sprite_collision_way{NORTH,SOUTH,WEST,EAST};
			
			struct _sprite_data {
				double dval[DVAL_MAX];
				bool bval[BVAL_MAX];
				ALLEGRO_COLOR tint = al_map_rgb(255,255,255);
			};

			struct _sprite_area {
				double cx, cy, sx, sy; // , rot = 0.0;// TO BE IMPLEMENTED LATER
				//Safer::safe_string* debug_name = nullptr; // not needed anymore (testing failed ;-;)
			};
			
			struct _sprite_collision_info {
				double dx, dy;
				double rdx, rdy;
				bool colliding = false;
				_sprite_collision_way way;
				double ang = 0.0; // degree
			};

			class sprite {
				Safer::safer_vector<Image::image_low> bmps;
				Safer::safe_string id_g;
				_sprite_data data;

				bool got_to_end_one_time_at_least = false;
				size_t lastcall_opt = 0;
				double lastcall = 0;
				double lastresetcollisioncall = 0;
				bool hasChangedSomehow = false;
				//double lastverify = 0.0;
				int layer = 0; // 0 is first to be drawn. (background)
				int bx = 0, by = 0;
				int delayed_collision_times = 0;
				Log::gfile logg;

				std::mutex replacing;

				void *custom_data = nullptr;

				double last_jump = 0;
				unsigned jump_times = 0;

				const _sprite_collision_info _doesCollideWith(const _sprite_area, const _sprite_area);
				const bool __quick_is_it_collision_possible(const _sprite_area, const _sprite_area);
				//const _sprite_collision_info _doTheyIntersect(const _sprite_area, const _sprite_area); // TODO
			public:
				sprite();
				~sprite();

				void add(Safer::safe_string, const size_t = 1); // search in database
				void replaceAllWith(Safer::safe_string, const size_t = 1); // add and remove at the same time
				void remove(const Safer::safe_string); // ids from images

				void setID(const Safer::safe_string); // set sprite id
				void getID(Safer::safe_string&);
				const bool amI(const Safer::safe_string);

				void setScaled(const _sprite_dvals, const double, double); // last double: coef; 0.0 = totally itself, 1.0 = totally new number

				void set(const _sprite_dvals, const double);
				void set(const _sprite_bvals, const bool);
				void set(const _sprite_ivals, const int);
				void set(const _sprite_cvals, const ALLEGRO_COLOR);
				void set(const _sprite_vvals, void*);
				
				void get(const _sprite_dvals, double&);
				void get(const _sprite_bvals, bool&);
				void get(const _sprite_ivals, int&);
				void get(const _sprite_zvals, size_t&);
				void get(const _sprite_vvals, void*&);

				void add(const _sprite_dvals, const double);
				void multiply(const _sprite_dvals, const double);

				//void setTint(const ALLEGRO_COLOR); // has to enable flag USE_TINTED_DRAWING!

				void forceDraw();
				const bool drawIfLayer(const int&);

				const bool checkExistance();
				const size_t _lastFrameNumber();
				const bool gotToEnd();

				const bool hasChanged();

				void _forceHasChangedTo(const bool);
				_sprite_area _getArea();
				void _setAsTarg(const size_t = 0);
				void _resetCollision();
				void _verifyCollision(const double, const double);
				void _verifyCollision(Sprite::sprite&);
				void _verifyCollision(const _sprite_area&, bool* = nullptr);
				void _updateAcceleration(const double = 1.0); // hmm change later? double was meant to be timer, but it doesn't work like I thought
				void _echoPropertiesOnConsole(); // debugging!

				const bool _quickIsDelayed();
				const bool _quickIsCollisionPossible(Sprite::sprite&);
				const bool _quickLayerAmIOn(const int);

				//void runOn(void(*fu)(ALLEGRO_FONT*, ALLEGRO_COLOR, float, float, const char*), ALLEGRO_FONT*, ALLEGRO_COLOR, float, float, const char*, const size_t = 0);
			};


			size_t _find(const Safer::safe_string, Safer::safer_vector<sprite>&, bool&);
			void _draw();

			void reset_sprite_data(_sprite_data&);
			
			Safer::safe_pointer<sprite> getOrCreate(const Safer::safe_string, const bool = false); // create?
			void easyRemove(const Safer::safe_string);
		}
	}
}