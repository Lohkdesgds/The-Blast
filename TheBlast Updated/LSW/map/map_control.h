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

			class map {
				int map_i[Defaults::map_size_default_x][Defaults::map_size_default_y];
				Sprite::sprite* map_p[Defaults::map_size_default_x][Defaults::map_size_default_y];
				double spawn[2] = { 0.0,0.0 };
				int seed = 0;
				int layerUsed = 0;

				const double x_off = (1.0*(1.0 / (Defaults::map_size_default_x*1.0)));
				const double y_off = (1.0*(1.0 / (Defaults::map_size_default_y*1.0)));

				Sprite::sprite* player = nullptr;

				void randomizer(const int);
				const bool workOnItAndAnswer(const int, const int);
				const int recommendation(int[Defaults::map_size_default_x][Defaults::map_size_default_y], const int, const int);

				const bool isthereanySEMSAIDA(int[Defaults::map_size_default_x][Defaults::map_size_default_y], const int, const int);
				void gotoVISITADO(int[Defaults::map_size_default_x][Defaults::map_size_default_y], int&, int&);
				const bool workto(int[Defaults::map_size_default_x][Defaults::map_size_default_y], int, int&, int&);

				void verifyCollision(Sprite::sprite&);
				const bool isBlockTransparent(const blockstats);
			public:
				map();
				~map();
				void setSeed(const int);
				void setLayer(const int);
				void generateMap();
				void killMap();
				void corruptWorldTick();
				void setPlayer(Sprite::sprite*);

				void testCollisionPlayer();
				void testCollisionOther(Sprite::sprite&);
			};
		}
	}
}