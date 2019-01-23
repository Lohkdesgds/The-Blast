#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Sound {

			enum track_p{ PLAYING };
			enum track_f{VOLUME, GLOBALVOLUME,SPEED};
			enum track_s{ID};
			enum track_i{PLAYMODE};

			enum track_i_0{ONCE = ALLEGRO_PLAYMODE_ONCE,LOOP = ALLEGRO_PLAYMODE_LOOP,BIDIR = ALLEGRO_PLAYMODE_BIDIR};

			/*
			class voice {
				ALLEGRO_VOICE* voice = nullptr;
				ALLEGRO_MIXER* mixer = nullptr;
			public:
				~voice();
				void start();
				void attach(ALLEGRO_SAMPLE*);
			};
			*/

			struct _all_track_voiceNmixer {
				ALLEGRO_VOICE* voice = nullptr;
				ALLEGRO_MIXER* mixer = nullptr;
			};

			class track {
				static _all_track_voiceNmixer vnm;

				ALLEGRO_SAMPLE				*mse = nullptr;
				ALLEGRO_SAMPLE_INSTANCE		*instance = nullptr;
				ALLEGRO_SAMPLE_ID			id;
				Safer::safe_string			my_nick;
			public:
				const bool load(const Safer::safe_string);
				void unload();

				void set(const track_p, const bool);
				const bool set(const track_f, const float);
				void set(const track_i, const track_i_0);
				void set(const track_s, const Safer::safe_string);

				void get(const track_p, bool&);
				void get(const track_f, float&);
				void get(const track_i, int&);
				void get(const track_s, Safer::safe_string&);
			};

			size_t _find(const Safer::safe_string, Safer::safe_vector<track*>&, bool&);


			track* getOrCreate(const Safer::safe_string, const bool = false); // create?
			void easyRemove(const Safer::safe_string);
		}
	}
}