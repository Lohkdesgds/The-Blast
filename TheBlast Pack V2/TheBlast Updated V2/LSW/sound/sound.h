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

#include "..\custom_abort\abort.h"
#include "..\system\system.h"
#include "..\big_templates\big_templates.h"

namespace LSW {
	namespace v4 {

		/*
		
		                       sample instance 1
							 / sample instance 2
		voice <--  mixer <---         .
							 \        .
							   sample instance N
		
		
		*/

		enum class track_p { PLAYING };
		enum class track_f { VOLUME, GLOBALVOLUME, SPEED };
		enum class track_s { LOADID, ID };
		enum class track_i { PLAYMODE };
		enum class track_i_0 { ONCE = ALLEGRO_PLAYMODE_ONCE, LOOP = ALLEGRO_PLAYMODE_LOOP, BIDIR = ALLEGRO_PLAYMODE_BIDIR };

		struct _all_track_voiceNmixer {
			ALLEGRO_VOICE* voice = nullptr; // soundcard
			ALLEGRO_MIXER* mixer = nullptr; // mixer
		};

		class Track {
			static _all_track_voiceNmixer vnm;

			ALLEGRO_SAMPLE* mse = nullptr;   // THE FILE
			ALLEGRO_SAMPLE_INSTANCE* instance = nullptr;  // LOCAL PLAYBACK INSTANCE
			std::string id;
		public:
			Track();
			void unload();

			void set(const track_p, const bool);
			void set(const track_f, const float);
			void set(const track_i, const track_i_0);
			void set(const track_s, const std::string);

			void get(const track_p, bool&);
			void get(const track_f, float&);
			void get(const track_i, int&);
			void get(const track_s, std::string&);
		};


	}
}