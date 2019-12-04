#include "system.h"

namespace LSW {
	namespace v4 {
		ALLEGRO_CONFIG* Config::c = nullptr;
		std::mutex Config::m;


		void __systematic::__extract_package(float* perc)
		{
			if (perc) *perc = 0.00;

			FILE* me;
			std::string work = Constants::start_zip_default_extract_path;
			char myself[1024];
			GetModuleFileNameA(NULL, myself, 1024);

			Tools::interpret_path(work);
			Tools::createPath(work);

			auto err0 = fopen_s(&me, myself, "rb");

			__ensure_warn_package();

			LONGLONG totalsiz = Tools::getFileSize(myself) - 2; // adjust
			LONGLONG nowstep = 0;

			if (err0) {
				if (me) fclose(me);
				throw Abort::abort("fopen_s", "__systematic::__extract_package", "Cannot proceed with temp file to zip");
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
					throw Abort::abort("Tools::getFileSize", "__systematic::__extract_package", "File size isn't right!", 1);
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
				throw Abort::abort("null", "__systematic::__extract_package", "data pack not available inside .exe file");
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

			Tools::interpret_path(work);
			Tools::createPath(work);

			__ensure_warn_package();

			auto err1 = fopen_s(&fi, work.c_str(), "rb");

			if (err1) {
				if (fi) fclose(fi);
				throw Abort::abort("fopen_s", "__systematic::__nointernalzip_extract_package", "ZIP not found, can't proceed");
				return;
			}

			extracted_zip_at = work;
		}

		void __systematic::__ensure_warn_package()
		{
			std::string work = Constants::start_zip_warning_file_txt;

			Tools::interpret_path(work);
			Tools::createPath(work);

			FILE* fp = nullptr;
			auto err = fopen_s(&fp, work.c_str(), "wb");

			if (err == 0 && fp) {
				fprintf_s(fp, Constants::start_zip_warning_default_text.c_str());
				fclose(fp);
			}
			else {
				throw Abort::abort("fopen_s", "system::__ensure_warn_package", "Could not create the warning file (zip)");
			}
		}
		bool __systematic::__loadPackage()
		{
			if (!initialized) throw Abort::abort("initialized", "system::__loadPackage", "Wasn't initialized properly!");


			if (extracted_zip_at.length() == 0) {
				__extract_package();
			}

			if (!PHYSFS_addToSearchPath(extracted_zip_at.c_str(), 1)) throw Abort::abort("PHYSFS_addToSearchPath", "system::__loadPackage", "Can't add datapack to internal search", 99);

			return true;
		}

		bool __systematic::__nointernalzip_loadPackage()
		{
			if (!initialized) throw Abort::abort("initialized", "system::__nointernalzip_loadPackage", "Wasn't initialized properly!");


			if (extracted_zip_at.length() == 0) {
				__nointernalzip_extract_package();
			}

			if (!PHYSFS_addToSearchPath(extracted_zip_at.c_str(), 1)) throw Abort::abort("PHYSFS_addToSearchPath", "system::__nointernalzip_loadPackage", "Can't add datapack to internal search");

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

		void __systematic::init_system()
		{

			if (!al_init())
				throw Abort::abort("al_init", "system::system", "Failed to open the game");
			if (!al_init_acodec_addon())
				throw Abort::abort("al_init_acodec_addon", "system::system", "Failed to open the game");
			if (!al_init_font_addon())
				throw Abort::abort("al_init_font_addon", "system::system", "Failed to open the game");
			if (!al_init_image_addon())
				throw Abort::abort("al_init_image_addon", "system::system", "Failed to open the game");
			if (!al_init_primitives_addon())
				throw Abort::abort("al_init_primitives_addon", "system::system", "Failed to open the game");
			if (!al_init_ttf_addon())
				throw Abort::abort("al_init_ttf_addon", "system::system", "Failed to open the game");
			if (!al_install_keyboard())
				throw Abort::abort("al_install_keyboard", "system::system", "Failed to open the game");
			if (!al_install_mouse())
				throw Abort::abort("al_install_mouse", "system::system", "Failed to open the game");
			if (!al_install_audio())
				throw Abort::abort("al_install_audio", "system::system", "Failed to open the game");
			if (!al_reserve_samples(Constants::start_audio_samples_max))
				throw Abort::abort("al_reserve_samples", "system::system", "Failed to open the game");


			char myself[1024];
			GetModuleFileNameA(NULL, myself, 1024);

			if (!already_set_physfs_root) {
				if (!PHYSFS_init(myself)) throw Abort::abort("PHYSFS_init", "system::system", "Failed to set the datapack to run", 1); // already set the whole thing
				already_set_physfs_root = true;
			}

			if (l) {
				delete l;
				l = nullptr;
			}

			l = new Assistance::modes();

			initialized = true;

			__set_new_display_mode(Constants::start_display_default_mode);

			__loadPackage(); // extract and set zip on physfs

			al_set_physfs_file_interface();
		}

		void __systematic::force_setzip()
		{
			__nointernalzip_loadPackage();
			al_set_physfs_file_interface();
		}

		void __systematic::__set_new_display_mode(const int mds)
		{
			if (!initialized) throw Abort::abort("initialized", "system::__set_new_display_mode", "Wasn't initialized properly!");


			if (!l) throw Abort::abort("struct modes* system::l", "system::__set_new_display_mode", "Shouldn't be NULL");

			int nva = al_get_num_video_adapters();
			if (nva < 1) throw Abort::abort("al_get_num_video_adapters", "system::system", "The number of modes was less than one.");

			l->options.clear();

			al_set_new_display_flags(mds);
			l->lastmode = mds;

			int num_modes = al_get_num_display_modes();
			for (int j = 0; j < num_modes; ++j) {
				ALLEGRO_DISPLAY_MODE admode;
				if (al_get_display_mode(j, &admode) == &admode) {
					Assistance::__submode mod;
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

		bool __systematic::__check_resolution_existance(const int x, const int y, const int hz)
		{
			if (!initialized) throw Abort::abort("initialized", "system::__check_resolution_existance", "Wasn't initialized properly!");


			if (!l) throw Abort::abort("struct modes* system::l", "system::__check_resolution_existance", "Shouldn't be NULL");

			Assistance::__submode sm;
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

		const auto __systematic::__get_available_res()
		{
			if (!initialized) throw Abort::abort("initialized", "system::__get_available_res", "Wasn't initialized properly!");


			if (!l) throw Abort::abort("struct modes* system::l", "system::__get_available_res", "Shouldn't be NULL");
			return l->options;
		}

		void Display::_init(const int x, const int y, const int flag, int hz)
		{
			if (d) {
				al_destroy_display(d);
				d = nullptr;
			}

			if (flag > 0) __g_sys.__set_new_display_mode(flag);

			auto u = __g_sys.__get_available_res();

			int tx, ty;
			tx = x;
			ty = y;

			if (hz == 0) {
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

			if (__g_sys.__check_resolution_existance(x, y, hz) || (flag & ALLEGRO_WINDOWED)) {
				al_set_new_bitmap_flags(Constants::start_bitmap_default_mode);
				al_set_new_display_refresh_rate(hz);
				al_set_new_display_option(ALLEGRO_VSYNC, 2, ALLEGRO_SUGGEST);
				d = al_create_display(x, y);
				al_convert_bitmaps();

				this->x = x;
				this->y = y;
				this->f = flag;
				this->h = hz;
			}
			else {
				throw Abort::abort("system::__check_resolution_existance", "display::display", "Resolution not available.");
			}
		}
		
		void Display::_print()
		{
			auto u = al_get_backbuffer(d);
			std::string path = Constants::default_print_path;
			Tools::interpret_path(path);
			Tools::createPath(path);

			auto rt = time(NULL);
			tm tt;
			auto v = localtime_s(&tt, &rt);

			if (v != 0) throw Abort::abort("localtime_s", "Display::_print", "Cannot get local time to name the print file!");

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
			__g_sys.__set_new_display_mode(flag);
			auto u = __g_sys.__get_available_res();

			Assistance::__submode md;

			Config config;
			config.get(Assistance::conf_i::SCREEN_X, md.x, 0);
			config.get(Assistance::conf_i::SCREEN_Y, md.y, 0);

			
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
				logg << L::SLL << fsr(__FUNCSIG__, E::WARN) << "Failed to find a supported refresh rate. Going with 60 hz instead." << L::BLL;
				md.hz = 60;
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
		void Display::clear_to(const ALLEGRO_COLOR v)
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
		ALLEGRO_DISPLAY*& Display::_getD()
		{
			return d;
		}

		Config::Config()
		{
			logg << L::SL << fsr(__FUNCSIG__ )<< "Registered spawn of Config ID#" + std::to_string((size_t)this) << L::BL;
			m.lock();
			if (!c) {
				std::string temporary = Constants::config_default_file;
				Tools::interpret_path(temporary);
				Tools::createPath(temporary);

				logg << L::SLL << fsr(__FUNCSIG__) << "Registered loading of Config file (global)" << L::BLL;
				if (!(c = al_load_config_file(temporary.c_str())))
				{
					c = al_create_config();
					if (!c) throw "at Config::Config [#" + std::to_string((size_t)this) + "]: Cannot load/create Config file!";
					logg << L::SLL << fsr(__FUNCSIG__) << "Is this the first time you're opening this game? Registered first Config load." << L::BLL;

					// set or load first values
					set(Assistance::conf_b::HAD_ERROR, true); // then texture download starts			// USED
					set(Assistance::conf_b::WAS_OSD_ON, false);											// USED
					set(Assistance::conf_b::WAS_FULLSCREEN, true);										// USED
					set(Assistance::conf_f::LAST_VOLUME, 0.5);
					set(Assistance::conf_s::LAST_VERSION, Constants::version_app);						// nah
					set(Assistance::conf_s::LAST_PLAYERNAME, "Player");
					set(Assistance::conf_s::LAST_COLOR, "GREEN");
					set(Assistance::conf_i::SCREEN_X, 0);
					set(Assistance::conf_i::SCREEN_Y, 0);
					set(Assistance::conf_ll::_TIMES_LIT, 0LL);

					if (!al_save_config_file(temporary.c_str(), c)) {
						throw "at Config::Config [#" + std::to_string((size_t)this) + "]: Cannot save Config file!";
					}
				}
				else {
					long long ll;
					get(Assistance::conf_ll::_TIMES_LIT, ll, 0LL);
					ll++;
					set(Assistance::conf_ll::_TIMES_LIT, ll);
				}
			}
			m.unlock();
		}
		Config::~Config()
		{
			logg << L::SL << fsr(__FUNCSIG__) << "Registered despawn of Config ID#" + std::to_string((size_t)this) << L::BL;
			if (c) {
				m.lock();
				std::string temporary = Constants::config_default_file;
				Tools::interpret_path(temporary);
				Tools::createPath(temporary);
				al_save_config_file(temporary.c_str(), c);
				m.unlock();
			}
		}

		void Config::set(const std::string e, const std::string v)
		{
			if (!c) throw "at Config::set [#" + std::to_string((size_t)this) + "]: Cannot set \"" + e + "\" as \"" + v + "\".";
			al_set_config_value(c, Constants::conf_string_default_txt.c_str(), e.c_str(), v.c_str());
		}
		void Config::set(const Assistance::conf_b e, const bool v)
		{
			set(Assistance::conf_b_str[+e], Assistance::conf_bool_s[(int)v]);
		}
		void Config::set(const Assistance::conf_f e, const float v)
		{
			set(Assistance::conf_f_str[+e], std::to_string(v));
		}
		void Config::set(const Assistance::conf_i e, const int v)
		{
			set(Assistance::conf_i_str[+e], std::to_string(v));
		}
		void Config::set(const Assistance::conf_ll e, const long long v)
		{
			set(Assistance::conf_ll_str[+e], std::to_string(v));
		}
		void Config::set(const Assistance::conf_s e, const std::string v)
		{
			set(Assistance::conf_s_str[+e], v);
		}

		void Config::get(const std::string e, std::string& v, const std::string defaul)
		{
			if (!c) throw "at Config::get [#" + std::to_string((size_t)this) + "]: Cannot get \"" + e + "\" as \"" + v + "\".";
			const char* chh = al_get_config_value(c, Constants::conf_string_default_txt.c_str(), e.c_str());
			if (!chh) {
				logg << L::SLL << "[CONF:GET()][WARN] There was no value to " << e << ", so the default value was set (" << defaul << ")." << L::BLL;
				set(e, defaul);
				v = defaul;
				return;
			}
			v = chh;
		}
		void Config::get(const Assistance::conf_b e, bool& v, const bool defaul)
		{
			std::string output;
			get(Assistance::conf_b_str[+e], output, Assistance::conf_bool_s[(int)defaul]);
			v = (output == Assistance::conf_bool_s[1 /*true*/]);
		}
		void Config::get(const Assistance::conf_f e, float& v, const float defaul)
		{
			std::string output;
			get(Assistance::conf_f_str[+e], output, std::to_string(defaul));
			v = atof(output.c_str());
		}
		void Config::get(const Assistance::conf_i e, int& v, const int defaul)
		{
			std::string output;
			get(Assistance::conf_i_str[+e], output, std::to_string(defaul));
			v = atoi(output.c_str());
		}
		void Config::get(const Assistance::conf_ll e, long long& v, const long long defaul)
		{
			std::string output;
			get(Assistance::conf_ll_str[+e], output, std::to_string(defaul));
			v = atoll(output.c_str());
		}
		void Config::get(const Assistance::conf_s e, std::string& v, const std::string defaul)
		{
			std::string output;
			get(Assistance::conf_s_str[+e], output, defaul);
			v = output;
		}


		const bool Config::isEq(const std::string e, const std::string v)
		{
			std::string oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		const bool Config::isEq(const Assistance::conf_b e, const bool v)
		{
			bool oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		const bool Config::isEq(const Assistance::conf_f e, const float v)
		{
			float oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		const bool Config::isEq(const Assistance::conf_i e, const int v)
		{
			int oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		const bool Config::isEq(const Assistance::conf_ll e, const long long v)
		{
			long long oth;
			get(e, oth, v); // meh
			return oth == v;
		}
		const bool Config::isEq(const Assistance::conf_s e, const std::string v)
		{
			std::string oth;
			get(e, oth, v); // meh
			return oth == v;
		}




		void lsw_init()
		{
			gfile logg;

			try {
				__g_sys.init_system();
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
							__g_sys.force_setzip();
						}
						catch (Abort::abort a) {

							std::string ext_exp = std::string("Function gone wrong: " + a.function() + "\n\nFrom what exactly: " + a.from() + "\n\nExtended explanation: " + a.details());

							logg << L::SLL << fsr(__FUNCSIG__, E::ERRR) << "User tried to continue, but something went wrong anyway." << L::BLL;
							logg << L::SLL << fsr(__FUNCSIG__, E::ERRR) << ext_exp << L::BLL;
							logg.flush();

							al_show_native_message_box(
								nullptr,
								"Something went wrong anyway!",
								"Please report the following:",
								ext_exp.c_str(),
								NULL,
								ALLEGRO_MESSAGEBOX_ERROR);

							exit(EXIT_FAILURE);
						}
					}
					else {
						logg << L::SLL << fsr(__FUNCSIG__) << "User abort." << L::BLL;
						logg.flush();

						exit(EXIT_FAILURE);
					}
				}
				else {
					std::string ext_exp = std::string("Function gone wrong: " + a.function() + "\n\nFrom what exactly: " + a.from() + "\n\nExtended explanation: " + a.details());

					logg << L::SLL << fsr(__FUNCSIG__, E::ERRR) << "Something went wrong opening the game." << L::BLL;
					logg << L::SLL << fsr(__FUNCSIG__, E::ERRR) << ext_exp << L::BLL;
					logg.flush();

					al_show_native_message_box(
						nullptr,
						"Something went wrong!",
						"Please report the following:",
						ext_exp.c_str(),
						NULL,
						ALLEGRO_MESSAGEBOX_ERROR);

					exit(EXIT_FAILURE);
				}
			}
		}
	}
}