#include "drawing.h"


namespace LSW {
	namespace v4 {

		std::map<int, camera_preset> Camera::presets;
		int Camera::lastapply = 0;

		namespace Assistance {

			auto __sprite_smart_images::get()
			{
				auto now = al_get_time();
				auto siz = copies.size();

				if (lastcall == 0 || difftimeanim <= 0) lastcall = now;

				if (difftimeanim > 0) {
					while ((now - lastcall) > difftimeanim)
					{
						if (++actual == siz) actual = 0;
						lastcall += difftimeanim;
					}
				}
				return copies[actual].lock();
			}
			void __sprite_smart_images::add(const std::string id)
			{
				Textures imgs;
				std::weak_ptr<__raw_image> wp;

				imgs.get(id, wp);
				copies.push_back(wp);
			}
			void __sprite_smart_images::remove(const std::string id)
			{
				for (size_t p = 0; p < copies.size(); p++) {
					auto strongcpy = copies[p].lock();
					if (strongcpy->id == id) {
						copies.erase(copies.begin() + p);
						return;
					}
				}
			}
			void __sprite_smart_images::setFPS(const double v)
			{
				if (v < 0) {
					difftimeanim = 0;
					actual = -(int)v;
				}
				else if (v == 0) {
					difftimeanim = 0;
				}
				else {
					difftimeanim = 1.0 / v;
				}
			}
			void __sprite_smart_images::loop(const bool b)
			{
				loopin = b;
			}
			void __sprite_smart_images::reset()
			{
				clear();
				loop(true);
				setFPS(60);
			}
			void __sprite_smart_images::clear()
			{
				copies.clear();
			}
			void __sprite_smart_images::check()
			{
				for (size_t p = 0; p < copies.size(); p++) {
					if (copies[p].expired()) {
						copies.erase(copies.begin() + (p--));
					}
				}
			}

			__sprite_smart_data::__sprite_smart_data()
			{
				/*for (auto& i : dval) i = 0.0;
				for (auto& j : bval) j = false;*/
				dval[+Assistance::_sprite_opt_dval::SCALEX] = 1.0;
				dval[+Assistance::_sprite_opt_dval::SCALEY] = 1.0;
				dval[+Assistance::_sprite_opt_dval::SCALEG] = 1.0;
				bval[+Assistance::_sprite_opt_bval::DRAW] = true;
				bval[+Assistance::_sprite_opt_bval::AFFECTED_BY_CAM] = true;
			}

		}//endof Assistance


		void camera_preset::set(const Assistance::_cam_opt u, const float v)
		{
			p[+u] = v;
		}

		float camera_preset::get(const Assistance::_cam_opt u)
		{
			return p[+u];
		}

		void camera_preset::setLayer(const int u, const bool enabl)
		{
			if (enabl) {
				for (auto& i : layers) {
					if (i == u) return;
				}
				layers.push_back(u);
			}
			else {
				for (size_t p = 0; p < layers.size(); p++)
				{
					if (layers[p] == u) {
						layers.erase(layers.begin() + p);
						return;
					}
				}
			}
		}


		void Camera::reset()
		{
			presets.clear();
			get();
		}

		void Camera::set(const camera_preset& p, const int i)
		{
			presets[i] = p;
		}

		void Camera::apply(const int a)
		{
			lastapply = a;
			camera_preset lastTransf = presets[a];
			ALLEGRO_DISPLAY* d = al_get_current_display();

			if (!d) throw Abort::abort("al_get_current_display", "transformator::apply", "Display not found to apply transformation");

			al_identity_transform(&g_t);

			al_rotate_transform(&g_t,
				lastTransf.get(Assistance::_cam_opt::ROTATION));

			al_scale_transform(&g_t,
				al_get_display_width(d) * 0.5 * lastTransf.get(Assistance::_cam_opt::SCALE_X) * lastTransf.get(Assistance::_cam_opt::SCALE_G),
				al_get_display_height(d) * 0.5 * lastTransf.get(Assistance::_cam_opt::SCALE_Y) * lastTransf.get(Assistance::_cam_opt::SCALE_G)); // SCALING NOT AS EXPECTED (multiplier by zoom is not being made by transformation!)

			al_translate_transform(&g_t,
				al_get_display_width(d) * 0.5 - lastTransf.get(Assistance::_cam_opt::OFFSET_X) * (al_get_display_width(d) * 0.5 * lastTransf.get(Assistance::_cam_opt::SCALE_X) * lastTransf.get(Assistance::_cam_opt::SCALE_G)),
				al_get_display_height(d) * 0.5 - lastTransf.get(Assistance::_cam_opt::OFFSET_Y) * (al_get_display_height(d) * 0.5 * lastTransf.get(Assistance::_cam_opt::SCALE_Y) * lastTransf.get(Assistance::_cam_opt::SCALE_G)));

			al_use_transform(&g_t);
		}

		void Camera::apply()
		{
			apply(lastapply);
		}

		camera_preset& Camera::get(const int u)
		{
			return this->presets[u];
		}

		camera_preset& Camera::get()
		{
			return get(lastapply);
		}

		
		Sprite::~Sprite()
		{
			bmps.reset();
		}

		void Sprite::apply(const Assistance::_sprite_opt_strg u, const std::string v)
		{
			switch (u) {
			case Assistance::_sprite_opt_strg::ADD:
				bmps.add(v);
				break;
			case Assistance::_sprite_opt_strg::REMOVE:
				bmps.remove(v);
				break;
			}
		}
		void Sprite::apply(const Assistance::_sprite_opt_fltv u, const float v)
		{
			switch (u) {
			case Assistance::_sprite_opt_fltv::FPS:
				bmps.setFPS(v);
				break;
			}
		}
		void Sprite::apply(const Assistance::_sprite_opt_bool u, const bool v)
		{
			switch (u) {
			case Assistance::_sprite_opt_bool::LOOP:
				bmps.loop(v);
				break;
			}
		}
		void Sprite::apply(const Assistance::_sprite_opt_dval u, const double v)
		{
			data.dval[+u] = v;
		}
		void Sprite::apply(const Assistance::_sprite_opt_bval u, const bool v)
		{
			data.bval[+u] = v;
		}
		void Sprite::apply(const Assistance::_sprite_opt_ival u, const int v)
		{
			switch (u) {
			case Assistance::_sprite_opt_ival::LAYER:
				layer = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::_sprite_opt_zval u, const size_t v)
		{
			switch (u) {
			case Assistance::_sprite_opt_zval::SIZE:
				// nothing to do
				break;
			case Assistance::_sprite_opt_zval::FRAME:
				bmps.setFPS(-(int)(v));
				break;
			}
		}
		void Sprite::apply(const Assistance::_sprite_opt_cval u, const ALLEGRO_COLOR v)
		{
			switch (u) {
			case Assistance::_sprite_opt_cval::TINT:
				data.tint = v;
				break;
			}
		}

		void Sprite::draw()
		{
			auto rn = bmps.get();

			// vars

			/*
			double targ_draw_xy[2];
			targ_draw_xy[0] = data.dval[POSX] * cos(rotation_rad) + data.dval[POSY] * sin(rotation_rad);
			targ_draw_xy[1] = -data.dval[POSX] * sin(rotation_rad) + data.dval[POSY] * cos(rotation_rad);

			double bmp_s_center[2];
			bmp_s_center[0] = al_get_bitmap_width(bmp) * ((data.dval[CENTERX] + 1.0) * 0.5);
			bmp_s_center[1] = al_get_bitmap_height(bmp) * ((data.dval[CENTERY] + 1.0) * 0.5);

			double targ_distortion_xy[2];
			targ_distortion_xy[0] = data.dval[SCALEX] * data.dval[SCALEG] * (1.0 / al_get_bitmap_width(bmp));
			targ_distortion_xy[1] = data.dval[SCALEY] * data.dval[SCALEG] * (1.0 / al_get_bitmap_height(bmp));

			//if (rotation_rad != 0.0) targ_distortion_xy[0] = targ_distortion_xy[1] = sqrt(targ_distortion_xy[0] * targ_distortion_xy[1]);

			cam.set(lastapply, Camera::ROTATION, cam.get(lastapply, Camera::ROTATION) + rotation_rad);

			cam.apply(lastapply);


			if (data.bval[USE_TINTED_DRAWING]) al_draw_tinted_scaled_rotated_bitmap(bmp, data.tint, bmp_s_center[0], bmp_s_center[1], targ_draw_xy[0], targ_draw_xy[1], targ_distortion_xy[0], targ_distortion_xy[1], rotation_rad, 0);
			else al_draw_scaled_rotated_bitmap(bmp, bmp_s_center[0], bmp_s_center[1], targ_draw_xy[0], targ_draw_xy[1], targ_distortion_xy[0], targ_distortion_xy[1], 0.0, 0);*/

			
			float cx, cy, px, py, dsx, dsy, rot_rad;
			cx =		1.0f * al_get_bitmap_width(rn->bmp) * ((data.dval[+Assistance::_sprite_opt_dval::CENTERX] + 1.0) * 0.5);
			cy =		1.0f * al_get_bitmap_height(rn->bmp) * ((data.dval[+Assistance::_sprite_opt_dval::CENTERY] + 1.0) * 0.5);
			rot_rad =	1.0f * data.dval[+Assistance::_sprite_opt_dval::ROTATION] * ALLEGRO_PI / 180.0;
			px =		1.0f * data.dval[+Assistance::_sprite_opt_dval::POSX] * cos(rot_rad) + data.dval[+Assistance::_sprite_opt_dval::POSY] * sin(rot_rad);
			py =		1.0f * data.dval[+Assistance::_sprite_opt_dval::POSY] * cos(rot_rad) - data.dval[+Assistance::_sprite_opt_dval::POSX] * sin(rot_rad);
			dsx =		1.0f * data.dval[+Assistance::_sprite_opt_dval::SCALEX] * data.dval[+Assistance::_sprite_opt_dval::SCALEG] * (1.0 / al_get_bitmap_width(rn->bmp));
			dsy =		1.0f * data.dval[+Assistance::_sprite_opt_dval::SCALEY] * data.dval[+Assistance::_sprite_opt_dval::SCALEG] * (1.0 / al_get_bitmap_height(rn->bmp));

			// draw

			if (data.bval[+Assistance::_sprite_opt_bval::USE_TINTED_DRAWING])	al_draw_tinted_scaled_rotated_bitmap(rn->bmp, data.tint, cx, cy, px, py, dsx, dsy, rot_rad, 0);
			else																al_draw_scaled_rotated_bitmap       (rn->bmp,            cx, cy, px, py, dsx, dsy, rot_rad, 0);

			// debug

			if (data.bval[+Assistance::_sprite_opt_bval::SHOWBOX] || data.bval[+Assistance::_sprite_opt_bval::SHOWDOT])
			{
				ALLEGRO_COLOR colorr = al_map_rgb(0, 255, 0);
				Camera camm;
				auto psf = camm.get();

				if (data.bval[+Assistance::_sprite_opt_bval::_IS_COLLIDING]) colorr = al_map_rgb(255, 0, 0);
				//else if (al_get_time() - lastresetcollisioncall < Defaults::diff_time_show_last_resetCollision) colorr = al_map_rgb(255, 255, 0);

				if (data.bval[+Assistance::_sprite_opt_bval::SHOWBOX]) {
					al_draw_filled_circle(px - cx * dsx, py - cy * dsy, 0.1 * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20), colorr);
					al_draw_filled_circle(px - cx * dsx, py + cy * dsy, 0.1 * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20), colorr);
					al_draw_filled_circle(px + cx * dsx, py - cy * dsy, 0.1 * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20), colorr);
					al_draw_filled_circle(px + cx * dsx, py + cy * dsy, 0.1 * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20), colorr);
				}

				if (data.bval[+Assistance::_sprite_opt_bval::SHOWDOT])
				{
					al_draw_filled_circle(px, py, 0.1 * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20), colorr);
				}
			}

		}
	}
}