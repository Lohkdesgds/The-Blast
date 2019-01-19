#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Image {

			bool image_low::keep_on_memory_always = false;
			//double image_low::lastimagereload = 0.0;
			//Safer::safe_vector<image_low*> images_database::imgs;

			const bool image_low::_load_NOADJUST(const Safer::safe_string s)
			{
				Log::gfile logg;

				logg << Log::START << "[IMGL:LOADN][INFO] Registered load of \"" << s << "\"... " << Log::ENDL;

				bmp = al_load_bitmap(s.g().c_str());
				if (bmp)
				{
					//proportion = 1.0;
					lastcall = GetTickCount64();
					orig_siz[0] = al_get_bitmap_width(bmp);
					orig_siz[1] = al_get_bitmap_height(bmp);
					mode = LOADED;
					path = s;

					
					logg << Log::START << "[IMGL:LOADN][INFO] LOADED Successfully: \"" << s << "\"." << Log::ENDL;
					
					return true;
				}

				logg << Log::ERRDV << Log::START << "[IMGL:LOADN][ERRR] ERROR LOADING \"" << s << "\"! Skipping!" << Log::ENDL;
				logg.flush();

				return false;
			}

			/*const bool image_low::set(const Safer::safe_string s_o)
			{
				unload();

				Safer::safe_string s = Defaults::default_data_path.g() + s_o.g();
				Tools::interpret_path(s);

				if (Tools::getFileSize(s) > 0) {
					path = s;
					lastcall = 0;
					return true;
				}
				return false;
			}*/

			const bool image_low::load(const Safer::safe_string s_o)
			{
				unload();
				Safer::safe_string s = Defaults::default_data_path.g() + s_o.g();
				Tools::interpret_path(s);

				return _load_NOADJUST(s);
			}

			/*const bool image_low::loadFromDatabase(const Safer::safe_string newname)
			{
				Safer::safe_string s = Defaults::initial_call_url.g() + newname.g();
				Tools::clearPath(s, true);
				return loadFromURL(s, newname);
			}
			
			const bool image_low::loadFromURL(const Safer::safe_string url, Safer::safe_string s_o)
			{
				if (!load(s_o)) assert(Tools::saveFromCloud(url, s_o));
				else return true;
				return load(s_o);
			}*/
			const bool image_low::create(int x, int y)
			{
				Log::gfile logg;

				if (x <= 0 || y <= 0) {
					ALLEGRO_DISPLAY *d = al_get_current_display();
					assert(d);
					x = al_get_display_width(d);
					y = al_get_display_height(d);
					created_itself = true;
				}

				logg << Log::START << "[IMGL:NEWBP][INFO] Registered creation of a bitmap with size " << x << "x" << y << "... " << Log::ENDL;

				unload();
				bmp = al_create_bitmap(x, y);
				if (bmp)
				{
					logg << Log::START << "[IMGL:NEWBP][INFO] CREATED Successfully a bitmap with size " << x << "x" << y << "... " << Log::ENDL;
					//proportion = 1.0;
					lastcall = GetTickCount64();
					orig_siz[0] = al_get_bitmap_width(bmp);
					orig_siz[1] = al_get_bitmap_height(bmp);
					mode = CREATED;
					return true;
				}

				logg << Log::ERRDV << Log::START << "[IMGL:NEWBP][ERRR] FAILED CREATING a bitmap with size " << x << "x" << y << "... " << Log::ENDL;

				return false;
			}
			void image_low::setID(const Safer::safe_string i)
			{
				id = i;
			}
			const bool image_low::amI(const Safer::safe_string o)
			{
				return (id == o);
			}

			const bool image_low::amI(const _img_mode m)
			{
				return (m == mode);
			}

			const Safer::safe_string image_low::whoAmI()
			{
				return id;
			}

			const bool image_low::isLoaded()
			{
				return (bmp);
			}

			void image_low::unload()
			{
				Log::gfile logg;

				if (path.g().length() > 0) logg << Log::START << "[IMGL:NLOAD][INFO] Registering unload of texture named \"" << path.g() << "\"." << Log::ENDL;
				else logg << Log::START << "[IMGL:NLOAD][INFO] Registering unload of texture with size " << orig_siz[0] << "x" << orig_siz[1] << "." << Log::ENDL;

				//proportion = 1.0;
				//orig_siz[0] = orig_siz[1] = 0;
				lastcall = 0;
				//path.clear();

				if (bmp) {
					al_destroy_bitmap(bmp);
					bmp = nullptr;
				}
			}

			const bool image_low::reload()
			{
				al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

				if (created_itself)
				{
					unload();
					return create(-1, -1);
				}
				else if (path.g().length() == 0 && orig_siz[0] > 0)
				{
					int backup[2];
					backup[0] = orig_siz[0];
					backup[1] = orig_siz[1];
					unload();
					return create(backup[0], backup[1]);
				}
				else if (path.g().length() > 0)
				{
					unload();
					return _load_NOADJUST(path);
				}
				return false;
			}

			void image_low::optimizeIt()
			{
				int new_x, new_y;
				new_x = al_get_bitmap_width(bmp) * 0.25 + 1;
				new_y = al_get_bitmap_height(bmp) * 0.25 + 1;

				ALLEGRO_BITMAP * newbmp = al_create_bitmap(new_x, new_y);
				ALLEGRO_BITMAP * targ = al_get_target_bitmap();

				al_set_target_bitmap(newbmp);

				al_draw_scaled_bitmap(bmp, 0, 0, al_get_bitmap_width(bmp), al_get_bitmap_height(bmp), 0, 0, new_x, new_y, 0);
				ALLEGRO_BITMAP* ref = bmp;
				bmp = newbmp;
				al_destroy_bitmap(ref);
				ref = nullptr;

				al_set_target_bitmap(targ);
				optimized = true;
			}

			void image_low::verify()
			{
				Log::gfile logg;

				if (!bmp && !optimized) return;

				if (created_itself && al_get_time() - check_time > Defaults::verify_imager_timer_created) { // right size for fullscreen bitmaps (not loaded)
					if (!bmp) return;

					check_time = al_get_time();

					ALLEGRO_DISPLAY *d = al_get_current_display();
					assert(d);
					int x, y;
					x = al_get_display_width(d);
					y = al_get_display_height(d);
					orig_siz[0] = al_get_bitmap_width(bmp);
					orig_siz[1] = al_get_bitmap_height(bmp);
					if (x != orig_siz[0] || y != orig_siz[1])
					{
						reload();
					}
				}
				
				else if (optimized)
				{
					optimizing.lock();
					
					reload();

					logg << Log::START << "[IMGL:VERIF][INFO] Reloaded " << id << Log::ENDL;

					optimized = false;

					optimizing.unlock();
				}
			}

			void image_low::checkMemory()
			{
				Log::gfile logg;
				if (keep_on_memory_always) return;

				//if (al_get_time() - lastimagereload < 1.0 / Defaults::max_images_reload_per_sec) return;
				if (al_get_time() - lastcall > Defaults::timeout_image_unload && !optimized)
				{
					optimizing.lock();

					//optimizeIt();
					unload();

					logg << Log::START << "[IMGL:CHKMM][INFO] Optimized " << id << Log::ENDL;

					optimized = true;

					//lastimagereload = al_get_time();
					optimizing.unlock();
				}
			}

			void image_low::get(ALLEGRO_BITMAP*& b)
			{
				lastcall = al_get_time();

				if (!bmp) verify();

				b = bmp;
			}

			const bool image_low::checkExistance()
			{
				return (this);
			}

			void image_low::_setKeepOnMemory(const bool b)
			{
				keep_on_memory_always = b;
			}

			
			size_t _find(const Safer::safe_string s, Safer::safe_vector<image_low*>& v, bool& u)
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
				d_images_database img_data;
				Events::big_event bev;

				bool hasChanged = bev.g().getKey(Events::CUSTOMEVENT_DISPLAY_TOGGLED);

				for (int u = 0; u < (int)img_data.work().getMax(); u++)
				{
					if (hasChanged && img_data.work().get(u)->amI(Image::CREATED)) img_data.work().get(u)->reload();
					else img_data.work().get(u)->checkMemory();
				}
			}

			/*void _start_thread()
			{
				d_images_database img_data;
				img_data.getArg() = new std::thread(_draw_complex);
			}

			void _draw_complex()
			{
				d_images_database img_data;
				Safer::safe_vector<Image::image_low*>& imgs = img_data.work();

				while (imgs.getMax() == 0);

				while (imgs.getMax() > 0) {

					Sleep(0.5*Defaults::timeout_image_unload);

					for (size_t p = 0; p < imgs.getMax(); p++)
					{
						imgs[p]->verify();
					}
				}
				
			}*/

			void multipleLoad(const Safer::safe_string nickname, Safer::safe_string start, const size_t max, const unsigned format, Safer::safe_string end, float *perc)
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

					//log << Log::START << "[multipleLoad] Registered load of \"" << newname << "\" named \"" << newnick << "\"... " << Log::ENDL;

					img_data.create(wi);
					wi->setID(newnick);
					wi->load(newname);
					/*if (!)
					{
						//log << Log::ERRDV << Log::START << "[multipleLoad] ERROR LOADING \"" << newname << "\" named \"" << newnick << "\"! Skipping!" << Log::ENDL;
						//log.flush();
					}
					else {
						log << Log::START << "[multipleLoad] LOADED Successfully: \"" << newname << "\" named \"" << newnick << "\"." << Log::ENDL;
					}*/

					//log << "(" << ((wi->isLoaded()) ? "LOADED" : "FAILED_TO_LOAD") << ")" << Log::ENDL;
					wi = nullptr;
				}

				if (perc) *perc = 1.0;
			}
			/*void multipleCloudLoad(const Safer::safe_string nickname, Safer::safe_string start, const size_t max, const unsigned format, Safer::safe_string end, const Safer::safe_string* urls, float* perc)
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

					log << "[multipleLoad] Registered download and load of \"" << newname << "\" named \"" << newnick << "\"... ";

					img_data.create(wi);
					wi->setID(newnick);
					if (!wi->load(newname)) {
						if (urls) wi->loadFromURL(urls[p], newname);
						else {
							wi->loadFromDatabase(newname);
						}
					}

					log << "(" << ((wi->isLoaded()) ? "READY" : "FAILED_TO_LOAD_OR_DOWNLOAD!") << ")" << Log::ENDL;
					wi = nullptr;
				}
				if (perc) *perc = 1.0;
			}*/
		}
	}
}