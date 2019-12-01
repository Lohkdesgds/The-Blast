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
#include <thread>
#include <functional>
#include <locale>

#include "..\organizer\organizer.h"

#define BLACK al_map_rgb(0,0,0)

/*
* What is console?
* This is the console, system, the one controlling everything.
* It will work everything together.
*/

namespace LSW {
	namespace v4 {

		namespace Constants {
			const auto lambda_bitmap_load = [](const char* p, ALLEGRO_BITMAP*& b) -> bool {
				return ((b = al_load_bitmap(p)));
			};

			const auto lambda_bitmap_unload = [](ALLEGRO_BITMAP*& b) -> void {
				if (al_is_system_installed() && b) { al_destroy_bitmap(b); }
			};

			const auto lambda_font_load = [](const char* p, ALLEGRO_FONT*& b) -> bool {
				return ((b = al_load_ttf_font(p, 20, 0)));
			};
			const auto lambda_font_unload = [](ALLEGRO_FONT*& b) -> void {
				if (al_is_system_installed() && b) { al_destroy_font(b); }
			};
		}

		namespace Assistance {

			struct __shared_routines {
				size_t threadcount = 0;
				std::mutex threadcountm;
				bool should_exit = true;
			};

			enum class __display_routines_timers { LOOPTRACK, CHECKKEEP, CHECKMEMORYBITMAP };
			enum class __collision_routines_timers { LOOPTRACK, CHECKKEEP, COLLISIONWORK };
			enum class __keyboardm_routines_timers { LOOPTRACK, CHECKKEEP, CHECKDISPLAYRESIZE };

		}

		typedef __template_multiple_timers<1, 2, 2>     __display_routines;
		typedef __template_multiple_timers<1, 5, 30>	__collision_routines;
		typedef __template_multiple_timers<1, 5, 5>		__keyboardmouse_routines;


		typedef __template_static_vector<ALLEGRO_BITMAP>  Textures;
		typedef __template_static_vector<Sprite>		  Sprites;
		typedef __template_static_vector<ALLEGRO_FONT>    Fonts;
		typedef __template_static_vector<ALLEGRO_SAMPLE>  Tracks;



		class Console {
			__raw_display* md = nullptr; // HANDLED INTERNALLY ON THREAD

			Textures gimg;
			Camera gcam;

			Assistance::__shared_routines thr_shared_arg; // shared

			/// DISPLAY AND DRAW
			std::thread* thr_md = nullptr; // unleash framerate
			//std::function <void(void*)> thr_md_func; // if user wants to run something before the actual init
			__display_routines* thr_md_arg; // already in here oops, HANDLED INTERNALLY ON THREAD

			/// COLLISION AND SPRITES
			std::thread* thr_cl = nullptr; // unleash collision work
			//std::function <void(void*)> thr_cl_func; // if user wants to run something before the actual init
			__collision_routines* thr_cl_arg; // already in here oops, HANDLED INTERNALLY ON THREAD
			
			/// KEYBOARD, MOUSE AND STUFF
			std::thread* thr_kb = nullptr; // unleash collision work
			//std::function <void(void*)> thr_kb_func; // if user wants to run something before the actual init
			__keyboardmouse_routines* thr_kb_arg; // already in here oops, HANDLED INTERNALLY ON THREAD

			ALLEGRO_EVENT_SOURCE evsrc;

			void __l_thr_md();
			void __l_thr_cl();
			void __l_thr_kb();
		public:
			Console();
			~Console();

			void launch();
			//void launch(const std::function <void(void*)>, const std::function <void(void*)>, const std::function <void(void*)>); // display, events, kb?
			void close();

			bool awakened();
			bool running();
		};

	}
}