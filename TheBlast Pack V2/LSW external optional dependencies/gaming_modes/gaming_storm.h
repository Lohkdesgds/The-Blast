#pragma once

#include "..\..\LSW\console\console.h"
#include "..\..\LSW\drawing\drawing.h"
#include "..\..\LSW\map_gen\map.h"

#include <chrono>

#define MILI_NOW std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
#define MILI_ZERO std::chrono::milliseconds::zero()

namespace LSW {
	namespace v4 {

		class GamingStorm {
			World* wd = nullptr;
			bool has_map_gen = false;
			bool has_sleeping_sprites = false;
			size_t level_count = 0;
			int task_id = 0;
			double current_delta = 1.0;
			double dynamic_speed_difft_m = 1.0;

			std::chrono::milliseconds game_started = MILI_ZERO,
									  level_started = MILI_ZERO,
									  paused_when = MILI_ZERO;

			std::function<void(const std::chrono::milliseconds)> function_tied_gaming,
															     function_tied_level;
			std::function<void(const std::string)>				 function_tied_level_name;
			

			Manager* consol = nullptr;
			Entity* this_is_the_player = nullptr;
			Track* playtrack = nullptr;

			bool need_refresh = false;
			std::vector<Sprite*> refreshing;
			std::mutex refresing_m;

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
			GamingStorm(Manager*, Entity*, Track*,  const int); // The main manager, the player, task id available for player timed task
			~GamingStorm();

			void startAutomatically(); // work by itself
			void pauseTask(const bool);
			void stopNow();

			void tieToGamingTime(const std::function<void(const std::chrono::milliseconds)> = std::function<void(const std::chrono::milliseconds)>());
			void tieToLevelTime(const std::function<void(const std::chrono::milliseconds)> = std::function<void(const std::chrono::milliseconds)>());
			void tieToLevelName(const std::function<void(const std::string)> = std::function<void(const std::string)>());

			bool hasEnded();
		};
	}

}