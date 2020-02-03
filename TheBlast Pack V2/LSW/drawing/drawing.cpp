#include "drawing.h"


namespace LSW {
	namespace v4 {

		std::map<int, Camera::paired> Camera::presets;
		int Camera::lastapply = 0;


		/*const Constants::cl__sprite_direction_mult resolveDir(const int d)
		{
			if ((d & +Constants::directions::NORTH) && (d & +Constants::directions::SOUTH) && (d & +Constants::directions::WEST) && (d & +Constants::directions::EAST))
				return Constants::cl__sprite_direction_mult::STAY;

			if ((d & +Constants::directions::NORTH) && (d & +Constants::directions::SOUTH))
				return Constants::cl__sprite_direction_mult::GO_EAST;

			if ((d & +Constants::directions::WEST) && (d & +Constants::directions::EAST))
				return Constants::cl__sprite_direction_mult::GO_NORTH;


			switch (d)
			{
			case +Constants::directions::NORTH: // block is north
				return Constants::cl__sprite_direction_mult::GO_SOUTH;
			case +Constants::directions::SOUTH: // block is south
				return Constants::cl__sprite_direction_mult::GO_NORTH;
			case +Constants::directions::EAST: // block is east
				return Constants::cl__sprite_direction_mult::GO_WEST;
			case +Constants::directions::WEST: // block is west
				return Constants::cl__sprite_direction_mult::GO_EAST;
			}

			return Constants::cl__sprite_direction_mult::GO_NORTH;
		}*/

		void draw_simple_bar(const double perc, const ALLEGRO_COLOR bg, const double w, const double h)
		{
			Camera gcam;
			gcam.applyNoSave(camera_preset());
			al_clear_to_color(bg);
			ALLEGRO_COLOR clrs[] = { al_map_rgb(250 - perc * 200, 4, 5 + perc * 100),al_map_rgb(180 - perc * 140, 27, 80 + perc * 70),al_map_rgb(5, 200 + perc * 50, 0),al_map_rgb(115, 190 + perc * 50, 24) };
			draw_confuse_rectangle(-w, -h, 2 * w * perc - w, h, clrs[0], clrs[1], clrs[2], clrs[3]);
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

		void draw_simple_txt(ALLEGRO_FONT* f, const std::string s, ALLEGRO_COLOR c, const int flag, const double scale)
		{
			Camera gcam;
			camera_preset cp;
			cp.set(Constants::io__camera_double::SCALE_G, scale / Constants::text_default_sharpness_font);
			cp.set(Constants::io__camera_double::SCALE_Y, 2.5);
			gcam.applyNoSave(cp);
			al_draw_text(f, c, 0, - 0.6 * Constants::text_default_sharpness_font, flag, s.c_str());
			gcam.apply();
		}

		ALLEGRO_TRANSFORM easyTransform(ALLEGRO_DISPLAY* const d, const float x, const float y, const float sx, const float sy, const float r)
		{
			ALLEGRO_TRANSFORM t;

			al_identity_transform(&t);

			al_rotate_transform(&t,	r);

			al_scale_transform(&t,
 				al_get_display_width(d) * 0.5 * sx,
				al_get_display_height(d) * 0.5 * sy);

			al_translate_transform(&t,
				al_get_display_width(d) * 0.5 - x * (al_get_display_width(d) * 0.5 * sx), 
				al_get_display_height(d) * 0.5 - y * (al_get_display_height(d) * 0.5 * sy));

			return t;
		}
		ALLEGRO_TRANSFORM easyTransform(ALLEGRO_BITMAP* const d, const float x, const float y, const float sx, const float sy, const float r)
		{
			ALLEGRO_TRANSFORM t;

			al_identity_transform(&t);

			al_rotate_transform(&t,	r);

			al_scale_transform(&t,
 				al_get_bitmap_width(d) * 0.5 * sx,
				al_get_bitmap_height(d) * 0.5 * sy);

			al_translate_transform(&t,
				al_get_bitmap_width(d) * 0.5 - x * (al_get_bitmap_width(d) * 0.5 * sx),
				al_get_bitmap_height(d) * 0.5 - y * (al_get_bitmap_height(d) * 0.5 * sy));

			return t;
		}

		void matrix_draw_help()
		{
			double m[2] = { 0.0 };
			double limits[4] = { 0.0 };
			Database db;
			Camera cam;
			camera_preset psf = cam.get();
			camera_preset org = camera_preset();

			db.get(Constants::ro__db_mouse_double::RAW_MOUSE_X, m[0]);
			db.get(Constants::ro__db_mouse_double::RAW_MOUSE_Y, m[1]);

			limits[0] = psf.get(Constants::io__camera_double::LIMIT_MIN_X);
			limits[1] = psf.get(Constants::io__camera_double::LIMIT_MIN_Y);
			limits[2] = psf.get(Constants::io__camera_double::LIMIT_MAX_X);
			limits[3] = psf.get(Constants::io__camera_double::LIMIT_MAX_Y);

			psf.refresh();

			float mt[2];
			for (short n = 0; n < 2; n++) mt[n] = +m[n];

			ALLEGRO_TRANSFORM untransf = psf.quick();
			al_invert_transform(&untransf);
			al_transform_coordinates(&untransf, &mt[0], &mt[1]);

			m[0] = +mt[0];
			m[1] = +mt[1];

			ALLEGRO_COLOR co = al_map_rgba_f(0.5, 0.5, 0.5, 0.5);
			ALLEGRO_COLOR cp = al_map_rgba_f(0.5, 0.25, 0.0, 0.5);
			ALLEGRO_COLOR cp2 = al_map_rgba_f(0.3, 0.08, 0.0, 0.3);

			cam.applyNoSave(org);

			// +
			al_draw_line(-5, 0, 5, 0, co, 0.007);
			al_draw_line(0, -5, 0, 5, co, 0.007);

			// |---|
			al_draw_line(-1, 0.1, -1, -0.1, co, 0.007);
			al_draw_line(1, 0.1, 1, -0.1, co, 0.007);
			al_draw_line(0.1, -1, -0.1, -1, co, 0.007);
			al_draw_line(0.1, 1, -0.1, 1, co, 0.007);

			// mouse
			al_draw_filled_circle(m[0], m[1], 0.018, al_map_rgba_f(0.75, 0, 0, 0.75)); // mouse is different color

			cam.applyNoSave(psf);

			// +
			al_draw_line(-5, 0, 5, 0, cp, 0.01);
			al_draw_line(0, -5, 0, 5, cp, 0.01);

			// x
			al_draw_line(-0.1, 0.1, 0.1, -0.1, cp, 0.008);
			al_draw_line(-0.1, -0.1, 0.1, 0.1, cp, 0.008);

			// |---|
			al_draw_line(-1, 0.1, -1, -0.1, cp, 0.005);
			al_draw_line(1, 0.1, 1, -0.1, cp, 0.005);
			al_draw_line(0.1, -1, -0.1, -1, cp, 0.005);
			al_draw_line(0.1, 1, -0.1, 1, cp, 0.005);

			// limits
			if (psf.get(Constants::io__camera_boolean::RESPECT_LIMITS)) al_draw_rectangle(limits[0], limits[1], limits[2], limits[3], cp2, 0.010);


			cam.apply();
		}


		bool camera_preset::canWrite()
		{
			return !b[+Constants::io__camera_boolean::READONLY_NOW];
		}

		void camera_preset::refresh()
		{
			ALLEGRO_DISPLAY* d = al_get_current_display();
			if (!d) return;

			latest = easyTransform(d,
				p[+Constants::io__camera_double::OFFSET_X], p[+Constants::io__camera_double::OFFSET_Y], p[+Constants::io__camera_double::SCALE_X] * p[+Constants::io__camera_double::SCALE_G], p[+Constants::io__camera_double::SCALE_Y] * p[+Constants::io__camera_double::SCALE_G], p[+Constants::io__camera_double::ROTATION_RAD]);
		}

		void camera_preset::reset()
		{
			if (!canWrite()) {
				throw Abort::abort(__FUNCSIG__, "Attempted to reset a camera_preset that has the flag READONLY set!");
				return;
			}
			p[0] = p[1] = p[2] = 1.0;
			p[3] = p[4] = p[5] = 0.0;
			al_identity_transform(&latest);
			layers.clear();
		}

		void camera_preset::set(const Constants::io__camera_double u, const double v)
		{
			if (!canWrite()) {
				throw Abort::abort(__FUNCSIG__, "Attempted to set a value to a camera_preset that has the flag READONLY set!");
				return;
			}

			if (u == Constants::io__camera_double::size) throw Abort::abort(__FUNCSIG__, "Shouldn't try to set ::size!");
			p[+u] = v;
			refresh();
		}

		void camera_preset::set(const Constants::io__camera_boolean u, const bool v)
		{
			if (u != Constants::io__camera_boolean::READONLY_NOW && !canWrite()) { // there has to be a way to unlock
				throw Abort::abort(__FUNCSIG__, "Attempted to set a value to a camera_preset that has the flag READONLY set!");
				return;
			}
			if (u == Constants::io__camera_boolean::size) throw Abort::abort(__FUNCSIG__, "Shouldn't try to set ::size!");
			b[+u] = v;
		}

		void camera_preset::merge(const Constants::io__camera_double u, const double v)
		{
			if (!canWrite()) {
				throw Abort::abort(__FUNCSIG__, "Attempted to merge a value to a camera_preset that has the flag READONLY set!");
				return;
			}

			switch (u) {
			case Constants::io__camera_double::SCALE_X:
			case Constants::io__camera_double::SCALE_Y:
			case Constants::io__camera_double::SCALE_G:
				p[+u] *= v;
				break;				
			case Constants::io__camera_double::OFFSET_X:
			case Constants::io__camera_double::OFFSET_Y:
			case Constants::io__camera_double::ROTATION_RAD:
				p[+u] += v;
				break;
			}
			refresh();
		}

		double camera_preset::get(const Constants::io__camera_double u) const
		{
			if (u == Constants::io__camera_double::size) throw Abort::abort(__FUNCSIG__, "Shouldn't try to get ::size!");
			return p[+u];
		}

		bool camera_preset::get(const Constants::io__camera_boolean u) const
		{
			if (u == Constants::io__camera_boolean::size) throw Abort::abort(__FUNCSIG__, "Shouldn't try to get ::size!");
			return b[+u];
		}

		void camera_preset::setInternalID(const std::string v)
		{
			if (!canWrite()) {
				throw Abort::abort(__FUNCSIG__, "Attempted to set a value to a camera_preset that has the flag READONLY set!");
				return;
			}
			if (v.length() > 0) internal_id = v;
		}

		std::string camera_preset::getInternalID()
		{
			return internal_id;
		}

		void camera_preset::setLayer(const int u, const bool enabl)
		{
			if (!canWrite()) {
				throw Abort::abort(__FUNCSIG__, "Attempted to set a layer to a camera_preset that has the flag READONLY set!");
				return;
			}

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
			if (layers.size() > 0) {
				std::sort(layers.begin(), layers.end(), [](int i, int j) {return i < j; });
			}
		}

		std::vector<int>::iterator camera_preset::begin()
		{
			return layers.begin();
		}

		std::vector<int>::iterator camera_preset::end()
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

		void Camera::set(const camera_preset& p, const int i, std::function<void(camera_preset&)> f)
		{
			presets[i].preset = p;
			presets[i].func = f;
			presets[i].preset.refresh();
		}

		
		void Camera::apply(const int a)
		{
			lastapply = a;
			apply();
		}

		void Camera::apply()
		{
			auto u = get();
			applyNoSave(u);
		}

		ALLEGRO_TRANSFORM Camera::applyNoSave(ALLEGRO_BITMAP* const d, camera_preset lastTransf)
		{
			if (!d) return ALLEGRO_TRANSFORM();

			g_t = easyTransform(d,
				lastTransf.get(Constants::io__camera_double::OFFSET_X), lastTransf.get(Constants::io__camera_double::OFFSET_Y),
				lastTransf.get(Constants::io__camera_double::SCALE_X) * lastTransf.get(Constants::io__camera_double::SCALE_G),
				lastTransf.get(Constants::io__camera_double::SCALE_Y) * lastTransf.get(Constants::io__camera_double::SCALE_G),
				lastTransf.get(Constants::io__camera_double::ROTATION_RAD));

			al_use_transform(&g_t);
			return g_t;
		}


		ALLEGRO_TRANSFORM Camera::applyNoSave(camera_preset lastTransf)
		{
			ALLEGRO_DISPLAY* d = al_get_current_display();

			if (!d) return ALLEGRO_TRANSFORM();

			g_t = easyTransform(d,
				lastTransf.get(Constants::io__camera_double::OFFSET_X), lastTransf.get(Constants::io__camera_double::OFFSET_Y),
				lastTransf.get(Constants::io__camera_double::SCALE_X) * lastTransf.get(Constants::io__camera_double::SCALE_G),
				lastTransf.get(Constants::io__camera_double::SCALE_Y) * lastTransf.get(Constants::io__camera_double::SCALE_G),
				lastTransf.get(Constants::io__camera_double::ROTATION_RAD));

			al_use_transform(&g_t);
			return g_t;
		}

		void Camera::applyNoSave(ALLEGRO_TRANSFORM* tr)
		{
			al_use_transform(tr);
		}

		

		camera_preset& Camera::get(const int u)
		{
			presets[u].preset.refresh();
			if (presets[u].func) presets[u].func(presets[u].preset);
			return presets[u].preset;
		}

		camera_preset& Camera::get()
		{
			return get(lastapply);
		}

		

		ALLEGRO_BITMAP* Sprite::__sprite_smart_images::get(const Constants::io__sprite_tie_func_to_state stt)
		{
			if (copies.size() == 0) {
				throw Abort::abort(__FUNCSIG__, "No bitmap found!", 1);
				return nullptr;
			}

			if (stt == Constants::io__sprite_tie_func_to_state::size) {
				auto now = al_get_time();
				auto siz = copies.size();

				if (lastcall == 0 || difftimeanim <= 0) lastcall = now;

				if (difftimeanim > 0 && siz > 1) {
					if ((now - lastcall) > difftimeanim * 10) { // skip if so far behind
						lastcall = now - difftimeanim * 10;
					}
					while ((now - lastcall) > difftimeanim)
					{
						if (!loopin) {
							if (actual < siz - 1) actual++;
							else {
								has_done_loop_once = true;
							}
						}
						else {
							if (++actual >= siz) actual = 0;
						}

						lastcall += difftimeanim;
					}
				}
			}
			else {
				auto vv = pair[+stt];
				if (vv >= copies.size() || vv == std::string::npos) {
					if (pair[+Constants::io__sprite_tie_func_to_state::COLLISION_NONE] != std::string::npos) return get(Constants::io__sprite_tie_func_to_state::COLLISION_NONE); // like default one
					return get(Constants::io__sprite_tie_func_to_state::size);
				}

				lastcall = 0;
				actual = 0;

				return copies[pair[+stt]]->bmp;
			}
			return copies[actual]->bmp;
		}
		size_t Sprite::__sprite_smart_images::lastFrame()
		{
			return actual;
		}
		ALLEGRO_BITMAP* Sprite::__sprite_smart_images::load(const std::string id)
		{
			__template_static_vector<ALLEGRO_BITMAP> imgs; // Textures


			__custom_data* cd = new __custom_data();

			if (!imgs.get(id, cd->bmp)) throw Abort::abort(__FUNCSIG__, "Couldn't add " + id + " to the sprite!");
			cd->idc = id;

			copies.push_back(cd);

			return cd->bmp;
		}
		ALLEGRO_BITMAP* Sprite::__sprite_smart_images::load(ALLEGRO_BITMAP* v, const std::string str)
		{
			if (!v) return nullptr;
			__custom_data* cd = new __custom_data();
			cd->idc = str;
			cd->bmp = v;
			copies.push_back(cd);
			return v;
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
			lastcall = 0;
			if (v < 0) {
				difftimeanim = 0;
				actual = -(int)v;
			}
			else if (v == 0) {
				if (reset_instead_of_pause) actual = 0;
				else difftimeanim = 0;
			}
			else {
				difftimeanim = v;
			}
		}
		void Sprite::__sprite_smart_images::setState(const Constants::io__sprite_tie_func_to_state s, const size_t v)
		{
			if (s != Constants::io__sprite_tie_func_to_state::size) pair[+s] = v;
		}
		size_t Sprite::__sprite_smart_images::getState(const Constants::io__sprite_tie_func_to_state s)
		{
			if (s != Constants::io__sprite_tie_func_to_state::size) return pair[+s];
			return std::string::npos;
		}
		void Sprite::__sprite_smart_images::loop(const bool b)
		{
			loopin = b;
			has_done_loop_once = false;
		}
		bool Sprite::__sprite_smart_images::hasLoopEndedOnce()
		{
			bool r = has_done_loop_once;
			has_done_loop_once = false;
			return r;
		}
		bool Sprite::__sprite_smart_images::isOnLastFrame()
		{
			return (actual >= copies.size() - 1);
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

		void Sprite::__sprite_smart_images::resetInsteadOfPause(const bool b)
		{
			reset_instead_of_pause = b;
		}

		void Sprite::__sprite_smart_images::checkAllReferences()
		{
			__template_static_vector<ALLEGRO_BITMAP> imgs; // Textures

			for (auto& i : copies)
			{
				if (!imgs.get(i->idc, i->bmp)) throw Abort::abort(__FUNCSIG__, "Unexpected failure rebinding references at Sprite internal vector!");
			}
		}

		bool Sprite::__sprite_smart_images::resizeAllTo(const int x, const int y)
		{
			if (copies.size() > 0) {
				{
					ALLEGRO_BITMAP* tmp = copies[0]->bmp;
					if (al_get_bitmap_width(tmp) == x && al_get_bitmap_height(tmp) == y) return true;
				}

				__template_static_vector<ALLEGRO_BITMAP> imgs; // Textures

				ALLEGRO_BITMAP* trg = al_get_target_bitmap();
				if (!trg) return true; // just keep going, no error, but no job

				for (auto& i : copies)
				{
					try {
						ALLEGRO_BITMAP* clon = al_clone_bitmap(i->bmp);
						imgs.remove(i->idc);
						ALLEGRO_BITMAP* ntarg = imgs.customLoad(i->idc, [&](ALLEGRO_BITMAP*& b)->bool {return (b = al_create_bitmap(x, y)); });
						ALLEGRO_BITMAP* tg = al_get_target_bitmap();
						al_set_target_bitmap(ntarg);

						al_draw_scaled_bitmap(clon, 0, 0, al_get_bitmap_width(clon), al_get_bitmap_height(clon), 0, 0, x, y, 0);
						al_destroy_bitmap(clon);

						al_set_target_bitmap(tg);
					}
					catch (Abort::abort err)
					{
						gfile logg;
						logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Failed recreating bitmap '" << i->idc << "' {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
						logg.flush();
						forceExit("Something went wrong!", "Please report the following:", (err.from() + " -> " + err.details()).c_str());
						return false;
					}
					catch (...) {
						return false;
					}
					/*ALLEGRO_BITMAP* n = al_create_bitmap(x, y);
					al_set_target_bitmap(n);
					al_draw_scaled_bitmap(i->bmp, 0, 0, al_get_bitmap_width(i->bmp), al_get_bitmap_height(i->bmp), 0, 0, x, y, 0);
					al_destroy_bitmap(i->bmp);
					imgs.remove(i->idc);
					i->bmp = n;*/
					/*if (!imgs.swap(i->idc, n)) {
						gfile logg;
						logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Failed to tie new bitmap on '" << i->idc << "'. Checking all references to avoid weird behaviour." << L::ELF;
						logg.flush();
						try {
							checkAllReferences();
						}
						catch (Abort::abort err)
						{
							logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Failed checking all references on " << i->idc << " {" << err.from() << "," << err.details() << ",#" << err.getErrN() << "}" << L::ELF;
							logg.flush();
							forceExit("Something went wrong!", "Please report the following:", (err.from() + " -> " + err.details()).c_str());
						}
						al_set_target_bitmap(trg);
						return false;
					}
					// swapped
					al_destroy_bitmap(n);*/
				}
				checkAllReferences();

				al_set_target_bitmap(trg);
			}
			return true;
		}


		Sprite::__sprite_smart_data::__sprite_smart_data()
		{
			Database conf;
			bool debugging = Constants::_is_on_debug_mode;
			conf.get(Constants::io__conf_boolean::ULTRADEBUG, debugging);
			debugging |= Constants::_is_on_debug_mode;
						
			dval[+Constants::io__sprite_double::SCALEX] = 1.0;
			dval[+Constants::io__sprite_double::SCALEY] = 1.0;
			dval[+Constants::io__sprite_double::SCALEG] = 1.0;
			dval[+Constants::io__sprite_double::SMOOTHNESS_X] = Constants::sprite_default_smoothness;
			dval[+Constants::io__sprite_double::SMOOTHNESS_Y] = Constants::sprite_default_smoothness;
			dval[+Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY] = 0.0;
			dval[+Constants::io__sprite_double::RO_OTHERS_DISTANCE_X] = 0.0;
			dval[+Constants::io__sprite_double::RO_OTHERS_DISTANCE_Y] = 0.0;
			bval[+Constants::io__sprite_boolean::AFFECTED_BY_CAM] = true;
			bval[+Constants::io__sprite_boolean::SHOWBOX] = debugging;
			bval[+Constants::io__sprite_boolean::SHOWDOT] = debugging;
			bval[+Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS] = true;
			dval[+Constants::io__sprite_double::SPEEDXY_LIMIT] = Constants::intensity_player_run_max;
			dval[+Constants::io__sprite_double::ACCELERATION_X] = Constants::intensity_player_run_max * 0.333;
			dval[+Constants::io__sprite_double::ACCELERATION_Y] = Constants::intensity_player_run_max * 0.333;
		}






		bool Sprite::fastIsColliding(camera_preset psf) // rad
		{
			psf.set(Constants::io__camera_boolean::READONLY_NOW, false);

			if (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY) { // HERE
				
				data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X] = data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y] = false;

				if (data.bval[+Constants::io__sprite_boolean::FOLLOWMOUSE]) {
					data.bval[+Constants::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] = true;
				}
				else {
					double m[2] = { 0.0 };
					bool is_mouse_pressed = false;
					Database db;

					db.get(Constants::ro__db_mouse_double::MOUSE_X, m[0]);
					db.get(Constants::ro__db_mouse_double::MOUSE_Y, m[1]);
					db.get(Constants::ro__db_mouse_boolean::IS_ANY_PRESSED, is_mouse_pressed);


					data.dval[+Constants::io__sprite_double::RO_MOUSE_DISTANCE_X] = (data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX]) - m[0];
					data.dval[+Constants::io__sprite_double::RO_MOUSE_DISTANCE_Y] = (data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY]) - m[1];


					data.bval[+Constants::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] = (
						(fabs(data.dval[+Constants::io__sprite_double::RO_MOUSE_DISTANCE_X]) < 0.5 * data.dval[+Constants::io__sprite_double::SCALEX] * data.dval[+Constants::io__sprite_double::SCALEG]) &&
						(fabs(data.dval[+Constants::io__sprite_double::RO_MOUSE_DISTANCE_Y]) < 0.5 * data.dval[+Constants::io__sprite_double::SCALEY] * data.dval[+Constants::io__sprite_double::SCALEG])
						);

					if (data.bval[+Constants::io__sprite_boolean::RO_IS_MOUSE_COLLIDING]) {
						data.dval[+Constants::io__sprite_double::RO_LAST_MOUSE_COLLISION] = al_get_time();
						if (is_mouse_pressed) {
							data.new_state = Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK;
						}
						else {
							if (data.new_state == Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK) data.new_state = Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_UNCLICK;
							else data.new_state = Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON;
						}
					}
				}
			}
			else {
				data.bval[+Constants::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] = false;
			}

			return false;
		}

		void Sprite::checkTied()
		{
			if (data.bval[+Constants::io__sprite_boolean::TIE_SIZE_TO_DISPLAY] && data.dval[+Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY] != 0.0 && al_get_time() - data.dval[+Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY] > 0.0) {
				Database db;
				int nv[2];

				db.get(Constants::io__conf_integer::SCREEN_X, nv[0]);
				db.get(Constants::io__conf_integer::SCREEN_Y, nv[1]);

				if (bmps.resizeAllTo(nv[0], nv[1])) {
					data.dval[+Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY] = 0.0;
					if (data.function_pair[+Constants::io__sprite_tie_func_to_state::WHEN_BITMAPS_RESIZED_AUTO]) data.function_pair[+Constants::io__sprite_tie_func_to_state::WHEN_BITMAPS_RESIZED_AUTO]();
				}
				else data.dval[+Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY] = al_get_time() + 0.5;
			}
		}

		bool Sprite::shouldThisCalcCollisionIn(const int l)
		{
			if (!data.bval[+Constants::io__sprite_boolean::AFFECTED_BY_CAM]) return false;
			if (+data.collision_mode <= +Constants::io__sprite_collision_mode::__COLLISION_HOLD_OR_TRANSPARENT_MAXVAL) return false;
			if (!isEq(Constants::io__sprite_integer::LAYER, l)) {
				if (+data.collision_mode >= +Constants::io__sprite_collision_mode::__COLLISION_ANYLAYER_MINVAL) return true;
				return false;
			}
			return true; // same layer and not transparent
		}

		Sprite::~Sprite()
		{
			bmps.reset();
		}

		void Sprite::hook(const Constants::io__sprite_tie_func_to_state w, std::function<void(void)> f, const bool looping)
		{
			if (w != Constants::io__sprite_tie_func_to_state::size) {
				data.function_pair[+w] = f;
				data.function_pair_looping[+w] = looping;
			}
		}

		void Sprite::unhook(const Constants::io__sprite_tie_func_to_state w)
		{
			if (w != Constants::io__sprite_tie_func_to_state::size) data.function_pair[+w] = std::function<void(void)>();
		}

		void Sprite::set(const Constants::io__sprite_string_vector u, const std::vector<std::string> v, float* perc)
		{
			if (perc) *perc = 0.00f;
			size_t countt = 0;
			switch (u) {
			case Constants::io__sprite_string_vector::ADDMULTIPLE:
				for (auto& i : v) {
					bmps.load(i);
					if (perc) *perc = 1.00f * countt++ / v.size();
				}
				break;
			case Constants::io__sprite_string_vector::REMOVEMULTIPLE:
				for (auto& i : v) {
					bmps.remove(i);
					if (perc) *perc = 1.00f * countt++ / v.size();
				}
				break;
			}
			if (perc) *perc = 1.00f;
		}

		void Sprite::set(const Constants::io__sprite_string_vector u, const std::function<bool(const std::string)> f)
		{
			__template_static_vector<ALLEGRO_BITMAP> v;
			switch (u) {
			case Constants::io__sprite_string_vector::ADDMULTIPLE:
				for (auto& i : v) {
					if (f(i->id)) {
						bmps.load(i->self, i->id);
					}
				}
				break;
			case Constants::io__sprite_string_vector::REMOVEMULTIPLE:
				for (auto& i : v) {
					if (f(i->id)) {
						bmps.remove(i->id);
					}
				}
				break;
			}
		}

		void Sprite::set(const Constants::io__sprite_string u, const std::string v)
		{
			switch (u) {
			case Constants::io__sprite_string::ADD:
				bmps.load(v);
				break;
			case Constants::io__sprite_string::REMOVE:
				bmps.remove(v);
				break;
			case Constants::io__sprite_string::ID:
				sprite_id = v;
				break;
			}
		}
		void Sprite::set(const Constants::io__sprite_double u, const double v)
		{
			switch (u) {
			case Constants::io__sprite_double::ANIMATION_FPS:  // don't forget COLLISION_STATE can bug this one
				bmps.setFPS(v);
				break;
			default:
				data.dval[+u] = v;
				// target set to value
				switch (+u) {
				case +Constants::io__sprite_double::POSX:
					data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] = v;
					break;
				case +Constants::io__sprite_double::POSY:
					data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] = v;
					break;
				case +Constants::io__sprite_double::ROTATION:
					data.dtarg[+Constants::ro__sprite_target_double::TARG_ROTATION] = v;
					break;
				}
				break;
			}
		}
		void Sprite::set(const Constants::io__sprite_boolean u, const bool v)
		{
			switch (u) {
			case Constants::io__sprite_boolean::LOOPFRAMES:
				bmps.loop(v);
				break;
			case Constants::io__sprite_boolean::HAS_DONE_LOOP_ONCE:
				// nothing to do
				break;
			case Constants::io__sprite_boolean::ZERO_RESETS_POSITION_INSTEAD_OF_FREEZING: // setFPS(0) stops animation or reset to 0?
				bmps.resetInsteadOfPause(v);
				break;
			default:
				data.bval[+u] = v;
				break;
			}
		}
		void Sprite::set(const Constants::io__sprite_integer u, const int v)
		{
			switch (u) {
			case Constants::io__sprite_integer::LAYER:
				layer = v;
				break;
			case Constants::io__sprite_integer::ADD_ALT_LAYER:
			{
				bool has = false;
				data.layers_colliding_m.lock();

				for (auto& i : data.layers_colliding) {
					if (i == v) {
						has = true;
						break;
					}
				}
				if (!has && v != layer) data.layers_colliding.push_back(v);

				data.layers_colliding_m.unlock();
			}
				break;
			case Constants::io__sprite_integer::REMOVE_ALT_LAYER:
			{
				data.layers_colliding_m.lock();

				for (size_t p = 0; p < data.layers_colliding.size(); p++) {
					if (data.layers_colliding[p] == v) {
						data.layers_colliding.erase(data.layers_colliding.begin() + p);
						break;
					}
				}

				data.layers_colliding_m.unlock();
			}
				break;
			}
		}
		void Sprite::set(const Constants::io__sprite_sizet u, const size_t v)
		{
			switch (u) {
			case Constants::io__sprite_sizet::FRAME:
				bmps.setFPS(-(int)(v));
				break;
			}
		}
		void Sprite::set(const Constants::io__sprite_color u, const ALLEGRO_COLOR v)
		{
			switch (u) {
			case Constants::io__sprite_color::TINT:
				data.tint = v;
				break;
			}
		}

		void Sprite::set(const Constants::io__sprite_tie_func_to_state u, const size_t v)
		{
			bmps.setState(u, v);
		}

		void Sprite::set(const Constants::io__sprite_collision_mode m)
		{
			data.collision_mode = m;
			//for (auto& i : data.tempblock_dirs) i = true;
		}

		void Sprite::set(const Constants::cl__sprite_direction_mult d)
		{
			switch (d) {
			case Constants::cl__sprite_direction_mult::GO_NORTH:
				data.combined_direction_i_wanna_go |= +Constants::directions::NORTH;
				break;
			case Constants::cl__sprite_direction_mult::GO_SOUTH:
				data.combined_direction_i_wanna_go |= +Constants::directions::SOUTH;
				break;
			case Constants::cl__sprite_direction_mult::GO_EAST:
				data.combined_direction_i_wanna_go |= +Constants::directions::EAST;
				break;
			case Constants::cl__sprite_direction_mult::GO_WEST:
				data.combined_direction_i_wanna_go |= +Constants::directions::WEST;
				break;
			case Constants::cl__sprite_direction_mult::STOP_NORTH:
				data.combined_direction_i_wanna_go &= ~ +Constants::directions::NORTH;
				break;
			case Constants::cl__sprite_direction_mult::STOP_SOUTH:
				data.combined_direction_i_wanna_go &= ~+ Constants::directions::SOUTH;
				break;
			case Constants::cl__sprite_direction_mult::STOP_EAST:
				data.combined_direction_i_wanna_go &= ~+ Constants::directions::EAST;
				break;
			case Constants::cl__sprite_direction_mult::STOP_WEST:
				data.combined_direction_i_wanna_go &= ~+ Constants::directions::WEST;
				break;
			}
		}

		bool Sprite::get(const Constants::io__sprite_string u, std::string& v)
		{
			switch (u) {
			case Constants::io__sprite_string::ADD:
			case Constants::io__sprite_string::REMOVE:
				// no sense
				return false;
			case Constants::io__sprite_string::ID:
				v = sprite_id;
				return true;
			}
			return false;
		}
		bool Sprite::get(const Constants::io__sprite_double u, double& v)
		{
			switch (u) {
			case Constants::io__sprite_double::ANIMATION_FPS:
				// no way
				return false;
			default:
				v = data.dval[+u];
				return true;
			}
			return false;
		}
		bool Sprite::get(const Constants::ro__sprite_target_double u, double& v)
		{
			if (u == Constants::ro__sprite_target_double::size) return false;
			v = data.dtarg[+u];
			return true;
		}
		bool Sprite::get(const Constants::io__sprite_boolean u, bool& v)
		{
			switch (u) {
			case Constants::io__sprite_boolean::LOOPFRAMES:
				// no way
				return false;
			case Constants::io__sprite_boolean::HAS_DONE_LOOP_ONCE:
				v = bmps.hasLoopEndedOnce();
				return true;
			case Constants::io__sprite_boolean::IS_IT_ON_LAST_FRAME:
				v = bmps.isOnLastFrame();
				return true;
			default:
				v = data.bval[+u];
				return true;
			}
			return false;
		}
		bool Sprite::get(const Constants::io__sprite_integer u, int& v)
		{
			switch (u) {
			case Constants::io__sprite_integer::LAYER:
				v = layer;
				return true;
			}
			return false;
		}
		bool Sprite::get(const Constants::io__sprite_sizet u, size_t& v)
		{
			switch (u) {
			case Constants::io__sprite_sizet::FRAME:
				v = bmps.lastFrame();
				return true;
			}
			return false;
		}
		bool Sprite::get(const Constants::ro__sprite_state u, Constants::io__sprite_tie_func_to_state& v)
		{
			switch (u) {
			case Constants::ro__sprite_state::STATE:
				v = data.last_state;
				return true;
			}
			return false;
		}
		bool Sprite::get(const Constants::io__sprite_color u, ALLEGRO_COLOR& v)
		{
			switch (u) {
			case Constants::io__sprite_color::TINT:
				v = data.tint;
				return true;
			}
			return false;
		}

		void Sprite::get(const Constants::io__sprite_tie_func_to_state u, size_t& v)
		{
			v = bmps.getState(u);
		}

		void Sprite::get(ALLEGRO_BITMAP*& g)
		{
			g = bmps.get();
		}

		void Sprite::get(Constants::io__sprite_collision_mode& m)
		{
			m = data.collision_mode;
		}


		bool Sprite::isEq(const Constants::io__sprite_string w, const std::string v)
		{
			std::string g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Constants::io__sprite_double w, const double v)
		{
			double g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Constants::io__sprite_boolean w, const bool v)
		{
			bool g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Constants::io__sprite_integer w, const int v)
		{
			int g;
			get(w, g); 
			return g == v || [&]() {
				if (w != Constants::io__sprite_integer::LAYER) return false; // return false forcing anything but LAYER to be TRUE on g == v part

				auto cpy = data.layers_colliding;
				for (auto& i : cpy) {
					if (i == v) {
						return true;
					}
				}
				return false;
			}();
		}
		bool Sprite::isEq(const Constants::io__sprite_sizet w, const size_t v)
		{
			size_t g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Constants::ro__sprite_state w, const Constants::io__sprite_tie_func_to_state v)
		{
			Constants::io__sprite_tie_func_to_state g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Constants::io__sprite_color w, const ALLEGRO_COLOR v)
		{
			ALLEGRO_COLOR g;
			get(w, g);
			return ((g.a == v.a) && (g.r == v.r) && (g.g == v.g) && (g.b == v.b));
		}

		bool Sprite::isEq(const Constants::io__sprite_tie_func_to_state w, const size_t v)
		{
			size_t g;
			get(w, g);
			return g == v;
		}

		bool Sprite::isEq(const Constants::io__sprite_collision_mode v)
		{
			Constants::io__sprite_collision_mode g;
			get(g);
			return g == v;
		}

		void Sprite::draw(const int is_layer)
		{
			if (!isEq(Constants::io__sprite_integer::LAYER, is_layer)) return;
			if (!data.bval[+Constants::io__sprite_boolean::DRAW]) return;

			checkTied();

			if (data.function_pair[+Constants::io__sprite_tie_func_to_state::ON_DRAW]) data.function_pair[+Constants::io__sprite_tie_func_to_state::ON_DRAW]();

			ALLEGRO_BITMAP* rn = bmps.get((data.bval[+Constants::io__sprite_boolean::USE_STATE_AS_BITMAP] ? data.last_state : Constants::io__sprite_tie_func_to_state::size));

			if (!rn) return; // if doesn't have what to draw (possible), skip

			Camera camm;
			camera_preset psf = camm.get();

			if (!data.bval[+Constants::io__sprite_boolean::AFFECTED_BY_CAM]) psf = camera_preset();


			double timee = al_get_time();
			double dt = timee - data.dtarg[+Constants::ro__sprite_target_double::INTERN_LASTDRAW];
			data.dtarg[+Constants::ro__sprite_target_double::INTERN_LASTDRAW] = timee;

			double perc_run = Constants::__i_col_pos_t_update * dt; // ex: 5 per sec * 0.2 (1/5 sec) = 1, so posx = actual posx...
			if (perc_run > 1.0) perc_run = 1.0; // 1.0 is "set value"
			if (perc_run < 1.0 / 10000) perc_run = 1.0 / 10000; // can't be infinitely smooth right? come on

			data.dval[+Constants::io__sprite_double::ROTATION] = (1.0 - perc_run) * data.dval[+Constants::io__sprite_double::ROTATION] + perc_run * data.dtarg[+Constants::ro__sprite_target_double::TARG_ROTATION];

			if (!data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X] || !data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y]) {

				if (!data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X]) data.dval[+Constants::io__sprite_double::POSX] = (1.0 - perc_run) * data.dval[+Constants::io__sprite_double::POSX] + perc_run * data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX];
				if (!data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y]) data.dval[+Constants::io__sprite_double::POSY] = (1.0 - perc_run) * data.dval[+Constants::io__sprite_double::POSY] + perc_run * data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY];
			}


			float cx, cy, px, py, dsx, dsy, rot_rad;
			int bmpx, bmpy;
			bmpx = al_get_bitmap_width(rn);
			bmpy = al_get_bitmap_height(rn);
			if (bmpx <= 0 || bmpy <= 0) {
				throw Abort::abort(__FUNCSIG__, "Somehow the texture have < 0 width / height id=[" + this->sprite_id + "] COLLISION_NONE={" + std::to_string(bmpx) + "," + std::to_string(bmpy) + "}", 1);
			}

			cx = 1.0f * bmpx * ((data.dval[+Constants::io__sprite_double::CENTERX] + 1.0) * 0.5);
			cy = 1.0f * bmpy * ((data.dval[+Constants::io__sprite_double::CENTERY] + 1.0) * 0.5);
			rot_rad = 1.0f * data.dval[+Constants::io__sprite_double::ROTATION] * ALLEGRO_PI / 180.0;
			/*px = 1.0f * data.dval[+Constants::io__sprite_double::POSX] * cos(rot_rad) + data.dval[+Constants::io__sprite_double::POSY] * sin(rot_rad);
			py = 1.0f * data.dval[+Constants::io__sprite_double::POSY] * cos(rot_rad) - data.dval[+Constants::io__sprite_double::POSX] * sin(rot_rad);*/
			px = data.dval[+Constants::io__sprite_double::POSX];
			py = data.dval[+Constants::io__sprite_double::POSY];
			dsx = 1.0f * data.dval[+Constants::io__sprite_double::SCALEX] * data.dval[+Constants::io__sprite_double::SCALEG] * (1.0 / bmpx);
			dsy = 1.0f * data.dval[+Constants::io__sprite_double::SCALEY] * data.dval[+Constants::io__sprite_double::SCALEG] * (1.0 / bmpy);

			// draw
			camm.applyNoSave(psf);

			if (data.bval[+Constants::io__sprite_boolean::USE_TINTED_DRAWING]) {
				al_draw_tinted_scaled_rotated_bitmap(rn, data.tint, cx, cy, px, py, dsx, dsy, rot_rad, 0);
			}
			else {
				al_draw_scaled_rotated_bitmap(rn, cx, cy, px, py, dsx, dsy, rot_rad, 0);
			}

			// debug

			if (data.bval[+Constants::io__sprite_boolean::SHOWBOX] || data.bval[+Constants::io__sprite_boolean::SHOWDOT])
			{
				ALLEGRO_COLOR colorr = 
					al_map_rgba(
						(data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X] || data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y]) ? 195 : 15,
						115,
						(data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X] || data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y]) ? 195 : 15,
						195
					);
				camera_preset psf = camm.get();

				if (data.bval[+Constants::io__sprite_boolean::FOLLOWMOUSE]) colorr = al_map_rgba(195, 0, 195, 195);
				//else if (al_get_time() - lastresetcollisioncall < Defaults::diff_time_show_last_resetCollision) colorr = al_map_rgb(255, 255, 0);

				if (data.bval[+Constants::io__sprite_boolean::SHOWBOX]) {
					al_draw_filled_circle(px - cx * dsx, py - cy * dsy, 0.1f * (fabs(psf.get(Constants::io__camera_double::SCALE_G) * sqrt(psf.get(Constants::io__camera_double::SCALE_X) * psf.get(Constants::io__camera_double::SCALE_Y)) * 0.20f) + 0.05f), colorr);
					al_draw_filled_circle(px - cx * dsx, py + cy * dsy, 0.1f * (fabs(psf.get(Constants::io__camera_double::SCALE_G) * sqrt(psf.get(Constants::io__camera_double::SCALE_X) * psf.get(Constants::io__camera_double::SCALE_Y)) * 0.20f) + 0.05f), colorr);
					al_draw_filled_circle(px + cx * dsx, py - cy * dsy, 0.1f * (fabs(psf.get(Constants::io__camera_double::SCALE_G) * sqrt(psf.get(Constants::io__camera_double::SCALE_X) * psf.get(Constants::io__camera_double::SCALE_Y)) * 0.20f) + 0.05f), colorr);
					al_draw_filled_circle(px + cx * dsx, py + cy * dsy, 0.1f * (fabs(psf.get(Constants::io__camera_double::SCALE_G) * sqrt(psf.get(Constants::io__camera_double::SCALE_X) * psf.get(Constants::io__camera_double::SCALE_Y)) * 0.20f) + 0.05f), colorr);
				}

				if (data.bval[+Constants::io__sprite_boolean::SHOWDOT])
				{
					al_draw_filled_circle(px, py, 0.1f * (fabs(psf.get(Constants::io__camera_double::SCALE_G) * sqrt(psf.get(Constants::io__camera_double::SCALE_X) * psf.get(Constants::io__camera_double::SCALE_Y)) * 0.30f) + 0.12f), colorr);
				}
			}

			camm.apply();
		}

		void Sprite::rebindReferences()
		{
			bmps.checkAllReferences();
		}

		void Sprite::clearUp()
		{
			data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X] = false;
			data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y] = false;
			data.bval[+Constants::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] = false;
			data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_ABOUT_TO_COLLIDE] = false;
			data.dval[+Constants::io__sprite_double::RO_OTHERS_DISTANCE_X] = 0.0;// 2.0 * data.dval[+Constants::io__sprite_double::SCALEX] * data.dval[+Constants::io__sprite_double::SCALEG];
			data.dval[+Constants::io__sprite_double::RO_OTHERS_DISTANCE_Y] = 0.0;// 2.0 * data.dval[+Constants::io__sprite_double::SCALEY] * data.dval[+Constants::io__sprite_double::SCALEG];
			data.new_state = Constants::io__sprite_tie_func_to_state::COLLISION_NONE;
			data.col_data.clear();
			//data.direction_col = 0;
			if (__debug_s.length() > 0) {
				gfile logg;
				logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Sprite " << this->sprite_id << " debug: " << __debug_s << L::EL;
			}
			__debug_s.clear();
		}

		void Sprite::process(const int is_layer, camera_preset psf)
		{
			//data.where_i_wanna_go
			psf.set(Constants::io__camera_boolean::READONLY_NOW, false);

			if ((!isEq(Constants::io__sprite_integer::LAYER, is_layer)) &&
				(data.collision_mode != Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC && data.collision_mode != Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) // HERE
				&& ([&]() {
					auto cpy = data.layers_colliding;
					for (auto& i : cpy) {
						if (i == is_layer) {
							return false;
						}
					}
					return true;
					}()))
			{
				//__debug_s += "SKIP_PROC;";
				return; // check layers if there's one to collide (ON)
			}


			if (!data.bval[+Constants::io__sprite_boolean::AFFECTED_BY_CAM]) psf = camera_preset();

			// LIMIT

			if (fabs(data.dval[+Constants::io__sprite_double::RO_SPEEDX]) > data.dval[+Constants::io__sprite_double::SPEEDXY_LIMIT])
			{
				data.dval[+Constants::io__sprite_double::RO_SPEEDX] = 
					(data.dval[+Constants::io__sprite_double::RO_SPEEDX] > 0.0) ? data.dval[+Constants::io__sprite_double::SPEEDXY_LIMIT] : -data.dval[+Constants::io__sprite_double::SPEEDXY_LIMIT];
			}
			if (fabs(data.dval[+Constants::io__sprite_double::RO_SPEEDY]) > data.dval[+Constants::io__sprite_double::SPEEDXY_LIMIT])
			{
				data.dval[+Constants::io__sprite_double::RO_SPEEDY] = 
					(data.dval[+Constants::io__sprite_double::RO_SPEEDY] > 0.0) ? data.dval[+Constants::io__sprite_double::SPEEDXY_LIMIT] : -data.dval[+Constants::io__sprite_double::SPEEDXY_LIMIT];
			}


			data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] += data.dval[+Constants::io__sprite_double::RO_SPEEDX];
			data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] += data.dval[+Constants::io__sprite_double::RO_SPEEDY];

			data.dtarg[+Constants::ro__sprite_target_double::TARG_ROTATION] += data.dval[+Constants::io__sprite_double::SPEEDROT];

			data.dval[+Constants::io__sprite_double::RO_SPEEDX] *= data.dval[+Constants::io__sprite_double::SMOOTHNESS_X] * psf.get(Constants::io__camera_double::SLIPPERINESS);
			data.dval[+Constants::io__sprite_double::RO_SPEEDY] *= data.dval[+Constants::io__sprite_double::SMOOTHNESS_Y] * psf.get(Constants::io__camera_double::SLIPPERINESS);

			fastIsColliding(psf);
		}

		void Sprite::collideWith(const int is_layer, Sprite* const mf)
		{
			if (!this) throw Abort::warn(__FUNCSIG__, "Unexpected null pointer");
			//gfile logg;
			std::string friendsname;

			if (mf == this) return;
			if (!shouldThisCalcCollisionIn(is_layer)) {	// test if not transparent and not a block so it has movement after a collision
				return;
			}
			if (!mf->isThisCollidableIn(is_layer)) {	// test if not transparent so it causes collision with moveable objects
				return;
			}
			mf->get(Constants::io__sprite_string::ID, friendsname);
			double multiplier = mf->doesThisReflect() ? 1.0 : 1.0;
			

			// OBRIGATORIAMENTE:
			// this -> ou é SAMELAYER ou ANYLAYER
			// othr -> ou é SAMELAYER ou ANYLAYER


			double otherpos[2] = { 0.0 };
			double otherscl[3] = { 1.0 };

			std::string oid;
			mf->get(Constants::io__sprite_string::ID, oid);

			mf->get(Constants::ro__sprite_target_double::TARG_POSX, otherpos[0]);
			mf->get(Constants::ro__sprite_target_double::TARG_POSY, otherpos[1]);

			mf->get(Constants::io__sprite_double::SCALEX, otherscl[0]);
			mf->get(Constants::io__sprite_double::SCALEY, otherscl[1]);
			mf->get(Constants::io__sprite_double::SCALEG, otherscl[2]);

			otherscl[0] *= otherscl[2];
			otherscl[1] *= otherscl[2];
			otherscl[2] = 1.0; // just to be sure nothing wrong happens


			double dxx = (data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX]) - otherpos[0];
			double dyy = (data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY]) - otherpos[1];

			double distx = 1e-50 + 0.5 * fabs(data.dval[+Constants::io__sprite_double::SCALEX] * data.dval[+Constants::io__sprite_double::SCALEG] + otherscl[0]);
			double disty = 1e-50 + 0.5 * fabs(data.dval[+Constants::io__sprite_double::SCALEY] * data.dval[+Constants::io__sprite_double::SCALEG] + otherscl[1]);

			double distx2 = distx + fabs(1.0 * data.dval[+Constants::io__sprite_double::RO_SPEEDX]);
			double disty2 = disty + fabs(1.0 * data.dval[+Constants::io__sprite_double::RO_SPEEDY]);



			double adapt_dxx = fabs(1.0 * dxx / distx);
			double adapt_dyy = fabs(1.0 * dyy / disty);
			double adapt_dxx2 = fabs(1.0 * dxx / distx2);
			double adapt_dyy2 = fabs(1.0 * dyy / disty2);

			if (adapt_dxx < 1.0 && adapt_dyy < 1.0) // collided
			{
				//logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << sprite_id << " COL W/ " << friendsname << L::EL;
				___collision_data this_collision;

				if (adapt_dxx > adapt_dyy) { // from horiz
					if (dxx > 0.0) { // block on the left of me
						this_collision.diretion_by_centers_interpreted = +Constants::directions::WEST;
						__debug_s += "W@" + std::to_string(adapt_dxx) + ":" + std::to_string(adapt_dyy) + ";";
						data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X] = true;
					}
					else {
						this_collision.diretion_by_centers_interpreted = +Constants::directions::EAST;
						__debug_s += "E@" + std::to_string(adapt_dxx) + ":" + std::to_string(adapt_dyy) + ";";
						data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X] = true;
					}
				}
				else { // from vertc
					if (dyy > 0.0) { // block on top
						this_collision.diretion_by_centers_interpreted = +Constants::directions::NORTH;
						__debug_s += "N@" + std::to_string(adapt_dxx) + ":" + std::to_string(adapt_dyy) + ";";
						data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y] = true;
					}
					else {
						this_collision.diretion_by_centers_interpreted = +Constants::directions::SOUTH;
						__debug_s += "S@" + std::to_string(adapt_dxx) + ":" + std::to_string(adapt_dyy) + ";";
						data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y] = true;
					}
				}

				this_collision.expected_walk_distances[0] = fabs(fabs(dxx) - fabs(distx));
				this_collision.expected_walk_distances[1] = fabs(fabs(dyy) - fabs(disty));

				data.col_data.push_back(this_collision);
			}

			else if (adapt_dxx2 < 1.0 && adapt_dyy2 < 1.0) // is about to collide soon
			{

				//logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << sprite_id << " WILL COL W/ " << friendsname << L::EL;
				data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_ABOUT_TO_COLLIDE] = true;
			
				data.dval[+Constants::io__sprite_double::RO_SPEEDX] *= 0.50; // < 0.50 so it won't bypass collision
				data.dval[+Constants::io__sprite_double::RO_SPEEDY] *= 0.50; // < 0.50 "

				if (adapt_dxx > adapt_dyy) { // from horiz
					if (dxx > 0.0)	__debug_s += "w@" + std::to_string(adapt_dxx) + ":" + std::to_string(adapt_dyy) + ";";
					else			__debug_s += "e@" + std::to_string(adapt_dxx) + ":" + std::to_string(adapt_dyy) + ";";
				}
				else { // from vertc
					if (dyy > 0.0)	__debug_s += "n@" + std::to_string(adapt_dxx) + ":" + std::to_string(adapt_dyy) + ";";
					else			__debug_s += "s@" + std::to_string(adapt_dxx) + ":" + std::to_string(adapt_dyy) + ";";
				}
			}
		}

		void Sprite::applyCollideData(camera_preset psf)
		{
			psf.set(Constants::io__camera_boolean::READONLY_NOW, false);

			if (data.new_state != Constants::io__sprite_tie_func_to_state::size) {
				if (data.last_state != data.new_state) {
					if (data.last_state != Constants::io__sprite_tie_func_to_state::size) __debug_s += "CUSTOM_F_STATE->#" + std::to_string(+data.new_state) + ";";
					if (data.function_pair[+data.new_state]) data.function_pair[+data.new_state]();
				}
				else if (data.function_pair_looping[+data.new_state]) data.function_pair[+data.new_state](); // run anyway (if loop)
			}
			data.last_state = data.new_state;

			int directions_can_go = +Constants::directions::EAST | +Constants::directions::WEST | +Constants::directions::NORTH | +Constants::directions::SOUTH;

			for (auto& i : data.col_data) directions_can_go &= ~i.diretion_by_centers_interpreted;

			// KEYBOARD INPUT
			if (data.combined_direction_i_wanna_go & +Constants::directions::NORTH) {
				if (directions_can_go & +Constants::directions::NORTH) data.dval[+Constants::io__sprite_double::RO_SPEEDY] -= data.dval[+Constants::io__sprite_double::ACCELERATION_Y];
				else __debug_s += "NOGO_NORTH;";
			}
			if (data.combined_direction_i_wanna_go & +Constants::directions::SOUTH) {
				if (directions_can_go & +Constants::directions::SOUTH) data.dval[+Constants::io__sprite_double::RO_SPEEDY] += data.dval[+Constants::io__sprite_double::ACCELERATION_Y];
				else __debug_s += "NOGO_SOUTH;";
			}
			if (data.combined_direction_i_wanna_go & +Constants::directions::EAST) {
				if (directions_can_go & +Constants::directions::EAST) data.dval[+Constants::io__sprite_double::RO_SPEEDX] += data.dval[+Constants::io__sprite_double::ACCELERATION_X];
				else __debug_s += "NOGO_EAST;";
			}
			if (data.combined_direction_i_wanna_go & +Constants::directions::WEST) {
				if (directions_can_go & +Constants::directions::WEST) data.dval[+Constants::io__sprite_double::RO_SPEEDX] -= data.dval[+Constants::io__sprite_double::ACCELERATION_X];
				else __debug_s += "NOGO_WEST;";
			}


			// COLLISION

			if (data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_X] || data.bval[+Constants::io__sprite_boolean::RO_IS_OTHERS_COLLIDING_FROM_Y]) {

				//data.dval[+Constants::io__sprite_double::RO_SPEEDY] = data.dval[+Constants::io__sprite_double::RO_SPEEDX] = 0.0;

				int where_are_them_at = 0;
				double sum_pos[2] = { Constants::sprite_default_min_movement, Constants::sprite_default_min_movement };
				size_t amount_sum[2] = { 0,0 };

				for (auto& i : data.col_data) where_are_them_at |= i.diretion_by_centers_interpreted;
				const auto b = Tools::translBinary(where_are_them_at, 4); // NORTH SOUTH EAST WEST

				for (auto& i : data.col_data) {
					if (b[2] || b[3]) {
						sum_pos[0] += i.expected_walk_distances[0];
						amount_sum[0]++;
					}
					if (b[0] || b[1]) {
						sum_pos[1] += i.expected_walk_distances[1];
						amount_sum[1]++;
					}
				}

				if (data.col_data.size() > 0) {
					sum_pos[0] /= 5.0 * amount_sum[0];
					sum_pos[1] /= 1.0 * amount_sum[1];
				}


				if (b[0] && b[1] && b[2] && b[3]) { // fully smashed
					__debug_s += "SMASHED;";
				}

				else if (b[1] && b[2] && b[3]) { // north is free (smashed horizontally)

					if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC))
						data.dval[+Constants::io__sprite_double::RO_SPEEDY] = -1.0 * data.dval[+Constants::io__sprite_double::ACCELERATION_Y];
					else if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH))
						data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] -= sum_pos[1];

					//data.dval[+Constants::io__sprite_double::RO_SPEEDY] = -0.15 * data.dval[+Constants::io__sprite_double::ACCELERATION_Y];
				}
				else if (b[0] && b[1] && b[2]) { // west is free (smashed vertically)
					if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC))
						data.dval[+Constants::io__sprite_double::RO_SPEEDX] = -1.0 * data.dval[+Constants::io__sprite_double::ACCELERATION_X];
					else if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH))
						data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] -= sum_pos[0];

					//data.dval[+Constants::io__sprite_double::RO_SPEEDX] = -0.15 * data.dval[+Constants::io__sprite_double::ACCELERATION_X];
				}

				else if (b[0] && b[1]) { // so go east (smashed vertically)
					if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC))
						data.dval[+Constants::io__sprite_double::RO_SPEEDX] = 1.0 * data.dval[+Constants::io__sprite_double::ACCELERATION_X];
					else if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH))
						data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] += sum_pos[0];

					//data.dval[+Constants::io__sprite_double::RO_SPEEDX] = 0.15 * data.dval[+Constants::io__sprite_double::ACCELERATION_X];
				}
				else if (b[2] && b[3]) { // so go south (smashed horizontally)
					if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC))
						data.dval[+Constants::io__sprite_double::RO_SPEEDY] = 1.0 * data.dval[+Constants::io__sprite_double::ACCELERATION_Y];
					else if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH))
						data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] += sum_pos[1];

					//data.dval[+Constants::io__sprite_double::RO_SPEEDY] = 0.15 * data.dval[+Constants::io__sprite_double::ACCELERATION_Y];
				}
				
				else if (b[0]) { // so go south
					if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC))
						data.dval[+Constants::io__sprite_double::RO_SPEEDY] = 1.0 * data.dval[+Constants::io__sprite_double::ACCELERATION_Y];
					else if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH))
						data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] += sum_pos[1];
				}
				else if (b[1]) { // so go north
					if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC))
						data.dval[+Constants::io__sprite_double::RO_SPEEDY] = -1.0 * data.dval[+Constants::io__sprite_double::ACCELERATION_Y];
					else if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH))
						data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] -= sum_pos[1];
				}
				else if (b[2]) { // so go west
					if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC))
						data.dval[+Constants::io__sprite_double::RO_SPEEDX] = -1.0 * data.dval[+Constants::io__sprite_double::ACCELERATION_X];
					else if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH))
						data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] -= sum_pos[0];
				}
				else if (b[3]) { // so go east
					if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ELASTIC) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC))
						data.dval[+Constants::io__sprite_double::RO_SPEEDX] = 1.0 * data.dval[+Constants::io__sprite_double::ACCELERATION_X];
					else if ((data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_ANY_ROUGH) || (data.collision_mode == Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH))
						data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] += sum_pos[0];
				}
				else {
					throw Abort::warn(__FUNCSIG__, "Unexpected collision behaviour!");
				}
			}


			if (psf.get(Constants::io__camera_boolean::RESPECT_LIMITS) && !(data.bval[+Constants::io__sprite_boolean::FOLLOWMOUSE] || !data.bval[+Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS])) /* FOLLOWMOUSE may glitch if outside limits, so skip limits then */
			{
				double lxl = psf.get(Constants::io__camera_double::LIMIT_MIN_X); // limit x lower
				double lyl = psf.get(Constants::io__camera_double::LIMIT_MIN_Y); // limit y lower
				double lxh = psf.get(Constants::io__camera_double::LIMIT_MAX_X); // limit x higher
				double lyh = psf.get(Constants::io__camera_double::LIMIT_MAX_Y); // limit y higher

				if (data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] < lxl) data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] = lxl;
				if (data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] > lxh) data.dtarg[+Constants::ro__sprite_target_double::TARG_POSX] = lxh;
				if (data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] < lyl) data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] = lyl;
				if (data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] > lyh) data.dtarg[+Constants::ro__sprite_target_double::TARG_POSY] = lyh;
			}
		}

		bool Sprite::isThisCollidableIn(const int l) // BLOCKS COLLIDE JUST WITH OTHERS, SO THIS ONLY WORKS TO TEST IF *SOMEONE* SHOULD COLLIDE WITH THIS (not the "should I collide?" inside itself because BLOCK doesn't collide with anyone, but anyone collides with block (HOLD))
		{
			if (!data.bval[+Constants::io__sprite_boolean::AFFECTED_BY_CAM]) return false;
			if (+data.collision_mode <= +Constants::io__sprite_collision_mode::__COLLISION_TRANSPARENT_MAXVAL) return false;
			if (!isEq(Constants::io__sprite_integer::LAYER, l)) {
				if (+data.collision_mode >= +Constants::io__sprite_collision_mode::__COLLISION_ANYLAYER_MINVAL) return true;
				return false;
			}
			return true; // same layer and not transparent
		}

		bool Sprite::doesThisReflect()
		{
			return (+data.collision_mode > +Constants::io__sprite_collision_mode::__COLLISION_HOLD_OR_TRANSPARENT_MAXVAL);
		}

		bool Sprite::isThisEntity()
		{
			return !is_this_sprite;
		}

		std::string Sprite::__debug_str()
		{
			return __debug_s;
		}



		Entity::Entity()
		{
			is_this_sprite = false;
		}

		bool Entity::isThisEntity()
		{
			return !is_this_sprite;
		}

		void Entity::set(Constants::io__entity_string u, const std::string v)
		{
			if (u != Constants::io__entity_string::size) edata.s[+u] = v;
		}

		void Entity::set(Constants::io__entity_color u, const ALLEGRO_COLOR v)
		{
			if (u != Constants::io__entity_color::size) edata.c[+u] = v;
		}

		void Entity::set(Constants::io__entity_double u, const double v)
		{
			if (u != Constants::io__entity_double::size) edata.d[+u] = v;
		}

		void Entity::get(Constants::io__entity_string u, std::string& v)
		{
			if (u != Constants::io__entity_string::size) v = edata.s[+u];
		}

		void Entity::get(Constants::io__entity_color u, ALLEGRO_COLOR& v)
		{
			if (u != Constants::io__entity_color::size) v = edata.c[+u];
		}

		void Entity::get(Constants::io__entity_double u, double& v)
		{
			if (u != Constants::io__entity_double::size) v = edata.d[+u];
		}

		bool Entity::isEq(Constants::io__entity_string u, const std::string v)
		{
			std::string e;
			get(u, e);
			return e == v;
		}

		bool Entity::isEq(Constants::io__entity_color u, const ALLEGRO_COLOR v)
		{
			ALLEGRO_COLOR e;
			get(u, e);
			return (e.r == v.r) && (e.g == v.g) && (e.b == v.b) && (e.a == v.a);
		}

		bool Entity::isEq(Constants::io__entity_double u, const double v)
		{
			double e;
			get(u, e);
			return e == v;
		}






		void Text::_draw(const double targ_draw_xy[2])
		{
			al_draw_text(data.font, data.c, 1.0 * targ_draw_xy[0] / (data.d[+Constants::io__text_double::SCALEG]), 1.0 * targ_draw_xy[1] / (data.d[+Constants::io__text_double::SCALEG]), data.i[+Constants::io__text_integer::MODE], data.str[+Constants::io__text_string::PROCESSED_STRING].c_str());
		}
		void Text::_interpretTags(std::string& s)
		{
			double timing = al_get_time();
			std::string fina = s;

			for (bool b = true; b;)
			{
				b = false;
				for (size_t q = 0; q < +Constants::tags_e::size; q++)
				{
					size_t poss = 0;
					if ((poss = fina.find(Constants::tags[q].s())) != std::string::npos)
					{
						b = true;
						Camera cam;
						Database conf;
						ALLEGRO_DISPLAY* d = al_get_current_display();
						char tempstr_c[128];
						//gfile logg;

						switch (q)
						{
						case +Constants::tags_e::T_POSX:
							sprintf_s(tempstr_c, "%.3lf", (data.d[+Constants::io__text_double::POSX] + data.d[+Constants::io__text_double::LAST_FOLLOW_POSX] + +(cam.get().get(Constants::io__camera_double::OFFSET_X))));
							break;
						case +Constants::tags_e::T_POSY:
							sprintf_s(tempstr_c, "%.3lf", (data.d[+Constants::io__text_double::POSY] + data.d[+Constants::io__text_double::LAST_FOLLOW_POSY] + +cam.get().get(Constants::io__camera_double::OFFSET_Y)));
							break;
						case +Constants::tags_e::T_SCREEN_POSX:
							sprintf_s(tempstr_c, "%.3lf", (data.d[+Constants::io__text_double::POSX] * (data.b[+Constants::io__text_boolean::AFFECTED_BY_CAM] ? +(cam.get().get(Constants::io__camera_double::SCALE_G) * cam.get().get(Constants::io__camera_double::SCALE_X)) : 1.0) + data.d[+Constants::io__text_double::LAST_FOLLOW_POSX] * +(cam.get().get(Constants::io__camera_double::SCALE_G) * cam.get().get(Constants::io__camera_double::SCALE_X))));
							break;
						case +Constants::tags_e::T_SCREEN_POSY:
							sprintf_s(tempstr_c, "%.3lf", (data.d[+Constants::io__text_double::POSY] * (data.b[+Constants::io__text_boolean::AFFECTED_BY_CAM] ? +(cam.get().get(Constants::io__camera_double::SCALE_G) * cam.get().get(Constants::io__camera_double::SCALE_Y)) : 1.0) + data.d[+Constants::io__text_double::LAST_FOLLOW_POSY] * +(cam.get().get(Constants::io__camera_double::SCALE_G) * cam.get().get(Constants::io__camera_double::SCALE_Y))));
							break;

						case +Constants::tags_e::T_CAM_X:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Constants::io__camera_double::OFFSET_X));
							break;
						case +Constants::tags_e::T_CAM_Y:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Constants::io__camera_double::OFFSET_Y));
							break;
						case +Constants::tags_e::T_CAM_ZOOM:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Constants::io__camera_double::SCALE_G) * sqrt(cam.get().get(Constants::io__camera_double::SCALE_X) * cam.get().get(Constants::io__camera_double::SCALE_Y)));
							break;
						case +Constants::tags_e::T_CAM_ZOOMG:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Constants::io__camera_double::SCALE_G));
							break;
						case +Constants::tags_e::T_CAM_ZOOMX:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Constants::io__camera_double::SCALE_X));
							break;
						case +Constants::tags_e::T_CAM_ZOOMY:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Constants::io__camera_double::SCALE_Y));
							break;

						case +Constants::tags_e::T_ISFOLLOWING:
							sprintf_s(tempstr_c, "%s", (follow ? "Y" : "N"));
							break;
						case +Constants::tags_e::T_COLOR_R:
							sprintf_s(tempstr_c, "%.3f", data.c.r);
							break;
						case +Constants::tags_e::T_COLOR_G:
							sprintf_s(tempstr_c, "%.3f", data.c.g);
							break;
						case +Constants::tags_e::T_COLOR_B:
							sprintf_s(tempstr_c, "%.3f", data.c.b);
							break;
						case +Constants::tags_e::T_COLOR_A:
							sprintf_s(tempstr_c, "%.3f", data.c.a);
							break;
						case +Constants::tags_e::T_MODE:
							sprintf_s(tempstr_c, "%d", data.i[+Constants::io__text_integer::MODE]);
							break;
						case +Constants::tags_e::T_TIME:
							sprintf_s(tempstr_c, "%.3lf", al_get_time());
							break;

						case +Constants::tags_e::T_GB_RESX:
							sprintf_s(tempstr_c, "%d", (d ? al_get_display_width(d) : -1));
							break;
						case +Constants::tags_e::T_GB_RESY:
							sprintf_s(tempstr_c, "%d", (d ? al_get_display_height(d) : -1));
							break;
						case +Constants::tags_e::T_REFRESHRATE:
							sprintf_s(tempstr_c, "%d", ((d) ? al_get_display_refresh_rate(d) : -1));
							break;


						case +Constants::tags_e::T_FPS:
						{
							size_t t;
							conf.get(Constants::ro__db_statistics_sizet::FRAMESPERSECOND, t);
							sprintf_s(tempstr_c, "%zu", t);
						}
						break;
						case +Constants::tags_e::T_TPS:
						{
							size_t t;
							conf.get(Constants::ro__db_statistics_sizet::COLLISIONSPERSECOND, t);
							sprintf_s(tempstr_c, "%zu", t);
						}
						break;
						case +Constants::tags_e::T_UPS:
						{
							size_t t;
							conf.get(Constants::ro__db_statistics_sizet::USEREVENTSPERSECOND, t);
							sprintf_s(tempstr_c, "%zu", t);
						}
						break;
						case +Constants::tags_e::T_APS:
						{
							size_t t;
							conf.get(Constants::ro__db_statistics_sizet::ADVANCEDFUNCSPERSECOND, t);
							sprintf_s(tempstr_c, "%zu", t);
						}
						break;

						case +Constants::tags_e::T_I_FPS:
						{
							double dt;
							conf.get(Constants::ro__db_statistics_double::INSTANT_FRAMESPERSECOND, dt);
							sprintf_s(tempstr_c, "%05.1lf", 1.0 / dt);
						}
						break;
						case +Constants::tags_e::T_I_TPS:
						{
							double dt;
							conf.get(Constants::ro__db_statistics_double::INSTANT_COLLISIONSPERSECOND, dt);
							sprintf_s(tempstr_c, "%05.1lf", 1.0 / dt);
						}
						break;
						case +Constants::tags_e::T_I_UPS:
						{
							double dt;
							conf.get(Constants::ro__db_statistics_double::INSTANT_USEREVENTSPERSECOND, dt);
							sprintf_s(tempstr_c, "%05.1lf", 1.0 / dt);
						}
						break;
						case +Constants::tags_e::T_I_APS:
						{
							double dt;
							conf.get(Constants::ro__db_statistics_double::INSTANT_ADVANCEDFUNCSPERSECOND, dt);
							sprintf_s(tempstr_c, "%05.1lf", 1.0 / dt);
						}
						break;

						case +Constants::tags_e::T_MS_FPS:
						{
							double dt;
							conf.get(Constants::ro__db_statistics_double::INSTANT_FRAMESPERSECOND, dt);
							sprintf_s(tempstr_c, "%04.3lf", 1000.0 * dt);
						}
						break;
						case +Constants::tags_e::T_MS_TPS:
						{
							double dt;
							conf.get(Constants::ro__db_statistics_double::INSTANT_COLLISIONSPERSECOND, dt);
							sprintf_s(tempstr_c, "%04.3lf", 1000.0 * dt);
						}
						break;
						case +Constants::tags_e::T_MS_UPS:
						{
							double dt;
							conf.get(Constants::ro__db_statistics_double::INSTANT_USEREVENTSPERSECOND, dt);
							sprintf_s(tempstr_c, "%04.3lf", 1000.0 * dt);
						}
						break;
						case +Constants::tags_e::T_MS_APS:
						{
							double dt;
							conf.get(Constants::ro__db_statistics_double::INSTANT_ADVANCEDFUNCSPERSECOND, dt);
							sprintf_s(tempstr_c, "%04.3lf", 1000.0 * dt);
						}
						break;


						case +Constants::tags_e::T_SPRITE_FRAME:
							if (follow) {
								size_t f;
								follow->get(Constants::io__sprite_sizet::FRAME, f);
								sprintf_s(tempstr_c, "%zu", f);
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Constants::tags_e::T_MOUSE_X:
						{
							double x;
							conf.get(Constants::ro__db_mouse_double::MOUSE_X, x);
							sprintf_s(tempstr_c, "%.3f", x);
						}
						break;
						case +Constants::tags_e::T_MOUSE_Y:
						{
							double y;
							conf.get(Constants::ro__db_mouse_double::MOUSE_Y, y);
							sprintf_s(tempstr_c, "%.3f", y);
						}
						break;
						case +Constants::tags_e::T_LASTSTRING:
						{
							std::string str;
							size_t p = 0;
							conf.get(Constants::ro__db_string::LAST_STRING, str);
							for (p = 0; p < 128 && p < str.length(); p++) tempstr_c[p] = str[p];
							if (p < 128) tempstr_c[p] = '\0';
						}
						break;
						case +Constants::tags_e::T_CURRSTRING:
						{
							std::string str;
							size_t p = 0;
							conf.get(Constants::ro__db_string::CURRENT_STRING, str);
							for (p = 0; p < 128 && p < str.length(); p++) tempstr_c[p] = str[p];
							if (p < 128) tempstr_c[p] = '\0';
						}
						break;
						case +Constants::tags_e::T_SPRITE_SPEEDX:
							if (follow) {
								double val;
								follow->get(Constants::io__sprite_double::RO_SPEEDX, val);
								sprintf_s(tempstr_c, "%.3lf", val);
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Constants::tags_e::T_SPRITE_SPEEDY:
							if (follow) {
								double val;
								follow->get(Constants::io__sprite_double::RO_SPEEDY, val);
								sprintf_s(tempstr_c, "%.3lf", val);
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Constants::tags_e::T_SPRITE_NAME:
							if (follow) {
								std::string temp;
								follow->get(Constants::io__sprite_string::ID, temp);
								sprintf_s(tempstr_c, "%s", temp.c_str());
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Constants::tags_e::T_SPRITE_ENTITY_HEALTH:
							if (follow) {
								if (follow->isThisEntity()) {
									double health = 0.0;
									Entity* lmao = (Entity*)follow;
									lmao->get(Constants::io__entity_double::HEALTH, health);
									sprintf_s(tempstr_c, "%.0lf%c", 100.0 * health, '%');
								}
								else sprintf_s(tempstr_c, "NULL"); 
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Constants::tags_e::T_SPRITE_ENTITY_NAME:
							if (follow) {
								if (follow->isThisEntity()) {
									std::string nickname;
									Entity* lmao = (Entity*)follow;
									lmao->get(Constants::io__entity_string::NICKNAME, nickname);
									sprintf_s(tempstr_c, "%s", nickname.c_str());
								}
								else sprintf_s(tempstr_c, "NULL");
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Constants::tags_e::_T_SPRITE_DEBUG:
							if (follow) {
								std::string str = follow->__debug_str();

								if (str.length() >= 128) str = str.substr(0, 124) + "...";

								if (str.length() > 0) sprintf_s(tempstr_c, "%s", str.c_str());
								else sprintf_s(tempstr_c, "NO INFORMATION");
							}
							else sprintf_s(tempstr_c, "NOT_IMPLEMENTED");
							break;
						case +Constants::tags_e::T_TEXTURES_LOADED:
						{
							Textures textures;
							sprintf_s(tempstr_c, "%zu", textures.size());
						}
						break;
						case +Constants::tags_e::T_FONTS_LOADED:
						{
							Fonts fonts;
							sprintf_s(tempstr_c, "%zu", fonts.size());
						}
						break;
						case +Constants::tags_e::T_SAMPLES_LOADED:
						{
							Samples samples;
							sprintf_s(tempstr_c, "%zu", samples.size());
						}
						break;
						case +Constants::tags_e::T_SPRITES_LOADED:
						{
							Sprites sprites;
							sprintf_s(tempstr_c, "%zu", sprites.size());
						}
						break;
						case +Constants::tags_e::T_TEXTS_LOADED:
						{
							Texts texts;
							sprintf_s(tempstr_c, "%zu", texts.size());
						}
						break;
						case +Constants::tags_e::T_TRACKS_LOADED:
						{
							Tracks tracks;
							sprintf_s(tempstr_c, "%zu", tracks.size());
						}
						break;
						case +Constants::tags_e::T_SPRITE_STATE:
							if (follow) {
								Constants::io__sprite_tie_func_to_state stat;
								follow->get(Constants::ro__sprite_state::STATE, stat);
								switch (stat) {
								case Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON:
									sprintf_s(tempstr_c, "MOUSE ON");
									break;
								case Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK:
									sprintf_s(tempstr_c, "MOUSE CLICK");
									break;
								case Constants::io__sprite_tie_func_to_state::COLLISION_COLLIDED_OTHER:
									sprintf_s(tempstr_c, "OTHER COLLIDING");
									break;
								case Constants::io__sprite_tie_func_to_state::COLLISION_NONE:
									sprintf_s(tempstr_c, "NONE");
									break;
								}
							}
							else sprintf_s(tempstr_c, "NULL");
						break;
						case +Constants::tags_e::T_VOLUME:
						{
							Mixer mixer;
							sprintf_s(tempstr_c, "%.0lf%c", mixer.getVolume() * 100.0, '%');
						}
						break;
						case +Constants::tags_e::T_VERSION:
						{
							Mixer mixer;
							sprintf_s(tempstr_c, "%s", Constants::version_app.c_str());
						}
						break;
						}


						if (fina.length() > (poss + Constants::tags[q].s().length()))  fina = fina.substr(0, poss) + tempstr_c + fina.substr(poss + Constants::tags[q].s().length());
						else														    fina = fina.substr(0, poss) + tempstr_c;
					}
				}
				if (al_get_time() - timing > Constants::text_timeout_interpret) { // should be fine, because it is probably impossible to take 500 ms to interpret a string, come on, less than 2 fps?
					throw Abort::abort(__FUNCSIG__, "Took too long interpreting '" + s + "'! [Stopped at '" + fina + "']");
					return;
				}
			}
			s = fina;
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
			/*gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__) << "Registered spawn of Text #" + std::to_string((size_t)this) << L::ELF;
			logg.flush();*/

			data.d[+Constants::io__text_double::POSX] = 0.0;
			data.d[+Constants::io__text_double::POSY] = 0.0;
			data.d[+Constants::io__text_double::SCALEX] = 1.0;
			data.d[+Constants::io__text_double::SCALEY] = 1.0;
			data.d[+Constants::io__text_double::ROTATION] = 0.0;
			data.d[+Constants::io__text_double::SCALEG] = 1.0;
			data.d[+Constants::io__text_double::LAST_FOLLOW_POSX] = 0.0;
			data.d[+Constants::io__text_double::LAST_FOLLOW_POSY] = 0.0;
			data.d[+Constants::io__text_double::LAST_INTERPRET] = 0.0;
			data.d[+Constants::io__text_double::UPDATETIME] = 1.0 / 24;

			data.i[+Constants::io__text_integer::LAYER] = 0;
			data.i[+Constants::io__text_integer::MODE] = ALLEGRO_ALIGN_CENTER;

			data.b[+Constants::io__text_boolean::AFFECTED_BY_CAM] = true;
			data.b[+Constants::io__text_boolean::SHOW] = true;

			data.font = nullptr;
			data.c = al_map_rgb(255, 255, 255);
		}

		/*Text::~Text()
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__) << "Registered despawn of Text #" + std::to_string((size_t)this)/* << ";ID=" << id / << L::ELF;
		}*/


		void Text::set(const Constants::io__text_string o, const std::string e)
		{
			data.str[+o] = e;

			switch (o) {
			case Constants::io__text_string::FOLLOW_SPRITE:
				setFollow(e);
				break;
			case Constants::io__text_string::FONT:
				__template_static_vector<ALLEGRO_FONT> fonts;
				fonts.get(e, data.font);
				break;
			}
		}

		void Text::set(const Constants::io__text_boolean o, const bool e)
		{
			if (o != Constants::io__text_boolean::size) data.b[+o] = e;
		}

		void Text::set(const Constants::io__text_double o, const double e)
		{
			if (o != Constants::io__text_double::size) data.d[+o] = e;
		}

		void Text::set(const Constants::io__text_color o, const ALLEGRO_COLOR e)
		{
			data.c = e; // no ::size
		}

		void Text::set(const Constants::io__text_integer o, const int e)
		{
			if (+o < +Constants::io__text_integer::size) data.i[+o] = e;
			else {
				switch (o) {

				case Constants::io__text_integer::ADD_ALT_LAYER:
				{
					bool has = false;
					data.layers_colliding_m.lock();

					for (auto& i : data.layers_colliding) {
						if (i == e) {
							has = true;
							break;
						}
					}
					if (!has && e != data.i[+Constants::io__text_integer::LAYER]) data.layers_colliding.push_back(e);

					data.layers_colliding_m.unlock();
				}
				break;
				case Constants::io__text_integer::REMOVE_ALT_LAYER:
				{
					data.layers_colliding_m.lock();

					for (size_t p = 0; p < data.layers_colliding.size(); p++) {
						if (data.layers_colliding[p] == e) {
							data.layers_colliding.erase(data.layers_colliding.begin() + p);
							break;
						}
					}

					data.layers_colliding_m.unlock();
				}
				break;
				}
			}
		}

		void Text::hook(const Constants::io__text_tie_func_to_state o, const std::function<void(std::string&)> e)
		{
			if (o != Constants::io__text_tie_func_to_state::size) data.pair_tied[+o] = e;
		}

		void Text::unhook(const Constants::io__text_tie_func_to_state o)
		{
			hook(o, std::function<void(std::string&)>());
		}


		void Text::get(const Constants::io__text_string o, std::string& e)
		{
			if (o != Constants::io__text_string::size) e = data.str[+o];
		}
		void Text::get(const Constants::io__text_boolean o, bool& e)
		{
			if (o != Constants::io__text_boolean::size) e = data.b[+o];
		}
		void Text::get(const Constants::io__text_double o, double& e)
		{
			if (o != Constants::io__text_double::size) e = data.d[+o];
		}
		void Text::get(const Constants::io__text_integer o, int& e)
		{
			if (o != Constants::io__text_integer::size) e = data.i[+o];
		}
		void Text::get(const Constants::io__text_color o, ALLEGRO_COLOR& e)
		{
			e = data.c; // no ::size
		}

		bool Text::isEq(const Constants::io__text_color e, const ALLEGRO_COLOR v)
		{
			ALLEGRO_COLOR g;
			get(e, g);
			return ((g.r == v.r) && (g.g == v.g) && (g.b == v.b) && (g.a == v.a));
		}
		bool Text::isEq(const Constants::io__text_string e, const std::string v)
		{
			std::string g;
			get(e, g);
			return g == v;
		}
		bool Text::isEq(const Constants::io__text_boolean e, const bool v)
		{
			bool g;
			get(e, g);
			return g == v;
		}
		bool Text::isEq(const Constants::io__text_double e, const double v)
		{
			double g;
			get(e, g);
			return g == v;
		}
		bool Text::isEq(const Constants::io__text_integer e, const int v)
		{
			int g;
			get(e, g);
			return g == v || [&]() {
				if (e != Constants::io__text_integer::LAYER) return false; // return false forcing anything but LAYER to be TRUE on g == v part
				auto cpy = data.layers_colliding;
				for (auto& i : cpy) {
					if (i == v) {
						return true;
					}
				}
				return false;
			}();
		}


		void Text::draw(const int is_layer)
		{
			if (!data.b[+Constants::io__text_boolean::SHOW]) return;
			if (!data.font) return;
			if (!isEq(Constants::io__text_integer::LAYER, is_layer)) return;

			if (follow) {
				if (follow->isEq(Constants::io__sprite_boolean::DRAW, false)) return;
				if (data.b[+Constants::io__text_boolean::USE_SPRITE_TINT_INSTEAD]) {
					follow->get(Constants::io__sprite_color::TINT, data.c);
				}
			}

			{
				std::string working_now = data.str[+Constants::io__text_string::STRING];
				if (data.pair_tied[+Constants::io__text_tie_func_to_state::ON_DRAW])
					data.pair_tied[+Constants::io__text_tie_func_to_state::ON_DRAW](working_now);

				if (al_get_time() - data.d[+Constants::io__text_double::LAST_INTERPRET] > data.d[+Constants::io__text_double::UPDATETIME]) {

					if (al_get_time() - data.d[+Constants::io__text_double::LAST_INTERPRET] > data.d[+Constants::io__text_double::UPDATETIME] * 3) data.d[+Constants::io__text_double::LAST_INTERPRET] = al_get_time();
					else data.d[+Constants::io__text_double::LAST_INTERPRET] += data.d[+Constants::io__text_double::UPDATETIME];
					if (follow) {
						Constants::io__sprite_tie_func_to_state st;
						follow->get(Constants::ro__sprite_state::STATE, st);
						if (+st <= +Constants::io__text_tie_func_to_state::__SPRITE_TIED_MAX) {
							if (data.pair_tied[+st]) data.pair_tied[+st](working_now);
						}
					}
					if (data.pair_tied[+Constants::io__text_tie_func_to_state::ON_UPDATE]) data.pair_tied[+Constants::io__text_tie_func_to_state::ON_UPDATE](working_now);

					std::string b4 = data.str[+Constants::io__text_string::PROCESSED_STRING];
					data.str[+Constants::io__text_string::PROCESSED_STRING] = working_now;
					_interpretTags(data.str[+Constants::io__text_string::PROCESSED_STRING]);
				}
			}

			while (data.d[+Constants::io__text_double::ROTATION] > 360.0) data.d[+Constants::io__text_double::ROTATION] -= 360.0;
			while (data.d[+Constants::io__text_double::ROTATION] < 0.0) data.d[+Constants::io__text_double::ROTATION] += 360.0;

			// follow sprite stuff
			double rotation = 0.0;

			Camera cam;
			
			camera_preset preset = cam.get();
			preset.set(Constants::io__camera_boolean::READONLY_NOW, false);

			ALLEGRO_BITMAP* d = al_get_target_bitmap();
			if (!d) return;// throw "TEXT::DRAW - NO DISPLAY!";

			if (follow) {
				follow->get(Constants::io__sprite_double::POSX, data.d[+Constants::io__text_double::LAST_FOLLOW_POSX]);
				follow->get(Constants::io__sprite_double::POSY, data.d[+Constants::io__text_double::LAST_FOLLOW_POSY]);
				follow->get(Constants::io__sprite_double::ROTATION, rotation);
			}
			else {
				data.d[+Constants::io__text_double::LAST_FOLLOW_POSX] = data.d[+Constants::io__text_double::LAST_FOLLOW_POSY] = 0.0;
			}

			double t_rotation_rad = (data.d[+Constants::io__text_double::ROTATION])*ALLEGRO_PI / 180.0;
			double p_rotation_rad = (rotation)*ALLEGRO_PI / 180.0;
			double rotation_rad = t_rotation_rad + p_rotation_rad;

			if (rotation_rad > ALLEGRO_PI * 2) rotation_rad -= ALLEGRO_PI * 2;
			if (rotation_rad < 0) rotation_rad += ALLEGRO_PI * 2;

			double pos_now[2];

			pos_now[0] = 1.0 * Constants::text_default_sharpness_font * 
				(((data.d[+Constants::io__text_double::POSX]) * cos(p_rotation_rad))
					- ((data.d[+Constants::io__text_double::POSY]) * sin(p_rotation_rad)) 
					+ data.d[+Constants::io__text_double::LAST_FOLLOW_POSX]);

			pos_now[1] = 1.0 * Constants::text_default_sharpness_font * 
				(((data.d[+Constants::io__text_double::POSY]) * cos(p_rotation_rad)) 
					- ((data.d[+Constants::io__text_double::POSX]) * sin(p_rotation_rad)) 
					+ data.d[+Constants::io__text_double::LAST_FOLLOW_POSY]);


			double targ_draw_xy[2];
			targ_draw_xy[0] = pos_now[0] * cos(rotation_rad) + pos_now[1] * sin(rotation_rad);
			targ_draw_xy[1] = pos_now[1] * cos(rotation_rad) - pos_now[0] * sin(rotation_rad);
			targ_draw_xy[0] /= data.d[+Constants::io__text_double::SCALEX];
			targ_draw_xy[1] /= data.d[+Constants::io__text_double::SCALEY];


			if (!data.b[+Constants::io__text_boolean::AFFECTED_BY_CAM]) preset.reset();

			preset.set(Constants::io__camera_double::SCALE_G,  preset.get(Constants::io__camera_double::SCALE_G) * data.d[+Constants::io__text_double::SCALEG] * 1.0 / Constants::text_default_sharpness_font);
			preset.set(Constants::io__camera_double::SCALE_X, preset.get(Constants::io__camera_double::SCALE_X) * data.d[+Constants::io__text_double::SCALEX]);
			preset.set(Constants::io__camera_double::SCALE_Y, preset.get(Constants::io__camera_double::SCALE_Y) * data.d[+Constants::io__text_double::SCALEY]);
			preset.set(Constants::io__camera_double::OFFSET_X, preset.get(Constants::io__camera_double::OFFSET_X) * Constants::text_default_sharpness_font / data.d[+Constants::io__text_double::SCALEG]);
			preset.set(Constants::io__camera_double::OFFSET_Y, preset.get(Constants::io__camera_double::OFFSET_Y) * Constants::text_default_sharpness_font / data.d[+Constants::io__text_double::SCALEG]);

			preset.set(Constants::io__camera_double::ROTATION_RAD, preset.get(Constants::io__camera_double::ROTATION_RAD) + rotation_rad);
			cam.applyNoSave(preset);

			_draw(targ_draw_xy);

			al_set_target_bitmap(d);
			cam.apply();
		}





		const double BubblesFX::maxone(double gotten, const double prop)
		{
			while (gotten > 1.0) {
				if (gotten > 1000.0) gotten -= 1000.0;
				else if (gotten > 100000.0) gotten -= 100000.0;
				else if (gotten > 2.0) gotten -= 1.0;
				else {
					if (gotten > 1.0) gotten = 2.0 - gotten;
					else gotten -= 1.0;
				}
			}
			return gotten * prop;
		}


		void BubblesFX::init(const unsigned amout, const double mfps, const int layer)
		{
			if (firstcall) {

				__template_static_vector<ALLEGRO_BITMAP> textures;
				__template_static_vector<Sprite> sprites;

				Database db;
				db.get(Constants::io__conf_integer::SCREEN_X, siz[0]);
				db.get(Constants::io__conf_integer::SCREEN_Y, siz[1]);

				siz[0] *= 0.8;
				siz[1] *= 0.8;

				if (siz[0] < 1280) siz[0] = 1280;
				if (siz[1] < 720 ) siz[1] = 720;

				imglw = textures.customLoad("BKG_NULL",	[&](ALLEGRO_BITMAP*& b) -> bool { return (b = al_create_bitmap(siz[0], siz[1]));}); // don't worry, it won't create if it is already there
				disguy = sprites.create("BKG_NULL"); // this one too

				disguy->set(Constants::io__sprite_string::REMOVE, "BKG_NULL"); // guarantee no duplicated stuff because it might have it already
				disguy->set(Constants::io__sprite_string::ADD, "BKG_NULL");
				disguy->set(Constants::io__sprite_double::ANIMATION_FPS, 0);
				disguy->set(Constants::io__sprite_boolean::DRAW, true);
				disguy->set(Constants::io__sprite_boolean::TIE_SIZE_TO_DISPLAY, true);
				disguy->set(Constants::io__sprite_string::ID, "BKG_NULL");
				disguy->set(Constants::io__sprite_integer::LAYER, layer);
				disguy->set(Constants::io__sprite_double::SCALEG, 2.0);
				disguy->set(Constants::io__sprite_boolean::AFFECTED_BY_CAM, false);

				if (mfps <= 0) fps = -1;
				else if (1.0/mfps > 1.0) fps = 1.0/mfps;
				lastdraw = al_get_time();

				if (amout > 0) {
					amountofentities = amout;
				}

				particle poss;

				if (positions.size() > 0) positions.clear();

				for (unsigned c = 0; c < amountofentities; c++)
				{
					poss.posx = 1.0 - (rand() % 1000) / 500.0;
					poss.posy = 1.0 - (rand() % 1000) / 500.0;

					positions.push_back(poss);
				}
				firstcall = false;
			}
		}

		bool BubblesFX::draw(const int lyr)
		{
			if (firstcall) return false;
			if (!disguy) return false;
			if (!disguy->isEq(Constants::io__sprite_integer::LAYER, lyr)) return false;

			if (alreadyreset) {
				alreadyreset = false;

				ALLEGRO_BITMAP* lastpoint = al_get_target_bitmap();

				disguy->get(imglw);
				al_set_target_bitmap(imglw);

				siz[0] = al_get_bitmap_width(imglw);
				siz[1] = al_get_bitmap_height(imglw);
				
				for (auto& i : positions)
				{
					al_draw_filled_circle(i.lastpositionscalculated[0], i.lastpositionscalculated[1], i.lastsize, al_map_rgba_f(
						+maxone(al_get_time() * 0.3f * ((rand() % 1000) / 1000.0f)) * (1.0f - (Constants::amountofblur_bubbles / 100.0f)),
						+maxone(al_get_time() * 0.1f * ((rand() % 1000) / 1000.0f)) * (1.0f - (Constants::amountofblur_bubbles / 100.0f)),
						+maxone(al_get_time() * 0.8f * ((rand() % 1000) / 1000.0f)) * (1.0f - (Constants::amountofblur_bubbles / 100.0f)),
						+(1.0f - (Constants::amountofblur_bubbles / 100.0f))));
				}

				al_set_target_bitmap(lastpoint);
			}
			return true;
		}
		bool BubblesFX::think(const int lyr)
		{
			if (firstcall) return false;
			if (!disguy) return false;
			if (!disguy->isEq(Constants::io__sprite_integer::LAYER, lyr)) return false;

			if (((al_get_time() - lastdraw) >= (1.0 / fps) || fps == -1))
			{
				lastdraw = al_get_time();
				alreadyreset = true;

				//gotten = true;

				for (auto& i : positions)
				{
					i.lastsize = (12.0 + (rand() % 500) / 100.0) * 3.0 * sqrt(siz[0] * siz[1]) / 1440.0;
					i.posx = 1.0 - (rand() % 1000) / 500.0;
					i.posy = 1.0 - (rand() % 1000) / 500.0;

					if (i.posx > 1.0) i.posx = 1.0;
					if (i.posy > 1.0) i.posy = 1.0;
					if (i.posx < -1.0) i.posx = -1.0;
					if (i.posy < -1.0) i.posy = -1.0;

					i.lastpositionscalculated[0] = ((i.posx + 1.0) / 2.0) * siz[0];
					i.lastpositionscalculated[1] = ((i.posy + 1.0) / 2.0) * siz[1];
				}
			}
			return true;
		}

		const double LiningFX::maxone(double gotten, const double prop)
		{
			while (gotten > 1.0) {
				if (gotten > 1000.0) gotten -= 1000.0;
				else if (gotten > 100000.0) gotten -= 100000.0;
				else if (gotten > 2.0) gotten -= 1.0;
				else {
					if (gotten > 1.0) gotten = 2.0 - gotten;
					else gotten -= 1.0;
				}
			}
			return gotten * prop;
		}

		void LiningFX::init(const int layer)
		{
			__template_static_vector<ALLEGRO_BITMAP> textures;
			__template_static_vector<Sprite> sprites;

			Database db;
			db.get(Constants::io__conf_integer::SCREEN_X, siz[0]);
			db.get(Constants::io__conf_integer::SCREEN_Y, siz[1]);
			
			if (siz[0] < 1280) siz[0] = 1280;
			if (siz[1] < 720) siz[1] = 720;

			imglw = textures.customLoad("LINES_NULL", [&](ALLEGRO_BITMAP*& b) -> bool { return (b = al_create_bitmap(siz[0], siz[1])); }); // don't worry, it won't create if it is already there
			disguy = sprites.create("LINES_NULL"); // this one too

			disguy->set(Constants::io__sprite_string::REMOVE, "LINES_NULL"); // guarantee no duplicated stuff because it might have it already
			disguy->set(Constants::io__sprite_string::ADD, "LINES_NULL");
			disguy->set(Constants::io__sprite_double::ANIMATION_FPS, 0);
			disguy->set(Constants::io__sprite_boolean::DRAW, true);
			disguy->set(Constants::io__sprite_boolean::TIE_SIZE_TO_DISPLAY, true);
			disguy->set(Constants::io__sprite_string::ID, "LINES_NULL");
			disguy->set(Constants::io__sprite_integer::LAYER, layer);
			disguy->set(Constants::io__sprite_double::SCALEG, 2.0);
			disguy->set(Constants::io__sprite_boolean::AFFECTED_BY_CAM, false);
		}

		bool LiningFX::draw(const int lyr)
		{
			if (!disguy) return false;
			if (!disguy->isEq(Constants::io__sprite_integer::LAYER, lyr)) return false;

			ALLEGRO_BITMAP* g = al_get_target_bitmap();
			assert(g);

			disguy->get(imglw);
			al_set_target_bitmap(imglw);

			al_clear_to_color(al_map_rgb_f(maxone(0.5 + cos(al_get_time() * 1.75), 0.6), maxone(0.5 + cos(al_get_time() * 2.15), 0.6), maxone((0.5 + cos(al_get_time() * 1.11)), 0.6)));

			//int rad = 2;
			//int xpress = al_get_bitmap_width(imglw);
			//int ypress = al_get_bitmap_height(imglw);

			const float precision = 0.1 * ALLEGRO_PI;// Constants::precision_rotation_draw;
			const double timerr = al_get_time();
			const ALLEGRO_COLOR using_rn = al_map_rgb_f(maxone(0.5 + cos(al_get_time() * 0.52), 0.7), maxone(0.5 + cos(al_get_time() * 0.15), 0.7), maxone((0.5 + cos(al_get_time() * 0.54)), 0.7));

			Camera cam;
			cam.applyNoSave(imglw);

			for (float radd = 0; radd < ALLEGRO_PI * 2; radd += precision) {
				ALLEGRO_VERTEX v[] = {
				  { +(2.0 * cos(radd + timerr * 0.2)), +(2.0 * sin(radd + timerr * 0.2)), 0, 0, 0, using_rn},   //top left
				  { +(2.0 * cos(radd + (precision * 0.5) + timerr * 0.2)), +(2.0 * sin(radd + (precision * 0.5) + timerr * 0.2)), 0, 0, 0, using_rn},   //top right
				  { 0.0, 0.0, 0, 0, 0, using_rn}//,							  //center
				  //{ corner_rightdn[0], corner_rightdn[1], 0, 0, 0, using_rn}, //bottom left
				  //{ corner_rightup[0], corner_rightup[1], 0, 0, 0, using_rn}  //bottom right
				};
				al_draw_prim(v, NULL, NULL, 0, 3, ALLEGRO_PRIM_TRIANGLE_LIST);
			}

			al_set_target_bitmap(g);

			return true;
		}
}
}