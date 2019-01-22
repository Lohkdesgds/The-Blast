#include "..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Entities {

			size_t entity::countt = 0;

			entity::~entity()
			{
				reset();
			}
			void entity::reset()
			{
				if (spr) {
					d_sprite_database spr_data;
					Safer::safe_string str;
					spr->getID(str);
					spr_data.remove(str);
					delete spr;
				}
				if (txt) {
					d_texts_database txt_data;
					Safer::safe_string str;
					txt->get(Text::SETID, str);
					txt_data.remove(str);
					delete txt;
				}
				spr = nullptr;
				txt = nullptr;

				name.clear();
				health = 1.0;
			}
			void entity::setMyName(const Safer::safe_string nam)
			{
				name = nam;
			}
			void entity::setMyHealth(const double d)
			{
				health = d;
			}
			const Safer::safe_string entity::getMyName()
			{
				return name;
			}
			const double entity::getMyHealth()
			{
				return health;
			}
			/*void entity::setType(const type t)
			{
				typ = t;
			}*/
			const type entity::getType()
			{
				return typ;
			}
			const bool entity::amI(const Safer::safe_string s)
			{
				return (name == s);
			}

			Sprite::sprite * entity::getS()
			{
				return spr;
			}


			badboy::badboy()
			{
				typ = BADBOY;
			}
			void badboy::setFollowing(Sprite::sprite * s)
			{
				player_to_follow = s;
			}
			void badboy::tick()
			{
				if (player_to_follow && spr)
				{
					double ppx, ppy, bpx, bpy;

					Layer::layerer lyr;
					double acceleration_gravity; // , scale;
					int layer;

					spr->get(Sprite::LAYER, layer);
					//spr->get(Sprite::ACCELERATION_GRAVITY_SCALE, scale);

					acceleration_gravity = lyr.getNow().getR(layer).gravity;

					spr->setScaled(Sprite::SPEEDY, acceleration_gravity, Defaults::entity_default_gravity_speed);



					player_to_follow->get(Sprite::POSX, ppx);
					player_to_follow->get(Sprite::POSY, ppy);

					spr->get(Sprite::POSX, bpx);
					spr->get(Sprite::POSY, bpy);

					if (bpx > ppx) {// has to go left
						spr->setScaled(Sprite::SPEEDX, -Defaults::acceleration_by_bot_normal, Defaults::sprite_bot_default_proportion_speed);
					}
					else {// has to go right
						spr->setScaled(Sprite::SPEEDX, Defaults::acceleration_by_bot_normal, Defaults::sprite_bot_default_proportion_speed);
					}

					if (bpy > ppy) { // has to go up
						spr->setScaled(Sprite::SPEEDY, -Defaults::acceleration_by_bot_normal, Defaults::sprite_bot_default_proportion_speed);
					}
					else { // has to go down
						//spr->setScaled(Sprite::SPEEDY, Defaults::acceleration_by_bot_normal, Defaults::sprite_bot_default_proportion_speed);
					}
				}
			}

			void badboy::load(const Safer::safe_string path, const int layer, const double siz)
			{
				if (spr) return;

				d_sprite_database spr_data;
				d_texts_database txt_data;

				Safer::safe_string temp = "_ENTITY_" + std::to_string(countt++);

				spr_data.create(spr);
				spr->add(path);
				spr->setID(temp);
				spr->set(Sprite::LAYER, layer);
				spr->set(Sprite::SCALEG, siz);
				//spr->set(Sprite::FOLLOWKEYBOARD, true);
				spr->set(Sprite::COLLIDE, true);
				spr->set(Sprite::AFFECTED_BY_COLLISION, true);
				spr->set(Sprite::ENTITY, (void*)this);
				spr->set(Sprite::POSY, 10.0);
				//spr->set(Sprite::AFFECTED_BY_GRAVITY, true);

				txt_data.create(txt);
				txt->set(Text::SETSTRING, "[BOT] %entity_name% [%entity_health%]");
				txt->set(Text::MODE, Text::ALIGN_CENTER);
				txt->set(Text::LAYER, 70);
				txt->set(Text::SETFOLLOW, temp);
				txt->set(Text::UPDATETIME, 0.5);
				txt->set(Text::SETID, temp);
			}


			player::player()
			{
				typ = PLAYER;
				reset_player_data(data);
			}

			void player::tick()
			{
				m.lock();
				if (!spr) {
					m.unlock();
					return;
				}

				Events::big_event bev;

				/* --------------- GRAVITY ---------------*/

				Layer::layerer lyr;
				double acceleration_gravity; // , scale;
				int layer;

				spr->get(Sprite::LAYER, layer);
				//spr->get(Sprite::ACCELERATION_GRAVITY_SCALE, scale);*/

				acceleration_gravity = lyr.getNow().getR(layer).gravity;

				spr->setScaled(Sprite::SPEEDY, acceleration_gravity, data.dval[ACCELERATION_GRAVITY_SCALE]);

				/* --------------- KEYBOARD ---------------*/

				bool wasd[4] = { bev.g().getKey(Events::KEY_W, false) || bev.g().getKey(Events::KEY_UP, false), bev.g().getKey(Events::KEY_A, false) || bev.g().getKey(Events::KEY_LEFT, false), bev.g().getKey(Events::KEY_S, false) || bev.g().getKey(Events::KEY_DOWN, false), bev.g().getKey(Events::KEY_D, false) || bev.g().getKey(Events::KEY_RIGHT, false) };

				if (wasd[0]) { // go north

					/*if (al_get_time() - data.dval[_LAST_JUMP_TIME] > data.dval[JUMP_TIME_DELAY])
					{
						data.dval[_LAST_JUMP_TIME] = al_get_time();
						spr->set(Sprite::SPEEDY, -data.dval[ACCELERATION_BY_KEYING] * data.dval[JUMP_SPEED_PROPORTION]);
					}*/
					spr->setScaled(Sprite::SPEEDY, -data.dval[ACCELERATION_BY_KEYING], data.dval[JUMP_SPEED_PROPORTION]);
				}
				else if (wasd[2]) {
					spr->setScaled(Sprite::SPEEDY, data.dval[ACCELERATION_BY_KEYING], data.dval[ACCELERATION_SCALE]);
				}

				if (wasd[1]) { // go left
					spr->setScaled(Sprite::SPEEDX, -data.dval[ACCELERATION_BY_KEYING], data.dval[ACCELERATION_SCALE]);
				}
				else if (wasd[3]) { // go right
					spr->setScaled(Sprite::SPEEDX, data.dval[ACCELERATION_BY_KEYING], data.dval[ACCELERATION_SCALE]);
				}

				//spr->setScaled(Sprite::SPEEDY, -data.dval[ACCELERATION_BY_KEYING], data.dval[ACCELERATION_SCALE]);

				m.unlock();
			}

			void player::set(const _eplayer_dvals e, const double v)
			{
				if (e != D_EPLAYERS_MAX)
					data.dval[e] = v;
			}

			void player::get(const _eplayer_dvals e, double & v)
			{
				if (e != D_EPLAYERS_MAX)
					v = data.dval[e];
			}
			
			void player::set(const _eplayer_bvals e, const bool v)
			{
				if (e != MOVE_MAX)
					data.bval[e] = v;
			}


			void player::load(const Safer::safe_string path, const int layer, const double siz)
			{
				m.lock();

				if (spr) reset();

				d_sprite_database spr_data;
				d_texts_database txt_data;

				Safer::safe_string temp = "_ENTITY_" + std::to_string(countt++);

				spr_data.create(spr);
				spr->add(path);
				spr->setID(temp);
				spr->set(Sprite::LAYER, layer);
				spr->set(Sprite::SCALEG, siz);
				//spr->set(Sprite::FOLLOWKEYBOARD, true);
				spr->set(Sprite::COLLIDE, true);
				spr->set(Sprite::AFFECTED_BY_COLLISION, true);
				spr->set(Sprite::ENTITY, (void*)this);
				spr->set(Sprite::POSY, 10.0);
				//spr->set(Sprite::AFFECTED_BY_GRAVITY, true);

				txt_data.create(txt);
				txt->set(Text::SETSTRING, "%entity_name% [%entity_health%]");
				txt->set(Text::MODE, Text::ALIGN_CENTER);
				txt->set(Text::LAYER, 70);
				txt->set(Text::SETFOLLOW, temp);
				txt->set(Text::UPDATETIME, 0.5);
				txt->set(Text::SETID, temp);

				m.unlock();
			}



			size_t _find(const Safer::safe_string s, Safer::safe_vector<entity*>& v, bool& u)
			{
				u = false;
				for (size_t p = 0; p < v.getMax(); p++)
				{
					if (v[p]->amI(s)) {
						u = true;
						return p;
					}
				}
				return 0;
			}

			void _draw()
			{
				d_entity_database ent_data;
				//Camera::camera_g cam;


				ent_data.work().lock();
				for (auto& i : ent_data.work().work())
				{
					switch (i->getType())
					{
					case PLAYER:
						((player*)i)->tick();
						break;
					case BADBOY:
						((badboy*)i)->tick();
						break;
					}
				}
				ent_data.work().unlock();
			}


			void reset_player_data(_player_data& u)
			{
				u.dval[ACCELERATION_BY_KEYING] = Defaults::acceleration_by_user_normal;
				u.dval[ACCELERATION_SCALE] = Defaults::entity_default_proportion_speed;
				u.dval[ACCELERATION_GRAVITY_SCALE] = Defaults::entity_default_gravity_speed;
				u.dval[JUMP_SPEED_PROPORTION] = Defaults::entity_default_jump_proportion_speed;
				//u.dval[JUMP_TIME_DELAY] = 0.7;
				//u.dval[_LAST_JUMP_TIME] = 0.0;

				u.bval[MOVE_LEFT] = u.bval[MOVE_RIGHT] = u.bval[JUMP] = false;
			}
		}
	}
}
