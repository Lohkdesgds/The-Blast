#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Sprite {
			//_sprites_database_data sprites_database::data;

			const _sprite_collision_info sprite::_doesCollideWith(const _sprite_area a, const _sprite_area b) // a should be this, and b the test. so if north, b is north compared to a.
			{
				_sprite_collision_info spi;
				Log::gfile logg;

				
				spi.rdx = b.cx - a.cx; // if > 0, b is right, else left
				spi.rdy = b.cy - a.cy; // if > 0, b is down, else up

				spi.dx = fabs(spi.rdx) - (a.sx + b.sx); // distances relative to the collision part  [ block ]<- this distance ->[ block ]
				spi.dy = fabs(spi.rdy) - (a.sy + b.sy); // "

				if (spi.colliding = (spi.dx < 0.0 && spi.dy < 0.0))
				{
					/*if (a.debug_name && b.debug_name)
					{
						logg.debug("COLLIDED: " + a.debug_name->g() + " with " + b.debug_name->g());
					}*/

					if (spi.dx > spi.dy) // x is nearer then y
					{
						if (spi.rdx > 0) { // rdx shows center distance for precised positioning
							spi.way = EAST;
						}
						else {
							spi.way = WEST;
						}
					}
					else {
						if (spi.rdy > 0) {
							spi.way = SOUTH;
						}
						else {
							spi.way = NORTH;
						}
					}
				}
				return spi;
			}
/*#undef max()
#undef min()
			const _sprite_collision_info sprite::_doTheyIntersect(const _sprite_area aa, const _sprite_area bb)
			{				
				QPolygonF a;
				QPolygonF b;

				a.push_back(clssss(aa.cx - aa.sx, aa.cy - aa.sy));
				a.push_back(clssss(aa.cx - aa.sx, aa.cy + aa.sy));
				a.push_back(clssss(aa.cx + aa.sx, aa.cy - aa.sy));
				a.push_back(clssss(aa.cx + aa.sx, aa.cy + aa.sy));


				for (int polyi = 0; polyi < 2; ++polyi)
				{
					QPolygonF& polygon = ((polyi == 0) ? a : b);

					for (int i1 = 0; i1 < polygon.size(); ++i1)
					{
						const int i2 = (i1 + 1) % polygon.size();

						const double normalx = polygon[i2].y - polygon[i1].y;
						const double normaly = polygon[i2].x - polygon[i1].x;

						double minA = std::numeric_limits<double>::max();
						double maxA = std::numeric_limits<double>::min();
						for (int ai = 0; ai < a.size(); ++ai)
						{
							double projected = normalx * a[ai].x +
								normaly * a[ai].y;
							if (projected < minA) minA = projected;
							if (projected > maxA) maxA = projected;
						}

						double minB = std::numeric_limits<double>::max();
						double maxB = std::numeric_limits<double>::min();
						for (int bi = 0; bi < b.size(); ++bi)
						{
							const double projected = normalx * b[bi].x +
								normaly * b[bi].y;
							if (projected < minB) minB = projected;
							if (projected > maxB) maxB = projected;
						}

						if (maxA < minB || maxB < minA)
							return false;
					}
				}

				return true;
			}*/

			sprite::sprite()
			{
				reset_sprite_data(data);
				Log::gfile logg;
				logg.debug(" -> CREATED NEW SPRITE ID#" + std::to_string((int)this) + " <- ");
			}

			sprite::~sprite()
			{
				Log::gfile logg;
				logg.debug(" -> DELETED SPRITE ID#" + std::to_string((int)this) + " <- ");
			}

			void sprite::add(Safer::safe_string s, const size_t amount) // search in database
			{
				Image::image_low* img = nullptr;
				//				Image::images_database dbs;

				d_images_database dbs;

				if (amount > 1)
				{
					s += "%lu";

					for (size_t p = 0; p < amount; p++)
					{
						char endval[128];
						sprintf_s(endval, s.g().c_str(), p);

						if (dbs.get(img, endval))
						{
							if (!img) {
								throw "FAILED TO FIND A LOW_IMAGE";
								assert(img);
								exit(-1);
							}
							bmps.push(img);
							img = nullptr;
						}
					}
				}
				else {
					if (dbs.get(img, s))
					{
						if (!img) {
							throw "FAILED TO FIND A LOW_IMAGE";
							assert(img);
							exit(-1);
						}
						bmps.push(img);
					}
				}
			}
			void sprite::replaceAllWith(Safer::safe_string s, const size_t i)
			{
				replacing.lock();

				bmps.clear();
				add(s, i);

				replacing.unlock();
			}
			void sprite::remove(const Safer::safe_string s) // ids from images
			{
				for (size_t p = 0; p < bmps.getMax(); p++)
				{
					if (bmps[p]->amI(s)) {
						bmps.erase(p);
						break;
					}
				}
			}

			void sprite::setID(const Safer::safe_string i) // set sprite id
			{
				id_g = i;
			}
			void sprite::getID(Safer::safe_string & u)
			{
				u = id_g;
			}
			const bool sprite::amI(const Safer::safe_string i)
			{
				return (id_g == i);
			}

			void sprite::set(const _sprite_dvals e, const double v)
			{
				data.dval[e] = v;

				if (data.dval[ANIMATIONTIME] <= 0.0)
				{
					lastcall_opt = (int)(-data.dval[ANIMATIONTIME]);
					if (lastcall_opt > bmps.getMax()) lastcall_opt = bmps.getMax() - 1;
					lastcall = al_get_time();
					got_to_end_one_time_at_least = false;
				}
			}
			void sprite::set(const _sprite_bvals e, const bool v)
			{
				data.bval[e] = v;
			}
			void sprite::set(const _sprite_ivals e, const int v)
			{
				switch (e) {
				case LAYER:
					layer = v;
					break;
				}
			}
			void sprite::set(const _sprite_cvals e, const ALLEGRO_COLOR v)
			{
				switch (e) {
				case TINT:
					data.tint = v;
					break;
				}
			}
			void sprite::get(const _sprite_dvals e, double& v)
			{
				v = data.dval[e];
			}
			void sprite::get(const _sprite_bvals e, bool& v)
			{
				v = data.bval[e];
			}

			void sprite::get(const _sprite_ivals e, int & v)
			{
				switch (e) {
				case LAYER:
					v = layer;
					break;
				}
			}

			void sprite::get(const _sprite_zvals e, size_t & v)
			{
				switch (e) {
				case SIZE:
					v = bmps.getMax();
					break;
				case FRAME:
					v = lastcall_opt;
					break;
				}
			}

			void sprite::add(const _sprite_dvals e, const double v)
			{
				data.dval[e] += v;
			}
			void sprite::multiply(const _sprite_dvals e, const double v)
			{
				data.dval[e] *= v;
			}
			/*void sprite::setTint(const ALLEGRO_COLOR u)
			{
				data.tint = u;
			}*/
			void sprite::forceDraw()
			{
				bool b = data.bval[DRAW];
				data.bval[DRAW] = true;
				drawIfLayer(layer);
				data.bval[DRAW] = b;
			}
			const bool sprite::drawIfLayer(const int& u)
			{
				/*if (al_get_time() - lastverify > Defaults::timeout_image_unload)
				{
					lastverify = al_get_time();
					//for (size_t p = 0; p < bmps.getMax(); p++) bmps[p]->verify();
				}*/

				if (!data.bval[DRAW]) {
					lastcall = al_get_time();
					return false;
				}

				if (data.bval[FOLLOWMOUSE])
				{
					Events::big_event bev;
					float x, y;

					bev.g().getMouse(x, Events::X);
					bev.g().getMouse(y, Events::Y);
					data.dval[POSX] = x;
					data.dval[POSY] = y;
				}

				if (u == layer)
				{

					/// draw
					/*
					get display by al_get_target_bitmap (probably it will be the buffer, so continue
					*/
					Camera::camera_g cam;
					size_t frame = lastcall_opt;

					replacing.lock();

					if (bmps.getMax() == 0) {
						replacing.unlock();
						return false;
					}

					if (lastcall == 0) lastcall = al_get_time();

					if (data.dval[ANIMATIONTIME] > 0.0)
					{
						while (al_get_time() - lastcall > 1.0 / data.dval[ANIMATIONTIME]) {
							lastcall += /*al_get_time();*/ 1.0 / data.dval[ANIMATIONTIME];
							lastcall_opt++;
							if (lastcall_opt >= bmps.getMax()) {
								got_to_end_one_time_at_least = true;
								if (data.bval[LOOPING_IMAGES]) lastcall_opt = 0;
								else lastcall_opt--;
							}
						}
						frame = lastcall_opt;
					}

					ALLEGRO_BITMAP* trg = al_get_target_bitmap();
					if (!trg) {
						replacing.unlock();
						return false;
					}
					ALLEGRO_BITMAP* bmp = nullptr; // current bitmap
					bmps.get(frame, Defaults::exp_veryfast)->get(bmp);
					if (!bmp) {
						replacing.unlock();
						return false;
					}

					bx = al_get_bitmap_width(bmp);
					by = al_get_bitmap_height(bmp);

					const int lastapply = cam.getLastApplyID();
					cam.copy(Defaults::default_layer_backup, lastapply);

					if (!data.bval[AFFECTED_BY_CAM]) cam.reset(lastapply);


					double rotation_rad = data.dval[ROTATION] * ALLEGRO_PI / 180.0;

					double targ_draw_xy[2];
					targ_draw_xy[0] = data.dval[POSX] * cos(rotation_rad) + data.dval[POSY] * sin(rotation_rad);
					targ_draw_xy[1] = - data.dval[POSX] * sin(rotation_rad) + data.dval[POSY] * cos(rotation_rad);
					
					double bmp_s_center[2];
					bmp_s_center[0] = al_get_bitmap_width(bmp) * ((data.dval[CENTERX] + 1.0) * 0.5);
					bmp_s_center[1] = al_get_bitmap_height(bmp)* ((data.dval[CENTERY] + 1.0) * 0.5);

					double targ_distortion_xy[2];
					targ_distortion_xy[0] = data.dval[SCALEX] * data.dval[SCALEG] * (1.0 / al_get_bitmap_width(bmp));
					targ_distortion_xy[1] = data.dval[SCALEY] * data.dval[SCALEG] * (1.0 / al_get_bitmap_height(bmp));

					//if (rotation_rad != 0.0) targ_distortion_xy[0] = targ_distortion_xy[1] = sqrt(targ_distortion_xy[0] * targ_distortion_xy[1]);

					cam.set(lastapply, Camera::ROTATION, cam.get(lastapply, Camera::ROTATION) + rotation_rad);

					cam.apply(lastapply);


					if (data.bval[USE_TINTED_DRAWING]) al_draw_tinted_scaled_rotated_bitmap(bmp, data.tint, bmp_s_center[0], bmp_s_center[1], targ_draw_xy[0], targ_draw_xy[1], targ_distortion_xy[0], targ_distortion_xy[1], rotation_rad, 0);
					else al_draw_scaled_rotated_bitmap(bmp, bmp_s_center[0], bmp_s_center[1], targ_draw_xy[0], targ_draw_xy[1], targ_distortion_xy[0], targ_distortion_xy[1], 0.0 /*rotation_rad*/, 0);

					replacing.unlock();

					if (data.bval[SHOWBOX] || data.bval[SHOWDOT]) {

						ALLEGRO_COLOR colorr = al_map_rgb(0, 255, 0);

						if (data.bval[FOLLOWKEYBOARD]) colorr = al_map_rgb(255, 255, 255);
						else if (data.bval[_IS_COLLIDING]) colorr = al_map_rgb(255, 0, 0);
						else if (al_get_time() - lastresetcollisioncall < Defaults::diff_time_show_last_resetCollision) colorr = al_map_rgb(255, 255, 0);
						

						//if (data.bval[_IS_COLLIDING]) {
						if (data.bval[SHOWBOX]) {
							al_draw_filled_circle(targ_draw_xy[0] - bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] - bmp_s_center[1] * targ_distortion_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20), colorr);
							al_draw_filled_circle(targ_draw_xy[0] - bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] + bmp_s_center[1] * targ_distortion_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20), colorr);
							al_draw_filled_circle(targ_draw_xy[0] + bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] - bmp_s_center[1] * targ_distortion_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20), colorr);
							al_draw_filled_circle(targ_draw_xy[0] + bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] + bmp_s_center[1] * targ_distortion_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20), colorr);
						}

						if (data.bval[SHOWDOT])
						{
							al_draw_filled_circle(targ_draw_xy[0], targ_draw_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20) * (data.bval[FOLLOWKEYBOARD] ? 1.2 : 1.0), colorr);
						}


							
							//al_draw_rectangle(targ_draw_xy[0] - bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] - bmp_s_center[1] * targ_distortion_xy[1], targ_draw_xy[0] + bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] + bmp_s_center[1] * targ_distortion_xy[1], al_map_rgb(255, 0, 0), 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20));
						/*}
						else {
							al_draw_filled_circle(targ_draw_xy[0] - bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] - bmp_s_center[1] * targ_distortion_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20), al_map_rgb(0, 255, 0));
							al_draw_filled_circle(targ_draw_xy[0] - bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] + bmp_s_center[1] * targ_distortion_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20), al_map_rgb(0, 255, 0));
							al_draw_filled_circle(targ_draw_xy[0] + bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] - bmp_s_center[1] * targ_distortion_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20), al_map_rgb(0, 255, 0));
							al_draw_filled_circle(targ_draw_xy[0] + bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] + bmp_s_center[1] * targ_distortion_xy[1], 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20), al_map_rgb(0, 255, 0));

							//al_draw_rectangle(targ_draw_xy[0] - bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] - bmp_s_center[1] * targ_distortion_xy[1], targ_draw_xy[0] + bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] + bmp_s_center[1] * targ_distortion_xy[1], al_map_rgb(0, 255, 0), 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20));
						}*/
					}

					if (Defaults::debug) {
						al_draw_circle(targ_draw_xy[0], targ_draw_xy[1], fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * 0.10), al_map_rgb(255, 255, 0), 0.3*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20));
					}

					cam.copy(lastapply, Defaults::default_layer_backup);
					cam.apply(lastapply);

					return true;
				}
				return false;
			}

			const bool sprite::checkExistance()
			{
				return (this);
			}

			const size_t sprite::_lastFrameNumber()
			{
				return lastcall_opt;
			}
			const bool sprite::gotToEnd()
			{
				bool r = got_to_end_one_time_at_least;
				got_to_end_one_time_at_least = false;
				return r;
			}


			_sprite_area sprite::_getArea()
			{
				_sprite_area ar;
				if (!data.bval[COLLIDE])
				{
					ar.sx = ar.sy = -1;
					return ar;
				}

				ar.cx = data.dval[POSX];
				ar.cy = data.dval[POSY];
				//ar.rot = data.dval[ROTATION] * ALLEGRO_PI / 180.0; // later
				//ar.debug_name = &id_g;  // later

				ar.sx = 0.5 * (data.dval[SCALEX] * data.dval[SCALEG]);
				ar.sy = 0.5 * (data.dval[SCALEY] * data.dval[SCALEG]);

				return ar;
			}

			void sprite::_setAsTarg(const size_t a)
			{
				ALLEGRO_BITMAP* bmp = nullptr;
				bmps.get(a, Defaults::exp_veryfast)->get(bmp);
				assert(bmp);

				al_set_target_bitmap(bmp);
			}

			void sprite::_resetCollision()
			{
				data.bval[_IS_COLLIDING] = false;
				lastresetcollisioncall = al_get_time();
			}

			void sprite::_verifyCollision(const double x, const double y)
			{
				if (!data.bval[COLLIDE]) return;

				//_sprite_area ths = _getArea();
				_sprite_area mse;

				mse.cx = x;
				mse.cy = y;
				mse.sx = mse.sy = Defaults::mouse_size_fixed_val;

				_verifyCollision(mse);
			}

			void sprite::_verifyCollision(Sprite::sprite& u)
			{
				if (!data.bval[COLLIDE]) return;
				bool bb, b2;
				_verifyCollision(u._getArea(), &bb);
				u.get(Sprite::_IS_COLLIDING, b2);
				u.set(Sprite::_IS_COLLIDING, b2 ? b2 : bb);
			}

			void sprite::_verifyCollision(const _sprite_area & mse, bool* saveit)
			{
				_sprite_area ths = _getArea();

				if (ths.sx < 0 || mse.sx < 0) return;

				_sprite_collision_info col = _doesCollideWith(ths, mse);
				if (saveit) *saveit = col.colliding;
				Layer::layerer lyr;
				Layer::each_layer& e = lyr.getNow();

				if ((data.bval[_IS_COLLIDING] = ((data.bval[_IS_COLLIDING]) ? true : col.colliding)))
				{
					if (data.bval[AFFECTED_BY_COLLISION]) {
						switch (col.way) { // col veio de
						case NORTH:
							if (data.dval[SPEEDY] < 0.0) data.dval[SPEEDY] = 0.0;
							data.dval[SPEEDY] += (fabs(col.dy * e.work()[layer].collision_acel_multiplier) + e.work()[layer].min_collision_acel);
							data.dval[POSY] += fabs(col.dy);
							break;
						case SOUTH:
							if (data.dval[SPEEDY] > 0.0) data.dval[SPEEDY] = 0.0;
							data.dval[SPEEDY] -= (fabs(col.dy * e.work()[layer].collision_acel_multiplier) + e.work()[layer].min_collision_acel);
							data.dval[POSY] -= fabs(col.dy);
							break;
						case EAST:
							if (data.dval[SPEEDX] > 0.0) data.dval[SPEEDX] = 0.0;
							data.dval[SPEEDX] -= (fabs(col.dx * e.work()[layer].collision_acel_multiplier) + e.work()[layer].min_collision_acel);
							data.dval[POSX] -= fabs(col.dx);
							break;
						case WEST:
							if (data.dval[SPEEDX] < 0.0) data.dval[SPEEDX] = 0.0;
							data.dval[SPEEDX] += (fabs(col.dx * e.work()[layer].collision_acel_multiplier) + e.work()[layer].min_collision_acel);
							data.dval[POSX] += fabs(col.dx);
							break;
						}
					}
					/*if (data.bval[DOES_COLLISION_ROTATE_OBJECT])
					{
						switch (col.way) {
						case NORTH:
							data.dval[SPEEDROT] += 0.01;//0.1 * col.rdx;
							break;
						case SOUTH:
							data.dval[SPEEDROT] -= 0.01;//0.1 * col.rdx;
							break;
						case EAST:
							data.dval[SPEEDROT] += 0.01;//0.1 * col.rdy;
							break;
						case WEST:
							data.dval[SPEEDROT] -= 0.01;//0.1 * col.rdy;
							break;
						}
					}*/ // later
				}
			}

			void sprite::_updateAcceleration(const double tick) // 1 / fps (expected fps)
			{
				//if (/*data.bval[_IS_COLLIDING] && */data.bval[AFFECTED_BY_COLLISION]) {
					Layer::layerer lyr;
					Layer::each_layer& e = lyr.getNow();

					data.dval[POSX] += data.dval[SPEEDX] * tick;
					data.dval[POSY] += data.dval[SPEEDY] * tick;

					data.dval[SPEEDX] *= data.dval[SMOOTHNESS_X] * tick * e.work()[layer].smoothness_keep_sprite_movement;
					data.dval[SPEEDY] *= data.dval[SMOOTHNESS_Y] * tick * e.work()[layer].smoothness_keep_sprite_movement;
				//}
				/*if (data.bval[DOES_COLLISION_ROTATE_OBJECT]){
					Layer::layerer lyr;
					Layer::each_layer& e = lyr.getNow();

					data.dval[ROTATION] += data.dval[SPEEDROT];

					data.dval[SPEEDROT] *= data.dval[SMOOTHNESS_ROT] * tick * e.work()[layer].smoothness_keep_sprite_movement_rotation;


					while (data.dval[ROTATION] > 360.0) data.dval[ROTATION] -= 360.0;
					while (data.dval[ROTATION] < 0.0) data.dval[ROTATION] += 360.0;
				}*/ // later
			}

			void sprite::_echoPropertiesOnConsole()
			{
				logg.debug("# ------------------------------------------");
				logg.debug("# Debug informations about Sprite ID#" + std::to_string((int)this) + ";NAME=" + id_g.g());
				logg.debug("# POSX:" + std::to_string(data.dval[POSX]));
				logg.debug("# POSY:" + std::to_string(data.dval[POSY]));
				logg.debug("# ROTT:" + std::to_string(data.dval[ROTATION]));
				logg.debug("# SPEX:" + std::to_string(data.dval[SPEEDX]));
				logg.debug("# SPEY:" + std::to_string(data.dval[SPEEDY]));
				logg.debug("# SCLX:" + std::to_string(data.dval[SCALEX]));
				logg.debug("# SCLY:" + std::to_string(data.dval[SCALEY]));
				logg.debug("# SCLG:" + std::to_string(data.dval[SCALEG]));
				logg.debug("# BMPS:");
				bmps.lock();
				size_t p = 0;
				for (auto& i : bmps.work()) {
					if (i->checkExistance()) {
						logg.debug("#" + std::to_string(p) + " > ID: " + i->whoAmI().g());
						logg.debug("#" + std::to_string(p) + " > ISLOADED: " + ((i->isLoaded()) ? "Y" : "N"));
					}
					else {
						logg.debug("#" + std::to_string(p) + " > FAILED TO CHECK EXISTANCE! SKIPPING...");
					}
					p++;
				}
				bmps.unlock();
				logg.debug("# ------------------------------------------");
			}

			const bool sprite::_quickIsDelayed()
			{
				return ((delayed_collision_times > 0) ? (delayed_collision_times--) : false);
			}

			const bool sprite::_quickIsCollisionPossible(Sprite::sprite & u)
			{
				if (!data.bval[COLLIDE]) {
					delayed_collision_times = Defaults::max_delayed_times;
					return false;
				}
				bool ub;
				u.get(Sprite::COLLIDE, ub);
				if (!ub) {;
					return false;
				}
				
				double ux, uy, usx, usy, us;

				u.get(Sprite::POSX, ux);
				u.get(Sprite::POSY, uy);
				u.get(Sprite::SCALEX, usx);
				u.get(Sprite::SCALEY, usy);
				u.get(Sprite::SCALEG, us);

				usx *= us;
				usy *= us;

				double diffx = (fabs(data.dval[POSX] - ux) - 1.1*(usx + (data.dval[SCALEG] * data.dval[SCALEX])));
				double diffy = (fabs(data.dval[POSY] - ux) - 1.1*(usx + (data.dval[SCALEG] * data.dval[SCALEY])));

				bool red = ((diffx < 0) && (diffy < 0));
				if (!red) {
					delayed_collision_times = (int)fabs(10.0*sqrt(diffx*diffy));
				}
				return red;
			}

			const bool sprite::_quickLayerAmIOn(const int u)
			{
				return (u == layer);
			}

			size_t _find(const Safer::safe_string s, Safer::safe_vector<sprite*>& v, bool& u)
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
				d_sprite_database spr_data;
				Camera::camera_g cam;

				/*int l0 = cam.get(Camera::LAYERLOWER);
				int l1 = cam.get(Camera::LAYERHIGHER);*/
				
				for (auto& p : cam.getLayers(cam.getLastApplyID())) {
					if (p.second) {
						for (int u = 0; u < (int)spr_data.work().getMax(); u++)
						{
							spr_data.work().get(u)->drawIfLayer(p.first);
						}
					}
				}
			}

			void reset_sprite_data(_sprite_data& u)
			{
				u.dval[POSX] = 0.0;
				u.dval[POSY] = 0.0;
				u.dval[SCALEX] = 1.0;
				u.dval[SCALEY] = 1.0;
				u.dval[CENTERX] = 0.0;
				u.dval[CENTERY] = 0.0;
				u.dval[SCALEG] = 1.0;
				u.dval[ROTATION] = 0.0;
				u.dval[ANIMATIONTIME] = 0.0;

				u.dval[SPEEDX] = 0.0;
				u.dval[SPEEDY] = 0.0;
				u.dval[SMOOTHNESS_X] = Defaults::smoothness_keep_sprite_movement;
				u.dval[SMOOTHNESS_Y] = Defaults::smoothness_keep_sprite_movement;
				//u.dval[SMOOTHNESS_ROT] = Defaults::smoothness_keep_sprite_movement_rotational; // later

				u.dval[ACCELERATION_BY_KEYING] = Defaults::acceleration_by_user_normal;

				u.bval[DRAW] = true;
				u.bval[COLLIDE] = false;
				u.bval[_IS_COLLIDING] = false;
				u.bval[AFFECTED_BY_COLLISION] = false;
				//u.bval[DOES_COLLISION_ROTATE_OBJECT] = false; // later
				u.bval[AFFECTED_BY_CAM] = true;
				u.bval[LOOPING_IMAGES] = true;
				u.bval[SHOWBOX] = false;
				u.bval[SHOWDOT] = false;
				u.bval[FOLLOWMOUSE] = false;
				u.bval[FOLLOWKEYBOARD] = false;
				u.bval[USE_TINTED_DRAWING] = false;
			}
		}
	}
}
