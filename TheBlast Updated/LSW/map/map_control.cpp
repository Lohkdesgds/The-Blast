
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
					for (int time = 0; time < 1000000 && al_get_time() - start < LOAD_MAX_TIME;)
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
							} while (map_i[posx][posy] != SPACEID && al_get_time() - start < LOAD_MAX_TIME);
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

					while (map_i[posx][posy] != STARTID && al_get_time() - start < LOAD_MAX_TIME)
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

						randomizer(seed*seed * 49);
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

				while (!done && al_get_time() - tima < RESOLVE_MAX_TIME)
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

			map::map()
			{
				for (int a = 0; a < Defaults::map_size_default_x; a++)
				{
					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{
						map_p[a][b] = nullptr;
					}
				}
			}

			map::~map()
			{
				killMap();
			}

			void map::setSeed(const int u)
			{
				Log::gfile logg;
				logg << Log::START << "[MAP:STSEED][INFO] Defined seed for map making as: " << u << Log::ENDL;

				seed = u;
			}
			void map::setLayer(const int u)
			{
				Log::gfile logg;
				logg << Log::START << "[MAP:SLAYER][INFO] Defined layer for further drawing as " << u << Log::ENDL;

				layerUsed = u;
			}
			void map::generateMap()
			{
				Log::gfile logg;
				logg << Log::START << "[MAP:GENMAP][INFO] Generating map..." << Log::ENDL;

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


				d_images_database img_data;
				d_sprite_database spr_data;
				
				for (int b = 0; b < Defaults::map_size_default_y; b++)
				{
					for (int a = 0; a < Defaults::map_size_default_x; a++)
					{
						Sprite::sprite* each = nullptr;
						int ouu = map_i[a][b];
						if (ouu < SPACEID) ouu = SPACEID;
						if (ouu > BLOCKID_C03) ouu = BLOCKID_C03;
						Safer::safe_string id_path = interpretIntToBlockName((blockstats)ouu);

						spr_data.create(each);
						each->setID("_MAP_" + std::to_string(a) + "_" + std::to_string(b));
						each->add(id_path);
						each->set(Sprite::POSX, x_off + (1.0*(2.0*a / (Defaults::map_size_default_x*1.0)) - 1.0));
						each->set(Sprite::POSY, y_off + (1.0*(2.0*b / (Defaults::map_size_default_y*1.0)) - 1.0));
						each->set(Sprite::SCALEX, 2.0 / Defaults::map_size_default_x);
						each->set(Sprite::SCALEY, 2.0 / Defaults::map_size_default_y);
						each->set(Sprite::LAYER, layerUsed);
						//each->set(Sprite::SHOWDOT, true);
						each->set(Sprite::COLLIDE, true);
						each->set(Sprite::DRAW, false);

						map_p[a][b] = each;

						each = nullptr;
					}
				}

				if (!big_map)
				{
					img_data.create(big_map_il);
					big_map_il->create(-1, -1);
					big_map_il->setID("__MAP_FRAME_FULL");

					spr_data.create(big_map);
					big_map->setID("__MAP_FRAME_FULL_S");
					big_map->add("__MAP_FRAME_FULL");
					big_map->set(Sprite::LAYER, layerUsed);
					big_map->set(Sprite::DRAW, true);
					big_map->set(Sprite::AFFECTED_BY_CAM, false);
					big_map->set(Sprite::SCALEG, 2.0);
				}

				hasToUpdate = true;

				/*for (int a = 0; a < Defaults::map_size_default_x; a++)
				{
					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{
						d_sprite_database spr_data;
						Sprite::sprite* spr = nullptr;
						if (spr_data.get(spr, "_MAP_" + std::to_string(a) + "_" + std::to_string(b)))
						{
							spr->_echoPropertiesOnConsole();
						}
						else {
							logg.debug("FAILED GETTING DEBUG INFORMATION ABOUT _MAP_" + std::to_string(a) + "_" + std::to_string(b) + "!");
							al_rest(1.0);
						}
					}
				}*/

				for (int a = 0; a < Defaults::map_size_default_x; a++)
				{
					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{						
						if (map_i[a][b] == STARTID)
						{
							spawn[0] = (1.0*(2.0*a / (Defaults::map_size_default_x*1.0)) - 1.0);
							spawn[1] = (1.0*(2.0*b / (Defaults::map_size_default_y*1.0)) - 1.0);

							if (player) {
								player->set(Sprite::POSX, spawn[0] + x_off);
								player->set(Sprite::POSY, spawn[1] + y_off);
								player->set(Sprite::SPEEDX, 0.0);
								player->set(Sprite::SPEEDY, 0.0);
								player->set(Sprite::SCALEX, 2.0 / Defaults::map_size_default_x);
								player->set(Sprite::SCALEY, 2.0 / Defaults::map_size_default_y);
								player->set(Sprite::SCALEG, Defaults::user_scale_compared_to_map);
							}

							a = Defaults::map_size_default_x;
							b = Defaults::map_size_default_y;
						}
					}
				}

				logg << Log::START << "[MAP:GENMAP][INFO] Generated map." << Log::ENDL;
			}
			void map::killMap()
			{
				Log::gfile logg;
				d_sprite_database spr_data;
				d_images_database img_data;

				logg << Log::START << "[MAP:KILLMP][INFO] Erasing map data..." << Log::ENDL;

				for (int a = 0; a < Defaults::map_size_default_x; a++)
				{
					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{
						spr_data.remove("_MAP_" + std::to_string(a) + "_" + std::to_string(b));
						/*delete map_p[a][b];*/
						map_p[a][b] = nullptr;
						map_i[a][b] = SPACEID;
					}
				}
				if (big_map)
				{
					if (big_map_il) big_map_il->unload();

					spr_data.remove("__MAP_FRAME_FULL_S");
					img_data.remove("__MAP_FRAME_FULL");

					if (big_map_il) delete big_map_il;
					delete big_map;
					big_map = nullptr;
					big_map_il = nullptr;
				}
				logg << Log::START << "[MAP:KILLMP][INFO] Done erasing mapa data." << Log::ENDL;
			}
			void map::corruptWorldTick()
			{
				Log::gfile logg;
				logg << Log::START << "[MAP:CORRPT][INFO] Changing some random blocks..." << Log::ENDL;

				int posx = rand() % (Defaults::map_size_default_x);
				int posy = rand() % (Defaults::map_size_default_y);

				if (map_i[posx][posy] == BLOCKID) {
					map_i[posx][posy] = BLOCKID_C00;

					Safer::safe_string id_path = interpretIntToBlockName(BLOCKID_C00);
					d_sprite_database spr_data;
					Sprite::sprite* spr = nullptr;
					if (spr_data.get(spr, "_MAP_" + std::to_string(posx) + "_" + std::to_string(posy)))
					{
						spr->replaceAllWith(id_path);
					}
					else {
						logg << Log::START << "[MAP:CORRPT][ERRR] Could not find MAP Sprite at " << posx << ":" << posy << "!" << Log::ENDL;
					}
					/*assert(map_p[posx][posy]);
					map_p[posx][posy]->replaceAllWith(id_path);*/
				}

				for (int a = 0; a < Defaults::map_size_default_x; a++)
				{
					for (int b = 0; b < Defaults::map_size_default_y; b++)
					{
						if (map_i[a][b] == BLOCKID_C03) {
							map_i[a][b] = INVISID;

							Safer::safe_string id_path = interpretIntToBlockName(INVISID);

							d_sprite_database spr_data;
							Sprite::sprite* spr = nullptr;
							if (spr_data.get(spr, "_MAP_" + std::to_string(posx) + "_" + std::to_string(posy)))
							{
								spr->replaceAllWith(id_path);
							}
							else {
								logg << Log::START << "[MAP:CORRPT][ERRR] Could not find MAP Sprite at " << posx << ":" << posy << "!" << Log::ENDL;
							}
						}
						else if (map_i[a][b] >= BLOCKID_C00 && map_i[a][b] < BLOCKID_C03) {
							map_i[a][b]++;

							Safer::safe_string id_path = interpretIntToBlockName((blockstats)map_i[a][b]);

							d_sprite_database spr_data;
							Sprite::sprite* spr = nullptr;
							if (spr_data.get(spr, "_MAP_" + std::to_string(posx) + "_" + std::to_string(posy)))
							{
								spr->replaceAllWith(id_path);
							}
							else {
								logg << Log::START << "[MAP:CORRPT][ERRR] Could not find MAP Sprite at " << posx << ":" << posy << "!" << Log::ENDL;
							}
						}
					}
				}
				hasToUpdate = true;

				logg << Log::START << "[MAP:CORRPT][INFO] Done processing blocks." << Log::ENDL;
			}
			void map::setPlayer(Sprite::sprite * s)
			{
				player = s;
			}
			void map::checkDraw()
			{
				if (hasToUpdate || big_map_il->hasReloaded()) {
					hasToUpdate = false;

					Camera::camera_g cam;
					Log::gfile logg;

					ALLEGRO_BITMAP* targ = al_get_target_bitmap();
					if (!targ) {
						logg << Log::ERRDV << Log::START << "[MAP:CHKDRW][WARN] Could not get target bitmap! Skipping map update for now..." << Log::ENDL;
						hasToUpdate = true;
						return;
					}
					else big_map_il->hasReloaded(true);

					big_map->_setAsTarg();

					const int lastapply = cam.getLastApplyID();
					cam.copy(Defaults::default_map_layer_backup, lastapply); // backup

					cam.apply(lastapply);

					for (auto& i : map_p)
					{
						for (auto& j : i)
						{
							j->forceDraw();
						}
					}

					/*cam.copy(lastapply, Defaults::default_layer_backup);
					cam.apply(lastapply);*/ // not needed because transformation is bitmap based.

					al_set_target_bitmap(targ);
					logg.debug("Map has updated its image.");
				}
			}
			void map::testCollisionPlayer()
			{
				if (player) verifyCollision(*player);
			}
			void map::testCollisionOther(Sprite::sprite & s)
			{
				verifyCollision(s);
			}
		}
	}
}