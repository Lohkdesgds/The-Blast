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
				if (ok) res |= +Constants::directions::WEST;
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
				if (ok) res |= +Constants::directions::EAST;
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
				if (ok) res |= +Constants::directions::NORTH;
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
				if (ok) res |= +Constants::directions::SOUTH;
			}

			return res;
		}

		int World::waysToSolve(const pt p, blocks tst)
		{
			int res = 0;

			// lefty
			{
				pt i = p;
				i.x--;
				if (isThis(i, +tst)) res |= +Constants::directions::WEST;
			}
			// righty
			{
				pt i = p;
				i.x++;
				if (isThis(i, +tst)) res |= +Constants::directions::EAST;
			}
			// northy
			{
				pt i = p;
				i.y--;
				if (isThis(i, +tst)) res |= +Constants::directions::NORTH;
			}
			// southy
			{
				pt i = p;
				i.y++;
				if (isThis(i, +tst)) res |= +Constants::directions::SOUTH;
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

		bool World::isSolvable(pt str, const pt end) // expect blocks::EXIT as exit
		{
			std::vector<pt> road;
			road.push_back(str);

			//ULONGLONG ul = GetTickCount64();

			while (road.size() > 0/* && GetTickCount64() - ul < 30*/)
			{
				auto stt = road.back();

				int wey = waysToSolve(stt);

				int dif[2];
				dif[0] = end.x - stt.x;
				dif[1] = end.y - stt.y;

				int options_optimal[2] = { (dif[0] != 0 ? (dif[0] < 0 ? +Constants::directions::WEST : +Constants::directions::EAST) : 0), (dif[1] != 0 ? (dif[1] < 0 ? +Constants::directions::SOUTH : +Constants::directions::NORTH) : 0) };

				int optimal = (fabs(dif[0]) < fabs(dif[1])) ? (options_optimal[0]) : (options_optimal[1]);
				if (!(wey & optimal)) optimal = options_optimal[0] | options_optimal[1];

				int goin;
				if (get(stt) != +blocks::DEBUG) goin = (wey & optimal) ? (wey & optimal) : wey;
				else goin = wey;

				if (!goin) { // 0
					road.pop_back();
				}
				else {
					if (goin & +Constants::directions::NORTH) // here go out of bounds
					{
						stt.y--;
					}
					else if (goin & +Constants::directions::SOUTH)
					{
						stt.y++;
					}
					else if (goin & +Constants::directions::EAST)
					{
						stt.x++;
					}
					else if (goin & +Constants::directions::WEST)
					{
						stt.x--;
					}

					if (waysToSolve(stt, blocks::EXIT)) {
						return true;
					}
					road.push_back(stt);
				}
				get(stt) = +blocks::DEBUG;
			}
			return false;
		}

		bool World::randomCorrosion(pt* p, const int to)
		{
			if (to < +blocks::__CORROSION_UNTIL) throw Abort::warn(__FUNCSIG__, "Unexpected corrosion (map corrosion) value (" + std::to_string(to) + ")");

			int posx = rand() % (wrld_s[0] - 2) + 1;
			int posy = rand() % (wrld_s[1] - 2) + 1;

			for (unsigned tries = 0; tries < 20 && readPos(posx, posy) > +blocks::__CORROSION_UNTIL; tries++)
			{
				posx = rand() % (wrld_s[0] - 2) + 1;
				posy = rand() % (wrld_s[1] - 2) + 1;
			}

			if (readPos(posx, posy) <= +blocks::__CORROSION_UNTIL) {
				if (readPos(posx, posy) == +blocks::EMPTY || readPos(posx, posy) == +blocks::LIFE) get(pt(posx, posy)) = +blocks::FAKE_EXIT; // empty/life should not become block like that (collision behaviour)
				else get(pt(posx, posy)) = to;
				if (p) {
					p->x = posx;
					p->y = posy;
				}
				return true;
			}
			return false;
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
		void World::setTime(const double plus)
		{
			now_t = al_get_time() + plus;
		}
		void World::setDelta(const double v)
		{
			delta_t_corrupt = v;
		}
		POINT World::generate(const int seed)
		{
			POINT thu_endd;

			for (size_t p = 0; p < wrld_s[0] * wrld_s[1]; p++) wrld[p] = +blocks::BLOCK;

			srand((unsigned int)GetTickCount64());

			pt first_point(rand() % (wrld_s[0] - 2) + 1, rand() % (wrld_s[1] - 2) + 1);

			std::vector<pt> road;

			road.push_back(first_point);
			get(first_point) = +blocks::EMPTY;

			// gen map
			while (road.size() > 0)
			{
				auto p = road.back();
				int ws = waysToGo(p);

				if (!ws) {
					//printf_s("Didn't find any route");
					get(p) = +blocks::DEBUG;
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
						case +Constants::directions::NORTH:
						{
							p.y--;
							for (auto& i : road) {
								if (i == p) {
									ws -= +Constants::directions::NORTH;
									continue;
								}
							}
							road.push_back(p);
							if (get(p) != +blocks::DEBUG) get(p) = +blocks::EMPTY;
						}
						break;
						case +Constants::directions::SOUTH:
						{
							p.y++;
							for (auto& i : road) {
								if (i == p) {
									ws -= +Constants::directions::SOUTH;
									continue;
								}
							}
							road.push_back(p);
							if (get(p) != +blocks::DEBUG) get(p) = +blocks::EMPTY;
						}
						break;
						case +Constants::directions::EAST:
						{
							p.x++;
							for (auto& i : road) {
								if (i == p) {
									ws -= +Constants::directions::EAST;
									continue;
								}
							}
							road.push_back(p);
							if (get(p) != +blocks::DEBUG) get(p) = +blocks::EMPTY;
						}
						break;
						case +Constants::directions::WEST:
						{
							p.x--;
							for (auto& i : road) {
								if (i == p) {
									ws -= +Constants::directions::WEST;
									continue;
								}
							}
							road.push_back(p);
							if (get(p) != +blocks::DEBUG) get(p) = +blocks::EMPTY;
						}
						break;
						}
						hasdone = true;
					}
				}
			}

			// clear TRAVELLED
			auto clearfunc = [&](const bool secure = false) {for (size_t d = 0; d < wrld_s[0] * wrld_s[1]; d++) if (wrld[d] >= +(secure ? blocks::__SECURE_CLEAR_FROM_WHERE : blocks::__CLEAR_FROM_WHAT_VAL)) wrld[d] = +blocks::EMPTY; };
			clearfunc();
			
			// random empty blocks
			ULONGLONG ul = GetTickCount64();
			for (short a = 0; a < 10 && GetTickCount64() - ul < 20;)
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

			// random start
			ul = GetTickCount64();
			for (bool don = false; !don;)
			{
				pt start_point(rand() % (wrld_s[0] - 2) + 1, rand() % (wrld_s[1] - 2) + 1);

				if (isThis(start_point, +blocks::EMPTY)) get(start_point) = +blocks::START_POINT;
				else {
					clearfunc();
					continue;
				}

				pt end_point(rand() % (wrld_s[0] - 2) + 1, rand() % (wrld_s[1] - 2) + 1);

				if (isThis(end_point, +blocks::BLOCK)) get(end_point) = +blocks::EXIT;
				else {
					clearfunc();
					continue;
				}

				if (!isSolvable(start_point, end_point)) {
					clearfunc();
					continue;
				}
				clearfunc(true);

				thu_endd.x = +start_point.x;
				thu_endd.y = +start_point.y;
				don = true;
			}


			// random life blocks
			ul = GetTickCount64();
			for (short a = 0; a < 10 && GetTickCount64() - ul < 20;) {
				pt p(rand() % (wrld_s[0] - 2) + 1, rand() % (wrld_s[1] - 2) + 1);
				if (isThis(p, +blocks::EMPTY)) {
					get(p) = +blocks::LIFE;
					a++;
				}
			}

			// random prim corrosion
			ul = GetTickCount64();
			for (short a = 0; a < 6 && GetTickCount64() - ul < 30; a += (short)randomCorrosion(nullptr, +blocks::FAKE_EXIT));

			return thu_endd;
		}
		size_t World::getLen(const bool b)
		{
			return wrld_s[(int)b];
		}
		int World::readPos(const int x, const int y)
		{
			if (isValid(pt(x, y))) return get(pt(x, y));
			return -1;
		}
		void World::setPos(const int x, const int y, const int v)
		{
			get({ +x, +y }) = v;
		}
		bool World::corruptWorld(std::vector<POINT>& v)
		{
			if (save_now_dif != 0.0) {
				now_t = al_get_time() - save_now_dif;
				save_now_dif = 0;
			}

			if (al_get_time() - now_t <= delta_t_corrupt) return false;
			now_t = al_get_time() + delta_t_corrupt;

			v.clear();

			pt np;
			if (randomCorrosion(&np))
			{
				POINT conv;
				conv.x = np.x;
				conv.y = np.y;
				v.push_back(conv);
			}

			for (int a = 1; a < wrld_s[0] - 1; a++)
			{
				for (int b = 1; b < wrld_s[1] - 1; b++)
				{
					if (readPos(a, b) == +blocks::BRICKING_3) {
						get(pt(a, b)) = +blocks::FAKE_EXIT;
						v.push_back({ a, b });
					}
					else if (readPos(a, b) >= +blocks::BRICKING_0 && readPos(a, b) < +blocks::BRICKING_3) {
						get(pt(a, b))++;
						v.push_back({ a, b });
					}
				}
			}

			return true;
		}
		void World::pauseCorrupt()
		{
			save_now_dif = al_get_time() - now_t;
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
					case +blocks::START_POINT:
						uc = 'T';
						break;
					case +blocks::FAKE_EXIT:
						uc = 'e';
						break;
					case +blocks::EXIT:
						uc = 'E';
						break;
					case +blocks::DEBUG:
						uc = 'D';
						break;
					default:
						uc = '~';
						break;
					}
					printf_s("%c ", uc);
				}
				printf_s("\n");
			}
			printf_s("\n");
		}
	}
}