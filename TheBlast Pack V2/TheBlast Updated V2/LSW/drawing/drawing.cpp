#include "drawing.h"


namespace LSW {
	namespace v4 {

		std::map<int, camera_preset> Camera::presets;
		int Camera::lastapply = 0;



		void draw_simple_bar(const float perc, const ALLEGRO_COLOR bg, const ALLEGRO_COLOR bar)
		{
			Camera gcam;
			gcam.applyNoSave(camera_preset());
			al_clear_to_color(bg);
			al_draw_filled_rectangle(-0.99, -0.99, 2.0 * perc - 1.0, 0.99, bar);
		}

		void draw_simple_txt(ALLEGRO_FONT* f, const std::string s, ALLEGRO_COLOR c, const int flag)
		{
			Camera gcam;
			camera_preset cp;
			cp.set(Assistance::io___float_camera::SCALE_G, 0.3 / Constants::text_default_sharpness_font);
			cp.set(Assistance::io___float_camera::SCALE_Y, 3.5);
			gcam.applyNoSave(cp);
			al_draw_text(f, c, 0, - 0.6 * Constants::text_default_sharpness_font, flag, s.c_str());
			gcam.apply();
		}


		void camera_preset::set(const Assistance::io___float_camera u, const float v)
		{
			p[+u] = v;
		}

		float camera_preset::get(const Assistance::io___float_camera u)
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

			if (!d) return;// throw Abort::abort("al_get_current_display", "transformator::apply", "Display not found to apply transformation"); (should not do that just because it failed)

			al_identity_transform(&g_t);

			al_rotate_transform(&g_t,
				lastTransf.get(Assistance::io___float_camera::ROTATION));

			al_scale_transform(&g_t,
				al_get_display_width(d) * 0.5 * lastTransf.get(Assistance::io___float_camera::SCALE_X) * lastTransf.get(Assistance::io___float_camera::SCALE_G),
				al_get_display_height(d) * 0.5 * lastTransf.get(Assistance::io___float_camera::SCALE_Y) * lastTransf.get(Assistance::io___float_camera::SCALE_G)); // SCALING NOT AS EXPECTED (multiplier by zoom is not being made by transformation!)

			al_translate_transform(&g_t,
				al_get_display_width(d) * 0.5 - lastTransf.get(Assistance::io___float_camera::OFFSET_X) * (al_get_display_width(d) * 0.5 * lastTransf.get(Assistance::io___float_camera::SCALE_X) * lastTransf.get(Assistance::io___float_camera::SCALE_G)),
				al_get_display_height(d) * 0.5 - lastTransf.get(Assistance::io___float_camera::OFFSET_Y) * (al_get_display_height(d) * 0.5 * lastTransf.get(Assistance::io___float_camera::SCALE_Y) * lastTransf.get(Assistance::io___float_camera::SCALE_G)));

			al_use_transform(&g_t);
		}

		void Camera::apply()
		{
			apply(lastapply);
		}

		void Camera::applyNoSave(camera_preset lastTransf)
		{
			ALLEGRO_DISPLAY* d = al_get_current_display();

			if (!d) throw Abort::abort("al_get_current_display", "transformator::apply", "Display not found to apply transformation");

			al_identity_transform(&g_t);

			al_rotate_transform(&g_t,
				lastTransf.get(Assistance::io___float_camera::ROTATION));

			al_scale_transform(&g_t,
				al_get_display_width(d) * 0.5 * lastTransf.get(Assistance::io___float_camera::SCALE_X) * lastTransf.get(Assistance::io___float_camera::SCALE_G),
				al_get_display_height(d) * 0.5 * lastTransf.get(Assistance::io___float_camera::SCALE_Y) * lastTransf.get(Assistance::io___float_camera::SCALE_G)); // SCALING NOT AS EXPECTED (multiplier by zoom is not being made by transformation!)

			al_translate_transform(&g_t,
				al_get_display_width(d) * 0.5 - lastTransf.get(Assistance::io___float_camera::OFFSET_X) * (al_get_display_width(d) * 0.5 * lastTransf.get(Assistance::io___float_camera::SCALE_X) * lastTransf.get(Assistance::io___float_camera::SCALE_G)),
				al_get_display_height(d) * 0.5 - lastTransf.get(Assistance::io___float_camera::OFFSET_Y) * (al_get_display_height(d) * 0.5 * lastTransf.get(Assistance::io___float_camera::SCALE_Y) * lastTransf.get(Assistance::io___float_camera::SCALE_G)));

			al_use_transform(&g_t);
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
			dval[+Assistance::io___double_sprite::SCALEX] = 1.0;
			dval[+Assistance::io___double_sprite::SCALEY] = 1.0;
			dval[+Assistance::io___double_sprite::SCALEG] = 1.0;
			//bval[+Assistance::io___boolean_sprite::DRAW] = false;
			bval[+Assistance::io___boolean_sprite::AFFECTED_BY_CAM] = true;
		}





		Sprite::~Sprite()
		{
			bmps.reset();
		}

		void Sprite::apply(const Assistance::io___vecstring_sprite u, const std::vector<std::string> v, float* perc)
		{
			if (perc) *perc = 0.00f;
			size_t countt = 0;
			switch (u) {
			case Assistance::io___vecstring_sprite::ADDMULTIPLE:
				for (auto& i : v) {
					bmps.add(i);
					if (perc) *perc = 1.00f * countt++ / v.size();
				}
				break;
			case Assistance::io___vecstring_sprite::REMOVEMULTIPLE:
				for (auto& i : v) {
					bmps.remove(i);
					if (perc) *perc = 1.00f * countt++ / v.size();
				}
				break;
			}
			if (perc) *perc = 1.00f;
		}

		void Sprite::apply(const Assistance::io___string_sprite u, const std::string v)
		{
			switch (u) {
			case Assistance::io___string_sprite::ADD:
				bmps.add(v);
				break;
			case Assistance::io___string_sprite::REMOVE:
				bmps.remove(v);
				break;
			case Assistance::io___string_sprite::SPRITE_ID:
				sprite_id = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::io___double_sprite u, const double v)
		{
			switch (u) {
			case Assistance::io___double_sprite::ANIMATION_FPS:
				bmps.setFPS(v);
				break;
			default:
				data.dval[+u] = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::io___boolean_sprite u, const bool v)
		{
			switch (u) {
			case Assistance::io___boolean_sprite::LOOPFRAMES:
				bmps.loop(v);
				break;
			default:
				data.bval[+u] = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::io___integer_sprite u, const int v)
		{
			switch (u) {
			case Assistance::io___integer_sprite::LAYER:
				layer = v;
				break;
			}
		}
		void Sprite::apply(const Assistance::io___size_sprite u, const size_t v)
		{
			switch (u) {
			case Assistance::io___size_sprite::FRAME:
				bmps.setFPS(-(int)(v));
				break;
			}
		}
		void Sprite::apply(const Assistance::io___color_sprite u, const ALLEGRO_COLOR v)
		{
			switch (u) {
			case Assistance::io___color_sprite::TINT:
				data.tint = v;
				break;
			}
		}

		bool Sprite::get(const Assistance::io___string_sprite u, std::string& v)
		{
			switch (u) {
			case Assistance::io___string_sprite::ADD:
			case Assistance::io___string_sprite::REMOVE:
				// no sense
				return false;
			case Assistance::io___string_sprite::SPRITE_ID:
				v = sprite_id;
				return true;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io___double_sprite u, double& v)
		{
			switch (u) {
			case Assistance::io___double_sprite::ANIMATION_FPS:
				// no way
				return false;
			default:
				v = data.dval[+u];
				return true;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io___boolean_sprite u, bool& v)
		{
			switch (u) {
			case Assistance::io___boolean_sprite::LOOPFRAMES:
				// no way
				return false;
			default:
				v = data.bval[+u];
				return true;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io___integer_sprite u, int& v)
		{
			switch (u) {
			case Assistance::io___integer_sprite::LAYER:
				v = layer;
				return true;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io___size_sprite u, size_t& v)
		{
			switch (u) {
			case Assistance::io___size_sprite::FRAME:
				// no way
				return false;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io___color_sprite u, ALLEGRO_COLOR& v)
		{
			switch (u) {
			case Assistance::io___color_sprite::TINT:
				v = data.tint;
				return true;
			}
			return false;
		}

		void Sprite::draw(const int is_layer)
		{
			if (layer != is_layer) return;

			ALLEGRO_BITMAP* rn = bmps.get();
			Camera camm;

			if (!data.bval[+Assistance::io___boolean_sprite::AFFECTED_BY_CAM]) camm.applyNoSave(camera_preset());

			float cx, cy, px, py, dsx, dsy, rot_rad;
			int bmpx, bmpy;
			bmpx =		al_get_bitmap_width(rn);
			bmpy =		al_get_bitmap_height(rn);
			if (bmpx <= 0 || bmpy <= 0) {
				throw Abort::abort("al_get_bitmap_width|al_get_bitmap_height", "Sprite::draw", "Somehow the texture have < 0 width / height id=[" + this->sprite_id + "] size={" + std::to_string(bmpx) + "," + std::to_string(bmpy) + "}", 1);
			}

			cx =		1.0f * bmpx * ((data.dval[+Assistance::io___double_sprite::CENTERX] + 1.0) * 0.5);
			cy =		1.0f * bmpy * ((data.dval[+Assistance::io___double_sprite::CENTERY] + 1.0) * 0.5);
			rot_rad =	1.0f * data.dval[+Assistance::io___double_sprite::ROTATION] * ALLEGRO_PI / 180.0;
			px =		1.0f * data.dval[+Assistance::io___double_sprite::POSX] * cos(rot_rad) + data.dval[+Assistance::io___double_sprite::POSY] * sin(rot_rad);
			py =		1.0f * data.dval[+Assistance::io___double_sprite::POSY] * cos(rot_rad) - data.dval[+Assistance::io___double_sprite::POSX] * sin(rot_rad);
			dsx =		1.0f * data.dval[+Assistance::io___double_sprite::SCALEX] * data.dval[+Assistance::io___double_sprite::SCALEG] * (1.0 / bmpx);
			dsy =		1.0f * data.dval[+Assistance::io___double_sprite::SCALEY] * data.dval[+Assistance::io___double_sprite::SCALEG] * (1.0 / bmpy);

			// draw

			if (data.bval[+Assistance::io___boolean_sprite::USE_TINTED_DRAWING])	al_draw_tinted_scaled_rotated_bitmap(rn, data.tint, cx, cy, px, py, dsx, dsy, rot_rad, 0);
			else																    al_draw_scaled_rotated_bitmap       (rn,            cx, cy, px, py, dsx, dsy, rot_rad, 0);

			// debug

			if (data.bval[+Assistance::io___boolean_sprite::SHOWBOX] || data.bval[+Assistance::io___boolean_sprite::SHOWDOT])
			{
				ALLEGRO_COLOR colorr = al_map_rgb(0, 255, 0);
				camera_preset psf = camm.get();

				if (data.bval[+Assistance::io___boolean_sprite::IS_COLLIDING]) colorr = al_map_rgb(255, 0, 0);
				//else if (al_get_time() - lastresetcollisioncall < Defaults::diff_time_show_last_resetCollision) colorr = al_map_rgb(255, 255, 0);

				if (data.bval[+Assistance::io___boolean_sprite::SHOWBOX]) {
					al_draw_filled_circle(px - cx * dsx, py - cy * dsy, 0.1f * fabs(psf.get(Assistance::io___float_camera::SCALE_G) * sqrt(psf.get(Assistance::io___float_camera::SCALE_X) * psf.get(Assistance::io___float_camera::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px - cx * dsx, py + cy * dsy, 0.1f * fabs(psf.get(Assistance::io___float_camera::SCALE_G) * sqrt(psf.get(Assistance::io___float_camera::SCALE_X) * psf.get(Assistance::io___float_camera::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px + cx * dsx, py - cy * dsy, 0.1f * fabs(psf.get(Assistance::io___float_camera::SCALE_G) * sqrt(psf.get(Assistance::io___float_camera::SCALE_X) * psf.get(Assistance::io___float_camera::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px + cx * dsx, py + cy * dsy, 0.1f * fabs(psf.get(Assistance::io___float_camera::SCALE_G) * sqrt(psf.get(Assistance::io___float_camera::SCALE_X) * psf.get(Assistance::io___float_camera::SCALE_Y)) * 0.20f), colorr);
				}

				if (data.bval[+Assistance::io___boolean_sprite::SHOWDOT])
				{
					al_draw_filled_circle(px, py, 0.1f * fabs(psf.get(Assistance::io___float_camera::SCALE_G) * sqrt(psf.get(Assistance::io___float_camera::SCALE_X) * psf.get(Assistance::io___float_camera::SCALE_Y)) * 0.20f), colorr);
				}
			}

			camm.apply();
		}











		///bool Text::is_gpath_raw = false;
		///std::string Text::gpath;

		void Text::_draw(const double targ_draw_xy[2])
		{
			al_draw_text(ffont, color, 1.0 * targ_draw_xy[0] / (scale), 1.0 * targ_draw_xy[1] / (scale), mode, string.c_str());
		}
		void Text::_interpretTags(std::string& s)
		{
			std::string local_t = s;

			std::vector<size_t> posicoes;
			double timing = al_get_time();

			for (size_t p = local_t.find('%'); (p != std::string::npos); p = local_t.find('%'))
			{
				if (al_get_time() - timing > Constants::text_timeout_interpret) {
					throw Abort::abort("null", "Text::_interprettags", "Failed interpreting " + s + "!");
					return;
				}

				if (p == 0 || (local_t[p - 1] != '\\')) {
					std::string elsee = local_t.substr(p);
					std::string substitute;

					size_t found = 0;
					bool rfound = false;
					for (auto& i : Assistance::tags)
					{
						if (elsee.find(i.s()) == 0)
						{
							rfound = true;
							break;
						}
						found++;
					}

					Camera cam;
					ALLEGRO_DISPLAY* d = al_get_current_display();
					char tempstr_c[128];
					//gfile logg;

					switch (found)
					{
					case +Assistance::tags_e::T_POSX:
						sprintf_s(tempstr_c, "%.3lf", (pos[0] + off_plr[0] + cam.get().get(Assistance::io___float_camera::OFFSET_X)));
						break;
					case +Assistance::tags_e::T_POSY:
						sprintf_s(tempstr_c, "%.3lf", (pos[1] + off_plr[1] + cam.get().get(Assistance::io___float_camera::OFFSET_Y)));
						break;
					case +Assistance::tags_e::T_SCREEN_POSX:
						sprintf_s(tempstr_c, "%.3lf", (pos[0] * (affected_by_camera ? (cam.get().get(Assistance::io___float_camera::SCALE_G) * cam.get().get(Assistance::io___float_camera::SCALE_X)) : 1.0) + off_plr[0] * (cam.get().get(Assistance::io___float_camera::SCALE_G) * cam.get().get(Assistance::io___float_camera::SCALE_X))));
						break;
					case +Assistance::tags_e::T_SCREEN_POSY:
						sprintf_s(tempstr_c, "%.3lf", (pos[1] * (affected_by_camera ? (cam.get().get(Assistance::io___float_camera::SCALE_G) * cam.get().get(Assistance::io___float_camera::SCALE_Y)) : 1.0) + off_plr[1] * (cam.get().get(Assistance::io___float_camera::SCALE_G) * cam.get().get(Assistance::io___float_camera::SCALE_Y))));
						break;

					case +Assistance::tags_e::T_CAM_X:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io___float_camera::OFFSET_X));
						break;
					case +Assistance::tags_e::T_CAM_Y:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io___float_camera::OFFSET_Y));
						break;
					case +Assistance::tags_e::T_CAM_ZOOM:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io___float_camera::SCALE_G));
						break;
					case +Assistance::tags_e::T_CAM_ZOOMX:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io___float_camera::SCALE_X));
						break;
					case +Assistance::tags_e::T_CAM_ZOOMY:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io___float_camera::SCALE_Y));
						break;

					case +Assistance::tags_e::T_ISFOLLOWING:
						sprintf_s(tempstr_c, "%s", (follow ? "Y" : "N"));
						break;
					case +Assistance::tags_e::T_COLOR_R:
						sprintf_s(tempstr_c, "%.3f", color.r);
						break;
					case +Assistance::tags_e::T_COLOR_G:
						sprintf_s(tempstr_c, "%.3f", color.g);
						break;
					case +Assistance::tags_e::T_COLOR_B:
						sprintf_s(tempstr_c, "%.3f", color.b);
						break;
					case +Assistance::tags_e::T_COLOR_A:
						sprintf_s(tempstr_c, "%.3f", color.a);
						break;
					case +Assistance::tags_e::T_MODE:
						sprintf_s(tempstr_c, "%d", mode);
						break;
					case +Assistance::tags_e::T_TIME:
						sprintf_s(tempstr_c, "%.3lf", al_get_time());
						break;

					case +Assistance::tags_e::T_GB_RESX:
						sprintf_s(tempstr_c, "%d", (d ? al_get_display_width(d) : -1));
						break;
					case +Assistance::tags_e::T_GB_RESY:
						sprintf_s(tempstr_c, "%d", (d ? al_get_display_height(d) : -1));
						break;
					case +Assistance::tags_e::T_REFRESHRATE:
						sprintf_s(tempstr_c, "%d", ((d) ? al_get_display_refresh_rate(d) : -1));
						break;
					case +Assistance::tags_e::T_FPS:
					{
						/*Display::big_display disp;
						double t;
						disp.get(Display::FPS, t);
						sprintf_s(tempstr_c, "%.2lf", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;

					case +Assistance::tags_e::T_TPS_COUNT:
						//sprintf_s(tempstr_c, "%u", bev._getEventLog().loops_per_second);
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
						break;
					case +Assistance::tags_e::T_TPS_COLLISION:
						//sprintf_s(tempstr_c, "%.3lf", bev._getEventLog().collisionTimer_tps);
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
						break;
					case +Assistance::tags_e::T_TPS_FUNCTIONS:
						//sprintf_s(tempstr_c, "%.3lf", bev._getEventLog().functionsTimer_tps);
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
						break;
					case +Assistance::tags_e::T_TPS_SECOND_TAKEN:
						//sprintf_s(tempstr_c, "%.3lf", bev._getEventLog().calcLoopsTimer_tps);
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
						break;
					case +Assistance::tags_e::T_TPS_POSUPDATE:
						//sprintf_s(tempstr_c, "%.3lf", bev._getEventLog().updatePosTimer_tps);
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
						break;

					case +Assistance::tags_e::T_SPRITE_FRAME:
						/*if (follow) sprintf_s(tempstr_c, "%lu", follow->_lastFrameNumber());
						else sprintf_s(tempstr_c, "NOT_AVAILABLE");*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
						break;
					case +Assistance::tags_e::T_MOUSE_X:
					{
						/*float x;
						bev.getMouse(x, Events::X);
						sprintf_s(tempstr_c, "%.3f", x);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_MOUSE_Y:
					{
						/*float y;
						bev.getMouse(y, Events::Y);
						sprintf_s(tempstr_c, "%.3f", y);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_LASTSTRING:
					{
						/*std::string str;
						bev.getLastString(str);
						sprintf_s(tempstr_c, "%s", str.c_str());*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_CURRSTRING:
					{
						/*std::string str;
						bev.getCurrentString(str);
						int u = (str.length()) / 20;
						if (u > 3) u = 3;
						if (u < 0) u = 0;

						for (int p = 0; p < u; p++) tempstr_c[p] = '.';

						sprintf_s(tempstr_c + u, 128 - u, "%s", (str.substr((str.length() >= 20) ? str.length() - 19 - u : 0) + '\0').c_str());*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_SPRITE_SPEEDX:
						if (follow) {
							double val;
							follow->get(Assistance::io___double_sprite::SPEEDX, val);
							sprintf_s(tempstr_c, "%.3lf", val);
						}
						else sprintf_s(tempstr_c, "UNDEF");
						break;
					case +Assistance::tags_e::T_SPRITE_SPEEDY:
						if (follow) {
							double val;
							follow->get(Assistance::io___double_sprite::SPEEDY, val);
							sprintf_s(tempstr_c, "%.3lf", val);
						}
						else sprintf_s(tempstr_c, "UNDEF");
						break;
					case +Assistance::tags_e::T_SPRITE_NAME:
						if (follow) {
							std::string temp;
							follow->get(Assistance::io___string_sprite::SPRITE_ID, temp);
							sprintf_s(tempstr_c, "%s", temp.c_str());
						}
						else sprintf_s(tempstr_c, "UNDEF");
						break;
					case +Assistance::tags_e::T_SPRITE_ENTITY_HEALTH: ///TODO
						/*if (follow) {
							Entities::entity* ent = nullptr;
							void* trash = nullptr;
							follow->get(Sprite::ENTITY, trash);
							ent = (Entities::entity*)trash;

							if (ent) {
								sprintf_s(tempstr_c, "%.1lf", 10.0 * ent->getMyHealth());
							}
							else sprintf_s(tempstr_c, "UNDEF");
						}
						else sprintf_s(tempstr_c, "UNDEF");*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
						break;
					case +Assistance::tags_e::T_SPRITE_ENTITY_NAME:
						/*if (follow) {

							Entities::entity* ent = nullptr;
							void* trash = nullptr;
							follow->get(Sprite::ENTITY, trash);
							ent = (Entities::entity*)trash;

							if (ent) {
								sprintf_s(tempstr_c, "%s", ent->getMyName().c_str());
							}
							else sprintf_s(tempstr_c, "UNDEF");
						}
						else sprintf_s(tempstr_c, "UNDEF");*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
						break;
					case +Assistance::tags_e::T_IMAGES_LOADED:
					{
						/*d_images_database img_data;
						size_t t = img_data.work().work().size();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_SPRITES_LOADED:
					{
						/*d_sprite_database spr_data;
						size_t t = spr_data.work().work().size();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_TEXTS_LOADED:
					{
						/*d_texts_database txt_data;
						size_t t = txt_data.work().work().size();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_TRACKS_LOADED:
					{
						/*d_musics_database msk_data;
						size_t t = msk_data.work().work().size();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_ENTITIES_LOADED:
					{
						/*d_entity_database ent_data;
						size_t t = ent_data.work().work().size();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_GARBAGE_TOTAL:
					{
						/*d_entity_database ent_data;
						d_images_database img_data;
						d_musics_database msk_data;
						d_sprite_database spr_data;
						d_texts_database txt_data;

						size_t sprs = spr_data.work().check_weak_count();
						size_t imgs = img_data.work().check_weak_count();
						size_t txts = txt_data.work().check_weak_count();
						size_t msks = msk_data.work().check_weak_count();
						size_t ents = ent_data.work().check_weak_count();

						size_t t = sprs + imgs + txts + msks + ents;

						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_IMAGES_GARBAGE:
					{
						/*d_images_database img_data;
						size_t t = img_data.work().check_weak_count();
						sprintf_s(tempstr_c, "%lu", t);*/
					}
					break;
					case +Assistance::tags_e::T_SPRITES_GARBAGE:
					{
						/*d_sprite_database spr_data;
						size_t t = spr_data.work().check_weak_count();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_TEXTS_GARBAGE:
					{
						/*d_texts_database txt_data;
						size_t t = txt_data.work().check_weak_count();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_TRACKS_GARBAGE:
					{
						/*d_musics_database msk_data;
						size_t t = msk_data.work().check_weak_count();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					break;
					case +Assistance::tags_e::T_ENTITIES_GARBAGE:
					{
						/*d_entity_database ent_data;
						size_t t = ent_data.work().check_weak_count();
						sprintf_s(tempstr_c, "%lu", t);*/
						sprintf_s(tempstr_c, "NOT_AVAILABLE");
					}
					}

					substitute = tempstr_c;

					std::string change;


					if (p > 0)
					{
						change = local_t.substr(0, p);
					}
					change += substitute + local_t.substr(p + Assistance::tags[found].s().length());
					local_t = change;
				}
				else {
					local_t[p] = 'a';
					if (p > 0) {
						p--;
						local_t.erase(local_t.begin() + p);
						posicoes.push_back(p);
					}
				}
			}

			for (auto& i : posicoes)
			{
				local_t[i] = '%';
			}
			posicoes.clear();

			s = local_t;
		}

		void Text::setFollow(const std::string u)
		{
			__template_static_vector<Sprite> spr_data;
			Sprite* spr;
			if (spr_data.get(u, spr))
			{
				setFollow(spr);
			}
		}

		void Text::setFollow(Sprite* u)
		{
			follow = u;
		}

		/*const bool Text::load(std::string s, const double siz)
		{
			gfile logg;
			logg << L::SLL << freg("Text", "load") << "Verifying new loading of texture called " << s << "... (ID#" << (int)this << ")" << L::BLL;

			if (!ffont) {
				//Camera::camera_g cam;

				logg << L::SLL << freg("Text", "load") << "Load allowed for " << s << "... (ID#" << (int)this << ")" << L::BLL;

				_bpath = s;

				Tools::interpret_path(s);
				path = s;

				bool aa = (ffont = al_load_ttf_font(s.c_str(), 1.0 * siz * Constants::text_default_sharpness_font / 20.0, 0));

				//logg << L::SLL << freg("Text", "load") << " Settings: {LOADSUCCESS=" << (aa ? "Y" : "N") << ";ID=#" << (int)this << ";PATH=" << s << ";SIZE=" << 1000000.0 * siz * Defaults::sharpness_font / 20.0 << "E-6}" << L::BLL;
				logg.flush();

				return aa;
			}
			return true;
		}

		void Text::reload()
		{
			unload();
			if (using_gpath) path.clear();

			if (path.length() == 0) {
				if (gpath.length() > 0)
				{
					using_gpath = true;
					path = gpath;
				}
				else path = "C:\\Windows\\Fonts\\calibri.ttf";
			}
			load(path, scale);
		}

		void Text::_verify()
		{
			if (!ffont) {
				reload();
			}
			if (using_gpath && gpath != _bpath)
			{
				reload();
			}
		}
		*/
		Text::Text()
		{
			gfile logg;
			logg << L::SLL << fsr(__FUNCSIG__) << "Registered spawn of Text #" + std::to_string((size_t)this) << L::BLL;
			logg.flush();
		}

		Text::~Text()
		{
			gfile logg;
			logg << L::SLL << fsr(__FUNCSIG__) << "Registered despawn of Text #" + std::to_string((size_t)this)/* << ";ID=" << id */ << L::BLL;
		}

		/*void Text::verify(const bool reloadd)
		{
			if (reloadd) {
				reload();
				return;
			}
			if (final_scale != last_final_scale) {
				unload();
				last_final_scale = final_scale;
			}
			_verify();
		}*/

		void Text::unload()
		{
			if (ffont) {
				al_destroy_font(ffont);
				ffont = nullptr;
			}
		}


		void Text::set(const Assistance::_text_opt_str o, const std::string e)
		{
			switch (o) {
			case Assistance::_text_opt_str::SETSTRING:
				orig_str = e;
				break;
			case Assistance::_text_opt_str::SETID:
				id = e;
				break;
			case Assistance::_text_opt_str::SETFOLLOW:
				setFollow(e);
				break;
			case Assistance::_text_opt_str::SETFONT:
				__template_static_vector<ALLEGRO_FONT> fonts;
				fonts.get(e, ffont);
				break;
			}
		}
		void Text::set(const Assistance::_text_opt_bool o, const bool e)
		{
			switch (o) {
			case Assistance::_text_opt_bool::SHOW:
				show = e;
				break;
			case Assistance::_text_opt_bool::AFFECTED_BY_CAM:
				affected_by_camera = e;
				break;
			}
		}
		void Text::set(const Assistance::_text_opt_db o, const double e)
		{
			switch (o) {
			case Assistance::_text_opt_db::POSX:
				pos[0] = e;
				break;
			case Assistance::_text_opt_db::POSY:
				pos[1] = e;
				break;
			case Assistance::_text_opt_db::SCALEG:
				scale = e;
				break;
			case Assistance::_text_opt_db::ROTATION:
				rot = e;
				break;
			case Assistance::_text_opt_db::UPDATETIME:
				update_time = e;
				break;
			}
		}
		void Text::set(const Assistance::_text_opt_color o, const ALLEGRO_COLOR e)
		{
			switch (o) {
			case Assistance::_text_opt_color::COLOR:
				color = e;
				break;
			}
		}
		void Text::set(const Assistance::_text_opt_int o, const int e)
		{
			switch (o) {
			case Assistance::_text_opt_int::MODE:
				mode = e;
				break;
			case Assistance::_text_opt_int::LAYER:
				layer = e;
				break;
			}
		}

		void Text::get(const Assistance::_text_opt_str o, std::string& e)
		{
			switch (o) {
			case Assistance::_text_opt_str::SETSTRING:
				e = string;
				break;
			case Assistance::_text_opt_str::SETID:
				e = id;
				break;
			case Assistance::_text_opt_str::SETFOLLOW:
				if (follow) follow->get(Assistance::io___string_sprite::SPRITE_ID, e);
				break;
			}
		}
		void Text::get(const Assistance::_text_opt_bool o, bool& e)
		{
			switch (o) {
			case Assistance::_text_opt_bool::SHOW:
				e = show;
				break;
			case Assistance::_text_opt_bool::AFFECTED_BY_CAM:
				e = affected_by_camera;
				break;
			}
		}
		void Text::get(const Assistance::_text_opt_db o, double& e)
		{
			switch (o) {
			case Assistance::_text_opt_db::POSX:
				e = pos[0];
				break;
			case Assistance::_text_opt_db::POSY:
				e = pos[1];
				break;
			case Assistance::_text_opt_db::SCALEG:
				e = scale;
				break;
			case Assistance::_text_opt_db::ROTATION:
				e = rot;
				break;
			case Assistance::_text_opt_db::UPDATETIME:
				e = update_time;
				break;
			}
		}
		void Text::get(const Assistance::_text_opt_int o, int& e)
		{
			switch (o) {
			case Assistance::_text_opt_int::MODE:
				e = mode;
				break;
			case Assistance::_text_opt_int::LAYER:
				e = layer;
				break;
			}
		}
		void Text::get(const Assistance::_text_opt_color o, ALLEGRO_COLOR& e)
		{
			switch (o) {
			case Assistance::_text_opt_color::COLOR:
				e = color;
				break;
			}
		}
		const bool Text::isEq(const Assistance::_text_opt_color e, const ALLEGRO_COLOR v)
		{
			ALLEGRO_COLOR g;
			get(e, g);
			return ((g.r == v.r) && (g.g == v.g) && (g.b == v.b) && (g.a == v.a));
		}
		const bool Text::isEq(const Assistance::_text_opt_str e, const std::string v)
		{
			std::string g;
			get(e, g);
			return g == v;
		}
		const bool Text::isEq(const Assistance::_text_opt_bool e, const bool v)
		{
			bool g;
			get(e, g);
			return g == v;
		}
		const bool Text::isEq(const Assistance::_text_opt_db e, const double v)
		{
			double g;
			get(e, g);
			return g == v;
		}
		const bool Text::isEq(const Assistance::_text_opt_int e, const int v)
		{
			int g;
			get(e, g);
			return g == v;
		}


		void Text::draw(const int is_layer)
		{
			if (!show) return;
			if (is_layer != layer) {
				//lastinterpret = al_get_time() - update_time;
				return;
			}
			if (follow) {
				bool k;
				follow->get(Assistance::io___boolean_sprite::DRAW, k);
				if (!k) return;
			}

			if (al_get_time() - lastinterpret > update_time/* || str_upd*/) {
				//str_upd = false;
				if (lastinterpret == 0) lastinterpret = al_get_time();
				else {
					if (al_get_time() - lastinterpret > update_time * 3) lastinterpret = al_get_time();
					else lastinterpret += update_time;
				}

				std::string b4 = string;
				string = orig_str;
				_interpretTags(string);/*
				if (string != b4) {
					str_upd = true;
				}*/
			}

			while (rot > 360.0) rot -= 360.0;
			while (rot < 0.0) rot += 360.0;

			Camera cam;
			camera_preset preset = cam.get();
			//const int lastapply = cam.getLastApplyID();
			ALLEGRO_BITMAP* d = al_get_target_bitmap();
			if (!d) return;// throw "TEXT::DRAW - NO DISPLAY!";

			/*if (usebuf) {

				loadInternalBMP();
				if (str_upd) {
					ALLEGRO_BITMAP* me;
					local_paint_i->get(me);
					al_set_target_bitmap(me);
					al_clear_to_color(al_map_rgba(0, 0, 0, 0));
					cam.copy(Defaults::default_layer_backup, lastapply);
					if (affected_by_camera)
					{
						cam.set(cam.getLastApplyID(), Camera::ZOOM, cam.get(lastapply, Camera::ZOOMX) * transf[1]);
						cam.set(cam.getLastApplyID(), Camera::ZOOM, cam.get(lastapply, Camera::ZOOMY) * transf[2]);
						cam.set(cam.getLastApplyID(), Camera::ZOOM, cam.get(lastapply, Camera::ZOOM) * transf[0] * 1.0 / Defaults::sharpness_font);
						cam.set(cam.getLastApplyID(), Camera::OFFX, cam.get(lastapply, Camera::OFFX) * Defaults::sharpness_font);
						cam.set(cam.getLastApplyID(), Camera::OFFY, cam.get(lastapply, Camera::OFFY) * Defaults::sharpness_font);
					}
					else {
						cam.reset(lastapply);
						cam.set(cam.getLastApplyID(), Camera::ZOOMX, transf[1]);
						cam.set(cam.getLastApplyID(), Camera::ZOOMY, transf[2]);
						cam.set(cam.getLastApplyID(), Camera::ZOOM, transf[0] * 1.0 / Defaults::sharpness_font);
						cam.set(cam.getLastApplyID(), Camera::OFFX, 0.0);
						cam.set(cam.getLastApplyID(), Camera::OFFY, 0.0);
					}
					cam.apply(lastapply);
					_draw();
					cam.copy(lastapply, Defaults::default_layer_backup);
					cam.apply(lastapply);
					al_set_target_bitmap(d);
					cam.apply(lastapply);
				}
			}
			else {*/

			if (follow) {
				follow->get(Assistance::io___double_sprite::POSX, off_plr[0]);
				follow->get(Assistance::io___double_sprite::POSY, off_plr[1]);
				follow->get(Assistance::io___double_sprite::ROTATION, ofr_plr);
			}
			else {
				off_plr[0] = off_plr[1] = 0.0;
			}

			double t_rotation_rad = (rot)*ALLEGRO_PI / 180.0;
			double p_rotation_rad = (ofr_plr)*ALLEGRO_PI / 180.0;
			double rotation_rad = t_rotation_rad + p_rotation_rad;

			if (rotation_rad > ALLEGRO_PI * 2) rotation_rad -= ALLEGRO_PI * 2;
			if (rotation_rad < 0) rotation_rad += ALLEGRO_PI * 2;

			double pos_now[2];
			pos_now[0] = 1.0 * Constants::text_default_sharpness_font * ((pos[0] * cos(p_rotation_rad) - pos[1] * sin(p_rotation_rad)) + off_plr[0]);
			pos_now[1] = 1.0 * Constants::text_default_sharpness_font * ((-pos[0] * sin(p_rotation_rad) + pos[1] * cos(p_rotation_rad)) + off_plr[1]);


			double targ_draw_xy[2];
			targ_draw_xy[0] = pos_now[0] * cos(rotation_rad) + pos_now[1] * sin(rotation_rad);
			targ_draw_xy[1] = -pos_now[0] * sin(rotation_rad) + pos_now[1] * cos(rotation_rad);


			//cam.copy(Defaults::default_layer_backup, lastapply);

			if (affected_by_camera)
			{
				preset.set(Assistance::io___float_camera::SCALE_G, preset.get(Assistance::io___float_camera::SCALE_G) * scale * 1.0 / Constants::text_default_sharpness_font);
				preset.set(Assistance::io___float_camera::OFFSET_X, preset.get(Assistance::io___float_camera::OFFSET_X) * Constants::text_default_sharpness_font / scale);
				preset.set(Assistance::io___float_camera::OFFSET_Y, preset.get(Assistance::io___float_camera::OFFSET_Y) * Constants::text_default_sharpness_font / scale);
			}
			else {
				preset = camera_preset();
				preset.set(Assistance::io___float_camera::SCALE_G, scale * 1.0 / Constants::text_default_sharpness_font);
			}

			preset.set(Assistance::io___float_camera::ROTATION, preset.get(Assistance::io___float_camera::ROTATION) + rotation_rad);
			cam.applyNoSave(preset);

			final_scale = scale;// *cam.get(lastapply, Camera::ZOOM);
			//verify();

			_draw(targ_draw_xy);

			cam.apply();
			al_set_target_bitmap(d);
			cam.apply();

			//}
		}

	}
}