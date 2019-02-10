#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Display {

			/*enum _display_targets {
				DISPLAY,
				BUFFER
			};*/
			/*enum _display_effects {
				NONE,
				BLURRY
			};*/

			enum vals_bp{RUNNING_P};
			enum vals_b{COLOR_SHIFTED};
			enum vals_dp{DISPLAY};
			enum vals_d{FPS,LOCK_FPS_TO};
			enum vals_i{SCREEN_X,SCREEN_Y,REFRESHRATE};
			
			struct lsw_mode {
				int x = 0;
				int y = 0;
				int hz = 0;
			};
			struct lsw_modes_all {
				std::vector<lsw_mode> modes;
				int using_rn = 0;
			};

			struct _big_display_data {
				ALLEGRO_DISPLAY* md = nullptr;
				lsw_modes_all d_mods;

				double last_toggleFS = 0;
				bool last_wasFullscreen = false;

				// information
				double last_fps_set = 0;
				double last_fps = 0.0;
				unsigned frames_count = 0;
				double lock_fps_to = 0;

				// fx
				bool color_shift = true; // test
				Sprite::sprite* spr[3] = { nullptr };
				Image::image_low* img_lw[3] = { nullptr };
				double last_color_shift_FX = 0;

				// external / aux
				bool* ext_isRunning = nullptr;

				// flush thing
				double last_flush = 0;
			};



			class big_display {
				Log::gfile logg;

				static _big_display_data data;
				static long long instance_count; // 0? kills display

				void _realDraw(const ALLEGRO_COLOR);
				void __flushCache(); // has to be in draw thread. Maybe later I'll put it somewhere else.
			public:
				big_display();
				~big_display();

				void _loadModes(const int);

				void set(const vals_bp, bool*);
				void set(const vals_b, const bool);
				void set(const vals_dp, ALLEGRO_DISPLAY*);
				void set(const vals_d, const double);
				void set(const vals_i, const int);

				void get(const vals_bp, bool*&);
				void get(const vals_b, bool&);
				void get(const vals_dp, ALLEGRO_DISPLAY*&);
				void get(const vals_d, double&);
				void get(const vals_i, int&);

				const bool isEq(const vals_bp, const bool); // faster way
				const bool isEq(const vals_bp, bool*);
				const bool isEq(const vals_b, const bool);
				const bool isEq(const vals_dp, ALLEGRO_DISPLAY*);
				const bool isEq(const vals_d, const double);
				const bool isEq(const vals_i, const int);

				void launch(const int, const int, int);

				const bool work(const bool = true, const ALLEGRO_COLOR = al_map_rgb(0,0,0)); // wait for draw moment (true), background color
			};

			void __ext_loadModes(big_display*, int);
		}
	}
}