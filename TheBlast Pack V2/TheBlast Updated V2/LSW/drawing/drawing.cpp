#include "drawing.h"


namespace LSW {
	namespace v4 {

		std::map<int, camera_preset> Camera::presets;
		int Camera::lastapply = 0;



		void draw_simple_bar(const float perc, const ALLEGRO_COLOR bg, const float w, const float h)
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

		void draw_simple_txt(ALLEGRO_FONT* f, const std::string s, ALLEGRO_COLOR c, const int flag, const float scale)
		{
			Camera gcam;
			camera_preset cp;
			cp.set(Assistance::io__camera_float::SCALE_G, scale / Constants::text_default_sharpness_font);
			cp.set(Assistance::io__camera_float::SCALE_Y, 2.5);
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


/*			al_build_transform(&t,
				al_get_display_width(d) * 0.5 * (1.0 + x),
				al_get_display_height(d) * 0.5 * (1.0 + y),
				al_get_display_width(d) * 0.5 * sx,
				al_get_display_height(d) * 0.5 * sy,
				r);

			return t;*/
		}

		void matrix_draw_help()
		{
			float m[2] = { 0.0 };
			float limits[4] = { 0.0 };
			Database db;
			Camera cam;
			camera_preset psf = cam.get();
			camera_preset org = camera_preset();

			db.get(Assistance::io__db_mouse_float::RAW_MOUSE_X, m[0]);
			db.get(Assistance::io__db_mouse_float::RAW_MOUSE_Y, m[1]);

			limits[0] = psf.get(Assistance::io__camera_float::LIMIT_MIN_X);
			limits[1] = psf.get(Assistance::io__camera_float::LIMIT_MIN_Y);
			limits[2] = psf.get(Assistance::io__camera_float::LIMIT_MAX_X);
			limits[3] = psf.get(Assistance::io__camera_float::LIMIT_MAX_Y);

			psf.refresh();

			ALLEGRO_TRANSFORM untransf = psf.quick();
			al_invert_transform(&untransf);
			al_transform_coordinates(&untransf, &m[0], &m[1]);


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
			if (psf.get(Assistance::io__camera_boolean::RESPECT_LIMITS)) al_draw_rectangle(limits[0], limits[1], limits[2], limits[3], cp2, 0.010);


			cam.apply();
		}


		void camera_preset::refresh()
		{
			ALLEGRO_DISPLAY* d = al_get_current_display();
			if (!d) return;


			/*al_rotate_transform(&g_t, lastTransf.rotation);

			al_scale_transform(&g_t,
				al_get_display_width(d) * 0.5 * lastTransf.scale_x * lastTransf.scale_g,
				al_get_display_height(d) * 0.5 * lastTransf.scale_y * lastTransf.scale_g); // SCALING NOT AS EXPECTED (multiplier by zoom is not being made by transformation!)

			al_translate_transform(&g_t,
				al_get_display_width(d) * 0.5 - lastTransf.offset_x * (al_get_display_width(d) * 0.5 * lastTransf.scale_x * lastTransf.scale_g),
				al_get_display_height(d) * 0.5 - lastTransf.offset_y * (al_get_display_height(d) * 0.5 * lastTransf.scale_y * lastTransf.scale_g));*/

			latest = easyTransform(d,
				p[+Assistance::io__camera_float::OFFSET_X], p[+Assistance::io__camera_float::OFFSET_Y], p[+Assistance::io__camera_float::SCALE_X] * p[+Assistance::io__camera_float::SCALE_G], p[+Assistance::io__camera_float::SCALE_Y] * p[+Assistance::io__camera_float::SCALE_G], p[+Assistance::io__camera_float::ROTATION_RAD]);

			/*al_build_transform(&latest,
			al_get_display_width(d) * 0.5 * p[+Assistance::io__camera_float::SCALE_X] * p[+Assistance::io__camera_float::SCALE_G] * (1.0 + p[+Assistance::io__camera_float::OFFSET_X]),
			al_get_display_height(d) * 0.5 * p[+Assistance::io__camera_float::SCALE_Y] * p[+Assistance::io__camera_float::SCALE_G] * (1.0 + p[+Assistance::io__camera_float::OFFSET_Y]),
			//al_get_display_width(d) * 0.5 - (0.5 + p[+Assistance::io__camera_float::OFFSET_X] * (0.5 * p[+Assistance::io__camera_float::SCALE_X] * p[+Assistance::io__camera_float::SCALE_G])),
			//al_get_display_height(d) * 0.5 - (0.5 + p[+Assistance::io__camera_float::OFFSET_Y] * (0.5 * p[+Assistance::io__camera_float::SCALE_Y] * p[+Assistance::io__camera_float::SCALE_G])),
			al_get_display_width(d) * 0.5 * p[+Assistance::io__camera_float::SCALE_X] * p[+Assistance::io__camera_float::SCALE_G],
			al_get_display_height(d) * 0.5 * p[+Assistance::io__camera_float::SCALE_Y] * p[+Assistance::io__camera_float::SCALE_G],
				p[+Assistance::io__camera_float::ROTATION_RAD]);*/


			/*
			al_identity_transform(&latest);

			al_build_transform(&latest,
			(1.0 + get(Assistance::io__camera_float::OFFSET_X) * (al_get_display_width(d) * 0.5 * get(Assistance::io__camera_float::SCALE_X) * get(Assistance::io__camera_float::SCALE_G))),
			(1.0 + get(Assistance::io__camera_float::OFFSET_Y) * (al_get_display_height(d) * 0.5 * get(Assistance::io__camera_float::SCALE_Y) * get(Assistance::io__camera_float::SCALE_G))),
			(al_get_display_width(d) / 2) * get(Assistance::io__camera_float::SCALE_X) * get(Assistance::io__camera_float::SCALE_G),
			(al_get_display_height(d) / 2) * get(Assistance::io__camera_float::SCALE_Y) * get(Assistance::io__camera_float::SCALE_G),
			get(Assistance::io__camera_float::ROTATION_RAD));*/
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
			if (u == Assistance::io__camera_float::size) throw Abort::abort(__FUNCSIG__, "Shouldn't try to set ::size!");
			p[+u] = v;
			refresh();
		}

		void camera_preset::set(const Assistance::io__camera_boolean u, const bool v)
		{
			if (u == Assistance::io__camera_boolean::size) throw Abort::abort(__FUNCSIG__, "Shouldn't try to set ::size!");
			b[+u] = v;
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
			case Assistance::io__camera_float::ROTATION_RAD:
				p[+u] += v;
				break;
			}
			refresh();
		}

		float camera_preset::get(const Assistance::io__camera_float u) const
		{
			if (u == Assistance::io__camera_float::size) throw Abort::abort(__FUNCSIG__, "Shouldn't try to get ::size!");
			return p[+u];
		}

		bool camera_preset::get(const Assistance::io__camera_boolean u) const
		{
			if (u == Assistance::io__camera_boolean::size) throw Abort::abort(__FUNCSIG__, "Shouldn't try to get ::size!");
			return b[+u];
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

		void Camera::set(const camera_preset& p, const int i)
		{
			presets[i] = p;
			presets[i].refresh();
		}

		
		void Camera::apply(const int a)
		{
			lastapply = a;
			applyNoSave(presets[a]);
		}

		void Camera::apply()
		{
			auto u = get();
			applyNoSave(u);
		}

		
		ALLEGRO_TRANSFORM Camera::applyNoSave(camera_preset lastTransf)
		{
			ALLEGRO_DISPLAY* d = al_get_current_display();

			if (!d) return ALLEGRO_TRANSFORM();

			/*al_identity_transform(&g_t);

			al_build_transform(&g_t,
				(al_get_display_width(d) / 2) * (1.0 + lastTransf.get(Assistance::io__camera_float::OFFSET_X) * (al_get_display_width(d) * 0.5 * lastTransf.get(Assistance::io__camera_float::SCALE_X) * lastTransf.get(Assistance::io__camera_float::SCALE_G))),
				(al_get_display_height(d) / 2) * (1.0 + lastTransf.get(Assistance::io__camera_float::OFFSET_Y) * (al_get_display_height(d) * 0.5 * lastTransf.get(Assistance::io__camera_float::SCALE_Y) * lastTransf.get(Assistance::io__camera_float::SCALE_G))),
				(al_get_display_width(d) / 2) * lastTransf.get(Assistance::io__camera_float::SCALE_X) * lastTransf.get(Assistance::io__camera_float::SCALE_G),
				(al_get_display_height(d) / 2) * lastTransf.get(Assistance::io__camera_float::SCALE_Y) * lastTransf.get(Assistance::io__camera_float::SCALE_G),
				lastTransf.get(Assistance::io__camera_float::ROTATION_RAD));*/

			g_t = easyTransform(d,
				lastTransf.get(Assistance::io__camera_float::OFFSET_X), lastTransf.get(Assistance::io__camera_float::OFFSET_Y),
				lastTransf.get(Assistance::io__camera_float::SCALE_X) * lastTransf.get(Assistance::io__camera_float::SCALE_G),
				lastTransf.get(Assistance::io__camera_float::SCALE_Y) * lastTransf.get(Assistance::io__camera_float::SCALE_G),
				lastTransf.get(Assistance::io__camera_float::ROTATION_RAD));

			al_use_transform(&g_t);
			return g_t;
		}

		void Camera::applyNoSave(ALLEGRO_TRANSFORM* tr)
		{
			al_use_transform(tr);
		}

		

		camera_preset& Camera::get(const int u)
		{
			presets[u].refresh();
			return presets[u];
		}

		camera_preset& Camera::get()
		{
			return get(lastapply);
		}

		

		ALLEGRO_BITMAP* Sprite::__sprite_smart_images::get(const Assistance::io__sprite_collision_state stt)
		{
			if (copies.size() == 0) {
				throw Abort::abort(__FUNCSIG__, "No bitmap found!", 1);
				return nullptr;
			}

			if (stt == Assistance::io__sprite_collision_state::size) {
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
					if (pair[+Assistance::io__sprite_collision_state::COLLISION_NONE] != std::string::npos) return get(Assistance::io__sprite_collision_state::COLLISION_NONE); // like default one
					return get(Assistance::io__sprite_collision_state::size);
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
		void Sprite::__sprite_smart_images::setState(const Assistance::io__sprite_collision_state s, const size_t v)
		{
			if (s != Assistance::io__sprite_collision_state::size) pair[+s] = v;
		}
		size_t Sprite::__sprite_smart_images::getState(const Assistance::io__sprite_collision_state s)
		{
			if (s != Assistance::io__sprite_collision_state::size) return pair[+s];
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


		Sprite::__sprite_smart_data::__sprite_smart_data()
		{
			Database conf;
			bool debugging = false;
			conf.get(Assistance::io__conf_boolean::WAS_OSD_ON, debugging, Constants::_is_on_debug_mode);
			debugging |= Constants::_is_on_debug_mode;
						
			dval[+Assistance::io__sprite_double::SCALEX] = 1.0;
			dval[+Assistance::io__sprite_double::SCALEY] = 1.0;
			dval[+Assistance::io__sprite_double::SCALEG] = 1.0;
			dval[+Assistance::io__sprite_double::SMOOTHNESS_X] = Constants::sprite_default_smoothness;
			dval[+Assistance::io__sprite_double::SMOOTHNESS_Y] = Constants::sprite_default_smoothness;
			dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X] = 0.0;
			dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y] = 0.0;
			bval[+Assistance::io__sprite_boolean::AFFECTED_BY_CAM] = true;
			bval[+Assistance::io__sprite_boolean::COLLIDE_MOUSE] = true;
			bval[+Assistance::io__sprite_boolean::COLLIDE_IGNORE_LAYER] = false;
			bval[+Assistance::io__sprite_boolean::SHOWBOX] = debugging;
			bval[+Assistance::io__sprite_boolean::SHOWDOT] = debugging;
			bval[+Assistance::io__sprite_boolean::RESPECT_CAMERA_LIMITS] = true;
		}






		bool Sprite::fastIsColliding(camera_preset psf) // rad
		{			
			data.bval[+Assistance::io__sprite_boolean::RO_IS_OTHERS_COLLIDING] = false;

			if (data.bval[+Assistance::io__sprite_boolean::COLLIDE_MOUSE]) {

				if (data.bval[+Assistance::io__sprite_boolean::FOLLOWMOUSE]) {
					data.bval[+Assistance::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] = true;
				}

				else {
					float m[2] = { 0.0 };
					bool is_mouse_pressed = false;
					Database db;

					db.get(Assistance::io__db_mouse_float::MOUSE_X, m[0]);
					db.get(Assistance::io__db_mouse_float::MOUSE_Y, m[1]);
					db.get(Assistance::io__db_mouse_boolean::IS_ANY_PRESSED, is_mouse_pressed);

					//ALLEGRO_TRANSFORM untransf = psf.quick();
					//al_invert_transform(&untransf);
					//al_transform_coordinates(&untransf, &m[0], &m[1]);


					data.dval[+Assistance::io__sprite_double::RO_MOUSE_DISTANCE_X] = (data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX]) - (double)m[0];
					data.dval[+Assistance::io__sprite_double::RO_MOUSE_DISTANCE_Y] = (data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY]) - (double)m[1];


					data.bval[+Assistance::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] = (
						(fabs(data.dval[+Assistance::io__sprite_double::RO_MOUSE_DISTANCE_X]) < 0.5 * data.dval[+Assistance::io__sprite_double::SCALEX] * data.dval[+Assistance::io__sprite_double::SCALEG]) &&
						(fabs(data.dval[+Assistance::io__sprite_double::RO_MOUSE_DISTANCE_Y]) < 0.5 * data.dval[+Assistance::io__sprite_double::SCALEY] * data.dval[+Assistance::io__sprite_double::SCALEG])
						);

					if (data.bval[+Assistance::io__sprite_boolean::RO_IS_MOUSE_COLLIDING]) {
						data.dval[+Assistance::io__sprite_double::RO_LAST_MOUSE_COLLISION] = al_get_time();
						if (is_mouse_pressed) {
							data.new_state = Assistance::io__sprite_collision_state::COLLISION_MOUSE_CLICK;
						}
						else {
							data.new_state = Assistance::io__sprite_collision_state::COLLISION_MOUSE_ON;
						}
					}
				}
			}
			else {
				data.bval[+Assistance::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] = false;
			}

			return false;
		}

		Sprite::~Sprite()
		{
			bmps.reset();
		}

		void Sprite::setTask(const std::function<void(Sprite&)> f)
		{
			data.task = f;
		}

		void Sprite::unsetTask()
		{
			data.task = std::function<void(Sprite&)>();
		}

		void Sprite::hook(const Assistance::io__sprite_collision_state w, std::function<void(void)> f)
		{
			if (w != Assistance::io__sprite_collision_state::size) data.function_pair[+w] = f;
		}

		void Sprite::unhook(const Assistance::io__sprite_collision_state w)
		{
			if (w != Assistance::io__sprite_collision_state::size) data.function_pair[+w] = std::function<void(void)>();
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
			case Assistance::io__sprite_double::ANIMATION_FPS:  // don't forget COLLISION_STATE can bug this one
				bmps.setFPS(v);
				break;
			default:
				data.dval[+u] = v;
				// target set to value
				switch (+u) {
				case +Assistance::io__sprite_double::POSX:
					data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX] = v;
					break;
				case +Assistance::io__sprite_double::POSY:
					data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY] = v;
					break;
				case +Assistance::io__sprite_double::ROTATION:
					data.dtarg[+Assistance::ro__sprite_target_double::TARG_ROTATION] = v;
					break;
				}
				break;
			}
		}
		void Sprite::set(const Assistance::io__sprite_boolean u, const bool v)
		{
			switch (u) {
			case Assistance::io__sprite_boolean::LOOPFRAMES:
				bmps.loop(v);
				break;
			case Assistance::io__sprite_boolean::HAS_DONE_LOOP_ONCE:
				// nothing to do
				break;
			case Assistance::io__sprite_boolean::ZERO_RESETS_POSITION_INSTEAD_OF_FREEZING: // setFPS(0) stops animation or reset to 0?
				bmps.resetInsteadOfPause(v);
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
			case Assistance::io__sprite_integer::ADD_ANOTHER_LAYER_COLLISION:
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
			case Assistance::io__sprite_integer::REMOVE_LAYER_COLLISION:
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

		void Sprite::set(const Assistance::io__sprite_collision_state u, const size_t v)
		{
			bmps.setState(u, v);
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
		bool Sprite::get(const Assistance::ro__sprite_target_double u, double& v)
		{
			if (u == Assistance::ro__sprite_target_double::size) return false;
			v = data.dtarg[+u];
			return true;
		}
		bool Sprite::get(const Assistance::io__sprite_boolean u, bool& v)
		{
			switch (u) {
			case Assistance::io__sprite_boolean::LOOPFRAMES:
				// no way
				return false;
			case Assistance::io__sprite_boolean::HAS_DONE_LOOP_ONCE:
				v = bmps.hasLoopEndedOnce();
				return true;
			case Assistance::io__sprite_boolean::IS_IT_ON_LAST_FRAME:
				v = bmps.isOnLastFrame();
				return true;
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
				v = bmps.lastFrame();
				return true;
			}
			return false;
		}
		bool Sprite::get(const Assistance::ro__sprite_state u, Assistance::io__sprite_collision_state& v)
		{
			switch (u) {
			case Assistance::ro__sprite_state::STATE:
				v = data.last_state;
				return true;
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

		void Sprite::get(const Assistance::io__sprite_collision_state u, size_t& v)
		{
			v = bmps.getState(u);
		}

		bool Sprite::isEq(const Assistance::io__sprite_string w, const std::string v)
		{
			std::string g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Assistance::io__sprite_double w, const double v)
		{
			double g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Assistance::io__sprite_boolean w, const bool v)
		{
			bool g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Assistance::io__sprite_integer w, const int v)
		{
			int g;
			get(w, g); 
			return g == v || [&]() {
				if (w != Assistance::io__sprite_integer::LAYER) return false; // return false forcing anything but LAYER to be TRUE on g == v part

				data.layers_colliding_m.lock();
				for (auto& i : data.layers_colliding) {
					if (i == v) {
						data.layers_colliding_m.unlock();
						return true;
					}
				}
				data.layers_colliding_m.unlock();
				return false;
			}();
		}
		bool Sprite::isEq(const Assistance::io__sprite_sizet w, const size_t v)
		{
			size_t g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Assistance::ro__sprite_state w, const Assistance::io__sprite_collision_state v)
		{
			Assistance::io__sprite_collision_state g;
			get(w, g);
			return g == v;
		}
		bool Sprite::isEq(const Assistance::io__sprite_color w, const ALLEGRO_COLOR v)
		{
			ALLEGRO_COLOR g;
			get(w, g);
			return ((g.a == v.a) && (g.r == v.r) && (g.g == v.g) && (g.b == v.b));
		}

		bool Sprite::isEq(const Assistance::io__sprite_collision_state w, const size_t v)
		{
			size_t g;
			get(w, g);
			return g == v;
		}

		void Sprite::draw(const int is_layer)
		{
			if (layer != is_layer) return;
			if (!data.bval[+Assistance::io__sprite_boolean::DRAW]) return;

			ALLEGRO_BITMAP* rn = bmps.get((data.bval[+Assistance::io__sprite_boolean::USE_STATE_AS_BITMAP] ? data.last_state : Assistance::io__sprite_collision_state::size));

			if (!rn) return; // if doesn't have what to draw (possible), skip

			Camera camm;
			camera_preset psf = camm.get();

			if (!data.bval[+Assistance::io__sprite_boolean::AFFECTED_BY_CAM]) psf = camera_preset();


			double timee = al_get_time();
			double dt = timee - data.dtarg[+Assistance::ro__sprite_target_double::INTERN_LASTDRAW];
			data.dtarg[+Assistance::ro__sprite_target_double::INTERN_LASTDRAW] = timee;

			double perc_run = Constants::__i_col_pos_t_update * dt; // ex: 5 per sec * 0.2 (1/5 sec) = 1, so posx = actual posx...
			if (perc_run > 1.0) perc_run = 1.0; // 1.0 is "set value"
			if (perc_run < 1.0 / 10000) perc_run = 1.0 / 10000; // can't be infinitely smooth right? come on

			data.dval[+Assistance::io__sprite_double::POSX] = (1.0 - perc_run) * data.dval[+Assistance::io__sprite_double::POSX] + perc_run * data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX];
			data.dval[+Assistance::io__sprite_double::POSY] = (1.0 - perc_run) * data.dval[+Assistance::io__sprite_double::POSY] + perc_run * data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY];
			data.dval[+Assistance::io__sprite_double::ROTATION] = (1.0 - perc_run) * data.dval[+Assistance::io__sprite_double::ROTATION] + perc_run * data.dtarg[+Assistance::ro__sprite_target_double::TARG_ROTATION];



			float cx, cy, px, py, dsx, dsy, rot_rad;
			int bmpx, bmpy;
			bmpx = al_get_bitmap_width(rn);
			bmpy = al_get_bitmap_height(rn);
			if (bmpx <= 0 || bmpy <= 0) {
				throw Abort::abort(__FUNCSIG__, "Somehow the texture have < 0 width / height id=[" + this->sprite_id + "] COLLISION_NONE={" + std::to_string(bmpx) + "," + std::to_string(bmpy) + "}", 1);
			}

			cx = 1.0f * bmpx * ((data.dval[+Assistance::io__sprite_double::CENTERX] + 1.0) * 0.5);
			cy = 1.0f * bmpy * ((data.dval[+Assistance::io__sprite_double::CENTERY] + 1.0) * 0.5);
			rot_rad = 1.0f * data.dval[+Assistance::io__sprite_double::ROTATION] * ALLEGRO_PI / 180.0;
			/*px = 1.0f * data.dval[+Assistance::io__sprite_double::POSX] * cos(rot_rad) + data.dval[+Assistance::io__sprite_double::POSY] * sin(rot_rad);
			py = 1.0f * data.dval[+Assistance::io__sprite_double::POSY] * cos(rot_rad) - data.dval[+Assistance::io__sprite_double::POSX] * sin(rot_rad);*/
			px = data.dval[+Assistance::io__sprite_double::POSX];
			py = data.dval[+Assistance::io__sprite_double::POSY];
			dsx = 1.0f * data.dval[+Assistance::io__sprite_double::SCALEX] * data.dval[+Assistance::io__sprite_double::SCALEG] * (1.0 / bmpx);
			dsy = 1.0f * data.dval[+Assistance::io__sprite_double::SCALEY] * data.dval[+Assistance::io__sprite_double::SCALEG] * (1.0 / bmpy);

			// draw
			camm.applyNoSave(psf);

			if (data.bval[+Assistance::io__sprite_boolean::USE_TINTED_DRAWING])	al_draw_tinted_scaled_rotated_bitmap(rn, data.tint, cx, cy, px, py, dsx, dsy, rot_rad, 0);
			else																    al_draw_scaled_rotated_bitmap(rn, cx, cy, px, py, dsx, dsy, rot_rad, 0);

			// debug

			if (data.bval[+Assistance::io__sprite_boolean::SHOWBOX] || data.bval[+Assistance::io__sprite_boolean::SHOWDOT])
			{
				ALLEGRO_COLOR colorr = 
					al_map_rgba(
						data.bval[+Assistance::io__sprite_boolean::RO_IS_OTHERS_COLLIDING] ? 195 : 15,
						115,
						data.bval[+Assistance::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] ? 195 : 15,
						195
					);
				camera_preset psf = camm.get();

				if (data.bval[+Assistance::io__sprite_boolean::FOLLOWMOUSE]) colorr = al_map_rgba(195, 0, 195, 195);
				//else if (al_get_time() - lastresetcollisioncall < Defaults::diff_time_show_last_resetCollision) colorr = al_map_rgb(255, 255, 0);

				if (data.bval[+Assistance::io__sprite_boolean::SHOWBOX]) {
					al_draw_filled_circle(px - cx * dsx, py - cy * dsy, 0.1f * (fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f) + 0.05f), colorr);
					al_draw_filled_circle(px - cx * dsx, py + cy * dsy, 0.1f * (fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f) + 0.05f), colorr);
					al_draw_filled_circle(px + cx * dsx, py - cy * dsy, 0.1f * (fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f) + 0.05f), colorr);
					al_draw_filled_circle(px + cx * dsx, py + cy * dsy, 0.1f * (fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.20f) + 0.05f), colorr);
				}

				if (data.bval[+Assistance::io__sprite_boolean::SHOWDOT])
				{
					al_draw_filled_circle(px, py, 0.1f * (fabs(psf.get(Assistance::io__camera_float::SCALE_G) * sqrt(psf.get(Assistance::io__camera_float::SCALE_X) * psf.get(Assistance::io__camera_float::SCALE_Y)) * 0.30f) + 0.12f), colorr);
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
			data.bval[+Assistance::io__sprite_boolean::RO_IS_OTHERS_COLLIDING] = false;
			data.bval[+Assistance::io__sprite_boolean::RO_IS_MOUSE_COLLIDING] = false;
			data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X] = 0.0;// 2.0 * data.dval[+Assistance::io__sprite_double::SCALEX] * data.dval[+Assistance::io__sprite_double::SCALEG];
			data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y] = 0.0;// 2.0 * data.dval[+Assistance::io__sprite_double::SCALEY] * data.dval[+Assistance::io__sprite_double::SCALEG];
			data.new_state = Assistance::io__sprite_collision_state::COLLISION_NONE;

		}

		void Sprite::process(const int is_layer, camera_preset psf) // later: be a target, so drawing it will get there (based on framerate)
		{
			if (data.task) data.task(*this);

			if ((layer != is_layer) && !data.bval[+Assistance::io__sprite_boolean::COLLIDE_IGNORE_LAYER] &&
			 ([&]() { data.layers_colliding_m.lock(); for (auto& i : data.layers_colliding) { if (i == is_layer) {data.layers_colliding_m.unlock(); return false;} } data.layers_colliding_m.unlock(); return true; }())) return; // check layers if there's one to collide (ON)


			if (!data.bval[+Assistance::io__sprite_boolean::AFFECTED_BY_CAM]) psf = camera_preset();

			data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX] += data.dval[+Assistance::io__sprite_double::SPEEDX];
			data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY] += data.dval[+Assistance::io__sprite_double::SPEEDY];

			data.dtarg[+Assistance::ro__sprite_target_double::TARG_ROTATION] += data.dval[+Assistance::io__sprite_double::SPEEDROT];

			data.dval[+Assistance::io__sprite_double::SPEEDX] *= data.dval[+Assistance::io__sprite_double::SMOOTHNESS_X] * psf.get(Assistance::io__camera_float::SLIPPERINESS);
			data.dval[+Assistance::io__sprite_double::SPEEDY] *= data.dval[+Assistance::io__sprite_double::SMOOTHNESS_Y] * psf.get(Assistance::io__camera_float::SLIPPERINESS);

			fastIsColliding(psf);
		}

		void Sprite::collideWith(const int is_layer, Sprite* const mf)
		{
			if (layer != is_layer) return;
			if (mf == this) return;
			//if (!data.bval[+Assistance::io__sprite_boolean::COLLIDE_IGNORE_LAYER]) return;
			if (!data.bval[+Assistance::io__sprite_boolean::AFFECTED_BY_CAM]) return;
			if (!data.bval[+Assistance::io__sprite_boolean::COLLIDE_OTHERS]) return;
			if (!mf) return;
			if (!(mf->isEq(Assistance::io__sprite_boolean::COLLIDE_IGNORE_LAYER, true) || mf->isEq(Assistance::io__sprite_integer::LAYER, is_layer))) return; // has not the layer to collide so shouldn't collide with this one. (it does check all possible layers)
			


			double otherpos[2] = { 0.0 };
			double otherscl[3] = { 1.0 };

			std::string oid;
			mf->get(Assistance::io__sprite_string::ID, oid);

			mf->get(Assistance::ro__sprite_target_double::TARG_POSX, otherpos[0]);
			mf->get(Assistance::ro__sprite_target_double::TARG_POSY, otherpos[1]);

			mf->get(Assistance::io__sprite_double::SCALEX, otherscl[0]);
			mf->get(Assistance::io__sprite_double::SCALEY, otherscl[1]);
			mf->get(Assistance::io__sprite_double::SCALEG, otherscl[2]);

			otherscl[0] *= otherscl[2];
			otherscl[1] *= otherscl[2];
			otherscl[2] = 1.0; // just to be sure nothing wrong happens


			double dxx = (data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX]) - otherpos[0]; // data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X]
			double dyy = (data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY]) - otherpos[1]; // data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y]

			double distx = 0.5 * (data.dval[+Assistance::io__sprite_double::SCALEX] * data.dval[+Assistance::io__sprite_double::SCALEG] + otherscl[0]);
			double disty = 0.5 * (data.dval[+Assistance::io__sprite_double::SCALEY] * data.dval[+Assistance::io__sprite_double::SCALEG] + otherscl[1]);

			if (
				(fabs(dxx) < distx) &&
				(fabs(dyy) < disty)
				)
			{
				data.bval[+Assistance::io__sprite_boolean::RO_IS_OTHERS_COLLIDING] = true;
				if (data.new_state == Assistance::io__sprite_collision_state::COLLISION_NONE) data.new_state = Assistance::io__sprite_collision_state::COLLISION_COLLIDED_OTHER; // mouse is higher priority


				if (data.bval[+Assistance::io__sprite_boolean::AFFECTED_BY_COLLISION_ELASTIC]) {
					double wx = fabs(dxx) <= 1e-10 ? 1e-10 : fabs(dxx);
					double wy = fabs(dyy) <= 1e-10 ? 1e-10 : fabs(dyy);
					// 
					double calcx = (dxx < 0 ? -1.0 : 1.0) * (fabs(powl(Constants::sprite_default_multiplier_global_div / wx, Constants::sprite_default_power_global_div)));
					double calcy = (dyy < 0 ? -1.0 : 1.0) * (fabs(powl(Constants::sprite_default_multiplier_global_div / wy, Constants::sprite_default_power_global_div)));

					// should limit because things are crazy
					bool _keep_bg = fabs(calcx * data.dval[+Assistance::io__sprite_double::SCALEX]) < fabs(calcy * data.dval[+Assistance::io__sprite_double::SCALEY]);
					if (fabs(calcx) > Constants::sprite_default_limit_speed_any) calcx /= fabs(calcx) / Constants::sprite_default_limit_speed_any;
					if (fabs(calcy) > Constants::sprite_default_limit_speed_any) calcy /= fabs(calcy) / Constants::sprite_default_limit_speed_any;

					if (_keep_bg) {
						data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X] += calcx;
						data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y] += calcy * fabs(calcx);
					}
					else {
						data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X] += calcx * fabs(calcy);
						data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y] += calcy;
					}

					/*gfile logg;
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Collided elasticly '" << sprite_id << "' with '" << oid << "' intensity: [" << calcx << ";" << calcy << "]" << L::EL;*/
				}

				if (data.bval[+Assistance::io__sprite_boolean::AFFECTED_BY_COLLISION_ROUGH]) {
					int wasp[2];
					wasp[0] = dxx > 0.0 ? 1 : -1;
					wasp[1] = dyy > 0.0 ? 1 : -1;

					double realdist[2];
					realdist[0] = -(double)wasp[0] * (fabs(dxx) - fabs(distx));
					realdist[1] = -(double)wasp[1] * (fabs(dyy) - fabs(disty));

					if (fabs(realdist[0]) < fabs(realdist[1])) data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X] = realdist[0];
					else									   data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y] = realdist[1];

					/*gfile logg;
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Collided roughly '" << sprite_id << "' with '" << oid << "' intensity: [" << realdist[0] << ";" << realdist[1] << "]" << L::EL;*/
				}
			}
		}

		void Sprite::applyCollideData(camera_preset psf)
		{
			if (data.last_state != data.new_state && data.new_state != Assistance::io__sprite_collision_state::size) {
				if (data.function_pair[+data.new_state]) data.function_pair[+data.new_state]();
			}
			data.last_state = data.new_state;

			if (data.bval[+Assistance::io__sprite_boolean::RO_IS_OTHERS_COLLIDING]) {
				// limit the maximum power of pos += distance value
				double& calcx = data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X];
				double& calcy = data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y];
				if (fabs(calcx) > Constants::sprite_default_limit_speed_any) calcx /= fabs(calcx) / Constants::sprite_default_limit_speed_any;
				if (fabs(calcy) > Constants::sprite_default_limit_speed_any) calcy /= fabs(calcy) / Constants::sprite_default_limit_speed_any;

				if (data.bval[+Assistance::io__sprite_boolean::AFFECTED_BY_COLLISION_ELASTIC]) {
					data.dval[+Assistance::io__sprite_double::SPEEDX] += data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X] * data.dval[+Assistance::io__sprite_double::SMOOTHNESS_X];
					data.dval[+Assistance::io__sprite_double::SPEEDY] += data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y] * data.dval[+Assistance::io__sprite_double::SMOOTHNESS_Y];
				}
				else if (data.bval[+Assistance::io__sprite_boolean::AFFECTED_BY_COLLISION_ROUGH]) {
					data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX] += data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_X];// *data.dval[+Assistance::io__sprite_double::SMOOTHNESS_X];
					data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY] += data.dval[+Assistance::io__sprite_double::RO_OTHERS_DISTANCE_Y];// *data.dval[+Assistance::io__sprite_double::SMOOTHNESS_Y];

				}
			}

			if (psf.get(Assistance::io__camera_boolean::RESPECT_LIMITS) && !(data.bval[+Assistance::io__sprite_boolean::FOLLOWMOUSE] || !data.bval[+Assistance::io__sprite_boolean::RESPECT_CAMERA_LIMITS])) /* FOLLOWMOUSE may glitch if outside limits, so skip limits then */
			{
				double lxl = (double)psf.get(Assistance::io__camera_float::LIMIT_MIN_X); // limit x lower
				double lyl = (double)psf.get(Assistance::io__camera_float::LIMIT_MIN_Y); // limit y lower
				double lxh = (double)psf.get(Assistance::io__camera_float::LIMIT_MAX_X); // limit x higher
				double lyh = (double)psf.get(Assistance::io__camera_float::LIMIT_MAX_Y); // limit y higher

				if (data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX] < lxl) data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX] = lxl;
				if (data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX] > lxh) data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSX] = lxh;
				if (data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY] < lyl) data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY] = lyl;
				if (data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY] > lyh) data.dtarg[+Assistance::ro__sprite_target_double::TARG_POSY] = lyh;
			}
		}








		void Text::_draw(const double targ_draw_xy[2])
		{
			al_draw_text(data.font, data.c, 1.0 * targ_draw_xy[0] / (data.d[+Assistance::io__text_double::SCALEG]), 1.0 * targ_draw_xy[1] / (data.d[+Assistance::io__text_double::SCALEG]), data.i[+Assistance::io__text_integer::MODE], data.str[+Assistance::io__text_string::PROCESSED_STRING].c_str());
		}
		void Text::_interpretTags(std::string& s)
		{
			double timing = al_get_time();
			std::string fina = s;

			for (bool b = true; b;)
			{
				b = false;
				for (size_t q = 0; q < +Assistance::tags_e::size; q++)
				{
					size_t poss = 0;
					if ((poss = fina.find(Assistance::tags[q].s())) != std::string::npos)
					{
						b = true;
						Camera cam;
						Database conf;
						ALLEGRO_DISPLAY* d = al_get_current_display();
						char tempstr_c[128];
						//gfile logg;

						switch (q)
						{
						case +Assistance::tags_e::T_POSX:
							sprintf_s(tempstr_c, "%.3lf", (data.d[+Assistance::io__text_double::POSX] + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX] + (double)cam.get().get(Assistance::io__camera_float::OFFSET_X)));
							break;
						case +Assistance::tags_e::T_POSY:
							sprintf_s(tempstr_c, "%.3lf", (data.d[+Assistance::io__text_double::POSY] + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY] + (double)cam.get().get(Assistance::io__camera_float::OFFSET_Y)));
							break;
						case +Assistance::tags_e::T_SCREEN_POSX:
							sprintf_s(tempstr_c, "%.3lf", (data.d[+Assistance::io__text_double::POSX] * (data.b[+Assistance::io__text_boolean::AFFECTED_BY_CAM] ? (double)(cam.get().get(Assistance::io__camera_float::SCALE_G) * cam.get().get(Assistance::io__camera_float::SCALE_X)) : 1.0) + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX] * (double)(cam.get().get(Assistance::io__camera_float::SCALE_G) * cam.get().get(Assistance::io__camera_float::SCALE_X))));
							break;
						case +Assistance::tags_e::T_SCREEN_POSY:
							sprintf_s(tempstr_c, "%.3lf", (data.d[+Assistance::io__text_double::POSY] * (data.b[+Assistance::io__text_boolean::AFFECTED_BY_CAM] ? (double)(cam.get().get(Assistance::io__camera_float::SCALE_G) * cam.get().get(Assistance::io__camera_float::SCALE_Y)) : 1.0) + data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY] * (double)(cam.get().get(Assistance::io__camera_float::SCALE_G) * cam.get().get(Assistance::io__camera_float::SCALE_Y))));
							break;

						case +Assistance::tags_e::T_CAM_X:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Assistance::io__camera_float::OFFSET_X));
							break;
						case +Assistance::tags_e::T_CAM_Y:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Assistance::io__camera_float::OFFSET_Y));
							break;
						case +Assistance::tags_e::T_CAM_ZOOM:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Assistance::io__camera_float::SCALE_G) * sqrt(cam.get().get(Assistance::io__camera_float::SCALE_X) * cam.get().get(Assistance::io__camera_float::SCALE_Y)));
							break;
						case +Assistance::tags_e::T_CAM_ZOOMG:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Assistance::io__camera_float::SCALE_G));
							break;
						case +Assistance::tags_e::T_CAM_ZOOMX:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Assistance::io__camera_float::SCALE_X));
							break;
						case +Assistance::tags_e::T_CAM_ZOOMY:
							sprintf_s(tempstr_c, "%.3f", cam.get().get(Assistance::io__camera_float::SCALE_Y));
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
							size_t t;
							conf.get(Assistance::io__db_statistics_sizet::FRAMESPERSECOND, t);
							sprintf_s(tempstr_c, "%zu", t);
						}
						break;
						case +Assistance::tags_e::T_TPS:
						{
							size_t t;
							conf.get(Assistance::io__db_statistics_sizet::COLLISIONSPERSECOND, t);
							sprintf_s(tempstr_c, "%zu", t);
						}
						break;
						case +Assistance::tags_e::T_UPS:
						{
							size_t t;
							conf.get(Assistance::io__db_statistics_sizet::USEREVENTSPERSECOND, t);
							sprintf_s(tempstr_c, "%zu", t);
						}
						break;
						case +Assistance::tags_e::T_APS:
						{
							size_t t;
							conf.get(Assistance::io__db_statistics_sizet::ADVANCEDFUNCSPERSECOND, t);
							sprintf_s(tempstr_c, "%zu", t);
						}
						break;

						case +Assistance::tags_e::T_I_FPS:
						{
							double dt;
							conf.get(Assistance::io__db_statistics_double::INSTANT_FRAMESPERSECOND, dt);
							sprintf_s(tempstr_c, "%05.1lf", 1.0 / dt);
						}
						break;
						case +Assistance::tags_e::T_I_TPS:
						{
							double dt;
							conf.get(Assistance::io__db_statistics_double::INSTANT_COLLISIONSPERSECOND, dt);
							sprintf_s(tempstr_c, "%05.1lf", 1.0 / dt);
						}
						break;
						case +Assistance::tags_e::T_I_UPS:
						{
							double dt;
							conf.get(Assistance::io__db_statistics_double::INSTANT_USEREVENTSPERSECOND, dt);
							sprintf_s(tempstr_c, "%05.1lf", 1.0 / dt);
						}
						break;
						case +Assistance::tags_e::T_I_APS:
						{
							double dt;
							conf.get(Assistance::io__db_statistics_double::INSTANT_ADVANCEDFUNCSPERSECOND, dt);
							sprintf_s(tempstr_c, "%05.1lf", 1.0 / dt);
						}
						break;

						case +Assistance::tags_e::T_MS_FPS:
						{
							double dt;
							conf.get(Assistance::io__db_statistics_double::INSTANT_FRAMESPERSECOND, dt);
							sprintf_s(tempstr_c, "%04.3lf", 1000.0 * dt);
						}
						break;
						case +Assistance::tags_e::T_MS_TPS:
						{
							double dt;
							conf.get(Assistance::io__db_statistics_double::INSTANT_COLLISIONSPERSECOND, dt);
							sprintf_s(tempstr_c, "%04.3lf", 1000.0 * dt);
						}
						break;
						case +Assistance::tags_e::T_MS_UPS:
						{
							double dt;
							conf.get(Assistance::io__db_statistics_double::INSTANT_USEREVENTSPERSECOND, dt);
							sprintf_s(tempstr_c, "%04.3lf", 1000.0 * dt);
						}
						break;
						case +Assistance::tags_e::T_MS_APS:
						{
							double dt;
							conf.get(Assistance::io__db_statistics_double::INSTANT_ADVANCEDFUNCSPERSECOND, dt);
							sprintf_s(tempstr_c, "%04.3lf", 1000.0 * dt);
						}
						break;


						case +Assistance::tags_e::T_SPRITE_FRAME:
							if (follow) {
								size_t f;
								follow->get(Assistance::io__sprite_sizet::FRAME, f);
								sprintf_s(tempstr_c, "%zu", f);
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Assistance::tags_e::T_MOUSE_X:
						{
							float x;
							conf.get(Assistance::io__db_mouse_float::MOUSE_X, x);
							sprintf_s(tempstr_c, "%.3f", x);
						}
						break;
						case +Assistance::tags_e::T_MOUSE_Y:
						{
							float y;
							conf.get(Assistance::io__db_mouse_float::MOUSE_Y, y);
							sprintf_s(tempstr_c, "%.3f", y);
						}
						break;
						case +Assistance::tags_e::T_LASTSTRING:
						{
							/*std::string str;
							bev.getLastString(str);
							sprintf_s(tempstr_c, "%s", str.c_str());*/
							sprintf_s(tempstr_c, "NOT_IMPLEMENTED");
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
							sprintf_s(tempstr_c, "NOT_IMPLEMENTED");
						}
						break;
						case +Assistance::tags_e::T_SPRITE_SPEEDX:
							if (follow) {
								double val;
								follow->get(Assistance::io__sprite_double::SPEEDX, val);
								sprintf_s(tempstr_c, "%.3lf", val);
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Assistance::tags_e::T_SPRITE_SPEEDY:
							if (follow) {
								double val;
								follow->get(Assistance::io__sprite_double::SPEEDY, val);
								sprintf_s(tempstr_c, "%.3lf", val);
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Assistance::tags_e::T_SPRITE_NAME:
							if (follow) {
								std::string temp;
								follow->get(Assistance::io__sprite_string::ID, temp);
								sprintf_s(tempstr_c, "%s", temp.c_str());
							}
							else sprintf_s(tempstr_c, "NULL");
							break;
						case +Assistance::tags_e::T_SPRITE_ENTITY_HEALTH:
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
							sprintf_s(tempstr_c, "NOT_IMPLEMENTED");
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
							sprintf_s(tempstr_c, "NOT_IMPLEMENTED");
							break;
						case +Assistance::tags_e::T_TEXTURES_LOADED:
						{
							__template_static_vector<ALLEGRO_BITMAP> Textures;
							sprintf_s(tempstr_c, "%zu", Textures.size());
						}
						break;
						case +Assistance::tags_e::T_FONTS_LOADED:
						{
							__template_static_vector<ALLEGRO_FONT> Fonts;
							sprintf_s(tempstr_c, "%zu", Fonts.size());
						}
						break;
						case +Assistance::tags_e::T_SAMPLES_LOADED:
						{
							__template_static_vector<ALLEGRO_SAMPLE> Samples;
							sprintf_s(tempstr_c, "%zu", Samples.size());
						}
						break;
						case +Assistance::tags_e::T_SPRITES_LOADED:
						{
							__template_static_vector<Sprite> Sprites;
							sprintf_s(tempstr_c, "%zu", Sprites.size());
						}
						break;
						case +Assistance::tags_e::T_TEXTS_LOADED:
						{
							__template_static_vector<Text> Texts;
							sprintf_s(tempstr_c, "%zu", Texts.size());
						}
						break;
						case +Assistance::tags_e::T_TRACKS_LOADED:
						{
							__template_static_vector<Track> Tracks;
							sprintf_s(tempstr_c, "%zu", Tracks.size());
						}
						break;
						case +Assistance::tags_e::T_SPRITE_STATE:
							if (follow) {
								Assistance::io__sprite_collision_state stat;
								follow->get(Assistance::ro__sprite_state::STATE, stat);
								switch (stat) {
								case Assistance::io__sprite_collision_state::COLLISION_MOUSE_ON:
									sprintf_s(tempstr_c, "MOUSE ON");
									break;
								case Assistance::io__sprite_collision_state::COLLISION_MOUSE_CLICK:
									sprintf_s(tempstr_c, "MOUSE CLICK");
									break;
								case Assistance::io__sprite_collision_state::COLLISION_COLLIDED_OTHER:
									sprintf_s(tempstr_c, "OTHER COLLIDING");
									break;
								case Assistance::io__sprite_collision_state::COLLISION_NONE:
									sprintf_s(tempstr_c, "NONE");
									break;
								}
							}
							else sprintf_s(tempstr_c, "NULL");
						break;
						}


						if (fina.length() > (poss + Assistance::tags[q].s().length()))  fina = fina.substr(0, poss) + tempstr_c + fina.substr(poss + Assistance::tags[q].s().length());
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

			data.d[+Assistance::io__text_double::POSX] = 0.0;
			data.d[+Assistance::io__text_double::POSY] = 0.0;
			data.d[+Assistance::io__text_double::SCALEX] = 1.0;
			data.d[+Assistance::io__text_double::SCALEY] = 1.0;
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

		/*Text::~Text()
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__) << "Registered despawn of Text #" + std::to_string((size_t)this)/* << ";ID=" << id / << L::ELF;
		}*/


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
			if (!data.font) return;
			if (is_layer != data.i[+Assistance::io__text_integer::LAYER]) return;

			if (follow) {
				if (follow->isEq(Assistance::io__sprite_boolean::DRAW, false)) return;
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

			pos_now[0] = 1.0 * Constants::text_default_sharpness_font * 
				(((data.d[+Assistance::io__text_double::POSX]) * cos(p_rotation_rad))
					- ((data.d[+Assistance::io__text_double::POSY]) * sin(p_rotation_rad)) 
					+ data.d[+Assistance::io__text_double::LAST_FOLLOW_POSX]);

			pos_now[1] = 1.0 * Constants::text_default_sharpness_font * 
				(((data.d[+Assistance::io__text_double::POSY]) * cos(p_rotation_rad)) 
					- ((data.d[+Assistance::io__text_double::POSX]) * sin(p_rotation_rad)) 
					+ data.d[+Assistance::io__text_double::LAST_FOLLOW_POSY]);


			double targ_draw_xy[2];
			targ_draw_xy[0] = pos_now[0] * cos(rotation_rad) + pos_now[1] * sin(rotation_rad);
			targ_draw_xy[1] = pos_now[1] * cos(rotation_rad) - pos_now[0] * sin(rotation_rad);
			targ_draw_xy[0] /= data.d[+Assistance::io__text_double::SCALEX];
			targ_draw_xy[1] /= data.d[+Assistance::io__text_double::SCALEY];


			if (!data.b[+Assistance::io__text_boolean::AFFECTED_BY_CAM]) preset.reset();

			preset.set(Assistance::io__camera_float::SCALE_G,  preset.get(Assistance::io__camera_float::SCALE_G) * data.d[+Assistance::io__text_double::SCALEG] * 1.0 / Constants::text_default_sharpness_font);
			preset.set(Assistance::io__camera_float::SCALE_X, preset.get(Assistance::io__camera_float::SCALE_X) * data.d[+Assistance::io__text_double::SCALEX]);
			preset.set(Assistance::io__camera_float::SCALE_Y, preset.get(Assistance::io__camera_float::SCALE_Y) * data.d[+Assistance::io__text_double::SCALEY]);
			preset.set(Assistance::io__camera_float::OFFSET_X, preset.get(Assistance::io__camera_float::OFFSET_X) * Constants::text_default_sharpness_font / data.d[+Assistance::io__text_double::SCALEG]);
			preset.set(Assistance::io__camera_float::OFFSET_Y, preset.get(Assistance::io__camera_float::OFFSET_Y) * Constants::text_default_sharpness_font / data.d[+Assistance::io__text_double::SCALEG]);

			preset.set(Assistance::io__camera_float::ROTATION_RAD, preset.get(Assistance::io__camera_float::ROTATION_RAD) + rotation_rad);
			cam.applyNoSave(preset);

			_draw(targ_draw_xy);

			al_set_target_bitmap(d);
			cam.apply();
		}





		const double Bubbles::maxone(double gotten, const double prop)
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


		void Bubbles::init(const unsigned amout, const float mfps, const int layer)
		{
			if (firstcall) {

				__template_static_vector<ALLEGRO_BITMAP> textures;
				__template_static_vector<Sprite> sprites;

				Database db;
				db.get(Assistance::io__conf_integer::SCREEN_X, siz[0], 1920);
				db.get(Assistance::io__conf_integer::SCREEN_Y, siz[1], 1080);

				siz[0] *= 0.8;
				siz[1] *= 0.8;

				if (siz[0] < 1280) siz[0] = 1280;
				if (siz[1] < 720 ) siz[1] = 720;

				imglw = textures.customLoad("BKG_NULL",	[&](ALLEGRO_BITMAP*& b) -> bool { return (b = al_create_bitmap(siz[0], siz[1]));}); // don't worry, it won't create if it is already there
				disguy = sprites.create("BKG_NULL"); // this one too

				disguy->set(Assistance::io__sprite_string::REMOVE, "BKG_NULL"); // guarantee no duplicated stuff because it might have it already
				disguy->set(Assistance::io__sprite_string::ADD, "BKG_NULL");
				disguy->set(Assistance::io__sprite_double::ANIMATION_FPS, 0);
				disguy->set(Assistance::io__sprite_boolean::DRAW, true);
				disguy->set(Assistance::io__sprite_string::ID, "BKG_NULL");
				disguy->set(Assistance::io__sprite_integer::LAYER, layer);
				disguy->set(Assistance::io__sprite_double::SCALEG, 2.0);
				disguy->set(Assistance::io__sprite_boolean::AFFECTED_BY_CAM, true);

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

		void Bubbles::draw()
		{
			if (firstcall) {
				return;
			}

			if (alreadyreset) {
				alreadyreset = false;

				ALLEGRO_BITMAP* lastpoint = al_get_target_bitmap();

				al_set_target_bitmap(imglw);
				
				for (auto& i : positions)
				{
					al_draw_filled_circle(i.lastpositionscalculated[0], i.lastpositionscalculated[1], i.lastsize, al_map_rgba_f(
						(float)maxone(al_get_time() * 0.3f * ((rand() % 1000) / 1000.0f)) * (1.0f - (Constants::amountofblur_bubbles / 100.0f)),
						(float)maxone(al_get_time() * 0.1f * ((rand() % 1000) / 1000.0f)) * (1.0f - (Constants::amountofblur_bubbles / 100.0f)),
						(float)maxone(al_get_time() * 0.8f * ((rand() % 1000) / 1000.0f)) * (1.0f - (Constants::amountofblur_bubbles / 100.0f)),
						(float)(1.0f - (Constants::amountofblur_bubbles / 100.0f))));
				}

				al_set_target_bitmap(lastpoint);
			}

			//al_draw_bitmap(dis, -1.0, -1.0, 0); 

		}
		void Bubbles::think()
		{
			if (firstcall) {
				return;
			}

			bool continuee = false;
			Camera cam;
			auto cpy = cam.get();
			for (auto& o : cpy) {
				if (continuee |= disguy->isEq(Assistance::io__sprite_integer::LAYER, o)) break;
			}

			if (continuee && ((al_get_time() - lastdraw) >= (1.0 / fps) || fps == -1))
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
		}

	}
}