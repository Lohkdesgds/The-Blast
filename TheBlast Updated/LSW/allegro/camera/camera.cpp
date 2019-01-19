#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Camera {

			_cam_g camera_g::cam;


			_transformator::_transformator() {
				assert(d = al_get_current_display());
				reset();
			}
			void _transformator::reset()
			{
				_transf_data td; // default values
				presets.clear();
				get(); // start 0 pos as default
				apply();
			}
			void _transformator::set(const _transf_data& td, const int a)
			{
				presets[a] = td;
			}
			void _transformator::apply(const int a)
			{
				_transf_data lastTransf = presets[a];
				al_identity_transform(&g_t);

				al_rotate_transform(&g_t,
					lastTransf.rotation);

				al_scale_transform(&g_t,
					al_get_display_width(d) * 0.5 * lastTransf.scale_x * lastTransf.scale_g,
					al_get_display_height(d) * 0.5 * lastTransf.scale_y * lastTransf.scale_g); // SCALING NOT AS EXPECTED (multiplier by zoom is not being made by transformation!)

				al_translate_transform(&g_t,
					al_get_display_width(d) * 0.5 - lastTransf.offset_x * (al_get_display_width(d) * 0.5 * lastTransf.scale_x * lastTransf.scale_g),
					al_get_display_height(d) * 0.5 - lastTransf.offset_y * (al_get_display_height(d) * 0.5 * lastTransf.scale_y * lastTransf.scale_g));

				al_use_transform(&g_t);
			}

			_transf_data& _transformator::get(const int a)
			{
				return presets[a];
			}

			ALLEGRO_DISPLAY * _transformator::getD()
			{
				return d;
			}


			void camera_g::setup(const bool reset)
			{
				if (cam.t) {
					if (reset) cam.t->reset();
					return;
				}
				cam.t = new _transformator();
			}
			void camera_g::apply()
			{
				apply(cam.last_used);
			}
			void camera_g::apply(const int a)
			{
				assert(cam.t);
				cam.last_used = a;
				cam.t->apply(a);
			}

			const int camera_g::getLastApplyID()
			{
				return cam.last_used;
			}

			/*_transformator& camera_g::t()
			{
				return *cam.t;
			}*/

			/*const double camera_g::get(const _cam_v a)
			{
				return get(a, cam.last_used);
			}
			const int camera_g::get(const _cam_p a)
			{
				return get(a, cam.last_used);
			}*/
			const double camera_g::get(const int o, const _cam_v a)
			{
				
				assert(cam.t);

				switch (a)
				{
				case OFFX:
					return cam.t->get(o).offset_x;
				case OFFY:
					return cam.t->get(o).offset_y;
				case ZOOM:
					return cam.t->get(o).scale_g;
				case ZOOMX:
					return cam.t->get(o).scale_x;
				case ZOOMY:
					return cam.t->get(o).scale_y;
				case ROTATION:
					return cam.t->get(o).rotation;
				}
				return 0.0;
			}
			void camera_g::get(/*const _cam_p a, */const int o, std::map<int, bool>& v)
			{
				assert(cam.t);

				/*switch (a)
				{
				case LAYERLOWER:
					return cam.t->get(o).limits[0];
				case LAYERHIGHER:
					return cam.t->get(o).limits[1];
				}*/
				v = cam.t->get(o).layers_enabled;
			}

			std::map<int, bool> camera_g::getLayers(const int u)
			{
				assert(cam.t);

				return cam.t->get(u).layers_enabled;
			}

			/*void camera_g::set(const _cam_v a, const double v)
			{
				set(a, v, cam.last_used);
			}
			void camera_g::set(const _cam_p a, const int v)
			{
				set(a, v, cam.last_used);
			}*/

			void camera_g::set( const int o, const _cam_v a, const double v)
			{
				assert(cam.t);

				switch (a)
				{
				case OFFX:
					cam.t->get(o).offset_x = v;
					break;
				case OFFY:
					cam.t->get(o).offset_y = v;
					break;
				case ZOOM:
					cam.t->get(o).scale_g = v;
					break;
				case ZOOMX:
					cam.t->get(o).scale_x = v;
					break;
				case ZOOMY:
					cam.t->get(o).scale_y = v;
					break;
				case ROTATION:
					cam.t->get(o).rotation = v;
					break;
				}
			}
			void camera_g::set(/*const _cam_p a, */const int o, const int v, const bool t)
			{
				assert(cam.t);


				cam.t->get(o).layers_enabled[v] = t;

				/*switch (a)
				{
				case LAYERLOWER:
					cam.t->get(o).layers_enabled[v] = t;
					break;
				case LAYERHIGHER:
					cam.t->get(o).limits[1] = v;
					break;
				}*/
			}

			void camera_g::copy(const int to)
			{
				copy(to, cam.last_used);
			}
			void camera_g::copy(const int to, const int from)
			{
				copy(to, cam.t->get(from));
				//cam.t->get(to) = cam.t->get(from);
			}
			void camera_g::copy(const int to, const _transf_data& from)
			{
				cam.t->get(to) = from;
			}
			void camera_g::reset(const int which)
			{
				_transf_data k;
				copy(which, k);
			}
			ALLEGRO_DISPLAY * camera_g::getD()
			{
				assert(cam.t);
				return cam.t->getD();
			}
		}
	}
}