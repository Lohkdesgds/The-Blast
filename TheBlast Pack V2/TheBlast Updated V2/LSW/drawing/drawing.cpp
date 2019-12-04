#include "drawing.h"


namespace LSW {
	namespace v4 {

		std::map<int, camera_preset> Camera::presets;
		int Camera::lastapply = 0;



		void draw_simple_bar(const float perc, const ALLEGRO_COLOR bg)
		{
			Camera gcam;
			gcam.applyNoSave(camera_preset());
			al_clear_to_color(bg);
			ALLEGRO_COLOR clrs[] = { al_map_rgb(250 - perc * 200, 4, 5 + perc * 100),al_map_rgb(180 - perc * 140, 27, 80 + perc * 70),al_map_rgb(5, 200 + perc * 50, 0),al_map_rgb(115, 190 + perc * 50, 24) };
			draw_confuse_rectangle(-0.98, -0.95, 1.96 * perc - 0.98, 0.95, clrs[0], clrs[1], clrs[2], clrs[3]);
		}

		void draw_confuse_rectangle(const float x1, const float y1, const float x2, const float y2,
			const ALLEGRO_COLOR c1, const ALLEGRO_COLOR c2, const ALLEGRO_COLOR c3, const ALLEGRO_COLOR c4)
		{
			ALLEGRO_VERTEX v[] = {
				  { x1, y1, 0, 0, 0, c1}, //top left
				  { x2, y1, 0, 0, 0, c3}, //top right
				  { x1, y2, 0, 0, 0, c2}, //bottom left
				  { x2, y2, 0, 0, 0, c4}  //bottom right
			};
			al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
		}

		void draw_simple_txt(ALLEGRO_FONT* f, const std::string s, ALLEGRO_COLOR c, const int flag)
		{
			Camera gcam;
			camera_preset cp;
			cp.set(Assistance::io__camera_float::SCALE_G, 0.2 / Constants::text_default_sharpness_font);
			cp.set(Assistance::io__camera_float::SCALE_Y, 3.5);
			gcam.applyNoSave(cp);
			al_draw_text(f, c, 0, - 0.6 * Constants::text_default_sharpness_font, flag, s.c_str());
			gcam.apply();
		}


		void camera_preset::_think_latest()
		{
			ALLEGRO_DISPLAY* d = al_get_current_display();
			if (!d) return;

			al_identity_transform(&latest);

			al_build_transform(&latest,
			(al_get_display_width(d) / 2) * (1.0 + get(Assistance::io__camera_float::OFFSET_X) * (al_get_display_width(d) * 0.5 * get(Assistance::io__camera_float::SCALE_X) * get(Assistance::io__camera_float::SCALE_G))),
			(al_get_display_height(d) / 2) * (1.0 + get(Assistance::io__camera_float::OFFSET_Y) * (al_get_display_height(d) * 0.5 * get(Assistance::io__camera_float::SCALE_Y) * get(Assistance::io__camera_float::SCALE_G))),
			(al_get_display_width(d) / 2) * get(Assistance::io__camera_float::SCALE_X) * get(Assistance::io__camera_float::SCALE_G),
			(al_get_display_height(d) / 2) * get(Assistance::io__camera_float::SCALE_Y) * get(Assistance::io__camera_float::SCALE_G),
			get(Assistance::io__camera_float::ROTATION));
		}

		void camera_preset::reset()
		{
			p[0] = p[1] = p[2] = 1.0;
			p[3] = p[4] = p[5] = 0.0;
			al_identity_transform(&latest);
			layers.clear();
		}

		void camera_preset::set(const Assistance::io__camera_float u, const float v)
		{
			p[+u] = v;
			_think_latest();
		}

		void camera_preset::merge(const Assistance::io__camera_float u, const float v)
		{
			switch (u) {
			case Assistance::io__camera_float::SCALE_X:
			case Assistance::io__camera_float::SCALE_Y:
			case Assistance::io__camera_float::SCALE_G:
				p[+u] *= v;
				break;				
			case Assistance::io__camera_float::OFFSET_X:
			case Assistance::io__camera_float::OFFSET_Y:
			case Assistance::io__camera_float::ROTATION:
				p[+u] += v;
				break;
			}
			_think_latest();
		}

		float camera_preset::get(const Assistance::io__camera_float u)
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

		auto camera_preset::begin()
		{
			return layers.begin();
		}

		auto camera_preset::end()
		{
			return layers.end();
		}

		ALLEGRO_TRANSFORM& camera_preset::quick()
		{
			return latest;
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
			applyNoSave(&lastTransf.quick());
		}

		void Camera::apply()
		{
			apply(lastapply);
		}

		
		ALLEGRO_TRANSFORM Camera::applyNoSave(camera_preset lastTransf)
		{
			ALLEGRO_DISPLAY* d = al_get_current_display();

			if (!d) return ALLEGRO_TRANSFORM();

			al_identity_transform(&g_t);

			al_build_transform(&g_t,
				(al_get_display_width(d) / 2) * (1.0 + lastTransf.get(Assistance::io__camera_float::OFFSET_X) * (al_get_display_width(d) * 0.5 * lastTransf.get(Assistance::io__camera_float::SCALE_X) * lastTransf.get(Assistance::io__camera_float::SCALE_G))),
				(al_get_display_height(d) / 2) * (1.0 + lastTransf.get(Assistance::io__camera_float::OFFSET_Y) * (al_get_display_height(d) * 0.5 * lastTransf.get(Assistance::io__camera_float::SCALE_Y) * lastTransf.get(Assistance::io__camera_float::SCALE_G))),
				(al_get_display_width(d) / 2) * lastTransf.get(Assistance::io__camera_float::SCALE_X) * lastTransf.get(Assistance::io__camera_float::SCALE_G),
				(al_get_display_height(d) / 2) * lastTransf.get(Assistance::io__camera_float::SCALE_Y) * lastTransf.get(Assistance::io__camera_float::SCALE_G),
				lastTransf.get(Assistance::io__camera_float::ROTATION));

			al_use_transform(&g_t);
			return g_t;
		}

		void Camera::applyNoSave(ALLEGRO_TRANSFORM* tr)
		{
			al_use_transform(tr);
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
		ALLEGRO_BITMAP* Sprite::__sprite_smart_images::load(const std::string id)
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
			dval[+Assistance::io__sprite_double::SCALEX] = 1.0;
			dval[+Assistance::io__sprite_double::SCALEY] = 1.0;
			dval[+Assistance::io__sprite_double::SCALEG] = 1.0;
			//bval[+Assistance::io__sprite_boolean::DRAW] = false;
			bval[+Assistance::io__sprite_boolean::AFFECTED_BY_CAM] = true;
		}





		Sprite::~Sprite()
		{
			bmps.reset();
		}

		void Sprite::set(const Assistance::io__sprite_string_vector u, const std::vector<std::string> v, float* perc)
		{
			if (perc) *perc = 0.00f;
			size_t countt = 0;
			switch (u) {
			case Assistance::io__sprite_string_vector::ADDMULTIPLE:
				for (auto& i : v) {
					bmps.load(i);
					if (perc) *perc = 1.00f * countt++ / v.size();
				}
				break;
			case Assistance::io__sprite_string_vector::REMOVEMULTIPLE:
				for (auto& i : v) {
					bmps.remove(i);
					if (perc) *perc = 1.00f * countt++ / v.size();
				}
				break;
			}
			if (perc) *perc = 1.00f;
		}

		void Sprite::set(const Assistance::io__sprite_string u, const std::string v)
		{
			switch (u) {
			case Assistance::io__sprite_string::ADD:
				bmps.load(v);
				break;
			case Assistance::io__sprite_string::REMOVE:
				bmps.remove(v);
				break;
			case Assistance::io__sprite_string::ID:
				sprite_id = v;
				break;
			}
		}
		void Sprite::set(const Assistance::io__sprite_double u, const double v)
		{
			switch (u) {
			case Assistance::io__sprite_double::ANIMATION_FPS:
				bmps.setFPS(v);
				break;
			default:
				data.dval[+u] = v;
				break;
			}
		}
		void Sprite::set(const Assistance::io__sprite_boolean u, const bool v)
		{
			switch (u) {
			case Assistance::io__sprite_boolean::LOOPFRAMES:
				bmps.loop(v);
				break;
			default:
				data.bval[+u] = v;
				break;
			}
		}
		void Sprite::set(const Assistance::io__sprite_integer u, const int v)
		{
			switch (u) {
			case Assistance::io__sprite_integer::LAYER:
				layer = v;
				break;
			}
		}
		void Sprite::set(const Assistance::io__sprite_sizet u, const size_t v)
		{
			switch (u) {
			case Assistance::io__sprite_sizet::FRAME:
				bmps.setFPS(-(int)(v));
				break;
			}
		}
		void Sprite::set(const Assistance::io__sprite_color u, const ALLEGRO_COLOR v)
		{
			switch (u) {
			case Assistance::io__sprite_color::TINT:
				data.tint = v;
				break;
			}
		}

		bool Sprite::get(const Assistance::io__sprite_string u, std::string& v)
		{
			switch (u) {
			case Assistance::io__sprite_string::ADD:
			case Assistance::io__sprite_string::REMOVE:
				// no sense
				return false;
			case Assistance::io__sprite_string::ID:
				v = sprite_id;
				return true;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io__sprite_double u, double& v)
		{
			switch (u) {
			case Assistance::io__sprite_double::ANIMATION_FPS:
				// no way
				return false;
			default:
				v = data.dval[+u];
				return true;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io__sprite_boolean u, bool& v)
		{
			switch (u) {
			case Assistance::io__sprite_boolean::LOOPFRAMES:
				// no way
				return false;
			default:
				v = data.bval[+u];
				return true;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io__sprite_integer u, int& v)
		{
			switch (u) {
			case Assistance::io__sprite_integer::LAYER:
				v = layer;
				return true;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io__sprite_sizet u, size_t& v)
		{
			switch (u) {
			case Assistance::io__sprite_sizet::FRAME:
				// no way
				return false;
			}
			return false;
		}

		bool Sprite::get(const Assistance::io__sprite_color u, ALLEGRO_COLOR& v)
		{
			switch (u) {
			case Assistance::io__sprite_color::TINT:
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
			camera_preset psf = camm.get();

			if (!data.bval[+Assistance::io__sprite_boolean::AFFECTED_BY_CAM]) psf = camera_preset();

			float cx, cy, px, py, dsx, dsy, rot_rad;
			int bmpx, bmpy;
			bmpx = al_get_bitmap_width(rn);
			bmpy = al_get_bitmap_height(rn);
			if (bmpx <= 0 || bmpy <= 0) {
				throw Abort::abort("al_get_bitmap_width|al_get_bitmap_height", "Sprite::draw", "Somehow the texture have < 0 width / height id=[" + this->sprite_id + "] size={" + std::to_string(bmpx) + "," + std::to_string(bmpy) + "}", 1);
			}

			cx = 1.0f * bmpx * ((data.dval[+Assistance::io__sprite_double::CENTERX] + 1.0) * 0.5);
			cy = 1.0f * bmpy * ((data.dval[+Assistance::io__sprite_double::CENTERY] + 1.0) * 0.5);
			rot_rad = 1.0f * data.dval[+Assistance::io__sprite_double::ROTATION] * ALLEGRO_PI / 180.0;
			px = 1.0f * data.dval[+Assistance::io__sprite_double::POSX] * cos(rot_rad) + data.dval[+Assistance::io__sprite_double::POSY] * sin(rot_rad);
			py = 1.0f * data.dval[+Assistance::io__sprite_double::POSY] * cos(rot_rad) - data.dval[+Assistance::io__sprite_double::POSX] * sin(rot_rad);
			dsx = 1.0f * data.dval[+Assistance::io__sprite_double::SCALEX] * data.dval[+Assistance::io__sprite_double::SCALEG] * (1.0 / bmpx);
			dsy = 1.0f * data.dval[+Assistance::io__sprite_double::SCALEY] * data.dval[+Assistance::io__sprite_double::SCALEG] * (1.0 / bmpy);

			// draw
			camm.applyNoSave(psf);

			if (data.bval[+Assistance::io__sprite_boolean::USE_TINTED_DRAWING])	al_draw_tinted_scaled_rotated_bitmap(rn, data.tint, cx, cy, px, py, dsx, dsy, rot_rad, 0);
			else																    al_draw_scaled_rotated_bitmap(rn, cx, cy, px, py, dsx, dsy, rot_rad, 0);

			// debug

			if (data.bval[+Assistance::io__sprite_boolean::SHOWBOX] || data.bval[+Assistance::io__sprite_boolean::SHOWDOT])
			{
				ALLEGRO_COLOR colorr = al_map_rgb(0, 255, 0);
				camera_preset psf = camm.get();

				if (data.bval[+Assistance::io__sprite_boolean::IS_COLLIDING]) colorr = al_map_rgb(255, 0, 0);
				//else if (al_get_time() - lastresetcollisioncall < Defaults::diff_time_show_last_resetCollision) colorr = al_map_rgb(255, 255, 0);

				if (data.bval[+Assistance::io__sprite_boolean::SHOWBOX]) {
					al_draw_filled_circle(px - cx * dsx, py - cy * dsy, 0.1f * fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px - cx * dsx, py + cy * dsy, 0.1f * fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px + cx * dsx, py - cy * dsy, 0.1f * fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f), colorr);
					al_draw_filled_circle(px + cx * dsx, py + cy * dsy, 0.1f * fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f), colorr);
				}

				if (data.bval[+Assistance::io__sprite_boolean::SHOWDOT])
				{
					al_draw_filled_circle(px, py, 0.1f * fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f), colorr);
				}
			}

			camm.apply();
		}








		void Text::_draw(const double targ_draw_xy[2])
		{
			al_draw_text(data.font, data.c, 1.0 * targ_draw_xy[0] / (data.d[+Assistance::io__text_double::SCALEG]), 1.0 * targ_draw_xy[1] / (data.d[+Assistance::io__text_double::SCALEG]), data.i[+Assistance::io__text_integer::MODE], data.str[+Assistance::io__text_string::PROCESSED_STRING].c_str());
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
						sprintf_s(tempstr_c, "%.3lf", (data.d[+Assistance::io__text_double::POSX] + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX] + cam.get().get(Assistance::io__camera_float::OFFSET_X)));
						break;
					case +Assistance::tags_e::T_POSY:
						sprintf_s(tempstr_c, "%.3lf", (data.d[+Assistance::io__text_double::POSY] + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY] + cam.get().get(Assistance::io__camera_float::OFFSET_Y)));
						break;
					case +Assistance::tags_e::T_SCREEN_POSX:
						sprintf_s(tempstr_c, "%.3lf", (data.d[+Assistance::io__text_double::POSX] * (data.b[+Assistance::io__text_boolean::AFFECTED_BY_CAM] ? (cam.get().get(Assistance::io__camera_float::SCALE_G) * cam.get().get(Assistance::io__camera_float::SCALE_X)) : 1.0) + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX] * (cam.get().get(Assistance::io__camera_float::SCALE_G) * cam.get().get(Assistance::io__camera_float::SCALE_X))));
						break;
					case +Assistance::tags_e::T_SCREEN_POSY:
						sprintf_s(tempstr_c, "%.3lf", (data.d[+Assistance::io__text_double::POSY] * (data.b[+Assistance::io__text_boolean::AFFECTED_BY_CAM] ? (cam.get().get(Assistance::io__camera_float::SCALE_G) * cam.get().get(Assistance::io__camera_float::SCALE_Y)) : 1.0) + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY] * (cam.get().get(Assistance::io__camera_float::SCALE_G) * cam.get().get(Assistance::io__camera_float::SCALE_Y))));
						break;

					case +Assistance::tags_e::T_CAM_X:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io__camera_float::OFFSET_X));
						break;
					case +Assistance::tags_e::T_CAM_Y:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io__camera_float::OFFSET_Y));
						break;
					case +Assistance::tags_e::T_CAM_ZOOM:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io__camera_float::SCALE_G));
						break;
					case +Assistance::tags_e::T_CAM_ZOOMX:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io__camera_float::SCALE_X));
						break;
					case +Assistance::tags_e::T_CAM_ZOOMY:
						sprintf_s(tempstr_c, "%.3lf", cam.get().get(Assistance::io__camera_float::SCALE_Y));
						break;

					case +Assistance::tags_e::T_ISFOLLOWING:
						sprintf_s(tempstr_c, "%s", (follow ? "Y" : "N"));
						break;
					case +Assistance::tags_e::T_COLOR_R:
						sprintf_s(tempstr_c, "%.3f", data.c.r);
						break;
					case +Assistance::tags_e::T_COLOR_G:
						sprintf_s(tempstr_c, "%.3f", data.c.g);
						break;
					case +Assistance::tags_e::T_COLOR_B:
						sprintf_s(tempstr_c, "%.3f", data.c.b);
						break;
					case +Assistance::tags_e::T_COLOR_A:
						sprintf_s(tempstr_c, "%.3f", data.c.a);
						break;
					case +Assistance::tags_e::T_MODE:
						sprintf_s(tempstr_c, "%d", data.i[+Assistance::io__text_integer::MODE]);
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
							follow->get(Assistance::io__sprite_double::SPEEDX, val);
							sprintf_s(tempstr_c, "%.3lf", val);
						}
						else sprintf_s(tempstr_c, "UNDEF");
						break;
					case +Assistance::tags_e::T_SPRITE_SPEEDY:
						if (follow) {
							double val;
							follow->get(Assistance::io__sprite_double::SPEEDY, val);
							sprintf_s(tempstr_c, "%.3lf", val);
						}
						else sprintf_s(tempstr_c, "UNDEF");
						break;
					case +Assistance::tags_e::T_SPRITE_NAME:
						if (follow) {
							std::string temp;
							follow->get(Assistance::io__sprite_string::ID, temp);
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




		Text::Text()
		{
			gfile logg;
			logg << L::SLL << fsr(__FUNCSIG__) << "Registered spawn of Text #" + std::to_string((size_t)this) << L::BLL;
			logg.flush();

			data.d[+Assistance::io__text_double::POSX] = 0.0;
			data.d[+Assistance::io__text_double::POSY] = 0.0;
			data.d[+Assistance::io__text_double::ROTATION] = 0.0;
			data.d[+Assistance::io__text_double::SCALEG] = 1.0;
			data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX] = 0.0;
			data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY] = 0.0;
			data.d[+Assistance::io__text_double::LAST_INTERPRET] = 0.0;
			data.d[+Assistance::io__text_double::UPDATETIME] = 1.0 / 24;

			data.i[+Assistance::io__text_integer::LAYER] = 0;
			data.i[+Assistance::io__text_integer::MODE] = ALLEGRO_ALIGN_CENTER;

			data.b[+Assistance::io__text_boolean::AFFECTED_BY_CAM] = true;
			data.b[+Assistance::io__text_boolean::SHOW] = true;

			data.font = nullptr;
			data.c = al_map_rgb(255, 255, 255);
		}

		Text::~Text()
		{
			gfile logg;
			logg << L::SLL << fsr(__FUNCSIG__) << "Registered despawn of Text #" + std::to_string((size_t)this)/* << ";ID=" << id */ << L::BLL;
		}


		void Text::set(const Assistance::io__text_string o, const std::string e)
		{
			data.str[+o] = e;

			switch (o) {
			case Assistance::io__text_string::FOLLOW_SPRITE:
				setFollow(e);
				break;
			case Assistance::io__text_string::FONT:
				__template_static_vector<ALLEGRO_FONT> fonts;
				fonts.get(e, data.font);
				break;
			}
		}

		void Text::set(const Assistance::io__text_boolean o, const bool e)
		{
			data.b[+o] = e;
		}

		void Text::set(const Assistance::io__text_double o, const double e)
		{
			data.d[+o] = e;
		}

		void Text::set(const Assistance::io__text_color o, const ALLEGRO_COLOR e)
		{
			data.c = e;
		}

		void Text::set(const Assistance::io__text_integer o, const int e)
		{
			data.i[+o] = e;
		}


		void Text::get(const Assistance::io__text_string o, std::string& e)
		{
			e = data.str[+o];
		}
		void Text::get(const Assistance::io__text_boolean o, bool& e)
		{
			e = data.b[+o];
		}
		void Text::get(const Assistance::io__text_double o, double& e)
		{
			e = data.d[+o];
		}
		void Text::get(const Assistance::io__text_integer o, int& e)
		{
			e = data.i[+o];
		}
		void Text::get(const Assistance::io__text_color o, ALLEGRO_COLOR& e)
		{
			e = data.c;
		}

		const bool Text::isEq(const Assistance::io__text_color e, const ALLEGRO_COLOR v)
		{
			ALLEGRO_COLOR g;
			get(e, g);
			return ((g.r == v.r) && (g.g == v.g) && (g.b == v.b) && (g.a == v.a));
		}
		const bool Text::isEq(const Assistance::io__text_string e, const std::string v)
		{
			std::string g;
			get(e, g);
			return g == v;
		}
		const bool Text::isEq(const Assistance::io__text_boolean e, const bool v)
		{
			bool g;
			get(e, g);
			return g == v;
		}
		const bool Text::isEq(const Assistance::io__text_double e, const double v)
		{
			double g;
			get(e, g);
			return g == v;
		}
		const bool Text::isEq(const Assistance::io__text_integer e, const int v)
		{
			int g;
			get(e, g);
			return g == v;
		}


		void Text::draw(const int is_layer)
		{
			if (!data.b[+Assistance::io__text_boolean::SHOW]) return;
			if (is_layer != data.i[+Assistance::io__text_integer::LAYER]) {
				return;
			}
			if (follow) {
				bool k;
				follow->get(Assistance::io__sprite_boolean::DRAW, k);
				if (!k) return;
			}

			if (al_get_time() - data.d[+Assistance::io__text_double::LAST_INTERPRET] > data.d[+Assistance::io__text_double::UPDATETIME]) {
				
				if (al_get_time() - data.d[+Assistance::io__text_double::LAST_INTERPRET] > data.d[+Assistance::io__text_double::UPDATETIME] * 3) data.d[+Assistance::io__text_double::LAST_INTERPRET] = al_get_time();
				else data.d[+Assistance::io__text_double::LAST_INTERPRET] += data.d[+Assistance::io__text_double::UPDATETIME];
				
				std::string b4 = data.str[+Assistance::io__text_string::PROCESSED_STRING];
				data.str[+Assistance::io__text_string::PROCESSED_STRING] = data.str[+Assistance::io__text_string::STRING];
				_interpretTags(data.str[+Assistance::io__text_string::PROCESSED_STRING]);
			}

			while (data.d[+Assistance::io__text_double::ROTATION] > 360.0) data.d[+Assistance::io__text_double::ROTATION] -= 360.0;
			while (data.d[+Assistance::io__text_double::ROTATION] < 0.0) data.d[+Assistance::io__text_double::ROTATION] += 360.0;

			// follow sprite stuff
			double rotation = 0.0;

			Camera cam;
			camera_preset preset = cam.get();
			ALLEGRO_BITMAP* d = al_get_target_bitmap();
			if (!d) return;// throw "TEXT::DRAW - NO DISPLAY!";

			if (follow) {
				follow->get(Assistance::io__sprite_double::POSX, data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX]);
				follow->get(Assistance::io__sprite_double::POSY, data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY]);
				follow->get(Assistance::io__sprite_double::ROTATION, rotation);
			}
			else {
				data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX] = data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY] = 0.0;
			}

			double t_rotation_rad = (data.d[+Assistance::io__text_double::ROTATION])*ALLEGRO_PI / 180.0;
			double p_rotation_rad = (rotation)*ALLEGRO_PI / 180.0;
			double rotation_rad = t_rotation_rad + p_rotation_rad;

			if (rotation_rad > ALLEGRO_PI * 2) rotation_rad -= ALLEGRO_PI * 2;
			if (rotation_rad < 0) rotation_rad += ALLEGRO_PI * 2;

			double pos_now[2];
			pos_now[0] = 1.0 * Constants::text_default_sharpness_font * ((data.d[+Assistance::io__text_double::POSX] * cos(p_rotation_rad) - data.d[+Assistance::io__text_double::POSY] * sin(p_rotation_rad)) + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX]);
			pos_now[1] = 1.0 * Constants::text_default_sharpness_font * ((data.d[+Assistance::io__text_double::POSY] * cos(p_rotation_rad)) - data.d[+Assistance::io__text_double::POSX] * sin(p_rotation_rad) + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY]);


			double targ_draw_xy[2];
			targ_draw_xy[0] = pos_now[0] * cos(rotation_rad) + pos_now[1] * sin(rotation_rad);
			targ_draw_xy[1] = -pos_now[0] * sin(rotation_rad) + pos_now[1] * cos(rotation_rad);


			if (!data.b[+Assistance::io__text_boolean::AFFECTED_BY_CAM]) preset.reset();

			preset.set(Assistance::io__camera_float::SCALE_G,  preset.get(Assistance::io__camera_float::SCALE_G) * data.d[+Assistance::io__text_double::SCALEG] * 1.0 / Constants::text_default_sharpness_font);
			preset.set(Assistance::io__camera_float::OFFSET_X, preset.get(Assistance::io__camera_float::OFFSET_X) * Constants::text_default_sharpness_font / data.d[+Assistance::io__text_double::SCALEG]);
			preset.set(Assistance::io__camera_float::OFFSET_Y, preset.get(Assistance::io__camera_float::OFFSET_Y) * Constants::text_default_sharpness_font / data.d[+Assistance::io__text_double::SCALEG]);

			preset.set(Assistance::io__camera_float::ROTATION, preset.get(Assistance::io__camera_float::ROTATION) + rotation_rad);
			cam.applyNoSave(preset);

			_draw(targ_draw_xy);

			al_set_target_bitmap(d);
			cam.apply();
		}

	}
}