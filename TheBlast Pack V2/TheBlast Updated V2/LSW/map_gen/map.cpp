#include "map.h"

namespace LSW {
	namespace v4 {
		int World::waysToGo(const pt p)
		{
			int res = 0;

			// lefty
			{
				pt pts[4] = { p, p, p, p };
				size_t h = 0;

				pts[h++].x -= 2; // has to be block

				pts[h++].x--; // has to be block

				pts[h].y++;
				pts[h++].x--; // should not be open

				pts[h].y--;
				pts[h++].x--; // should not be open

				bool ok = true;
				for (auto& i : pts) {
					if (!isThis(i, +blocks::BLOCK)) ok = false;
				}
				if (ok) res |= +Assistance::directions::WEST;
			}
			// righty
			{
				pt pts[4] = { p, p, p, p };
				size_t h = 0;

				pts[h++].x += 2; // has to be block

				pts[h++].x++; // has to be block

				pts[h].y++;
				pts[h++].x++; // should not be open

				pts[h].y--;
				pts[h++].x++; // should not be open

				bool ok = true;
				for (auto& i : pts) {
					if (!isThis(i, +blocks::BLOCK)) ok = false;
				}
				if (ok) res |= +Assistance::directions::EAST;
			}
			// northy
			{
				pt pts[4] = { p, p, p, p };
				size_t h = 0;

				pts[h++].y -= 2; // has to be block

				pts[h++].y--; // has to be block

				pts[h].x++;
				pts[h++].y--; // should not be open

				pts[h].x--;
				pts[h++].y--; // should not be open

				bool ok = true;
				for (auto& i : pts) {
					if (!isThis(i, +blocks::BLOCK)) ok = false;
				}
				if (ok) res |= +Assistance::directions::NORTH;
			}
			// southy
			{
				pt pts[4] = { p, p, p, p };
				size_t h = 0;

				pts[h++].y += 2; // has to be block

				pts[h++].y++; // has to be block

				pts[h].x++;
				pts[h++].y++; // should not be open

				pts[h].x--;
				pts[h++].y++; // should not be open

				bool ok = true;
				for (auto& i : pts) {
					if (!isThis(i, +blocks::BLOCK)) ok = false;
				}
				if (ok) res |= +Assistance::directions::SOUTH;
			}

			return res;
		}

		bool World::isThis(const pt p, const int v)
		{
			if (isValid(p)) {
				return (v == get(p));
			}
			return false;
		}

		bool World::isValid(const pt p)
		{
			return (p.x >= 0 && p.y >= 0 && p.x < wrld_s[0] && p.y < wrld_s[1]);
		}

		int& World::get(const pt p)
		{
			return wrld[p.x + p.y * wrld_s[0]];
		}

		World::World(const size_t x, const size_t y)
		{
			wrld = new int[x * y];
			for (size_t p = 0; p < x * y; p++) wrld[p] = +blocks::BLOCK;
			wrld_s[0] = x;
			wrld_s[1] = y;
		}
		World::~World()
		{
			if (wrld) {
				delete[] wrld;
				wrld_s[0] = wrld_s[1] = 0;
				wrld = nullptr;
			}
		}
		void World::generate(const int seed)
		{
			for (size_t p = 0; p < wrld_s[0] * wrld_s[1]; p++) wrld[p] = +blocks::BLOCK;

			srand((unsigned int)GetTickCount64());

			pt first_point(rand() % (wrld_s[0] - 2) + 1, rand() % (wrld_s[1] - 2) + 1);

			std::vector<pt> road;

			road.push_back(first_point);
			get(first_point) = +blocks::EMPTY;


			while (road.size() > 0)
			{
				//gotoxy(0, 0);
				//__print();

				auto p = road.back();
				int ws = waysToGo(p);

				if (!ws) {
					//printf_s("Didn't find any route");
					get(p) = +blocks::TRAVELLED;
					road.pop_back();
				}
				else {
					//printf_s("                      ");
					int which = 0;

					int options[4] = { 0 };
					for (int k = 0; k < 4; k++) options[k] = (ws & 1 << k);

					for (bool hasdone = false; !hasdone && ws > 0;) {

						while (!which) {
							which = options[rand() % 4];
						}

						switch (which) {
						case +Assistance::directions::NORTH:
						{
							p.y--;
							for (auto& i : road) {
								if (i == p) {
									ws -= +Assistance::directions::NORTH;
									continue;
								}
							}
							road.push_back(p);
							if (get(p) != +blocks::TRAVELLED) get(p) = +blocks::EMPTY;
						}
						break;
						case +Assistance::directions::SOUTH:
						{
							p.y++;
							for (auto& i : road) {
								if (i == p) {
									ws -= +Assistance::directions::SOUTH;
									continue;
								}
							}
							road.push_back(p);
							if (get(p) != +blocks::TRAVELLED) get(p) = +blocks::EMPTY;
						}
						break;
						case +Assistance::directions::EAST:
						{
							p.x++;
							for (auto& i : road) {
								if (i == p) {
									ws -= +Assistance::directions::EAST;
									continue;
								}
							}
							road.push_back(p);
							if (get(p) != +blocks::TRAVELLED) get(p) = +blocks::EMPTY;
						}
						break;
						case +Assistance::directions::WEST:
						{
							p.x--;
							for (auto& i : road) {
								if (i == p) {
									ws -= +Assistance::directions::WEST;
									continue;
								}
							}
							road.push_back(p);
							if (get(p) != +blocks::TRAVELLED) get(p) = +blocks::EMPTY;
						}
						break;
						}
						hasdone = true;
					}
				}
			}

			ULONGLONG ul = GetTickCount64();
			for (short a = 0; a < 20 && GetTickCount64() - ul < 20;)
			{
				pt p(rand() % (wrld_s[0] - 2) + 1, rand() % (wrld_s[1] - 2) + 1);
				pt opts[4] = { p,p,p,p };
				opts[0].x++;
				opts[1].x--;
				opts[2].y++;
				opts[3].x--;

				int cases = 0;
				for (auto& i : opts) {
					if (!isThis(i, +blocks::BLOCK)) cases++;
				}

				if (isThis(p, +blocks::BLOCK) && (cases < 2)) {

					get(p) = +blocks::EMPTY;
					a++;
				}
			}

		}
		size_t World::getLen(const bool b)
		{
			return wrld_s[(int)b];
		}
		int World::readPos(const int x, const int y)
		{
			if (isValid(pt(x,y))) return get(pt(x,y));
			return -1;
		}
		void World::__print()
		{
			for (size_t y = 0; y < wrld_s[1]; y++)
			{
				for (size_t x = 0; x < wrld_s[0]; x++)
				{
					int u = get(pt(x, y));
					char uc;
					switch (u) {
					case +blocks::BLOCK:
						uc = '#';
						break;
					case +blocks::EMPTY:
						uc = ' ';
						break;
					case +blocks::TRAVELLED:
						uc = '`';
						break;
					default:
						uc = '~';
						break;
					}
					printf_s("%c ", uc);
				}
				printf_s("\n");
			}
		}
	}
}