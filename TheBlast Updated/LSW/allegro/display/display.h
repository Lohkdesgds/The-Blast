#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Display {

			/*enum _display_targets {
				DISPLAY,
				BUFFER
			};*/
			enum _display_effects {
				NONE,
				BLURRY
			};
			
			struct lsw_mode {
				int x = 0;
				int y = 0;
				int hz = 0;
			};
			struct lsw_modes_all {
				std::vector<lsw_mode> modes;
				int using_rn = 0;
			};

			class _display_raw {
				ALLEGRO_DISPLAY* display = nullptr;
				//ALLEGRO_BITMAP* blur_b = nullptr;

				Image::image_low* img_low = nullptr; // target
				Sprite::sprite* spr_low = nullptr; // target

				Image::image_low* img_low2 = nullptr; // effect (rastro)
				Sprite::sprite* spr_low2 = nullptr; // effect (rastro)
				double lastrastro = 0;

				ALLEGRO_MONITOR_INFO moninf;
				lsw_modes_all d_mods;

				bool usebuf = false;

				double lastToggleFS = 0;
				bool isFullscreen = false;

				ALLEGRO_COLOR blur = al_map_rgba_f(Defaults::blur_buf_frame, Defaults::blur_buf_frame, Defaults::blur_buf_frame, Defaults::blur_buf_frame);

				const bool loadModes(const int);
				void _clearUp();
			public:
				~_display_raw();

				const bool launch(const int = -1, const int = -1, const int = Defaults::default_display_settings);
				void setTarget(/*const _display_targets*/);

				lsw_mode getMode(const int);

				void enableBlur();
				void disableBlur();
				void setBlur(const ALLEGRO_COLOR);

				void flip();
				void toggleFullscreen();
			};

			class display {
				_display_raw *disp_raw = nullptr;
				double time_frame = 0.0;
				float fps = 0.0;
				int cap_fps_to = -1;
				bool *keep = nullptr;
			public:
				void custom_launch(const int, const int, const int); // x, y, flags
				void launch();
				void close();

				void checkUpImages();

				void setCustomRaw(_display_raw*);

				const bool isOpen();
				
				const bool flip(const bool = true, const ALLEGRO_COLOR = al_map_rgb(0,0,0)); // lock? color bg

				void setKeep(bool*);

				float getFPS();
				void capFPS(const int = -1);

				void toggleFS();

				_display_raw& _get();
			};
		}
	}
}