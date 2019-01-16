#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Layer {

			class each_layer;

			struct _all_layers {
				std::map<int, each_layer> l;
				int being_used = 0;
			};

			/*struct each_layer {
				std::vector<_layer_down> layers_enabled; // layer 0, 1, and 3 must show up ...
			};*/

			struct _layer_down { // properties_of_layers
				//bool hasCollision = true;
				double smoothness_keep_sprite_movement = Defaults::smoothness_keep_sprite_movement;
				double collision_acel_multiplier = Defaults::collision_acel_multiplier;
				double min_collision_acel = Defaults::min_collision_acel;
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
			public:
				_layer_down& set(const int);
				void unset(const int);

				std::map<int, _layer_down>& work();
			};

		}
	}
}