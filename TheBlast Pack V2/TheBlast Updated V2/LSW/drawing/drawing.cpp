#include "drawing.h"


namespace LSW {
	namespace v4 {

		std::map<int, camera_preset> Camera::presets;
		int Camera::lastapply = 0;


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

		

		auto Sprite::__sprite_smart_images::get()
		{
			auto now = al_get_time();
			auto siz = copies.size();

			if (lastcall == 0 || difftimeanim <= 0) lastcall = now;

			if (difftimeanim > 0 && siz > 1) {
				while ((now - lastcall) > difftimeanim)
				{
					if (++actual == siz) actual = 0;
					lastcall += difftimeanim;
				}
			}
			return copies[actual]->bmp;
		}
		ALLEGRO_BITMAP* Sprite::__sprite_smart_images::add(const std::string id)
		{
			__template_static_vector<ALLEGRO_BITMAP> imgs; // Textures


			__custom_data* cd = new __custom_data();

			imgs.get(id, cd->bmp);
			cd->idc = id;


			int _flags = al_get_bitmap_flags(cd->bmp);
			int _format = al_get_bitmap_format(cd->bmp);

			copies.push_back(cd);

			int _flags2 = al_get_bitmap_flags(cd->bmp);
			int _format2 = al_get_bitmap_format(cd->bmp);

			return cd->bmp;
		}
		void Sprite::__sprite_smart_images::remove(const std::string id)
		{
			for (size_t p = 0; p < copies.size(); p++) {
				if (copies[p]->idc == id) {
					copies.erase(copies.begin() + p);
					return;
				}
			}
		}
		void Sprite::__sprite_smart_images::setFPS(const double v)
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
		void Sprite::__sprite_smart_images::loop(const bool b)
		{
			loopin = b;
		}
		void Sprite::__sprite_smart_images::reset()
		{
			clear();
			loop(true);
			setFPS(60);
		}
		void Sprite::__sprite_smart_images::clear()
		{
			copies.clear();
		}


		Sprite::__sprite_smart_data::__sprite_smart_data()
		{
			/*for (auto& i : dval) i = 0.0;
			for (auto& j : bval) j = false;*/
			dval[+Assistance::in___double_sprite::SCALEX] = 1.0;
			dval[+Assistance::in___double_sprite::SCALEY] = 1.0;
			dval[+Assistance::in___double_sprite::SCALEG] = 1.0;
			//bval[+Assistance::in___boolean_sprite::DRAW] = false;
			bval[+Assistance::in___boolean_sprite::AFFECTED_BY_CAM] = true;
		}





		Sprite::~Sprite()
		{
			bmps.reset();
		}

		void Sprite::apply(const Assistance::in___string_sprite u, const std::string v)
		{
			switch (u) {
			case Assistance::in___string_sprite::ADD:
				bmps.add(v);
				break;
			case Assistance::in___string_sprite::REMOVE:
				bmps.remove(v);
				break;
			case Assistance::in___string_sprite::SPRITE_ID:
				sprite_id = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::in___double_sprite u, const double v)
		{
			switch (u) {
			case Assistance::in___double_sprite::ANIMATION_FPS:
				bmps.setFPS(v);
				break;
			default:
				data.dval[+u] = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::in___boolean_sprite u, const bool v)
		{
			switch (u) {
			case Assistance::in___boolean_sprite::LOOPFRAMES:
				bmps.loop(v);
				break;
			default:
				data.bval[+u] = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::in___integer_sprite u, const int v)
		{
			switch (u) {
			case Assistance::in___integer_sprite::LAYER:
				layer = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::in___size_sprite u, const size_t v)
		{
			switch (u) {
			case Assistance::in___size_sprite::FRAME:
				bmps.setFPS(-(int)(v));
				break;
			}
		}
		void Sprite::apply(const Assistance::in___color_sprite u, const ALLEGRO_COLOR v)
		{
			switch (u) {
			case Assistance::in___color_sprite::TINT:
				data.tint = v;
				break;
			}
		}

		void Sprite::draw()
		{
			ALLEGRO_BITMAP* rn = bmps.get();
			

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
			int bmpx, bmpy;
			bmpx =		al_get_bitmap_width(rn);
			bmpy =		al_get_bitmap_height(rn);
			if (bmpx <= 0 || bmpy <= 0) {
				throw Abort::abort("al_get_bitmap_width|al_get_bitmap_height", "Sprite::draw", "Somehow the texture have < 0 width / height id=[" + this->sprite_id + "] size={" + std::to_string(bmpx) + "," + std::to_string(bmpy) + "}", 1);
			}
			cx =		1.0f * bmpx * ((data.dval[+Assistance::in___double_sprite::CENTERX] + 1.0) * 0.5);
			cy =		1.0f * bmpy * ((data.dval[+Assistance::in___double_sprite::CENTERY] + 1.0) * 0.5);
			rot_rad =	1.0f * data.dval[+Assistance::in___double_sprite::ROTATION] * ALLEGRO_PI / 180.0;
			px =		1.0f * data.dval[+Assistance::in___double_sprite::POSX] * cos(rot_rad) + data.dval[+Assistance::in___double_sprite::POSY] * sin(rot_rad);
			py =		1.0f * data.dval[+Assistance::in___double_sprite::POSY] * cos(rot_rad) - data.dval[+Assistance::in___double_sprite::POSX] * sin(rot_rad);
			dsx =		1.0f * data.dval[+Assistance::in___double_sprite::SCALEX] * data.dval[+Assistance::in___double_sprite::SCALEG] * (1.0 / bmpx);
			dsy =		1.0f * data.dval[+Assistance::in___double_sprite::SCALEY] * data.dval[+Assistance::in___double_sprite::SCALEG] * (1.0 / bmpy);

			// draw

			if (data.bval[+Assistance::in___boolean_sprite::USE_TINTED_DRAWING])	al_draw_tinted_scaled_rotated_bitmap(rn, data.tint, cx, cy, px, py, dsx, dsy, rot_rad, 0);
			else																    al_draw_scaled_rotated_bitmap       (rn,            cx, cy, px, py, dsx, dsy, rot_rad, 0);

			// debug

			if (data.bval[+Assistance::in___boolean_sprite::SHOWBOX] || data.bval[+Assistance::in___boolean_sprite::SHOWDOT])
			{
				ALLEGRO_COLOR colorr = al_map_rgb(0, 255, 0);
				Camera camm;
				auto psf = camm.get();

				if (data.bval[+Assistance::in___boolean_sprite::IS_COLLIDING]) colorr = al_map_rgb(255, 0, 0);
				//else if (al_get_time() - lastresetcollisioncall < Defaults::diff_time_show_last_resetCollision) colorr = al_map_rgb(255, 255, 0);

				if (data.bval[+Assistance::in___boolean_sprite::SHOWBOX]) {
					al_draw_filled_circle(px - cx * dsx, py - cy * dsy, 0.1f * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px - cx * dsx, py + cy * dsy, 0.1f * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px + cx * dsx, py - cy * dsy, 0.1f * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px + cx * dsx, py + cy * dsy, 0.1f * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20f), colorr);
				}

				if (data.bval[+Assistance::in___boolean_sprite::SHOWDOT])
				{
					al_draw_filled_circle(px, py, 0.1f * fabs(psf.get(Assistance::_cam_opt::SCALE_G) * sqrt(psf.get(Assistance::_cam_opt::SCALE_X) * psf.get(Assistance::_cam_opt::SCALE_Y)) * 0.20f), colorr);
				}
			}

		}
	}
}