#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Image {

			bool image_low::global_no_optimization = Defaults::Image::no_optimization;
			double image_low::global_timing_optimization = Defaults::Image::timing_optimization;

			void image_low::paint()
			{
				if (!using_color || mode != CREATED) throw "at image_low::paint [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: Unexpected situation. It is not supposed to be painted!";

				ALLEGRO_BITMAP* targ = al_get_target_bitmap();
				if (!targ) {
					throw "at image_low::paint [#" + std::to_string((size_t)this) + "]: Could not get current target bitmap for swap.";
					return;
				}
				al_set_target_bitmap(bmp);
				al_clear_to_color(color);
				al_set_target_bitmap(targ);
			}

			image_low::image_low()
			{
				logg << Log::START << "[IMGL:CONST][INFO] Registered spawn of image_low ID#" + std::to_string((size_t)this) << Log::ENDL;
			}
			image_low::~image_low()
			{
				unload();
				logg << Log::START << "[IMGL:DESTR][INFO] Registered despawn of image_low ID#" + std::to_string((size_t)this) << ";ID=" << id << Log::ENDL;
			}



			void image_low::set(const vals_string e, const Safer::safe_string v)
			{
				switch (e) {
				case RAW_PATH:
					path_raw = v;
					path = path_raw;
					mode = LOADED;
					break;
				case PATH:
					path = Defaults::default_data_path.g() + v.g();
					Tools::interpret_path(path);
					mode = LOADED;
					break;
				case ID:
					id = v;
					break;
				}
			}
			void image_low::set(const vals_int e, const int v)
			{
				switch (e) {
				case CREATE_X:
					mode = CREATED;
					size_for_creation[0] = v;
					break;
				case CREATE_Y:
					mode = CREATED;
					size_for_creation[1] = v;
					break;
				}
			}
			void image_low::set(const vals_bool e, const bool v)
			{
				switch (e) {
				case HAS_LOADED:
					if (!Defaults::enable_all_possible_setget) throw "at image_low::set [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: Tried to set a value \"read-only\" like (HAS_LOADED).";
					is_loaded = v; // not meant to be used that way
					break;
				case HAS_RELOADED:
					if (!Defaults::enable_all_possible_setget) throw "at image_low::set [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: Tried to set a value \"read-only\" like (HAS_RELOADED)";
					has_reloaded = v;
					break;
				case GLOBAL_SET_NO_OPTIMIZING_SETTING:
					global_no_optimization = v;
					break;
				case FORCE_ON_MEMORY:
					no_optimization = v;
					break;
				case USE_COLOR:
					using_color = v;
					break;
				case LOAD_LATER:
					load_later = v;
					break;
				}
			}
			void image_low::set(const vals_double e, const double v)
			{
				switch (e) {
				case GLOBAL_OPTIMIZATION_TIMING:
					global_timing_optimization = v;
					break;
				}
			}
			void image_low::set(const vals_color e, const ALLEGRO_COLOR v)
			{
				switch (e) {
				case COLOR:
					using_color = true;
					color = v;
					break;
				}
			}
			void image_low::set(const vals_bmpp e, ALLEGRO_BITMAP* const v)
			{
				switch (e) {
				case BMP:
					if (!Defaults::enable_all_possible_setget) throw "at image_low::set [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: Tried to set a value \"read-only\" like (BMP).";
					bmp = v;
					break;
				}
			}
			void image_low::set(const vals_x_enum1 e, const _img_mode v)
			{
				switch (e) {
				case MODE:
					mode = v;
					break;
				}
			}

			void image_low::get(const vals_string e, Safer::safe_string& v)
			{
				switch (e) {
				case RAW_PATH:
					v = path_raw;
					break;
				case PATH:
					v = path;
					break;
				case ID:
					v = id;
					break;
				}
			}
			void image_low::get(const vals_int e, int& v)
			{
				switch (e) {
				case CREATE_X:
					v = size_for_creation[0];
					break;
				case CREATE_Y:
					v = size_for_creation[1];
					break;
				}
			}
			void image_low::get(const vals_bool e, bool& v)
			{
				switch (e) {
				case HAS_LOADED:
					v = is_loaded;
					break;
				case HAS_RELOADED:
					v = has_reloaded;
					has_reloaded = false;
					break;
				case GLOBAL_SET_NO_OPTIMIZING_SETTING:
					v = global_no_optimization;
					break;
				case FORCE_ON_MEMORY:
					v = no_optimization;
					break;
				case USE_COLOR:
					v = using_color;
					break;
				case LOAD_LATER:
					v = load_later;
					break;
				}
			}
			void image_low::get(const vals_double e, double& v)
			{
				switch (e) {
				case GLOBAL_OPTIMIZATION_TIMING:
					v = global_timing_optimization;
					break;
				}
			}
			void image_low::get(const vals_color e, ALLEGRO_COLOR& v)
			{
				switch (e) {
				case COLOR:
					v = color;
					break;
				}
			}
			void image_low::get(const vals_bmpp e, ALLEGRO_BITMAP*& v)
			{
				switch (e) {
				case BMP:
					if (!bmp) verify();
					v = bmp;
					break;
				}
			}
			void image_low::get(const vals_x_enum1 e, _img_mode& v)
			{
				switch (e) {
				case MODE:
					v = mode;
					break;
				}
			}

			const bool image_low::isEq(const vals_string e, const Safer::safe_string v)
			{
				bool result = false;

				switch (e) {
				case RAW_PATH:
					result = (path_raw == v);
					break;
				case PATH:
					result = (path == v);
					break;
				case ID:
					result = (id == v);
					break;
				}

				return result;
			}
			const bool image_low::isEq(const vals_int e, const int v)
			{
				bool result = false;

				switch (e) {
				case CREATE_X:
					result = (size_for_creation[0] == v);
					break;
				case CREATE_Y:
					result = (size_for_creation[1] == v);
					break;
				}

				return result;
			}
			const bool image_low::isEq(const vals_bool e, const bool v)
			{
				bool result = false;

				switch (e) {
				case HAS_LOADED:
					result = (is_loaded == v);
					break;
				case HAS_RELOADED:
					result = (has_reloaded == v);
					break;
				case GLOBAL_SET_NO_OPTIMIZING_SETTING:
					result = (global_no_optimization == v);
					break;
				case FORCE_ON_MEMORY:
					result = (no_optimization == v);
					break;
				case USE_COLOR:
					result = (using_color == v);
					break;
				case LOAD_LATER:
					result = (load_later == v);
					break;
				}

				return result;
			}
			const bool image_low::isEq(const vals_double e, const double v)
			{
				bool result = false;

				switch (e) {
				case GLOBAL_OPTIMIZATION_TIMING:
					result = (global_timing_optimization == v);
					break;
				}

				return result;
			}
			const bool image_low::isEq(const vals_color e, const ALLEGRO_COLOR v)
			{
				bool result = false;

				switch (e) {
				case COLOR:
					result = ((color.r == v.r) && (color.g == v.g) && (color.b == v.b) && (color.a == v.a));
					break;
				}

				return result;
			}
			const bool image_low::isEq(const vals_bmpp e, const ALLEGRO_BITMAP* v)
			{
				bool result = false;

				switch (e) {
				case BMP:
					result = (bmp == v);
					break;
				}

				return result;
			}
			const bool image_low::isEq(const vals_x_enum1 e, const _img_mode v)
			{
				bool result = false;

				switch (e) {
				case MODE:
					result = (mode == v);
					break;
				}

				return result;
			}



			const bool image_low::load()
			{
				switch (mode) {
				case CREATED:
					if (size_for_creation[0] == 0 || size_for_creation[1] == 0) return false;
					else {
						if (size_for_creation[0] < 0 || size_for_creation[1] < 0) {
							ALLEGRO_DISPLAY *d = al_get_current_display();
							if (!d) throw "at image_low::initInstance [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: Could not get current display for creation of display size based bitmap.";
							size[0] = al_get_display_width(d);
							size[1] = al_get_display_height(d);
						}
						else {
							size[0] = size_for_creation[0];
							size[1] = size_for_creation[1];
						}
						bmp = al_create_bitmap(size[0], size[1]);
						if (bmp)
						{
							lastcall = GetTickCount64();
							is_loaded = true;
							if (using_color) paint();
							if (!optimized) logg << Log::START << "image_low::load[#" << std::to_string((size_t)this) << ";ID=" << id << "]: Created successfully its image with size \"" << size[0] << "x" << size[1] << "\"" << Log::ENDL;
							return true;
						}
						else throw "at image_low::initInstance [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: Could not create valid bitmap.";

						return false;
					}
					break;
				case LOADED:
					{
						bmp = al_load_bitmap(path.g().c_str());
						if (bmp)
						{
							lastcall = GetTickCount64();
							size[0] = al_get_bitmap_width(bmp);
							size[1] = al_get_bitmap_height(bmp);
							is_loaded = true;
							if (!optimized) logg << Log::START << "image_low::load[#" << std::to_string((size_t)this) << ";ID=" << id << "]: Loaded successfully its image with path \"" << path << "\"" << Log::ENDL;
							return true;
						}
						throw "at image_low::initInstance [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: Cannot load texture \"" + path.g() + "\"";
					}
					break;
				}
				return false;
			}

			const bool image_low::reload()
			{
				unload();
				return (has_reloaded = load());
			}

			void image_low::unload()
			{
				lastcall = 0;
				is_loaded = false;

				if (bmp) {
					al_destroy_bitmap(bmp);
					bmp = nullptr;
				}
			}
			
			void image_low::verify()
			{
				//if (!bmp && !optimized) return;
				if (!is_loaded) {
					if (load_later) load();
					return;
				}

				switch (mode) {
				case CREATED:
					{
						ALLEGRO_DISPLAY *d = al_get_current_display();
						if (!d) throw "at image_low::verify [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: Could not get current display for verification of bitmap's size.";
						int x, y;
						x = al_get_display_width(d);
						y = al_get_display_height(d);
						if (x != size[0] || y != size[1])
						{
							reload();
							logg << Log::START << "image_low::verify[#" << std::to_string((size_t)this) << ";ID=" << id << "]: Reallocated." << Log::ENDL;
						}
					}
					break;
				case LOADED:
					if (optimized)
					{
						optimizing.lock();
						reload();
						logg << Log::START << "image_low::verify[#" << std::to_string((size_t)this) << ";ID=" << id << "]: Reallocated." << Log::ENDL;
						optimized = false;
						optimizing.unlock();
					}
					break;
				}
			}

			void image_low::checkMemory()
			{
				if (global_no_optimization || no_optimization) return;

				if (al_get_time() - lastcall > global_timing_optimization && !optimized)
				{
					optimizing.lock();
					unload();
					logg << Log::START << "image_low::checkMemory[#" << std::to_string((size_t)this) << ";ID=" << id << "]: Optimized." << Log::ENDL;
					optimized = true;
					optimizing.unlock();
				}
			}






			
			size_t _find(const Safer::safe_string s, Safer::safe_vector<image_low*>& v, bool& u)
			{
				u = false;
				for (size_t p = 0; p < v.getMax(); p++)
				{
					if (v[p]->isEq(Image::ID, s)) {
						u = true;
						return p;
					}
				}
				return 0;
			}

			void _draw()
			{
				d_images_database img_data;
				Events::big_event bev;

				bool hasChanged = bev.g().getKey(Events::CUSTOMEVENT_BITMAPS_RELOAD);

				img_data.work().lock();
				for (auto& i : img_data.work().work())
				{
					if (hasChanged && i->isEq(Image::MODE, Image::CREATED)) i->reload();
					else i->checkMemory();
				}
				img_data.work().unlock();
			}

			void multipleLoad(const Safer::safe_string nickname, Safer::safe_string start, const size_t max, const unsigned format, Safer::safe_string end, float *perc, const bool optimizethem)
			{
				Tools::interpret_path(start);
				Tools::interpret_path(end);

				Tools::clearPath(start);
				Tools::clearPath(end);

				d_images_database img_data;
				Image::image_low* wi = nullptr;

				char format_ch[256];
				char format_nick[128];

				sprintf_s(format_ch, "%s%c0%ulu%s", start.g().c_str(), '%', format, end.g().c_str());
				sprintf_s(format_nick, "%s%clu", nickname.g().c_str(), '%');

				for (size_t p = 0; p < max; p++) {
					if (perc) {
						*perc = 1.0f * p / max;
					}

					char newname[276];
					char newnick[148];

					sprintf_s(newname, format_ch, p);
					sprintf_s(newnick, format_nick, p);

					img_data.create(wi);
					wi->set(Image::ID, newnick);
					wi->set(Image::PATH, newname);
					wi->set(Image::FORCE_ON_MEMORY, !optimizethem);
					wi = nullptr;
				}

				if (perc) *perc = 1.0;
			}
			


			image_low* getOrCreate(const Safer::safe_string s, const bool create)
			{
				d_images_database img_data;
				image_low* ref = nullptr;
				if (img_data.get(ref, s)) return ref;
				if (create){
					img_data.create(ref);
					ref->set(Image::ID, s);
					return ref;
				}
				else {
					throw "at Image::getOrCreate(): Could not find \"" + s.g() + "\".";
					return nullptr;
				}
			}
			void easyRemove(const Safer::safe_string s)
			{
				d_images_database img_data;
				image_low* ref = nullptr;
				if (img_data.get(ref, s)) {
					img_data.remove(s);
					ref->unload();
					delete ref;
				}
			}
		}
	}
}