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
		namespace Assistance {

			enum class ro__thread_display_routines_timers { LOOPTRACK, CHECKKEEP, CHECKMEMORYBITMAP, UPDATELOGONSCREEN };
			enum class ro__thread_collision_routines_timers { LOOPTRACK, CHECKKEEP, COLLISIONWORK };
			enum class ro__thread_keyboardm_routines_timers { LOOPTRACK, CHECKKEEP, UPDATEMOUSE };

		}

		typedef __template_multiple_timers<1, 2, 2, 5> __display_routines;
		typedef __template_multiple_timers<1, 5, 20>	__collision_routines;
		typedef __template_multiple_timers<1, 5, 60>	__keyboardmouse_routines;

		typedef __template_static_vector<ALLEGRO_BITMAP>  Textures;
		typedef __template_static_vector<ALLEGRO_FONT>    Fonts;
		typedef __template_static_vector<ALLEGRO_SAMPLE>  Samples;

		typedef __template_static_vector<Sprite>		  Sprites;
		typedef __template_static_vector<Text>			  Texts;
		typedef __template_static_vector<Track>			  Tracks;



		class Console {
			Display* md = nullptr; // HANDLED INTERNALLY ON THREAD

			Textures gimg;
			Camera gcam;

			struct __shared_routines {
				size_t threadcount = 0;
				std::mutex threadcountm;
				bool should_exit = true;
			} thr_shared_arg; // shared

			/// DISPLAY AND DRAW
			std::thread* thr_md = nullptr; // unleash framerate
			bool thr_md_upnrunnin = false;
			__display_routines* thr_md_arg; // already in here oops, HANDLED INTERNALLY ON THREAD

			/// COLLISION AND SPRITES
			std::thread* thr_cl = nullptr; // unleash collision work
			bool thr_cl_upnrunnin = false;
			__collision_routines* thr_cl_arg; // already in here oops, HANDLED INTERNALLY ON THREAD
			
			/// KEYBOARD, MOUSE AND STUFF
			std::thread* thr_kb = nullptr; // unleash collision work
			bool thr_kb_upnrunnin = false;
			__keyboardmouse_routines* thr_kb_arg; // already in here oops, HANDLED INTERNALLY ON THREAD

			ALLEGRO_EVENT_SOURCE evsrc;

			void __l_thr_md();
			void __l_thr_cl();
			void __l_thr_kb();
		public:
			Console();
			~Console();

			void start();
			//void launch(const std::function <void(void*)>, const std::function <void(void*)>, const std::function <void(void*)>); // display, events, kb?
			void stop();

			bool isOpen();
			bool isRunning();
		};

	}
}