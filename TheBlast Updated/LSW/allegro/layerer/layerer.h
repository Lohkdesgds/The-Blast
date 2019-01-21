#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Layer {

			class each_layer;

			enum mode{STANDARD,USEMAP};

			struct _all_layers {
				std::map<int, each_layer> l;
				int being_used = Defaults::default_layer_colliding_first;
			};

			/*struct each_layer {
				std::vector<_layer_down> layers_enabled; // layer 0, 1, and 3 must show up ...
			};*/

			struct _layer_down { // properties_of_layers
				//bool hasCollision = true;
				double smoothness_keep_sprite_movement = Defaults::smoothness_keep_sprite_movement;
				//double smoothness_keep_sprite_movement_rotation = Defaults::smoothness_keep_sprite_movement_rotational;
				double collision_acel_multiplier = Defaults::collision_acel_multiplier;
				double min_collision_acel = Defaults::min_collision_acel;
				double gravity = Defaults::gravity_default;
				std::map<int, bool> colliding; // layers id, colliding with what?
			};

			class layerer {
				static _all_layers layers;
			public:
				each_layer& getOne(const int);
				each_layer& getNow();
				void setUsingNow(const int);
				void clearNumber(const int);
			};

			class each_layer {
				std::map<int, _layer_down> lr;
				mode moe = STANDARD;
				void* ppp = nullptr;
			public:
				_layer_down& getR(const int);
				void unset(const int);
				void setMode(const mode);
				const mode getMode();

				void save_package(void*);
				void* get_package();

				std::map<int, _layer_down>& work();
			};

		}
	}
}