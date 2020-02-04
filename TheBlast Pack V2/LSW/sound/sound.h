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
#include <mutex>

#include "..\custom_abort\abort.h"
#include "..\big_templates\small_templates.h"
#include "..\big_templates\big_templates.h"
#include "..\logger\logger.h"
#include "..\system\system.h"

namespace LSW {
	namespace v4 {

		/*
		
		                       sample instance 1
							 / sample instance 2
		voice <--  mixer <---         .
							 \        .
							   sample instance N
		
		
		*/

		namespace Constants {

			enum class io__track_boolean { PLAYING };
			enum class io__track_double { VOLUME, SPEED };
			enum class io__track_string { LOADID, ID };
			enum class io__track_integer { PLAYMODE };
			enum class io__track_integer_modes { ONCE = ALLEGRO_PLAYMODE_ONCE, LOOP = ALLEGRO_PLAYMODE_LOOP, BIDIR = ALLEGRO_PLAYMODE_BIDIR };

		}

		class Mixer {
			struct controller {
				ALLEGRO_VOICE* device = nullptr;
				ALLEGRO_MIXER* mixing = nullptr;
				std::mutex nd;
			};

			static controller control;
		public:
			Mixer();

			void volume(const double);
			double getVolume();
			void attachInstance(ALLEGRO_SAMPLE_INSTANCE*);
		};
				

		class Track {
			ALLEGRO_SAMPLE* mse = nullptr;   // THE FILE
			ALLEGRO_SAMPLE_INSTANCE* instance = nullptr;  // LOCAL PLAYBACK INSTANCE
			std::string id;
		public:
			Track();
			void unload();

			void set(const Constants::io__track_boolean, const bool);
			void set(const Constants::io__track_double, const double);
			void set(const Constants::io__track_integer, const Constants::io__track_integer_modes);
			void set(const Constants::io__track_string, const std::string);

			void get(const Constants::io__track_boolean, bool&);
			void get(const Constants::io__track_double, double&);
			void get(const Constants::io__track_integer, int&);
			void get(const Constants::io__track_string, std::string&);
		};


	}
}