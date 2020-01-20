#pragma once

#include "..\shared_constants\constants.h"
#include "..\custom_abort\abort.h"
#include "..\big_templates\small_templates.h"

#include <vector>
#include <Windows.h>

namespace LSW {
	namespace v4 {

		enum class blocks { BLOCK, EMPTY, TRAVELLED };
		enum class directions {
			NORTH = 1 << 0,
			SOUTH = 1 << 1,
			EAST = 1 << 2,
			WEST = 1 << 3
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
				bool operator==(const pt p) {
					return p.x == x && p.y == y;
				}
			};

			int waysToGo(const pt);

			bool isThis(const pt, const int);

			bool isValid(const pt);
			int& get(const pt);


		public:
			World(const size_t = Constants::map_size_default_x, const size_t = Constants::map_size_default_y);
			~World();

			void generate(const int);

			size_t getLen(const bool); // false = X, true = Y
			int readPos(const int, const int);

			void __print();
		};


	}
}