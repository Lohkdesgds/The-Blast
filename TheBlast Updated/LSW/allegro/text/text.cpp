#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Text {

			ALLEGRO_FONT* text::default_font = nullptr;
			Display::display* text::ref_disp = nullptr;
			unsigned text::text_count = 0;
			//Sprite::sprite* text::global_paint = nullptr;
			//size_t text::counter = 0;

			void text::_draw()
			{
				//Camera::camera_g cam;

				if (follow) {
					follow->get(Sprite::POSX, off_plr[0]);
					follow->get(Sprite::POSY, off_plr[1]);
				}
				else {
					off_plr[0] = off_plr[1] = 0.0;
				}


				double targ_draw_xy[2];

				if (!usebuf) {
					targ_draw_xy[0] = 1.0 * Defaults::sharpness_font *(pos[0] + off_plr[0]);// /*+ (affected_by_camera ? 0.0 : cam.get(Camera::OFFX))*/)* (affected_by_camera ? cam.get(Camera::ZOOM) * cam.get(Camera::ZOOMX) : 1.0);
					targ_draw_xy[1] = 1.0 * Defaults::sharpness_font *(pos[1] + off_plr[1]);// /*+ (affected_by_camera ? 0.0 : cam.get(Camera::OFFY))*/)* (affected_by_camera ? cam.get(Camera::ZOOM) * cam.get(Camera::ZOOMY) : 1.0);
				}
				else {
					targ_draw_xy[0] = targ_draw_xy[1] = 0.0;
					loadInternalBMP();
					if (local_paint)
					{
						local_paint->set(Sprite::POSX, (pos[0] + off_plr[0]));
						local_paint->set(Sprite::POSY, (pos[1] + off_plr[1]));
					}
				}

				al_draw_text(default_font, color, targ_draw_xy[0], targ_draw_xy[1], mode, string.g().c_str());
			}
			void text::_interpretTags(Safer::safe_string& s)
			{
				std::string local_t = s.gC();

				std::vector<size_t> posicoes;

				for (size_t p = local_t.find('%'); (p != std::string::npos); p = local_t.find('%'))
				{
					if (p == 0 || (local_t[p - 1] != '\\')) {
						std::string elsee = local_t.substr(p);
						Safer::safe_string substitute;

						size_t found = 0;
						bool rfound = false;
						for (auto& i : tags)
						{
							if (elsee.find(i) == 0)
							{
								rfound = true;
								break;
							}
							found++;
						}

						Camera::camera_g cam;
						ALLEGRO_DISPLAY* d = al_get_current_display();
						char tempstr_c[128];
						Events::big_event bev;

						switch (found)
						{
						case T_POSX:
							sprintf_s(tempstr_c, "%.3lf", (pos[0] + off_plr[0] + cam.get(cam.getLastApplyID(), Camera::OFFX)));
							break;
						case T_POSY:
							sprintf_s(tempstr_c, "%.3lf", (pos[1] + off_plr[1] + cam.get(cam.getLastApplyID(), Camera::OFFY)));
							break;
						case T_SCREEN_POSX:
							sprintf_s(tempstr_c, "%.3lf", (pos[0] * (affected_by_camera ? cam.get(cam.getLastApplyID(), Camera::ZOOM) : 1.0) + off_plr[0] * cam.get(cam.getLastApplyID(), Camera::ZOOM)));
							break;
						case T_SCREEN_POSY:
							sprintf_s(tempstr_c, "%.3lf", (pos[1] * (affected_by_camera ? cam.get(cam.getLastApplyID(), Camera::ZOOM) : 1.0) + off_plr[1] * cam.get(cam.getLastApplyID(), Camera::ZOOM)));
							break;

						case T_CAM_X:
							sprintf_s(tempstr_c, "%.3lf", cam.get(cam.getLastApplyID(), Camera::OFFX));
							break;
						case T_CAM_Y:
							sprintf_s(tempstr_c, "%.3lf", cam.get(cam.getLastApplyID(), Camera::OFFY));
							break;
						case T_CAM_ZOOM:
							sprintf_s(tempstr_c, "%.3lf", cam.get(cam.getLastApplyID(), Camera::ZOOM));
							break;

						case T_ISFOLLOWING:
							sprintf_s(tempstr_c, "%s", (follow ? "Y" : "N"));
							break;
						case T_COLOR_R:
							sprintf_s(tempstr_c, "%.3f", color.r);
							break;
						case T_COLOR_G:
							sprintf_s(tempstr_c, "%.3f", color.g);
							break;
						case T_COLOR_B:
							sprintf_s(tempstr_c, "%.3f", color.b);
							break;
						case T_COLOR_A:
							sprintf_s(tempstr_c, "%.3f", color.a);
							break;
						case T_MODE:
							sprintf_s(tempstr_c, "%d", mode);
							break;
						case T_TIME:
							sprintf_s(tempstr_c, "%.3lf", al_get_time());
							break;
						case T_ISUSINGBUF:
							sprintf_s(tempstr_c, "%s", (usebuf ? "Y" : "N"));
							break;
						case T_GB_RESX:
							sprintf_s(tempstr_c, "%d", (d ? al_get_display_width(d) : -1));
							break;
						case T_GB_RESY:
							sprintf_s(tempstr_c, "%d", (d ? al_get_display_height(d) : -1));
							break;
						case T_REFRESHRATE:
							sprintf_s(tempstr_c, "%d", ((d) ? al_get_display_refresh_rate(d) : -1));
							break;
						case T_FPS:
							if (ref_disp) sprintf_s(tempstr_c, "%.2f", ref_disp->getFPS());
							else sprintf_s(tempstr_c, "NOT_AVAILABLE");
							break;
						case T_TPS:
							sprintf_s(tempstr_c, "%u", bev.g().getCurrentMaxTickCounted());
							break;
						case T_SPRITE_FRAME:
							if (follow) sprintf_s(tempstr_c, "%lu", follow->_lastFrameNumber());
							else sprintf_s(tempstr_c, "NOT_AVAILABLE");
							break;
						case T_MOUSE_X:
							{
								float x;
								bev.g().getMouse(x, Events::X);
								sprintf_s(tempstr_c, "%.3f", x);
							}
							break;
						case T_MOUSE_Y:
							{
								float y;
								bev.g().getMouse(y, Events::Y);
								sprintf_s(tempstr_c, "%.3f", y);
							}
							break;
						case T_LASTSTRING:
							{
								Safer::safe_string str;
								bev.g().getLastString(str);
								sprintf_s(tempstr_c, "%s", str.g().c_str());
							}
							break;
						case T_CURRSTRING:
							{
								Safer::safe_string str;
								bev.g().getCurrentString(str);
								sprintf_s(tempstr_c, "%s", str.g().c_str());
							}
							break;
						case T_SPRITE_SPEEDX:
							if (follow) {
								double val;
								follow->get(Sprite::SPEEDX, val);
								sprintf_s(tempstr_c, "%.3lf", val);
							}
							else sprintf_s(tempstr_c, "UNDEF");
							break;
						case T_SPRITE_SPEEDY:
							if (follow) {
								double val;
								follow->get(Sprite::SPEEDY, val);
								sprintf_s(tempstr_c, "%.3lf", val);
							}
							else sprintf_s(tempstr_c, "UNDEF");
							break;
						}

						substitute = tempstr_c;

						std::string change;


						if (p > 0)
						{
							change = local_t.substr(0, p);
						}
						change += substitute.gC() + local_t.substr(p + tags[found].length());
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

			void text::setFollow(const Safer::safe_string u)
			{
				d_sprite_database spr_data;
				Sprite::sprite* spr = nullptr;
				if (spr_data.get(spr, u))
				{
					setFollow(spr);
				}
			}

			void text::setFollow(Sprite::sprite* u)
			{
				follow = u;
			}

			void text::loadInternalBMP()
			{
				if (!usebuf) return;

				if (!local_paint)
				{
					ALLEGRO_BITMAP* d = al_get_target_bitmap();
					assert(d);

					d_sprite_database spr_data;
					d_images_database img_data;

					spr_data.create(local_paint);
					img_data.create(local_paint_i);

					local_paint_i->setID("_TEXT_" + text_count);
					local_paint_i->create(al_get_bitmap_width(d), al_get_bitmap_height(d));

					local_paint->add("_TEXT_" + text_count);
					//local_paint->set(Sprite::AFFECTED_BY_CAM, false);
					//local_paint->set(Sprite::DRAW, false);
					local_paint->set(Sprite::SCALEG, 2.0);
					local_paint->set(Sprite::LAYER, layer);
					///local_paint->set(Sprite::SHOWBOX, true);

					text_count++;
				}
			}

			void text::verify(const bool reload)
			{
				if (reload) {
					unload();
				}
				if (!default_font) {
					if (path.g().length() == 0) path = Defaults::font_default_name;
					load(path);
				}
				loadInternalBMP();
			}

			const bool text::load(const Safer::safe_string p)
			{
				if (!default_font) {
					//Camera::camera_g cam;

					Safer::safe_string s = (Defaults::default_data_path.g() + p.g());
					Tools::interpret_path(s);

					path = s;

					return (default_font = al_load_ttf_font(s.g().c_str(), 1.0 * Defaults::sharpness_font / 20.0/*(sqrt(cam.get(Camera::BUF_X)*cam.get(Camera::BUF_Y)))*(2.5 / 144)*/, 0));
				}
				return true;
			}

			const bool text::loadFromDatabase(const Safer::safe_string newname)
			{
				Safer::safe_string s = Defaults::initial_call_url.g() + newname.g();
				Tools::clearPath(s, true);
				return loadFromURL(s, newname);
			}

			const bool text::loadFromURL(const Safer::safe_string url, Safer::safe_string s_o)
			{
				if (!load(s_o)) assert(Tools::saveFromCloud(url, s_o));
				else return true;
				return load(s_o);
			}

			void text::unload()
			{
				if (default_font) {
					al_destroy_font(default_font);
					default_font = nullptr;
				}
			}


			void text::set(const _text_opt_str o, const Safer::safe_string e)
			{
				switch (o) {
				case SETSTRING:
					orig_str = e;
					break;
				case SETID:
					id = e;
					break;
				case SETFOLLOW:
					setFollow(e);
					break;
				}
			}
			void text::set(const _text_opt_bool o, const bool e)
			{
				switch (o) {
				case SHOW:
					show = e;
					break;
				case USEBUFOPT:
					usebuf = e;
					if (usebuf) loadInternalBMP();
					break;
				case AFFECTED_BY_CAM:
					affected_by_camera = e;
					break;
				}
			}
			void text::set(const _text_opt_db o, const double e)
			{
				switch (o) {
				case POSX:
					pos[0] = e;
					break;
				case POSY:
					pos[1] = e;
					break;
				case UPDATETIME:
					update_time = e;
					break;
				}
			}
			void text::set(const _text_opt_color o, const ALLEGRO_COLOR e)
			{
				switch (o) {
				case COLOR:
					color = e;
					break;
				}
			}
			void text::set(const _text_opt_int o, const int e)
			{
				switch (o) {
				case MODE:
					mode = e;
					break;
				case LAYER:
					layer = e;
					if (usebuf) {
						loadInternalBMP();
						local_paint->set(Sprite::LAYER, layer);
					}
					break;
				}
			}

			void text::get(const _text_opt_str o, Safer::safe_string& e)
			{
				switch (o) {
				case SETSTRING:
					e = string;
					break;
				case SETID:
					e = id;
					break;
				case SETFOLLOW:
					if (follow) follow->getID(e);
					break;
				}
			}
			void text::get(const _text_opt_bool o, bool& e)
			{
				switch (o) {
				case SHOW:
					e = show;
					break;
				case USEBUFOPT:
					e = usebuf;
					break;
				case AFFECTED_BY_CAM:
					e = affected_by_camera;
					break;
				}
			}
			void text::get(const _text_opt_db o, double& e)
			{
				switch (o) {
				case POSX:
					e = pos[0];
					break;
				case POSY:
					e = pos[1];
					break;
				case UPDATETIME:
					e = update_time;
					break;
				}
			}
			void text::get(const _text_opt_int o, int& e)
			{
				switch (o) {
				case MODE:
					e = mode;
					break;
				case LAYER:
					e = layer;
					if (local_paint) {
						local_paint->set(Sprite::LAYER, layer);
					}
					break;
				}
			}
			void text::get(const _text_opt_color o, ALLEGRO_COLOR& e)
			{
				switch (o) {
				case COLOR:
					e = color;
					break;
				}
			}

			void text::setID(const Safer::safe_string u)
			{
				set(Text::SETID, u);
			}

			const bool text::amI(const Safer::safe_string o)
			{
				return (id == o);
			}

			void text::setMainDisplay(Display::display* d)
			{
				ref_disp = d;
			}

			
			void text::draw(const int is_layer)
			{
				if (!show) return;
				if (is_layer != layer) {
					//lastinterpret = al_get_time() - update_time;
					return;
				}

				if (!default_font) {
					verify();
					return;
				}
				if (al_get_time() - lastinterpret > update_time) {
					if(lastinterpret == 0) lastinterpret = al_get_time();
					else {
						if (al_get_time() - lastinterpret > update_time * 3) lastinterpret = al_get_time();
						else lastinterpret += update_time;
					}

					Safer::safe_string b4 = string;
					string = orig_str;
					_interpretTags(string);

					if (string != b4) {
						str_upd = true;
					}
				}

				Camera::camera_g cam;
				const int lastapply = cam.getLastApplyID();
				ALLEGRO_BITMAP* d = al_get_target_bitmap();
				assert(d);

				if (usebuf) {
					
					loadInternalBMP();

					if (str_upd) {
						ALLEGRO_BITMAP* me;
						local_paint_i->get(me);

						al_set_target_bitmap(me);
						al_clear_to_color(al_map_rgba(0, 0, 0, 0));

						cam.copy(Defaults::default_layer_backup, lastapply);

						if (affected_by_camera)
						{
							cam.set(cam.getLastApplyID(), Camera::ZOOM, cam.get(lastapply, Camera::ZOOM) * 1.0 / Defaults::sharpness_font);
							cam.set(cam.getLastApplyID(), Camera::OFFX, cam.get(lastapply, Camera::OFFX) * Defaults::sharpness_font);
							cam.set(cam.getLastApplyID(), Camera::OFFY, cam.get(lastapply, Camera::OFFY) * Defaults::sharpness_font);
						}
						else {
							cam.reset(lastapply);
							cam.set(cam.getLastApplyID(), Camera::ZOOM, 1.0 / Defaults::sharpness_font);
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
				else {
					cam.copy(Defaults::default_layer_backup, lastapply);
					
					if (affected_by_camera)
					{
						cam.set(cam.getLastApplyID(), Camera::ZOOM, cam.get(lastapply, Camera::ZOOM) * 1.0 / Defaults::sharpness_font);
						cam.set(cam.getLastApplyID(), Camera::OFFX, cam.get(lastapply, Camera::OFFX) * Defaults::sharpness_font);
						cam.set(cam.getLastApplyID(), Camera::OFFY, cam.get(lastapply, Camera::OFFY) * Defaults::sharpness_font);
					}
					else {
						cam.reset(lastapply);
						cam.set(cam.getLastApplyID(), Camera::ZOOM, 1.0 / Defaults::sharpness_font);
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
			





			size_t _find(const Safer::safe_string s, Safer::safe_vector<text*>& v, bool& u)
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
				d_texts_database txt_data;
				Camera::camera_g cam;

				/*int l0 = cam.get(Camera::LAYERLOWER);
				int l1 = cam.get(Camera::LAYERHIGHER);*/

				for (auto& p : cam.getLayers(cam.getLastApplyID())) {
					if (p.second) {
						for (int u = 0; u < (int)txt_data.work().getMax(); u++)
						{
							txt_data.work().get(u)->draw(p.first);
						}
					}
				}
			}
		}
	}
}