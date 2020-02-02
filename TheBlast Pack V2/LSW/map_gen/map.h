#pragma once

#include "..\shared_constants\constants.h"
#include "..\custom_abort\abort.h"
#include "..\big_templates\small_templates.h"

#include <vector>
#include <Windows.h>

namespace LSW {
	namespace v4 {

		enum class blocks {
			BLOCK, EMPTY, LIFE, START_POINT, EXIT, FAKE_EXIT, FAILEDEXIT, BRICKING_0, BRICKING_1, BRICKING_2, BRICKING_3, DEBUG,
			__CLEAR_FROM_WHAT_VAL = START_POINT,
			__SECURE_CLEAR_FROM_WHERE = FAKE_EXIT,
			__CORROSION_UNTIL = LIFE
		};

		class World {
			int* wrld = nullptr;
			size_t wrld_s[2] = { 0 };

			struct pt {
				size_t x;
				size_t y;

				pt() {
					x = y = 0;
				}
				pt(const size_t a, const size_t b) {
					x = a;
					y = b;
				}
				pt(const int a, const int b) {
					x = a;
					y = b;
				}
				bool operator==(const pt p) {
					return p.x == x && p.y == y;
				}
				bool operator!=(const pt p) {
					return p.x != x || p.y != y;
				}
			};

			double delta_t_corrupt = 2.0;
			double now_t = 0;
			double save_now_dif = 0;

			int waysToGo(const pt);

			int waysToSolve(const pt, blocks = blocks::EMPTY); // Constants::directions == free

			bool isThis(const pt, const int);

			bool isValid(const pt);
			int& get(const pt);

			bool isSolvable(pt, const pt);

			bool randomCorrosion(pt*, const int = +blocks::BRICKING_0);
		public:
			World(const size_t = Constants::map_size_default_x, const size_t = Constants::map_size_default_y);
			~World();

			void setTime(const double = 0.0); // al_get_time + arg;
			void setDelta(const double = 1.5); // delta_t

			POINT generate(const int);

			size_t getLen(const bool); // false = X, true = Y
			int readPos(const int, const int);
			void setPos(const int, const int, const int);

			bool corruptWorld(std::vector<POINT>&);
			void pauseCorrupt();

			void __print();
		};


	}
}