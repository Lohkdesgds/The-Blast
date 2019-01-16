#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Sprite {
			//_sprites_database_data sprites_database::data;

			const _sprite_collision_info sprite::_doesCollideWith(const _sprite_area a, const _sprite_area b) // a should be this, and b the test. so if north, b is north compared to a.
			{
				_sprite_collision_info spi;

				spi.rdx = b.cx - a.cx; // if > 0, b is right, else left
				spi.rdy = b.cy - a.cy; // if > 0, b is down, else up

				spi.dx = fabs(spi.rdx) - (a.sx + b.sx); // distances relative to the collision part  [ block ]<- this distance ->[ block ]
				spi.dy = fabs(spi.rdy) - (a.sy + b.sy); // "

				if (spi.colliding = (spi.dx < 0.0 && spi.dy < 0.0))
				{
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

			sprite::sprite()
			{
				reset_sprite_data(data);
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
					Log::gfile log;
					size_t frame = lastcall_opt;

					if (bmps.getMax() == 0) return false;

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
						return false;
					}
					ALLEGRO_BITMAP* bmp = nullptr; // current bitmap
					bmps.get(frame, Defaults::exp_veryfast)->get(bmp);
					if (!bmp) return false;

					bx = al_get_bitmap_width(bmp);
					by = al_get_bitmap_height(bmp);

					const int lastapply = cam.getLastApplyID();

					if (!data.bval[AFFECTED_BY_CAM])
					{
						cam.copy(Defaults::default_layer_backup, lastapply);
						cam.reset(lastapply);
						cam.apply(lastapply);
					}

					double targ_draw_xy[2];
					targ_draw_xy[0] = data.dval[POSX];
					targ_draw_xy[1] = data.dval[POSY];
					
					double bmp_s_center[2];
					bmp_s_center[0] = al_get_bitmap_width(bmp) * ((data.dval[CENTERX] + 1.0) * 0.5);
					bmp_s_center[1] = al_get_bitmap_height(bmp)* ((data.dval[CENTERY] + 1.0) * 0.5);

					double rotation_rad = data.dval[ROTATION] * ALLEGRO_PI / 180.0;

					double targ_distortion_xy[2];
					targ_distortion_xy[0] = data.dval[SCALEX] * data.dval[SCALEG] * (1.0 / al_get_bitmap_width(bmp));
					targ_distortion_xy[1] = data.dval[SCALEY] * data.dval[SCALEG] * (1.0 / al_get_bitmap_height(bmp));

					if (rotation_rad != 0.0) targ_distortion_xy[0] = targ_distortion_xy[1] = sqrt(targ_distortion_xy[0] * targ_distortion_xy[1]);

					if (data.bval[USE_TINTED_DRAWING]) al_draw_tinted_scaled_rotated_bitmap(bmp, data.tint, bmp_s_center[0], bmp_s_center[1], targ_draw_xy[0], targ_draw_xy[1], targ_distortion_xy[0], targ_distortion_xy[1], rotation_rad, 0);
					else al_draw_scaled_rotated_bitmap(bmp, bmp_s_center[0], bmp_s_center[1], targ_draw_xy[0], targ_draw_xy[1], targ_distortion_xy[0], targ_distortion_xy[1], rotation_rad, 0);

					if (!data.bval[AFFECTED_BY_CAM])
					{
						cam.copy(lastapply, Defaults::default_layer_backup);
						cam.apply(lastapply);
					}

					if (data.bval[SHOWBOX]) {
						if (data.bval[_IS_COLLIDING]) {
							al_draw_rectangle(targ_draw_xy[0] - bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] - bmp_s_center[1] * targ_distortion_xy[1], targ_draw_xy[0] + bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] + bmp_s_center[1] * targ_distortion_xy[1], al_map_rgb(255, 0, 0), 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20));
						}
						else {
							al_draw_rectangle(targ_draw_xy[0] - bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] - bmp_s_center[1] * targ_distortion_xy[1], targ_draw_xy[0] + bmp_s_center[0] * targ_distortion_xy[0], targ_draw_xy[1] + bmp_s_center[1] * targ_distortion_xy[1], al_map_rgb(0, 255, 0), 0.1*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20));
						}
					}

					if (Defaults::debug) {
						al_draw_circle(targ_draw_xy[0], targ_draw_xy[1], fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * 0.10), al_map_rgb(255, 255, 0), 0.3*fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM) * sqrt(cam.get(cam.getLastApplyID(), Camera::ZOOMX)*cam.get(cam.getLastApplyID(), Camera::ZOOMY)) * 0.20));
					}

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
				_verifyCollision(u._getArea());
			}

			void sprite::_verifyCollision(const _sprite_area & mse)
			{
				_sprite_area ths = _getArea();

				if (ths.sx < 0 || mse.sx < 0) return;

				_sprite_collision_info col = _doesCollideWith(ths, mse);
				Layer::layerer lyr;
				Layer::each_layer& e = lyr.getNow();

				if ((data.bval[_IS_COLLIDING] = ((data.bval[_IS_COLLIDING]) ? true : col.colliding)) && data.bval[AFFECTED_BY_COLLISION])
				{
					switch (col.way) {
					case NORTH:
						if (data.dval[SPEEDY] < 0.0) data.dval[SPEEDY] = 0.0;
						data.dval[SPEEDY] += (fabs(col.dy * e.work()[layer].collision_acel_multiplier) + e.work()[layer].min_collision_acel);
						break;
					case SOUTH:
						if (data.dval[SPEEDY] > 0.0) data.dval[SPEEDY] = 0.0;
						data.dval[SPEEDY] -= (fabs(col.dy * e.work()[layer].collision_acel_multiplier) + e.work()[layer].min_collision_acel);
						break;
					case EAST:
						if (data.dval[SPEEDX] > 0.0) data.dval[SPEEDX] = 0.0;
						data.dval[SPEEDX] -= (fabs(col.dx * e.work()[layer].collision_acel_multiplier) + e.work()[layer].min_collision_acel);
						break;
					case WEST:
						if (data.dval[SPEEDX] < 0.0) data.dval[SPEEDX] = 0.0;
						data.dval[SPEEDX] += (fabs(col.dx * e.work()[layer].collision_acel_multiplier) + e.work()[layer].min_collision_acel);
						break;
					}

				}
			}

			void sprite::_updateAcceleration(const double tick) // 1 / fps (expected fps)
			{
				if (/*data.bval[_IS_COLLIDING] && */data.bval[AFFECTED_BY_COLLISION]) {
					Layer::layerer lyr;
					Layer::each_layer& e = lyr.getNow();

					data.dval[POSX] += data.dval[SPEEDX] * tick;
					data.dval[POSY] += data.dval[SPEEDY] * tick;

					data.dval[SPEEDX] *= data.dval[SMOOTHNESS_X] * tick * e.work()[layer].smoothness_keep_sprite_movement;
					data.dval[SPEEDY] *= data.dval[SMOOTHNESS_Y] * tick * e.work()[layer].smoothness_keep_sprite_movement;
				}
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

				u.bval[DRAW] = true;
				u.bval[COLLIDE] = false;
				u.bval[_IS_COLLIDING] = false;
				u.bval[AFFECTED_BY_COLLISION] = false;
				u.bval[AFFECTED_BY_CAM] = true;
				u.bval[LOOPING_IMAGES] = true;
				u.bval[SHOWBOX] = false;
				u.bval[FOLLOWMOUSE] = false;
				u.bval[USE_TINTED_DRAWING] = false;
			}
		}
	}
}
