#pragma once

#include "..\all\enable_all.h"


#define LIVRE       0
#define VISITADO   -2
#define SEM_SAIDA  -3
#define LOAD_MAX_TIME 0.05
#define RESOLVE_MAX_TIME 0.05

namespace LSW {
	namespace v2 {
		namespace Map {

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

				Safer::safe_string last_player_path;
				double last_player_size = Defaults::user_default_size;
				int last_player_layer = Defaults::user_default_layer;

				Safer::safe_string last_badboys_path;
				unsigned last_badboy_count = 4;
				int last_badboys_layer = Defaults::badboys_default_layer;

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

				void launch_player(const Safer::safe_string, const double = Defaults::user_default_size, const int = Defaults::user_default_layer); // path
				const bool launch_badboys(const Safer::safe_string, const unsigned, const int = Defaults::badboys_default_layer); // path, how many, layer

				void checkDraw();
				void checkPositionChange();

				const bool isCPUtasking();
				void setCPULock(const bool);

				void testCollisionPlayer();
				//void testCollisionOther(Sprite::sprite&);

				const bool hasReachedEnd();
				const bool isMapLoaded();

				const bool try_lock();
				void lock();
				void unlock();

				Sprite::sprite* _getPlayerSprite();
			};

			const Safer::safe_string randomName();
		}
	}
}