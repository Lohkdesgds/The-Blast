#pragma once

#include "..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Entities {

			enum type {PLAYER,BADBOY};

			enum _eplayer_dvals { ACCELERATION_BY_KEYING, ACCELERATION_SCALE, ACCELERATION_GRAVITY_SCALE, JUMP_SPEED_PROPORTION/*, JUMP_TIME_DELAY,_LAST_JUMP_TIME*/,
				D_EPLAYERS_MAX };

			enum _eplayer_bvals { JUMP, MOVE_LEFT, MOVE_RIGHT, MOVE_MAX };

			class entity {
			protected:
				static size_t countt;

				Sprite::sprite* spr = nullptr;
				Image::image_low* rgb = nullptr;
				Text::text* txt = nullptr;

				Safer::safe_string name;
				double health = 1.0;
				
				type typ = BADBOY;
				// chapeu
			public:
				~entity();
				virtual void load(const ALLEGRO_COLOR, const int = Defaults::user_default_layer, const double = Defaults::user_default_size) = 0; // color, layer, size
				virtual void load(const Safer::safe_string, const int = Defaults::user_default_layer, const double = Defaults::user_default_size) = 0; // path, layer, size
				void reset();

				//void setType(const type);

				void setMyName(const Safer::safe_string);
				void setMyHealth(const double);

				const Safer::safe_string getMyName();
				const double getMyHealth();

				const type getType();
				const bool amI(const Safer::safe_string);

				Sprite::sprite* getS();

				virtual void tick() = 0;
			};

			struct _player_data {
				double dval[D_EPLAYERS_MAX];
				bool bval[MOVE_MAX];
				bool sleep = false;
			};

			class player : public entity
			{
				std::mutex m;
				_player_data data;
			public:
				player();
				void tick();

				void set(const _eplayer_dvals, const double);
				void set(const _eplayer_bvals, const bool);

				void get(const _eplayer_dvals, double&);

				void sleep(const bool);

				void load(const ALLEGRO_COLOR, const int = Defaults::user_default_layer, const double = Defaults::user_default_size);
				void load(const Safer::safe_string, const int = Defaults::user_default_layer, const double = Defaults::user_default_size);
			};

			class badboy : public entity
			{
				Sprite::sprite* player_to_follow = nullptr;
			public:
				badboy();
				void setFollowing(Sprite::sprite*);
				void tick();

				void load(const ALLEGRO_COLOR, const int = Defaults::user_default_layer, const double = Defaults::user_default_size);
				void load(const Safer::safe_string, const int = Defaults::user_default_layer, const double = Defaults::user_default_size);
			};


			size_t _find(const Safer::safe_string, Safer::safe_vector<entity*>&, bool&);
			void _draw();

			void reset_player_data(_player_data&);

		}
	}
}