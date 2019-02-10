#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Camera {

			enum _cam_v {
				OFFX,
				OFFY,
				ZOOM,
				ZOOMX,
				ZOOMY,
				ROTATION,
				MAX_OFFXY
			};
			/*enum _cam_p {
				//BUF_X,
				//BUF_Y,
				LAYERLOWER,
				LAYERHIGHER,
				MAX_BUF_XY
			};*/
			/*enum _cam_r {
				BASE_X,
				BASE_Y,
				MAX_BASE_XY
			};*/

			struct _transf_data {
				float scale_x = 1.0;
				float scale_y = 1.0;
				float scale_g = 1.0;
				float offset_x = 0.0;
				float offset_y = 0.0;
				float rotation = 0.0;
				std::vector<int> layers_enabled;
			};

			// transformator has scale, pos and rotation transformations
			// cam should become a kind of transformator.


			class _transformator {
				ALLEGRO_DISPLAY* d = nullptr;
				ALLEGRO_TRANSFORM g_t;

				std::map<int, _transf_data> presets;
			public:
				_transformator();
				void reset();
				void set(const _transf_data&, const int = 0);
				void apply(const int = 0);
				_transf_data& get(const int = 0);
				ALLEGRO_DISPLAY* getD();


			};



			struct _cam_g {
				int last_used = 0;
				_transformator* t = nullptr;
			};					

			class camera_g {
				static _cam_g cam;
			public:
				void setup(const bool = false);

				void apply();
				void apply(const int);

				const int getLastApplyID();

				const double get(const int, const _cam_v); // get config of n int, typ _cam_v
				void get(const int, Safer::safe_vector<int>&);
				std::vector<int>& getLayers(const int);
				
				void set(const int, const _cam_v, const double);
				void set(const int, const int, const bool); // set config n int if layer int is true/false

				void copy(const int);
				void copy(const int, const int);
				void copy(const int, const _transf_data&);

				void reset(const int);

				ALLEGRO_DISPLAY* getD();
			};
		}
	}
}
