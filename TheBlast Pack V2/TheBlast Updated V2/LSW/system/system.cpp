#include "system.h"

namespace LSW {
	namespace v4 {
		Database::custom_data Database::data;

		void __systematic::__extract_package(float* perc)
		{
			if (perc) *perc = 0.00;

			FILE* me;
			std::string work = Constants::start_zip_default_extract_path;
			char myself[1024];
			GetModuleFileNameA(NULL, myself, 1024);

			Tools::interpretPath(work);
			Tools::createPath(work);

			auto err0 = fopen_s(&me, myself, "rb");

			__ensure_warn_package();

			LONGLONG totalsiz = Tools::getFileSize(myself) - 2; // adjust
			LONGLONG nowstep = 0;

			if (err0) {
				if (me) fclose(me);
				throw Abort::abort(__FUNCSIG__, "Can't extract file to a temporary place!");
				return;
			}

			size_t p = 0;
			char ch[1];
			bool still = true;
			bool found = false;

			while (still && (me != nullptr))
			{
				auto u = fread_s(ch, 1, 1, 1, me);
				if (u != 1) {
					still = false;
					continue;
				}
				if (Constants::_match_extract[p] == ch[0]) {
					if (++p == Constants::_match_extract.length()) {
						still = false;
						found = true;
						fread_s(ch, 1, 1, 1, me);
						break;
					}
				}
				else p = 0;

				if (totalsiz-- <= 0) {
					if (me) fclose(me);
					throw Abort::abort(__FUNCSIG__, "File size isn't right! Your file might be incomplete!", 1);
					return;
				}
			}

			if (found) {
				FILE *fi;
				auto err1 = fopen_s(&fi, work.c_str(), "wb");
				char buf[512];
				for (size_t s = 0;;) {
					s = fread_s(buf, sizeof(buf), sizeof(char), 512, me);
					if (fi) fwrite(buf, sizeof(char), s, fi);
					nowstep += s;

					if (perc) *perc = (1.0 * nowstep / totalsiz);
					if (s != 512) break;
				}
				if (fi) fclose(fi);
			}
			else {
				if (me) fclose(me);
				throw Abort::abort(__FUNCSIG__, "Datapack not available inside .exe file!", 1);
			}

			if (me) fclose(me);

			if (perc) *perc = 1.0;
			extracted_zip_at = work;
		}

		void __systematic::__nointernalzip_extract_package()
		{
			FILE* fi;
			std::string work = Constants::start_zip_default_extract_path;
			char myself[1024];
			GetModuleFileNameA(NULL, myself, 1024);

			Tools::interpretPath(work);
			Tools::createPath(work);

			__ensure_warn_package();

			auto err1 = fopen_s(&fi, work.c_str(), "rb");

			if (err1) {
				if (fi) fclose(fi);
				throw Abort::abort(__FUNCSIG__, "External ZIP not found, please report.");
				return;
			}

			extracted_zip_at = work;
		}

		void __systematic::__ensure_warn_package()
		{
			std::string work = Constants::start_zip_warning_file_txt;

			Tools::interpretPath(work);
			Tools::createPath(work);

			FILE* fp = nullptr;
			auto err = fopen_s(&fp, work.c_str(), "wb");

			if (err == 0 && fp) {
				fprintf_s(fp, Constants::start_zip_warning_default_text.c_str());
				fclose(fp);
			}
			else {
				throw Abort::abort(__FUNCSIG__, "Could not create the warning file (zip)");
			}
		}
		bool __systematic::__loadPackage()
		{
			if (!initialized) throw Abort::abort(__FUNCSIG__, "Wasn't initialized properly!");


			if (extracted_zip_at.length() == 0) {
				__extract_package();
			}

			if (!PHYSFS_addToSearchPath(extracted_zip_at.c_str(), 1)) throw Abort::abort(__FUNCSIG__, "Can't add datapack to internal search");

			return true;
		}

		bool __systematic::__nointernalzip_loadPackage()
		{
			if (!initialized) throw Abort::abort(__FUNCSIG__, "Wasn't initialized properly!");


			if (extracted_zip_at.length() == 0) {
				__nointernalzip_extract_package();
			}

			if (!PHYSFS_addToSearchPath(extracted_zip_at.c_str(), 1)) throw Abort::abort(__FUNCSIG__, "Can't add datapack to internal search");

			return true;
		}

		__systematic::~__systematic()
		{
			if (l) {
				delete l;
				l = nullptr;
			}
			al_uninstall_keyboard();
			al_uninstall_mouse();
			al_stop_samples();
			al_uninstall_audio();
			al_shutdown_font_addon();
			al_shutdown_image_addon();
			al_shutdown_primitives_addon();
			al_shutdown_ttf_addon();
			al_shutdown_native_dialog_addon();
			PHYSFS_deinit();

			initialized = false;
			exit(EXIT_SUCCESS);
		}

		void __systematic::initSystem()
		{

			if (!al_init())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#01]");
			if (!al_init_acodec_addon())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#02]");
			if (!al_init_font_addon())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#03]");
			if (!al_init_image_addon())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#04]");
			if (!al_init_primitives_addon())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#05]");
			if (!al_init_ttf_addon())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#06]");
			if (!al_install_keyboard())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#07]");
			if (!al_install_mouse())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#08]");
			if (!al_install_audio())
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#09]");
			if (!al_reserve_samples(Constants::start_audio_samples_max))
				throw Abort::abort(__FUNCSIG__, "Failed to open the game [E#10]");


			char myself[1024];
			GetModuleFileNameA(NULL, myself, 1024);

			if (!already_set_physfs_root) {
				if (!PHYSFS_init(myself)) throw Abort::abort(__FUNCSIG__, "Failed to set the datapack to run", 1);
				already_set_physfs_root = true;
			}

			if (l) {
				delete l;
				l = nullptr;
			}

			l = new modes();

			initialized = true;

			setNewDisplayMode(Constants::start_display_default_mode);

			__loadPackage(); // extract and set zip on physfs

			al_set_physfs_file_interface();
		}

		void __systematic::forceInitWithNoZipAnyway()
		{
			__nointernalzip_loadPackage();
			al_set_physfs_file_interface();
		}

		void __systematic::setInterface()
		{
			al_set_physfs_file_interface();
		}

		void __systematic::setNewDisplayMode(const int mds)
		{
			if (!initialized) throw Abort::abort(__FUNCSIG__, "Wasn't initialized properly!");


			if (!l) throw Abort::abort(__FUNCSIG__, "Display modes shouldn't be NULL");

			int nva = al_get_num_video_adapters();
			if (nva < 1) throw Abort::abort(__FUNCSIG__, "The number of display modes was less than one.");

			l->options.clear();

			al_set_new_display_flags(mds);
			l->lastmode = mds;

			int num_modes = al_get_num_display_modes();
			for (int j = 0; j < num_modes; ++j) {
				ALLEGRO_DISPLAY_MODE admode;
				if (al_get_display_mode(j, &admode) == &admode) {
					Assistance::display_mode mod;
					mod.x = admode.width;
					mod.y = admode.height;
					mod.hz = admode.refresh_rate;
					bool push = true;
					for (auto& i : l->options) {
						if (mod.x == i.x && mod.y == i.y && mod.hz == i.hz) {
							push = false;
							break;
						}
					}
					if (push) {
						l->options.push_back(mod);
					}
				}
			}
		}

		bool __systematic::checkResolutionExistance(const int x, const int y, const int hz)
		{
			if (!initialized) throw Abort::abort(__FUNCSIG__, "Wasn't initialized properly!");


			if (!l) throw Abort::abort(__FUNCSIG__, "Display modes shouldn't be NULL");

			Assistance::display_mode sm;
			sm.x = x;
			sm.y = y;
			sm.hz = hz;
			bool alright = false;

			if (l->lastmode & ALLEGRO_WINDOWED) // doesn't need to check fixed window size
			{
				alright = true;
				for (auto& i : l->options)
				{
					if (i.hz < sm.hz || sm.x > i.x || sm.y > i.y) {
						alright = false;
						break;
					}
				}
			}
			else {
				for (auto& i : l->options)
				{
					if (i == sm) {
						alright = true;
						break;
					}
				}
			}

			return alright;
		}

		const auto __systematic::getAvailableResolutions()
		{
			if (!initialized) throw Abort::abort(__FUNCSIG__, "Wasn't initialized properly!");


			if (!l) throw Abort::abort(__FUNCSIG__, "Display modes shouldn't be NULL");
			return l->options;
		}

		void Display::_init(const int x, const int y, const int flag, int hz)
		{
			if (d) {
				al_destroy_display(d);
				d = nullptr;
			}

			if (flag > 0) __g_sys.setNewDisplayMode(flag);

			auto u = __g_sys.getAvailableResolutions();

			int tx, ty;
			tx = x;
			ty = y;

			if (hz < 0) {
				hz = 0;
				for (auto& i : u) {
					bool higher = ((i.x > tx && i.y >= ty) || (i.x >= tx && i.y > ty));
					bool equall = (i.x == tx && i.y == ty);
					bool faster = i.hz > hz;

					if (higher) {
						hz = i.hz;
						tx = i.x;
						ty = i.y;
					}
					if (equall && faster) {
						hz = i.hz;
					}
				}
			}

			if (__g_sys.checkResolutionExistance(x, y, hz) || (flag & ALLEGRO_WINDOWED)) {
				al_set_new_bitmap_flags(Constants::start_bitmap_default_mode);
				al_set_new_display_refresh_rate(hz);
				al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
				if (d = al_create_display(x, y)) {
					gfile logg;
					logg << L::SLL << fsr(__FUNCSIG__) << "Created display successfully: " << x << "x" << y << "@" << (hz == 0 ? "auto" : std::to_string(hz)) << ";flags=" << flag << L::BLL;
				}
				else {
					throw Abort::abort(__FUNCSIG__, "Could not create display!");
				}
				al_convert_bitmaps();

				this->x = x;
				this->y = y;
				this->f = flag;
				this->h = al_get_display_refresh_rate(d);
			}
			else {
				throw Abort::abort(__FUNCSIG__, "Invalid resolution, sorry!");
			}
		}
		
		void Display::_print()
		{
			auto u = al_get_backbuffer(d);
			std::string path = Constants::default_print_path;
			Tools::interpretPath(path);
			Tools::createPath(path);

			auto rt = time(NULL);
			tm tt;
			auto v = localtime_s(&tt, &rt);

			if (v != 0) throw Abort::abort(__FUNCSIG__, "Can't set a proper name to save the screenshot!");

			path = path + std::to_string(tt.tm_year + 1900) + "_" + std::to_string(tt.tm_mon + 1) + "_" + std::to_string(tt.tm_mday) + "-" + std::to_string(tt.tm_hour) + "_" + std::to_string(tt.tm_min) + "_" + std::to_string(tt.tm_sec) + "-" + std::to_string((int)(GetTickCount64()%1000)) + ".jpg";
			
			int savv = al_get_new_bitmap_flags();
			al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
			auto cpy = al_clone_bitmap(u);
			al_set_new_bitmap_flags(savv);			

			if (printthr) {
				if (!printthrdone) return;

				printthr->join();
				delete printthr;
				printthr = nullptr;
			}

			printthrdone = false;
			printthr = new std::thread([=] {__print_thr_autodel(cpy, path); });

			printing = false;
		}

		void Display::__print_thr_autodel(ALLEGRO_BITMAP* u, const std::string path)
		{
			al_save_bitmap(path.c_str(), u);
			al_destroy_bitmap(u);
			printthrdone = true;
		}

		Display::Display()
		{
			gfile logg;
			logg << L::SLL << fsr(__FUNCSIG__) << "Creating new display (blank)" << L::BLL;

			int flag = Constants::start_display_default_mode;
			__g_sys.setNewDisplayMode(flag);
			auto u = __g_sys.getAvailableResolutions();

			Assistance::display_mode md;

			Database config;
			config.get(Assistance::io__conf_integer::SCREEN_X, md.x, 0);
			config.get(Assistance::io__conf_integer::SCREEN_Y, md.y, 0);

			
			if (md.x != 0 && md.y != 0) {
				logg << L::SLL << fsr(__FUNCSIG__) << "Found config from config file. Searching available refresh rates..." << L::BLL;
				md.hz = 0;
				for (auto& i : u) {
					if ((i.x == md.x && i.y == md.y)) {
						if (i.hz > md.hz) {
							md.hz = i.hz;
						}
					}
				}
			}
			else if (Constants::start_force_720p)
			{
				logg << L::SLL << fsr(__FUNCSIG__, E::WARN) << "No config file found. 720P is set to default, so, searching available refresh rates..." << L::BLL;
				md.x = 1280;
				md.y = 720;
				md.hz = 0;
				for (auto& i : u) {
					if ((i.x == md.x && i.y == md.y)) {
						if (i.hz > md.hz) {
							md.hz = i.hz;
						}
					}
				}
			}
			if (md.hz == 0){ // both before failed, search for another option afaik
				logg << L::SLL << fsr(__FUNCSIG__, E::WARN) << "Failed to find a supported refresh rate. Trying automatic instead." << L::BLL;
				md.hz = 0;
			}


			_init(md.x, md.y, -1, md.hz);
		}

		Display::Display(const int x, const int y, const int flag, int hz)
		{
			gfile logg;
			logg << L::SLL << fsr(__FUNCSIG__) << "Creating new custom display [" << x << "x" << y << "@" << hz << " w/ " << flag << "]" << L::BLL;
			_init(x, y, flag, hz);
		}
		Display::~Display()
		{
			close();
		}
		void Display::restart()
		{
			close();
			_init(this->x, this->y, this->f, this->h);
		}
		void Display::flip()
		{
			al_flip_display();
			if (printing) _print();
			al_set_target_backbuffer(d);
		}
		void Display::clearTo(const ALLEGRO_COLOR v)
		{
			al_clear_to_color(v);
		}
		void Display::close()
		{
			gfile logg;
			logg << L::SLL << fsr(__FUNCSIG__) << "Deleting display..." << L::BLL;

			d_try.lock();
			if (d) {
				al_set_target_backbuffer(d);
				al_destroy_display(d);
				d = nullptr;
			}
			d_try.unlock();
		}
		bool Display::exist()
		{
			return (d);
		}
		void Display::print()
		{
			printing = true;
		}
		ALLEGRO_DISPLAY*& Display::getDisplay()
		{
			return d;
		}

		Database::Database()
		{
			logg << L::SL << fsr(__FUNCSIG__ )<< "Registered spawn of Database ID#" + std::to_string((size_t)this) << L::BL;
			data.m.lock();
			if (!data.c) {
				std::string temporary = Constants::config_default_file;
				Tools::interpretPath(temporary);
				Tools::createPath(temporary);

				logg << L::SLL << fsr(__FUNCSIG__) << "Registered loading of Database file (global)" << L::BLL;
				if (!(data.c = al_load_config_file(temporary.c_str())))
				{
					data.c = al_create_config();
					if (!data.c) throw Abort::abort(__FUNCSIG__, "Cannot load/create Database file!");
					logg << L::SLL << fsr(__FUNCSIG__) << "Is this the first time you're opening this game? Registered first Database load." << L::BLL;

					// set or load first values
					set(Assistance::io__conf_boolean::HAD_ERROR, true); // then texture download starts			// USED
					set(Assistance::io__conf_boolean::WAS_OSD_ON, false);											// USED
					set(Assistance::io__conf_boolean::WAS_FULLSCREEN, true);										// USED
					set(Assistance::io__conf_float::LAST_VOLUME, 0.5);
					set(Assistance::io__conf_string::LAST_VERSION, Constants::version_app);						// nah
					set(Assistance::io__conf_string::LAST_PLAYERNAME, "Player");
					set(Assistance::io__conf_string::LAST_COLOR, "GREEN");
					set(Assistance::io__conf_integer::SCREEN_X, 0);
					set(Assistance::io__conf_integer::SCREEN_Y, 0);
					set(Assistance::io__conf_longlong::_TIMES_LIT, 0LL);

					if (!al_save_config_file(temporary.c_str(), data.c)) {
						throw Abort::abort(__FUNCSIG__, "Cannot save Database file!");
					}
				}
				else {
					long long ll;
					get(Assistance::io__conf_longlong::_TIMES_LIT, ll, 0LL);
					ll++;
					set(Assistance::io__conf_longlong::_TIMES_LIT, ll);
				}
			}
			data.m.unlock();
		}
		Database::~Database()
		{
			logg << L::SL << fsr(__FUNCSIG__) << "Registered despawn of Database ID#" + std::to_string((size_t)this) << L::BL;
			if (data.c) {
				data.m.lock();
				std::string temporary = Constants::config_default_file;
				Tools::interpretPath(temporary);
				Tools::createPath(temporary);
				al_save_config_file(temporary.c_str(), data.c);
				data.m.unlock();
			}
		}

		void Database::set(const std::string e, const std::string v)
		{
			if (!data.c) throw Abort::abort(__FUNCSIG__, "Cannot set \"" + e + "\" as \"" + v + "\".");
			al_set_config_value(data.c, Constants::conf_string_default_txt.c_str(), e.c_str(), v.c_str());
		}
		void Database::set(const Assistance::io__conf_boolean e, const bool v)
		{
			set(Assistance::ro__conf_boolean_str[+e], Assistance::ro__conf_truefalse_str[(int)v]);
		}
		void Database::set(const Assistance::io__conf_float e, const float v)
		{
			set(Assistance::ro__conf_float_str[+e], std::to_string(v));
		}
		void Database::set(const Assistance::io__conf_integer e, const int v)
		{
			set(Assistance::ro__conf_integer_str[+e], std::to_string(v));
		}
		void Database::set(const Assistance::io__conf_longlong e, const long long v)
		{
			set(Assistance::ro__conf_longlong_str[+e], std::to_string(v));
		}
		void Database::set(const Assistance::io__conf_string e, const std::string v)
		{
			set(Assistance::ro__conf_string_str[+e], v);
		}
		void Database::set(const int al_keycod, const bool v)
		{
			if (al_keycod >= 0 && al_keycod < ALLEGRO_KEY_MAX) data.keys[al_keycod] = v;
		}
		void Database::set(const Assistance::io__conf_mouse_boolean e, const bool v)
		{
			data.mouse[+e] = v;
		}
		void Database::set(const Assistance::io__conf_mouse_float e, const float v)
		{
			data.mouse_axes[+e] = v;
		}

		void Database::get(const std::string e, std::string& v, const std::string defaul)
		{
			if (!data.c) throw Abort::abort(__FUNCSIG__, "Cannot get \"" + e + "\" as \"" + v + "\".");
			const char* chh = al_get_config_value(data.c, Constants::conf_string_default_txt.c_str(), e.c_str());
			if (!chh) {
				logg << L::SLL << "[CONF:GET()][WARN] There was no value to " << e << ", so the default value was set (" << defaul << ")." << L::BLL;
				set(e, defaul);
				v = defaul;
				return;
			}
			v = chh;
		}
		void Database::get(const Assistance::io__conf_boolean e, bool& v, const bool defaul)
		{
			std::string output;
			get(Assistance::ro__conf_boolean_str[+e], output, Assistance::ro__conf_truefalse_str[(int)defaul]);
			v = (output == Assistance::ro__conf_truefalse_str[1 /*true*/]);
		}
		void Database::get(const Assistance::io__conf_float e, float& v, const float defaul)
		{
			std::string output;
			get(Assistance::ro__conf_float_str[+e], output, std::to_string(defaul));
			v = atof(output.c_str());
		}
		void Database::get(const Assistance::io__conf_integer e, int& v, const int defaul)
		{
			std::string output;
			get(Assistance::ro__conf_integer_str[+e], output, std::to_string(defaul));
			v = atoi(output.c_str());
		}
		void Database::get(const Assistance::io__conf_longlong e, long long& v, const long long defaul)
		{
			std::string output;
			get(Assistance::ro__conf_longlong_str[+e], output, std::to_string(defaul));
			v = atoll(output.c_str());
		}
		void Database::get(const Assistance::io__conf_string e, std::string& v, const std::string defaul)
		{
			std::string output;
			get(Assistance::ro__conf_string_str[+e], output, defaul);
			v = output;
		}

		void Database::get(const int al_keycod, bool& v, const bool defaul)
		{
			if (al_keycod >= 0 && al_keycod < ALLEGRO_KEY_MAX) v = data.keys[al_keycod];
			else v = defaul;
		}
		void Database::get(const Assistance::io__conf_mouse_boolean e, bool& v)
		{
			v = data.mouse[+e];
		}
		void Database::get(const Assistance::io__conf_mouse_float e, float& v)
		{
			v = data.mouse_axes[+e];
		}


		bool Database::isEq(const std::string e, const std::string v)
		{
			std::string oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		bool Database::isEq(const Assistance::io__conf_boolean e, const bool v)
		{
			bool oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		bool Database::isEq(const Assistance::io__conf_float e, const float v)
		{
			float oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		bool Database::isEq(const Assistance::io__conf_integer e, const int v)
		{
			int oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		bool Database::isEq(const Assistance::io__conf_longlong e, const long long v)
		{
			long long oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		bool Database::isEq(const Assistance::io__conf_string e, const std::string v)
		{
			std::string oth;
			get(e, oth, v); // meh
			return oth == v;
		}

		bool Database::isEq(const int e, const bool v)
		{
			bool oth;
			get(e, oth, v); // meh
			return oth == v;
		}

		bool Database::isEq(const Assistance::io__conf_mouse_boolean e, const bool v)
		{
			bool oth;
			get(e, oth); // meh
			return oth == v;
		}

		bool Database::isEq(const Assistance::io__conf_mouse_float e, const float v)
		{
			float oth;
			get(e, oth); // meh
			return oth == v;
		}




		void lswInit()
		{
			gfile logg;
			Database conf;

			try {
				__g_sys.initSystem();
			}
			catch (Abort::abort a) {
				if (a.getErrN() == 1) {

					logg << L::SLL << fsr(__FUNCSIG__, E::WARN) << "Internal datapack wasn't found." << L::BLL;
					logg.flush();

					int res = al_show_native_message_box(
						nullptr,
						"Internal datapack not found!",
						"Your file may be incomplete!",
						"You can still run the game, but there will be no verification about the resource pack. Click OK if you want to continue anyway.",
						NULL,
						ALLEGRO_MESSAGEBOX_OK_CANCEL);

					if (res == 1) {
						try {
							__g_sys.forceInitWithNoZipAnyway();
						}
						catch (Abort::abort a) {

							std::string ext_exp = std::string("Function gone wrong: " + a.from() + "\n\nExplanation: " + a.details());

							logg << L::SLL << fsr(__FUNCSIG__, E::ERRR) << "User tried to continue, but something went wrong anyway." << L::BLL;
							logg << L::SLL << fsr(__FUNCSIG__, E::ERRR) << ext_exp << L::BLL;
							logg.flush();

							forceExit("Something went wrong anyway!", "Please report the following:", ext_exp.c_str());
						}
					}
					else {
						logg << L::SLL << fsr(__FUNCSIG__) << "User abort." << L::BLL;
						logg.flush();

						forceExit();
					}
				}
				else {
					std::string ext_exp = std::string("Function gone wrong: " + a.from() + "\n\nExplanation: " + a.details());

					logg << L::SLL << fsr(__FUNCSIG__, E::ERRR) << "Something went wrong opening the game." << L::BLL;
					logg << L::SLL << fsr(__FUNCSIG__, E::ERRR) << ext_exp << L::BLL;
					logg.flush();

					forceExit("Something went wrong anyway!", "Please report the following:", ext_exp.c_str());
				}
			}
		}
		void forceExit(const char* windw, const char* title, const char* ext)
		{
			gfile logg;
			logg << L::SLL << fsr(__FUNCSIG__, windw ? E::ERRR : E::WARN) << "FORCEEXIT: " << (windw ? windw : "NULL") << ": " << (title ? title : "NULL") << "; " << (ext ? ext : "NULL") << L::BLL;
			logg.flush();
			if (windw) {
				al_show_native_message_box(
					nullptr,
					windw,
					title,
					ext,
					NULL,
					ALLEGRO_MESSAGEBOX_ERROR);
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
		}
	}
}