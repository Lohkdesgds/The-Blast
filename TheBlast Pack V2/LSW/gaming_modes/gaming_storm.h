#pragma once

#include "..\console\console.h"
#include "..\drawing\drawing.h"
#include "..\map_gen\map.h"

namespace LSW {
	namespace v4 {

		class GamingStorm {
			World* wd = nullptr;
			bool has_map_gen = false;
			bool has_sleeping_sprites = false;
			size_t level_count = 0;
			double level_fx_text = 0;
			int task_id = 0;

			Manager* consol = nullptr;
			Entity* this_is_the_player = nullptr;
			Text* level_text = nullptr;

			bool need_refresh = false;
			bool keep_game_going = false, is_paused = false;
			bool has_player_died = false;

			Sprites sprites;
			Textures textures;
			Texts texts;
			gfile logg;

			std::thread* thr_taskin = nullptr;

			void updateBlock(const POINT, Sprite*);
			void regenMap();
			void refreshMap();

			void threadTasking();
		public:
			GamingStorm(Manager*, Entity*, const int); // The main manager, the player, task id available for player timed task
			~GamingStorm();

			void startAutomatically(); // work by itself
			void pauseTask(const bool);
			void stopNow();

			bool hasEnded();
		};
	}

}