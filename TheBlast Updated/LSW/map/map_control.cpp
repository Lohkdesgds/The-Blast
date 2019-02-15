
#include "..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Map {



			const Safer::safe_string interpretIntToBlockName(const blockstats u)
			{
				unsigned p = (unsigned)u;
				Safer::safe_string s = Defaults::map_default_start.g() + std::string("%u");

				char endval[128];
				sprintf_s(endval, s.g().c_str(), p);

				return endval;
			}

			void map::randomizer(const int seed)
			{
				double start = al_get_time();
				bool not_done = true;

				for (int b = 0; b < Defaults::map_size_default_y; b++)
				{
					for (int a = 0; a < Defaults::map_size_default_x; a++)
						map_i[a][b] = 1;
				}

				int posx = 1, posy = 1;
				int temp = 0, backup = 1;

				map_i[posx][posy] = 0;

				srand((seed + 12)*(seed + 9));

				while (not_done)
				{
					for (int time = 0; time < 1000000 && al_get_time() - start < Defaults::map_load_max_time;)
					{
						//getch();
						map_i[posx][posy] = backup;
						/*system("cls");
						for (int b = 0; b < Defaults::map_size_default_y; b++){
							for (int a = 0; a < Defaults::map_size_default_x; a++)
								cout << map_i[a][b];
							cout << endl;
						}
						cout << endl;*/
						if (temp < 4) temp = rand() % 4;
						/// 123
						/// 456
						/// 789
						/*
									map_i[(posx  )+Defaults::map_size_default_y*(posy  )] == 1 &&
									map_i[(posx  )+Defaults::map_size_default_y*(posy  )] == 1 &&
									map_i[(posx  )+Defaults::map_size_default_y*(posy  )] == 1 &&
									map_i[(posx  )+Defaults::map_size_default_y*(posy  )] == 1 &&
									map_i[(posx  )+Defaults::map_size_default_y*(posy  )] == 1
						*/
						switch (temp)
						{
						case 0: // GO TO RIGHT
							if (posx < Defaults::map_size_default_x - 2)
							{
								posx++;
								if (map_i[(posx)][(posy - 1)] == 1 &&
									map_i[(posx + 1)][(posy - 1)] == 1 &&
									(map_i[(posx + 1)][(posy)] == 1 || map_i[(posx + 1)][(posy)] == 0) &&
									map_i[(posx + 1)][(posy + 1)] == 1 &&
									map_i[(posx)][(posy + 1)] == 1)
								{
									map_i[posx][posy] = 0;
									time--;
								}
								else if (map_i[(posx - 1)][(posy)] == 0)
								{
									posx--;
									time++;
									//cout << "Processing... (" << time << "%) \r";
								}
							}
							else temp = 4;
							break;
						case 1: // GO TO LEFT
							if (posx > 1)
							{
								posx--;
								if (map_i[(posx)][(posy - 1)] == 1 &&
									map_i[(posx - 1)][(posy - 1)] == 1 &&
									(map_i[(posx - 1)][(posy)] == 1 || map_i[(posx - 1)][(posy)] == 0) &&
									map_i[(posx - 1)][(posy + 1)] == 1 &&
									map_i[(posx)][(posy + 1)] == 1)
								{
									map_i[posx][posy] = 0;
									time--;
								}
								else if (map_i[(posx + 1)][(posy)] == 0)
								{
									posx++;
									time++;
									//cout << "Processing... (" << time << "%) \r";
								}
							}
							else temp = 4;
							break;
						case 2: // GO TO NORTH
							if (posy > 1)
							{
								posy--;
								if (map_i[(posx - 1)][(posy)] == 1 &&
									map_i[(posx - 1)][(posy - 1)] == 1 &&
									(map_i[(posx)][(posy - 1)] == 1 || map_i[(posx)][(posy - 1)] == 0) &&
									map_i[(posx + 1)][(posy - 1)] == 1 &&
									map_i[(posx + 1)][(posy)] == 1)
								{
									map_i[posx][posy] = 0;
									time--;
								}
								else if (map_i[(posx)][(posy + 1)] == 0)
								{
									posy++;
									time++;
									//cout << "Processing... (" << time << "%) \r";
								}
							}
							else temp = 4;
							break;
						case 3: // GO TO SOUTH
							if (posy < Defaults::map_size_default_y - 2)
							{
								posy++;
								if (map_i[(posx - 1)][(posy)] == 1 &&
									map_i[(posx - 1)][(posy + 1)] == 1 &&
									(map_i[(posx)][(posy + 1)] == 1 || map_i[(posx)][(posy + 1)] == 0) &&
									map_i[(posx + 1)][(posy + 1)] == 1 &&
									map_i[(posx + 1)][(posy)] == 1)
								{
									map_i[posx][posy] = 0;
									time--;
								}
								else if (map_i[(posx)][(posy - 1)] == 0)
								{
									posy--;
									time++;
									//cout << "Processing... (" << time << "%) \r";
								}
							}
							else temp = 4;
							break;
						}

						int tryy = 0;

						while (temp > 3 && tryy < 20)
						{
							tryy++;
							temp = rand() % 4;
							switch (temp)
							{
							case 0: // GO TO RIGHT
								if (map_i[(posx - 1)][(posy)] == 0)
								{
									posx--;
								}
								else temp = 4;
								break;
							case 1: // GO TO LEFT
								if (map_i[(posx + 1)][(posy)] == 0)
								{
									posx++;
								}
								else temp = 4;
								break;
							case 2: // GO TO NORTH
								if (map_i[(posx)][(posy + 1)] == 0)
								{
									posy++;
								}
								else temp = 4;
								break;
							case 3: // GO TO SOUTH
								if (map_i[(posx)][(posy - 1)] == 0)
								{
									posy--;
								}
								else temp = 4;
								break;
							}
						}
						if (tryy >= 20)
						{
							temp = 0;
							do
							{
								tryy++;
								posx = rand() % (Defaults::map_size_default_x - 2) + 1;
								posy = rand() % (Defaults::map_size_default_y - 2) + 1;
							} while (map_i[posx][posy] == 0 && tryy < 50);
						}

						if (time % 100 == 0)
						{
							do
							{
								posx = rand() % (Defaults::map_size_default_x - 2) + 1;
								posy = rand() % (Defaults::map_size_default_y - 2) + 1;
							} while (map_i[posx][posy] != SPACEID && al_get_time() - start < Defaults::map_load_max_time);
						}
						backup = map_i[posx][posy];
						map_i[posx][posy] = 3;
					}

					//map_i[1][1] = STARTID;

					int timess = 0;

					for (int xtras = 0; xtras < 10; timess++)
					{
						posx = rand() % (Defaults::map_size_default_x - 2) + 1;
						posy = rand() % (Defaults::map_size_default_y - 2) + 1;
						if ((map_i[posx][posy] == BLOCKID) &&
							((map_i[posx + 1][posy] == 0 && map_i[posx - 1][posy] == 0) ||
							(map_i[posx][posy + 1] == 0 && map_i[posx][posy - 1] == 0))
							)
						{
							int isthere = 0;

							for (int a = -3; a < 4; a++)
							{
								for (int b = -3; b < 4; b++)
								{
									if (posx + a >= 0 && posx + a < Defaults::map_size_default_x && posy + b >= 0 && posy + b < Defaults::map_size_default_y)
									{
										if (map_i[posx + a][posy + b] == INVISID || map_i[posx + a][posy + b] == THENDID)
											isthere++;
									}
								}
							}
							if (isthere < 1)
							{
								map_i[posx][posy] = INVISID;
								xtras++;
							}
						}
						if (timess > 100)
							break;
					}

					start = al_get_time();

					while (map_i[posx][posy] != STARTID && al_get_time() - start < Defaults::map_load_max_time)
					{
						posx = rand() % (Defaults::map_size_default_x - 2) + 1;
						posy = rand() % (Defaults::map_size_default_y - 2) + 1;

						int isthere = 0;

						for (int a = -2; a < 3; a++)
						{
							for (int b = -2; b < 3; b++)
							{
								if (posx + a >= 0 && posx + a < Defaults::map_size_default_x && posy + b >= 0 && posy + b < Defaults::map_size_default_y)
								{
									if (map_i[posx + a][posy + b] == INVISID || map_i[posx + a][posy + b] == THENDID)
										isthere++;
								}
							}
						}

						if (isthere == 0)
						{
							map_i[posx][posy] = STARTID;
						}
					}

					temp = 0;
					int isitok = 0;

					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{
						for (int a = 0; a < Defaults::map_size_default_x; a++)
							if (map_i[a][b] == 0)
								isitok++;
					}
					if (isitok > 50 && workOnItAndAnswer(posx, posy))
						not_done = false;
					else
					{
						// failed, trying another one

						randomizer(seed * seed * rand() + time(0));
					}
				}
			}
			const bool map::workOnItAndAnswer(const int pxx, const int pyy)
			{
				int man[32][18];
				int posx = pxx, posy = pyy;
				bool done = false;

				double tima = al_get_time();

				for (int b = 0; b < Defaults::map_size_default_y; b++)
				{
					for (int a = 0; a < Defaults::map_size_default_x; a++)
						man[a][b] = map_i[a][b];
				}

				while (!done && al_get_time() - tima < Defaults::map_resolve_max_time)
				{
					int loko = rand() % 4;

					int recc = recommendation(man, posx, posy);

					if (recc != -1) loko = recc;

					done = workto(man, loko, posx, posy);

					if (posx < 1) posx = 1;
					if (posy < 1) posy = 1;
					if (posx > Defaults::map_size_default_x - 2) posx = Defaults::map_size_default_x - 2;
					if (posy > Defaults::map_size_default_y - 2) posy = Defaults::map_size_default_y - 2;
				}
				return done;
			}
			const int map::recommendation(int to_work[Defaults::map_size_default_x][Defaults::map_size_default_y], const int posx, const int posy)
			{
				int directions = 0;
				if (to_work[posx + 1][posy] == LIVRE || to_work[posx + 1][posy] == INVISID) directions++;
				if (to_work[posx - 1][posy] == LIVRE || to_work[posx - 1][posy] == INVISID) directions++;
				if (to_work[posx][posy + 1] == LIVRE || to_work[posx][posy + 1] == INVISID) directions++;
				if (to_work[posx][posy - 1] == LIVRE || to_work[posx][posy - 1] == INVISID) directions++;

				if (directions < 2)
				{
					if (to_work[posx + 1][posy] == LIVRE || to_work[posx + 1][posy] == INVISID) return KEY_RIGHT;
					if (to_work[posx - 1][posy] == LIVRE || to_work[posx - 1][posy] == INVISID) return KEY_LEFT;
					if (to_work[posx][posy + 1] == LIVRE || to_work[posx][posy + 1] == INVISID) return KEY_DOWN;
					if (to_work[posx][posy - 1] == LIVRE || to_work[posx][posy - 1] == INVISID) return KEY_UP;
				}
				return -1;
			}

			const bool map::isthereanySEMSAIDA(int to_work[Defaults::map_size_default_x][Defaults::map_size_default_y], const int posx, const int posy)
			{
				bool isthere = false;
				if ((to_work[posx + 1][posy] == SEM_SAIDA || to_work[posx + 1][posy] == BLOCKID || to_work[posx + 1][posy] == VISITADO || to_work[posx + 1][posy] == INVISID) &&
					(to_work[posx - 1][posy] == SEM_SAIDA || to_work[posx - 1][posy] == BLOCKID || to_work[posx - 1][posy] == VISITADO || to_work[posx - 1][posy] == INVISID) &&
					(to_work[posx][posy + 1] == SEM_SAIDA || to_work[posx][posy + 1] == BLOCKID || to_work[posx][posy + 1] == VISITADO || to_work[posx][posy + 1] == INVISID) &&
					(to_work[posx][posy - 1] == SEM_SAIDA || to_work[posx][posy - 1] == BLOCKID || to_work[posx][posy - 1] == VISITADO || to_work[posx][posy - 1] == INVISID)) isthere = true;
				return isthere;
			}
			void map::gotoVISITADO(int to_work[Defaults::map_size_default_x][Defaults::map_size_default_y], int& posx, int& posy)
			{
				to_work[posx][posy] = SEM_SAIDA;

				if (to_work[posx + 1][posy] == VISITADO) posx++;
				else if (to_work[posx - 1][posy] == VISITADO) posx--;
				else if (to_work[posx][posy + 1] == VISITADO) posy++;
				else if (to_work[posx][posy - 1] == VISITADO) posy--;
				else
				{
					switch (rand() % 4)
					{
					case KEY_UP:
						if (to_work[posx][posy - 1] == SEM_SAIDA)
						{
							posy--;
						}
						break;
					case KEY_DOWN:
						if (to_work[posx][posy - 1] == SEM_SAIDA)
						{
							posy++;
						}
						break;
					case KEY_LEFT:
						if (to_work[posx - 1][posy] == SEM_SAIDA)
						{
							posx--;
						}
						break;
					case KEY_RIGHT:
						if (to_work[posx + 1][posy] == SEM_SAIDA)
						{
							posx++;
						}
						break;
					}
				}
			}
			const bool map::workto(int to_work[Defaults::map_size_default_x][Defaults::map_size_default_y], int to_where, int& posx, int& posy)
			{
				bool retuns = false;
				switch (to_where)
				{
				case KEY_UP:
					if (to_work[posx][posy - 1] == LIVRE || to_work[posx][posy - 1] == INVISID || to_work[posx][posy - 1] == STARTID)
					{
						//if (to_work[posx][posy] == VISITADO) to_work[posx][posy] = SEM_SAIDA;
						posy--;
					}
					else if (isthereanySEMSAIDA(to_work, posx, posy))
						gotoVISITADO(to_work, posx, posy);

					else if (to_work[posx][posy - 1] == THENDID)
					{
						posy--;
						retuns = true;
					}
					else if (to_work[posx][posy + 1] == THENDID)
					{
						posy++;
						retuns = true;
					}
					else if (to_work[posx - 1][posy] == THENDID)
					{
						posx--;
						retuns = true;
					}
					else if (to_work[posx + 1][posy] == THENDID)
					{
						posx++;
						retuns = true;
					}
					break;
				case KEY_DOWN:
					if (to_work[posx][posy + 1] == LIVRE || to_work[posx][posy + 1] == INVISID || to_work[posx][posy + 1] == STARTID)
					{
						//if (to_work[posx][posy] == VISITADO) to_work[posx][posy] = SEM_SAIDA;
						posy++;
					}
					else if (isthereanySEMSAIDA(to_work, posx, posy))
						gotoVISITADO(to_work, posx, posy);

					else if (to_work[posx][posy - 1] == THENDID)
					{
						posy--;
						retuns = true;
					}
					else if (to_work[posx][posy + 1] == THENDID)
					{
						posy++;
						retuns = true;
					}
					else if (to_work[posx - 1][posy] == THENDID)
					{
						posx--;
						retuns = true;
					}
					else if (to_work[posx + 1][posy] == THENDID)
					{
						posx++;
						retuns = true;
					}
					break;
				case KEY_LEFT:
					if (to_work[posx - 1][posy] == LIVRE || to_work[posx - 1][posy] == INVISID || to_work[posx - 1][posy] == STARTID)
					{
						//if (to_work[posx][posy] == VISITADO) to_work[posx][posy] = SEM_SAIDA;
						posx--;
					}
					else if (isthereanySEMSAIDA(to_work, posx, posy))
						gotoVISITADO(to_work, posx, posy);

					else if (to_work[posx][posy - 1] == THENDID)
					{
						posy--;
						retuns = true;
					}
					else if (to_work[posx][posy + 1] == THENDID)
					{
						posy++;
						retuns = true;
					}
					else if (to_work[posx - 1][posy] == THENDID)
					{
						posx--;
						retuns = true;
					}
					else if (to_work[posx + 1][posy] == THENDID)
					{
						posx++;
						retuns = true;
					}
					break;
				case KEY_RIGHT:
					if (to_work[posx + 1][posy] == LIVRE || to_work[posx + 1][posy] == INVISID || to_work[posx + 1][posy] == STARTID)
					{
						//if (to_work[posx][posy] == VISITADO) to_work[posx][posy] = SEM_SAIDA;
						posx++;
					}
					else if (isthereanySEMSAIDA(to_work, posx, posy))
						gotoVISITADO(to_work, posx, posy);

					else if (to_work[posx][posy - 1] == THENDID)
					{
						posy--;
						retuns = true;
					}
					else if (to_work[posx][posy + 1] == THENDID)
					{
						posy++;
						retuns = true;
					}
					else if (to_work[posx - 1][posy] == THENDID)
					{
						posx--;
						retuns = true;
					}
					else if (to_work[posx + 1][posy] == THENDID)
					{
						posx++;
						retuns = true;
					}
					break;
				}
				if (to_work[posx][posy] == LIVRE || to_work[posx][posy] == INVISID || to_work[posx][posy] == STARTID)    to_work[posx][posy] = VISITADO;
				return retuns;
			}
			void map::verifyCollision(Sprite::sprite & s)
			{
				double orig_x, orig_y;
				int posx, posy;

				s.get(Sprite::POSX, orig_x);
				posx = (0.5 * ((orig_x + x_off) + 1.0))*Defaults::map_size_default_x;
				s.get(Sprite::POSY, orig_y);
				posy = (0.5 * ((orig_y + y_off) + 1.0))*Defaults::map_size_default_y;

				for (int a = posx - 1; a < posx + 2; a++)
				{
					for (int b = posy - 1; b < posy + 2; b++)
					{
						if (a >= 0 && a < Defaults::map_size_default_x && b >= 0 && b < Defaults::map_size_default_y)
						{

							if (map_p[a][b] && !isBlockTransparent((blockstats)map_i[a][b])) {
								map_p[a][b]->_resetCollision();
								s._resetCollision();
								s._verifyCollision(*map_p[a][b]);

								//if () s._resetCollision();
							}
						}
					}
				}
			}

			const bool map::isBlockTransparent(const blockstats u)
			{
				for (auto& i : blocks_air)
				{
					if ((int)u == i) return true;
				}
				return false;
			}

			void map::generateMap()
			{
				Log::gfile logg;
				logg << Log::START << Log::_func("map", "generateMap") << "Generating map..." << Log::ENDL;

				has_reachedEnd = false;
				hasToUpdate = true;
				has_loadedMap = false;

				spawn[0] = spawn[1] = 0.0;

				if (seed == 0)
				{
					for (int a = 0; a < Defaults::map_size_default_x; a++)
					{
						for (int b = 0; b < Defaults::map_size_default_y; b++)
						{
							if (a == 0 || a == Defaults::map_size_default_x - 1 || b == 0 || b == Defaults::map_size_default_y - 1)
							{
								map_i[a][b] = 1;
							}
							else
							{
								map_i[a][b] = 0;
							}
						}
					}

					map_i[5][5] = 1;

					map_i[10][16] = 1;
					map_i[10][15] = 1;
					map_i[10][14] = 1;
					map_i[10][13] = 1;

					map_i[11][16] = -1;
					map_i[11][15] = -1;
					map_i[11][14] = -1;
					map_i[11][13] = -1;
					map_i[11][12] = 1;
					map_i[11][11] = 1;
					map_i[11][10] = 1;
					map_i[11][9] = 1;
					map_i[2][13] = 4;
					map_i[10][5] = 1;
					map_i[11][5] = 1;
					map_i[12][6] = 1;
					map_i[13][5] = 1;
					map_i[17][17] = 1;
					map_i[18][16] = 1;
					map_i[19][15] = 1;
					map_i[20][14] = 1;
					map_i[21][13] = 1;
					map_i[1][1] = 2;
				}
				else
				{
					randomizer(seed);
				}

				{
					char debugg[Defaults::map_size_default_x*Defaults::map_size_default_y + Defaults::map_size_default_y + 5];
					int debugi = 0;
					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{
						for (int a = 0; a < Defaults::map_size_default_x; a++)
						{
							debugg[debugi++] = '0' + map_i[a][b];
						}
						debugg[debugi++] = '\n';
					}
					debugg[debugi] = '\0';
					logg.debug("New map:\n" + std::string(debugg));
				}

				has_loadedMap = true;
				//is_new_Map = true;
				logg << Log::START << Log::_func("map", "generateMap") << "Generated map." << Log::ENDL;
			}

			void map::_checkBitmapsOnMapP()
			{
				d_sprite_database spr_data;
				Log::gfile logg;

				lock();

				for (int b = 0; b < Defaults::map_size_default_y; b++)
				{
					for (int a = 0; a < Defaults::map_size_default_x; a++)
					{
						Safer::safe_pointer<Sprite::sprite> each;
						if (!map_p[a][b]) {

							if (spr_data.get(each, "_MAP_" + std::to_string(a) + "_" + std::to_string(b)))
							{
								map_p[a][b] = each;

								logg.debug("Adjusted bitmap at " + std::to_string(a) + ":" + std::to_string(b));


								continue;
							}
							else {
								logg << Log::START << Log::_func("map", "_checkBitmapsOnMapP") << "Prepared Sprite on " << a << ":" << b << Log::ENDL;
								//spr_data.create(each);
								each = Sprite::getOrCreate("_MAP_" + std::to_string(a) + "_" + std::to_string(b), true);

								each->set(Sprite::POSX, x_off + (1.0*(2.0*a / (Defaults::map_size_default_x*1.0)) - 1.0));
								each->set(Sprite::POSY, y_off + (1.0*(2.0*b / (Defaults::map_size_default_y*1.0)) - 1.0));
								each->set(Sprite::SCALEX, 2.0 / Defaults::map_size_default_x);
								each->set(Sprite::SCALEY, 2.0 / Defaults::map_size_default_y);
								each->set(Sprite::LAYER, layerUsed);
								each->set(Sprite::COLLIDE, true);
								each->set(Sprite::DRAW, false);
								each->set(Sprite::_SKIP_DEFAULT_COLLISION_METHOD, true);

								map_p[a][b] = each;
							}
						}
						else each = map_p[a][b];

						int ouu = map_i[a][b];
						if (ouu < SPACEID) ouu = SPACEID;
						if (ouu > BLOCKID_C03) ouu = BLOCKID_C03;
						Safer::safe_string id_path = interpretIntToBlockName((blockstats)ouu);

						each->replaceAllWith(id_path);
					}
				}

				unlock();
			}

			void map::_checkBitmapsBigMap()
			{
				if (!big_map || !big_map_il)
				{
					if (big_map) {
						Image::easyRemove("__MAP_FRAME_FULL");
						//delete big_map;
					}
					if (big_map_il) {
						///big_map_il->unload();
						Image::easyRemove("__MAP_FRAME_FULL_S");
						//delete big_map_il;
					}

					big_map_il = Image::getOrCreate("__MAP_FRAME_FULL", true);
					big_map_il->set(Image::CREATE_X, -1);
					big_map_il->set(Image::CREATE_Y, -1);
					big_map_il->set(Image::FORCE_ON_MEMORY, true);
					big_map_il->load();

					big_map = Sprite::getOrCreate("__MAP_FRAME_FULL_S", true);
					big_map->add("__MAP_FRAME_FULL");
					big_map->set(Sprite::LAYER, layerUsed);
					big_map->set(Sprite::DRAW, true);
					big_map->set(Sprite::SCALEG, 2.0);

					hasToUpdate = true;
				}
			}

			void map::_redraw()
			{
				Log::gfile logg;
				Camera::camera_g cam;
				bool redrawall = false;

				if (!big_map) { //return;
					logg << Log::NEEDED_START << Log::_func("map", "_redraw", Log::ERRR) << "Big_map is unreachable! Trying to fix this..." << Log::NEEDED_ENDL;

					_checkBitmapsBigMap();

					if (!big_map) {
						logg << Log::NEEDED_START << Log::_func("map", "_redraw", Log::ERRR) << "Big_map STILL unreachable! Aborting _REDRAW!" << Log::NEEDED_ENDL;
						return;
					}

					redrawall = true;
				}
				if (!big_map_il) {
					logg << Log::NEEDED_START << Log::_func("map", "_redraw", Log::ERRR) << "Big_map_il is unreachable! Trying to fix this..." << Log::NEEDED_ENDL;

					_checkBitmapsBigMap();

					if (!big_map_il) {
						logg << Log::NEEDED_START << Log::_func("map", "_redraw", Log::ERRR) << "Big_map_il STILL unreachable! Aborting _REDRAW!" << Log::NEEDED_ENDL;
						return;
					}

					redrawall = true;
				}


				ALLEGRO_BITMAP* targ = al_get_target_bitmap();
				if (!targ) {
					logg << Log::START << Log::_func("map", "_redraw", Log::WARN) << "Could not get target bitmap! Skipping map update for now..." << Log::ENDL;
					hasToUpdate = true;
					return;
				}
				big_map_il->get(Image::HAS_RELOADED, redrawall);

				big_map->_setAsTarg();

				//if (is_new_Map) al_clear_to_color(al_map_rgb(0, 0, 0));

				const int lastapply = cam.getLastApplyID();
				/*cam.copy(Defaults::default_map_layer_backup, lastapply); // backup
				cam.set(lastapply, Camera::OFFX, 0.0);
				cam.set(lastapply, Camera::OFFY, 0.0);
				cam.set(lastapply, Camera::ZOOM, 1.0);
				cam.set(lastapply, Camera::ZOOMX, 1.0);
				cam.set(lastapply, Camera::ZOOMY, 1.0);
				cam.apply(lastapply);*/

				cam.reset(Defaults::default_map_layer_backup);
				cam.apply(Defaults::default_map_layer_backup);

				for (auto& i : map_p)
				{
					size_t pp = 0;
					for (auto& j : i)
					{
						if (!j) {
							logg << Log::NEEDED_START << Log::_func("map", "_redraw", Log::ERRR) << "MAP NULL AT " << pp << "! Trying to adjust map_p!" << Log::NEEDED_ENDL;
							_checkBitmapsOnMapP();
							return _redraw();
						}
						/*else if (is_new_Map) {
							j->forceDraw();
						}*/
						else if (j->hasChanged() || redrawall || hasToUpdate) j->forceDraw();
					}
				}
				///cam.copy(lastapply, Defaults::default_layer_backup);
				//cam.apply(lastapply); // not needed because transformation is bitmap based.
				al_set_target_bitmap(targ);
				cam.apply(lastapply);

				//is_new_Map = false;

				logg.debug("Map has updated its image.");
			}

			void map::_getActualPos(int &posx, int &posy, const double orig_x, const double orig_y)
			{
				posx = (0.5 * ((orig_x/* + x_off*/)+1.0))*Defaults::map_size_default_x;
				posy = (0.5 * ((orig_y/* + y_off*/)+1.0))*Defaults::map_size_default_y;

				if (posx < 0) posx = 0;
				if (posx > Defaults::map_size_default_x) posx = Defaults::map_size_default_x - 1;

				if (posy < 0) posy = 0;
				if (posy > Defaults::map_size_default_y) posy = Defaults::map_size_default_y - 1;
			}

			const bool map::_getPlayerPos(int &x, int &y)
			{
				Safer::safe_pointer<Sprite::sprite> player = plr->getS();
				double orig_x, orig_y;

				if (!player) return false;

				player->get(Sprite::POSX, orig_x);
				player->get(Sprite::POSY, orig_y);
				_getActualPos(x, y, orig_x, orig_y);
				return true;
			}

			void map::_savePlayerLastPos()
			{
				_getPlayerPos(pausepos[0], pausepos[1]);
			}

			void map::_setPlayerAtLastPos()
			{
				plr->getS()->set(Sprite::POSX, x_off + (1.0*(2.0*pausepos[0] / (Defaults::map_size_default_x*1.0)) - 1.0));
				plr->getS()->set(Sprite::POSY, y_off + (1.0*(2.0*pausepos[1] / (Defaults::map_size_default_y*1.0)) - 1.0));
			}

			map::map()
			{
				if (!plr) {
					d_entity_database ent_data;
					ent_data.try_create_as<Entities::player>(plr);
				}

				cpu_thr_ready = gpu_thr_ready = false;
			}


			map::~map()
			{
				if (cpu_thr_ready) reset_cpu_thr();
				if (gpu_thr_ready) reset_draw_thr();

				d_sprite_database spr_data;
				d_images_database img_data;

				Log::gfile logg;

				if (big_map)
				{
					Sprite::easyRemove("__MAP_FRAME_FULL_S");
					/*Safer::safe_pointer<Sprite::sprite> temp = big_map;
					big_map = nullptr;
					delete temp;*/
				}
				if (big_map_il)
				{
					///big_map_il->unload();
					Image::easyRemove("__MAP_FRAME_FULL");
					/*Safer::safe_pointer<Image::image_low> temp = big_map_il;
					big_map_il = nullptr;
					delete big_map_il;*/
				}
				for (int a = 0; a < Defaults::map_size_default_x; a++)
				{
					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{
						Sprite::easyRemove("_MAP_" + std::to_string(a) + "_" + std::to_string(b));
						/*logg << Log::START << Log::_func("map", "~map") << "Finally erased sprite at " << a << ":" << b << Log::ENDL;
						Safer::safe_pointer<Sprite::sprite> temp = map_p[a][b];*/
						map_p[a][b].reset();
						//delete temp;

						map_i[a][b] = SPACEID;
					}
				}
			}

			void map::reset_draw_thr()
			{
				/* ---------------| IMAGES & SPRITES |--------------- */


				/*lock();
				d_entity_database ent_data;
				ent_data.remove(plr.getMyName());
				unlock();

				plr.reset();*/

				gpu_thr_ready = false;
			}
			void map::reset_cpu_thr()
			{
				/* ---------------| Entities |--------------- */

				d_entity_database ent_data;

				is_player_enabled = false;

				for (size_t pos = 0; pos < ent_data.work().work().size();)
				{
					auto i = ent_data.work().get(pos);
					if (Safer::_check_pointer_existance<Entities::entity>(i)) {
						
						switch (i->getType())
						{
						case Entities::PLAYER:
						{
							Safer::safe_pointer<Entities::player> tempp = std::dynamic_pointer_cast<Entities::player>(i);
							tempp->reset();
						}
						break;
						case Entities::BADBOY:
						{
							Safer::safe_pointer<Entities::badboy> tempp = std::dynamic_pointer_cast<Entities::badboy>(i);
							tempp->reset();
						}
						break;
						}

						pos++;
					}
					else {
						ent_data.work().erase(pos);
					}
				}

				ent_data.clear();

				has_loadedMap = false;

				cpu_thr_ready = false;
			}

			const bool map::start_draw_thr()
			{
				Log::gfile logg;

				d_images_database img_data;
				d_sprite_database spr_data;

				if (!is_player_enabled) {
					if (!was_player_rgb_instead) launch_player(last_player_path, last_player_size, last_player_layer);
					else launch_player(last_player_color, last_player_size, last_player_layer);
				}
				if (badboys.size() == 0) {
					launch_badboys(last_badboys_path, last_badboy_count, last_badboys_layer);
				}

				if (!plr->getS()) return false;
				if (!cpu_thr_ready) return false;

				if (!has_loadedMap) {
					logg << Log::START << Log::_func("map", "start_draw_thr", Log::WARN) << "Called generateBitmaps before generating a map! Just call generateMap() before this!" << Log::ENDL;
					return false;
				}


				/* ---------------| Sprites all over the map |--------------- */

				_checkBitmapsOnMapP();

				/* ---------------| The hero sprite |--------------- */

				_checkBitmapsBigMap();

				/* ---------------| Setup of first spawn for player |--------------- */

				//lock();

				hasToUpdate = true;
				std::vector<point> poss;
				point player_temp_point;

				for (int a = 0; a < Defaults::map_size_default_x; a++)
				{
					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{
						if (map_i[a][b] == STARTID)
						{
							spawn[0] = (1.0*(2.0*a / (Defaults::map_size_default_x*1.0)) - 1.0);
							spawn[1] = (1.0*(2.0*b / (Defaults::map_size_default_y*1.0)) - 1.0);

							Safer::safe_pointer<Sprite::sprite> player = plr->getS();

							if (player) {
								player->set(Sprite::POSX, spawn[0] + x_off);
								player->set(Sprite::POSY, spawn[1] + y_off);
								player->set(Sprite::SPEEDX, 0.0);
								player->set(Sprite::SPEEDY, 0.0);
								player->set(Sprite::SCALEX, 2.0 / Defaults::map_size_default_x);
								player->set(Sprite::SCALEY, 2.0 / Defaults::map_size_default_y);
								player->set(Sprite::SCALEG, Defaults::user_scale_compared_to_map);
							}

							player_temp_point.x = a;
							player_temp_point.y = b;
							poss.push_back(player_temp_point);

							logg.debug("[" + std::to_string(poss.size()) + "/??]Defined player spawn pos as " + std::to_string(a) + ":" + std::to_string(b));

							a = Defaults::map_size_default_x;
							b = Defaults::map_size_default_y;
						}
					}
				}

				//unlock(); // set position on screen should not crash the game (now)

				/* ---------------| Setup of first spawn for badboys |--------------- */

				for (auto& i : badboys) {
					double x, y;

					int mapx = rand() % Defaults::map_size_default_x, mapy = rand() % Defaults::map_size_default_y;

					for (bool donee = false; !donee;) {

						mapx = (rand() % Defaults::map_size_default_x);
						mapy = (rand() % Defaults::map_size_default_y);

						if (!isBlockTransparent((blockstats)map_i[mapx][mapy])) continue;

						float dd = sqrt((player_temp_point.x - mapx) * (player_temp_point.y - mapy));
						if (dd < 5.0) continue;
	
						donee = true;

						for (auto& i : poss)
						{
							if (mapx == i.x && mapy == i.y) {
								donee = false;
								break;
							}
						}
					}
					{
						point ab;
						ab.x = mapx;
						ab.y = mapy;
						poss.push_back(ab);
					}

					logg.debug("[" + std::to_string(poss.size()) + "/" + std::to_string((size_t)badboys.size()) + "]Defined position of an enemy at " + std::to_string(mapx) + ":" + std::to_string(mapy));

					x = x_off + (1.0*(2.0*mapx / (Defaults::map_size_default_x*1.0)) - 1.0);
					y = y_off + (1.0*(2.0*mapy / (Defaults::map_size_default_y*1.0)) - 1.0);

					Safer::safe_pointer<Sprite::sprite> sprt = i->getS();
					if (sprt) {
						sprt->set(Sprite::POSX, x);
						sprt->set(Sprite::POSY, y);
					}
				}

				gpu_thr_ready = true;

				return true;
			}

			const bool map::start_cpu_thr()
			{
				lock();

				generateMap();

				hasToUpdate = true;
				cpu_thr_ready = true;

				unlock();

				return true;
			}

			void map::setSeed(const int u)
			{
				Log::gfile logg;
				logg << Log::START << Log::_func("map", "setSeed") << "Defined seed for map making as: " << u << Log::ENDL;

				seed = u;
			}
			void map::setLayer(const int u)
			{
				Log::gfile logg;
				logg << Log::START << Log::_func("map", "setLayer") << "Defined layer for further drawing as " << u << Log::ENDL;

				layerUsed = u;
			}

			void map::corruptWorldTick()
			{
				Log::gfile logg;

				int posx = rand() % (Defaults::map_size_default_x - 2) + 1;
				int posy = rand() % (Defaults::map_size_default_y - 2) + 1;

				for (unsigned tries = 0; tries < 50 && map_i[posx][posy] != BLOCKID; tries++)
				{
					posx = rand() % (Defaults::map_size_default_x - 2) + 1;
					posy = rand() % (Defaults::map_size_default_y - 2) + 1;
				}

				logg.debug("Changing some random blocks...");


				if (map_i[posx][posy] == BLOCKID) {
					logg.debug("Corrupted " + std::to_string(posx) + ":" + std::to_string(posy));
					map_i[posx][posy] = BLOCKID_C00;

					Safer::safe_string id_path = interpretIntToBlockName(BLOCKID_C00);
					map_p[posx][posy]->replaceAllWith(id_path);
				}

				for (int a = 1; a < Defaults::map_size_default_x - 1; a++)
				{
					for (int b = 1; b < Defaults::map_size_default_y - 1; b++)
					{
						if (map_i[a][b] == BLOCKID_C03) {
							map_i[a][b] = INVISID;

							Safer::safe_string id_path = interpretIntToBlockName(INVISID);

							map_p[a][b]->replaceAllWith(id_path);
						}
						else if (map_i[a][b] >= BLOCKID_C00 && map_i[a][b] < BLOCKID_C03) {
							map_i[a][b]++;

							Safer::safe_string id_path = interpretIntToBlockName((blockstats)map_i[a][b]);

							map_p[a][b]->replaceAllWith(id_path);
						}
					}
				}
				hasToUpdate_a_block = true;

				logg.debug("Done processing blocks.");
			}
			void map::setPlayerName(const Safer::safe_string s)
			{
				if (s.g().length() > 0) {
					plr->setMyName(s);
					last_player_name = s;
				}
			}
			/*void map::setPlayer(Sprite::sprite * s)
			{
				player = s;
			}*/
			void map::launch_player(const ALLEGRO_COLOR color, const double size, const int layer)
			{
				//plr.reset();
				was_player_rgb_instead = true;
				plr->load(color, layer, size);
				//plr.setMyHealth(1.0);
				if (last_player_name.g().length() == 0) last_player_name = randomName();
				plr->setMyName(last_player_name);
				/*{
					lock();
					d_entity_database ent_data;
					Safer::safe_pointer<Entities::entity> tempp = std::dynamic_pointer_cast<Entities::entity>(plr);
					ent_data.add(tempp);
					unlock();
				}*/

				last_player_color = color;
				//last_player_path = path;
				last_player_size = size;
				last_player_layer = layer;

				is_player_enabled = true;
			}
			void map::launch_player(const Safer::safe_string path, const double size, const int layer)
			{
				if (path.g().length() == 0) {
					throw "MAP::LAUNCH_PLAYER EXPECTED PATH, BUT DOESN'T HAVE ONE!";
					return;
				}

				//plr.reset();
				was_player_rgb_instead = false;
				plr->load(path, layer, size);
				//plr.setMyHealth(1.0);
				if (last_player_name.g().length() == 0) last_player_name = randomName();
				plr->setMyName(last_player_name);
				/*{
					lock();
					d_entity_database ent_data;
					Safer::safe_pointer<Entities::entity> tempp = std::dynamic_pointer_cast<Entities::entity>(plr);
					ent_data.add(tempp);
					unlock();
				}*/

				//last_player_color = color;
				last_player_path = path;
				last_player_size = size;
				last_player_layer = layer;

				is_player_enabled = true;
			}
			const bool map::launch_badboys(const Safer::safe_string path, const unsigned how_many, const int layerforbb)
			{
				if (path.g().length() == 0) {
					throw "MAP::LAUNCH_BADBOYS EXPECTED PATH, BUT DOESN'T HAVE ONE!";
					return false;
				}

				if (!is_player_enabled) return false;

				last_badboys_path = path;
				last_badboy_count = how_many;
				last_badboys_layer = layerforbb;

				for (unsigned o = 0; o < how_many; o++)
				{
					Safer::safe_pointer<Entities::badboy> bb;
					{
						lock();
						d_entity_database ent_data;
						ent_data.try_create_as<Entities::badboy>(bb);
						unlock();
					}
					bb->load(path);

					Safer::safe_pointer<Sprite::sprite> sprt = bb->getS();
					if (sprt) {
						sprt->set(Sprite::SPEEDX, 0.0);
						sprt->set(Sprite::SPEEDY, 0.0);
						sprt->set(Sprite::SCALEX, 2.0 / Defaults::map_size_default_x);
						sprt->set(Sprite::SCALEY, 2.0 / Defaults::map_size_default_y);
						sprt->set(Sprite::SCALEG, Defaults::user_scale_compared_to_map);
						sprt->set(Sprite::COLLIDE, true);
						sprt->set(Sprite::AFFECTED_BY_COLLISION, true);
						sprt->set(Sprite::LAYER, layerforbb);
						bb->setMyName(randomName());
					}
					else {
						throw "FAILED CREATING BADBOY!";
					}

					badboys.push_back(bb);
				}

				Safer::safe_pointer<Sprite::sprite> spr = plr->getS();
				if (spr) {
					for (auto& i : badboys)
					{
						i->setFollowing(spr);
					}
				}

				return true;
			}
			Entities::player& map::getPlayer()
			{
				return *plr;
			}
			Entities::badboy & map::getBB(const size_t p)
			{
				if (p < badboys.size()) return *badboys[p];
				else {
					throw "MAP::GETBB - CANNOT GET #" + std::to_string(p) + " BADBOY! (out of range)";
					return (Entities::badboy&)plr;
				}
			}
			void map::checkDraw()
			{
				if (!big_map || !big_map_il) {
					_checkBitmapsBigMap();
					return;
				}

				Log::gfile logg;
				bool has_img_reload;
				has_img_reload = big_map_il->isEq(Image::HAS_RELOADED, true);
				if (hasToUpdate || has_img_reload || hasToUpdate_a_block/* || is_new_Map*/) {

					_redraw();
					hasToUpdate = hasToUpdate_a_block = false;

				}
			}
			void map::checkPositionChange()
			{
				if (!this) return; // quick fix

				if (!is_player_enabled || paused) return;

				int posx, posy;

				if (!_getPlayerPos(posx, posy)) {
					has_reachedEnd = false;
					return;
				}

				switch (map_i[posx][posy])
				{
				case THENDID:
					has_reachedEnd = true;
					break;
				case NOEXTID:
					plr->setMyHealth(plr->getMyHealth() - 0.0003);
					if (plr->getMyHealth() < 0.0) {
						game_ended_dead = true;
						plr->setMyHealth(0.0);
					}
					break;
				case STARTID:
					plr->setMyHealth(plr->getMyHealth() + 0.001);
					if (plr->getMyHealth() > 1.0) plr->setMyHealth(1.0);
					break;
				case INVISID:
				{
					map_i[posx][posy] = NOEXTID;
					Safer::safe_string id_path = interpretIntToBlockName(NOEXTID);
					map_p[posx][posy]->replaceAllWith(id_path);
					hasToUpdate_a_block = true;
				}
				break;
				}
			}
			const bool map::isCPUtasking()
			{
				return cpu_tasking;
			}
			void map::setCPULock(const bool b)
			{
				set_cpu_lock = b;
			}
			void map::cpuTask()
			{
				if (!this) return; // quick fix
				cpu_tasking = true;
				testCollisionPlayer();
				checkPositionChange();
				cpu_tasking = false;
			}
			void map::testCollisionPlayer()
			{
				if (!this) return; // quick fix
				if (set_cpu_lock) return;

				Log::gfile logg;

				if (paused) {
					verifyCollision(*plr->getS());
					return;
				}

				if (!is_player_enabled) {
					logg.debug("map::testCollisionPlayer() skipped task: no player found.");
					return;
				}

				if (!try_lock()) {
					logg.debug("map::testCollisionPlayer() skipped task: cannot lock map for task.");
					return;
				}

				Safer::safe_pointer<Sprite::sprite> player = plr->getS();
				if (player) {
					verifyCollision(*player);
					for (auto& e : badboys) {

						e->getS()->_verifyCollision(*player);
						player->_verifyCollision(*e->getS());

						for (auto& r : badboys) {
							if (r != e) {
								e->getS()->_verifyCollision(*r->getS());
							}
						}

						verifyCollision(*e->getS());
					}
				}

				unlock();
			}
			const bool map::hasReachedEnd()
			{
				return has_reachedEnd;
			}
			const bool map::isDead()
			{
				return game_ended_dead;
			}
			const bool map::isMapLoaded()
			{
				return has_loadedMap;
			}
			void map::Pause(const bool b)
			{
				if (b) {
					_savePlayerLastPos();
				}
				else {
					_setPlayerAtLastPos();
				}
				paused = b;
				plr->sleep(b);
				hasToUpdate = true;
			}
			const bool map::isPaused()
			{
				if (!this) return true; // quick fix
				return paused;
			}
			const bool map::try_lock()
			{
				return muu.try_lock();
			}
			void map::lock()
			{
				muu.lock();
			}
			void map::unlock()
			{
				muu.unlock();
			}
			Safer::safe_pointer<Sprite::sprite> map::_getPlayerSprite()
			{
				return plr->getS();
			}
			const Safer::safe_string randomName()
			{
				switch (rand() % 10) {
				case 0:
					return "Jonas";
				case 1:
					return "Brother";
				case 2:
					return "Random";
				case 3:
					return "Flier";
				case 4:
					return "Banana";
				case 5:
					return "Disguised";
				case 6:
					return "Potato";
				case 7:
					return "Flash";
				case 8:
					return "Birdo";
				case 9:
					return "Luiz";
				}

				return "NULL";
			}
		}
	}
}