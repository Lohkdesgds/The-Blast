#pragma once

#include "..\all\enable_all.h"



namespace LSW {
	namespace v2 {
		namespace Map {

			enum keys_map_gen {LIVRE=0,VISITADO=-2,SEM_SAIDA=-3};

			enum keysgamehu { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
			enum blockstats { /*GAMEPLAY_CORRUPTION = -4, INTRO_CORRUPTION = -3, INTRO_ANIMATED = -2, MENU_ANIMATED = -1,*/
				SPACEID, BLOCKID, STARTID, THENDID, INVISID, NOEXTID, BLOCKID_C00, BLOCKID_C01, BLOCKID_C02, BLOCKID_C03 };

			const int blocks_air[] = {SPACEID, STARTID, THENDID, INVISID, NOEXTID};

			const Safer::safe_string interpretIntToBlockName(const blockstats);

			struct point {
				int x, y;
			};

			class map {
				int map_i[Defaults::map_size_default_x][Defaults::map_size_default_y];
				
				Sprite::sprite* map_p[Defaults::map_size_default_x][Defaults::map_size_default_y];

				Sprite::sprite* big_map = nullptr;
				Image::image_low* big_map_il = nullptr;

				int pausepos[2] {0,0};
				double spawn[2] = { 0.0,0.0 };
				int seed = 0;
				int layerUsed = Defaults::map_default_layer;

				bool hasToUpdate = false;
				bool hasToUpdate_a_block = false;
				bool has_reachedEnd = false;
				bool has_loadedMap = false;
				//bool is_new_Map = false;

				const double x_off = (1.0*(1.0 / (Defaults::map_size_default_x*1.0)));
				const double y_off = (1.0*(1.0 / (Defaults::map_size_default_y*1.0)));

				bool is_player_enabled = false;
				Entities::player plr;
				Safer::safe_vector<Entities::badboy*> badboys;

				bool cpu_thr_ready, gpu_thr_ready;

				std::mutex muu;
				bool set_cpu_lock = false;
				bool cpu_tasking = false;

				bool was_player_rgb_instead;
				ALLEGRO_COLOR last_player_color;
				Safer::safe_string last_player_path, last_player_name;
				double last_player_size = Defaults::Entity::size;
				int last_player_layer = Defaults::Entity::common_layer;

				Safer::safe_string last_badboys_path;
				unsigned last_badboy_count = 4;
				int last_badboys_layer = Defaults::badboys_default_layer;

				bool paused = false;
				bool game_ended_dead = false;

				void randomizer(const int);
				const bool workOnItAndAnswer(const int, const int);
				const int recommendation(int[Defaults::map_size_default_x][Defaults::map_size_default_y], const int, const int);

				const bool isthereanySEMSAIDA(int[Defaults::map_size_default_x][Defaults::map_size_default_y], const int, const int);
				void gotoVISITADO(int[Defaults::map_size_default_x][Defaults::map_size_default_y], int&, int&);
				const bool workto(int[Defaults::map_size_default_x][Defaults::map_size_default_y], int, int&, int&);

				void verifyCollision(Sprite::sprite&);
				const bool isBlockTransparent(const blockstats);
				void generateMap();

				void _checkBitmapsOnMapP();
				void _checkBitmapsBigMap();
				void _redraw();

				void _getActualPos(int&, int&, const double, const double);
				const bool _getPlayerPos(int&, int&);

				void _savePlayerLastPos();
				void _setPlayerAtLastPos();
			public:
				map();
				~map();

				void reset_draw_thr();
				void reset_cpu_thr();

				const bool start_draw_thr();
				const bool start_cpu_thr();

				void setSeed(const int);
				void setLayer(const int);
				//void generateBitmaps();
				//void killMap();
				//void killEntities();
				//void setPlayer(Sprite::sprite*);
				void corruptWorldTick();

				void setPlayerName(const Safer::safe_string);
				void launch_player(const ALLEGRO_COLOR, const double = Defaults::Entity::size, const int = Defaults::Entity::common_layer); // path
				void launch_player(const Safer::safe_string, const double = Defaults::Entity::size, const int = Defaults::Entity::common_layer); // path
				const bool launch_badboys(const Safer::safe_string, const unsigned, const int = Defaults::badboys_default_layer); // path, how many, layer

				Entities::player& getPlayer();
				Entities::badboy& getBB(const size_t);

				void checkDraw();
				void checkPositionChange();

				const bool isCPUtasking();
				void setCPULock(const bool);

				void cpuTask();
				void testCollisionPlayer();
				//void testCollisionOther(Sprite::sprite&);

				const bool hasReachedEnd();
				const bool isDead();
				const bool isMapLoaded();

				void Pause(const bool);
				const bool isPaused();

				const bool try_lock();
				void lock();
				void unlock();

				Sprite::sprite* _getPlayerSprite();
			};

			const Safer::safe_string randomName();
		}
	}
}