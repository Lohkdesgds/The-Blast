#include "system.h"

namespace LSW {
	namespace v4 {
		Database::custom_data Database::data;

		void __systematic::__extract_package(float* perc)
		{
			if (perc) *perc = 0.00;

			FILE* me;
			std::string work = expect_zip_at;
			if (work.length() < 0) throw Abort::abort(__FUNCSIG__, "Datapack path not set!", 2);
			char myself[1024];
			GetModuleFileNameA(NULL, myself, 1024);

			Tools::interpretPath(work);
			Tools::createPath(work);

			auto err0 = fopen_s(&me, myself, "rb");

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
			std::string work = expect_zip_at;
			if (work.length() < 0) throw Abort::abort(__FUNCSIG__, "Datapack path not set!", 2);
			char myself[1024];
			GetModuleFileNameA(NULL, myself, 1024);

			Tools::interpretPath(work);
			Tools::createPath(work);

			auto err1 = fopen_s(&fi, work.c_str(), "rb");

			if (err1) {
				if (fi) fclose(fi);
				throw Abort::abort(__FUNCSIG__, "External ZIP not found, please report.");
				return;
			}

			extracted_zip_at = work;
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

		void __systematic::setZipLocation(const std::string a)
		{
			expect_zip_at = a;
		}

		void __systematic::initSystem(const bool with_zip)
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

			if (with_zip) __loadPackage(); // extract and set zip on physfs
			else __nointernalzip_loadPackage(); // ignore zips

			al_set_physfs_file_interface();
		}

		void __systematic::posInit_forceWithNoZipAnyway()
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
					Constants::display_mode mod;
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

		bool __systematic::checkResolutionExistance(const int x, const int y, const int hz) const
		{
			if (!initialized) throw Abort::abort(__FUNCSIG__, "Wasn't initialized properly!");


			if (!l) throw Abort::abort(__FUNCSIG__, "Display modes shouldn't be NULL");

			Constants::display_mode sm;
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

		const auto __systematic::getAvailableResolutions() const
		{
			if (!initialized) throw Abort::abort(__FUNCSIG__, "Wasn't initialized properly!");


			if (!l) throw Abort::abort(__FUNCSIG__, "Display modes shouldn't be NULL");
			return l->options;
		}

		void Database::internalCheck()
		{
			if (!check(Constants::io__conf_boolean::WAS_OSD_ON))								set(Constants::io__conf_boolean::WAS_OSD_ON, false);
			if (!check(Constants::io__conf_boolean::ENABLE_SECOND_DEBUGGING_SCREEN))			set(Constants::io__conf_boolean::ENABLE_SECOND_DEBUGGING_SCREEN, false);
			if (!check(Constants::io__conf_boolean::ULTRADEBUG))								set(Constants::io__conf_boolean::ULTRADEBUG, false);
			if (!check(Constants::io__conf_boolean::DOUBLEBUFFERING))							set(Constants::io__conf_boolean::DOUBLEBUFFERING, false);

			if (!check(Constants::io__conf_double::LAST_VOLUME))								set(Constants::io__conf_double::LAST_VOLUME, 0.5);
			if (!check(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION))				set(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION, 1.0);
			if (!check(Constants::io__conf_double::FX_AMOUNT))									set(Constants::io__conf_double::FX_AMOUNT, 1.0);

			if (!check(Constants::io__conf_integer::SCREEN_X))									set(Constants::io__conf_integer::SCREEN_X, 0);
			if (!check(Constants::io__conf_integer::SCREEN_Y))									set(Constants::io__conf_integer::SCREEN_Y, 0);
			if (!check(Constants::io__conf_integer::SCREEN_FLAGS))								set(Constants::io__conf_integer::SCREEN_FLAGS, Constants::start_display_default_mode);
			if (!check(Constants::io__conf_integer::SCREEN_PREF_HZ))							set(Constants::io__conf_integer::SCREEN_PREF_HZ, 0);

			if (!check(Constants::io__conf_longlong::_TIMES_LIT))								set(Constants::io__conf_longlong::_TIMES_LIT, 0LL);

			if (!check(Constants::io__conf_string::LAST_PLAYERNAME))							set(Constants::io__conf_string::LAST_PLAYERNAME, "Player");
			if (!check(Constants::io__conf_string::PRINT_PATH))									set(Constants::io__conf_string::PRINT_PATH, Constants::default_print_path);

																								set(Constants::io__conf_string::LAST_VERSION, Constants::version_app); // just set
			if (!check(Constants::io__conf_color::LAST_COLOR_TRANSLATE))						set(Constants::io__conf_color::LAST_COLOR_TRANSLATE, data.color);
		}
		
		void Database::load(std::string temporary)
		{
			data.m.lock();
			if (!data.c && temporary.length() > 0) {
				Tools::interpretPath(temporary);
				Tools::createPath(temporary);

				data.config_raw_path = temporary;

				logg << L::SLF << fsr(__FUNCSIG__) << "Registered loading of Database file (global)" << L::ELF;
				if (!(data.c = al_load_config_file(temporary.c_str())))
				{
					data.c = al_create_config();
					if (!data.c) throw Abort::abort(__FUNCSIG__, "Cannot load/create Database file!");
					logg << L::SLF << fsr(__FUNCSIG__) << "Is this the first time you're opening this game? Registered first Database load." << L::ELF;

					internalCheck();

					if (!al_save_config_file(temporary.c_str(), data.c)) {
						throw Abort::abort(__FUNCSIG__, "Cannot save Database file!");
					}
				}
				else {
					internalCheck();
					flush();

					long long ll;
					get(Constants::io__conf_longlong::_TIMES_LIT, ll);
					ll++;
					set(Constants::io__conf_longlong::_TIMES_LIT, ll);
				}

				/*complicated stuff*/
				{
					std::string str;
					if (get(Constants::ro__conf_color_str[+Constants::io__conf_color::LAST_COLOR_TRANSLATE], str)) {
						if (str.length() > 0) {
							if (str[0] == '{') {
								sscanf_s(str.c_str(), "{%f;%f;%f}", &data.color.r, &data.color.g, &data.color.b);
								data.color.a = 1.0;
							}
						}
					}
					set(Constants::io__conf_color::LAST_COLOR_TRANSLATE, data.color);
				}

			}
			data.m.unlock();
		}

		void Database::flush()
		{
			if (data.savethr) {
				if (!data.savethrdone) return;

				data.savethr->join();
				delete data.savethr;
				data.savethr = nullptr;
			}

			data.savethrdone = false;
			data.savethr = new std::thread([&] {
				if (data.c) {
					data.m.lock();
					std::string temporary = data.config_raw_path;
					Tools::interpretPath(temporary);
					Tools::createPath(temporary);
					al_save_config_file(temporary.c_str(), data.c);
					data.m.unlock();
				}
				data.savethrdone = true;
			});
			
		}
		

		void Database::set(const std::string e, const std::string v)
		{
			if (!data.c) throw Abort::abort(__FUNCSIG__, "Cannot set \"" + e + "\" as \"" + v + "\".");
			al_set_config_value(data.c, Constants::conf_string_default_txt.c_str(), e.c_str(), v.c_str());
		}
		void Database::set(const Constants::io__conf_boolean e, const bool v)
		{
			set(Constants::ro__conf_boolean_str[+e], Constants::ro__conf_truefalse_str[(int)v]);
		}
		void Database::set(const Constants::io__conf_double e, const double v)
		{
			set(Constants::ro__conf_double_str[+e], std::to_string(v));
		}
		void Database::set(const Constants::io__conf_integer e, const int v)
		{
			set(Constants::ro__conf_integer_str[+e], std::to_string(v));
		}
		void Database::set(const Constants::io__conf_longlong e, const long long v)
		{
			set(Constants::ro__conf_longlong_str[+e], std::to_string(v));
		}
		void Database::set(const Constants::io__conf_string e, const std::string v)
		{
			set(Constants::ro__conf_string_str[+e], v);
		}
		void Database::set(const Constants::io__conf_color e, const ALLEGRO_COLOR clr)
		{
			if (e == Constants::io__conf_color::LAST_COLOR_TRANSLATE) {
				data.color = clr;
				data.color.a = 1.0;
				char expurt[32];
				sprintf_s(expurt, "{%.4f;%.4f;%.4f}", clr.r, clr.g, clr.b);
				set(Constants::ro__conf_color_str[+e], expurt);
			}
		}
		void Database::set(const Constants::io__db_boolean e, const bool v)
		{
			if (e != Constants::io__db_boolean::size) {
				data.b[+e] = v;

				switch (e) {
				case Constants::io__db_boolean::SAVING_STRING_INPUT:
					if (!data.b[+e]) {
						data.curr_string.clear();
						data.last_string.clear();
						data.real_string_size = 0;
						data.real_string_size_delayed = 0;
						data.curr_string_keylen.clear();
					}
					break;
				}
			}
		}
		void Database::set(const Constants::io__db_sizet e, const size_t v)
		{
			if (e != Constants::io__db_sizet::size) data.sz[+e] = v;
		}
		void Database::set(const int al_keycod, const bool v)
		{
			if (al_keycod >= 0 && al_keycod < ALLEGRO_KEY_MAX) {
				data.keys[al_keycod] = v;
				if (v && data.func_kb[al_keycod]) {
					data.func_kb[al_keycod]();
				}
				if (!v && data.func_kb_u[al_keycod]) {
					data.func_kb_u[al_keycod]();
				}
			}
		}
		void Database::set(const Constants::ro__db_mouse_boolean e, const bool v)
		{
			if (+e < +Constants::ro__db_mouse_boolean::size) {
				data.mouse[+e] = v;
				if (v && data.func_mb[+e]) {
					data.func_mb[+e]();
				}
				if (!v && data.func_mb_u[+e]) {
					data.func_mb_u[+e]();
				}
			}
		}
		void Database::set(const Constants::ro__db_mouse_double e, const double v)
		{
			data.db_mouse_axes[+e] = +v;
		}

		void Database::set(const Constants::ro__db_statistics_sizet e, const size_t v)
		{
			if (e != Constants::ro__db_statistics_sizet::size) data.db_statistics_sizet[+e] = v;
		}

		void Database::set(const Constants::ro__db_statistics_double e, const double v)
		{
			if (e != Constants::ro__db_statistics_double::size) data.db_statistics_double[+e] = v;
		}

		void Database::set(const Constants::ro__db_thread_string e, const char v)
		{
			if (isEq(Constants::io__db_boolean::SAVING_STRING_INPUT, false)) {
				data.curr_string.clear();
				data.real_string_size = 0;
				data.real_string_size_delayed = 0;
				return;
			}

			switch (e) {
			case Constants::ro__db_thread_string::KEY_ADD:
				if (data.real_string_size < (data.sz[+Constants::io__db_sizet::MAXIMUM_STRING_INPUT_LEN] ? data.sz[+Constants::io__db_sizet::MAXIMUM_STRING_INPUT_LEN] : std::string::npos)) {
					data.curr_string += v;
					if (data.curr_string_keylen_val <= ++data.real_string_size_delayed) {
						data.real_string_size++;
						data.real_string_size_delayed = 0;
					}
					data.curr_string_keylen.push_back(data.curr_string_keylen_val);
				}
				break;
			case Constants::ro__db_thread_string::KEY_ERASE:
				if (data.real_string_size > 0) {
					char siz = data.curr_string_keylen.back();
					for (char p = 0; p < siz; p++) { // multibyte
						data.curr_string.pop_back();
						data.curr_string_keylen.pop_back();
					}
					data.real_string_size--;
				}
				break;
			case Constants::ro__db_thread_string::KEY_SET:
				data.last_string = data.curr_string;
				data.curr_string.clear();
				data.real_string_size = 0;
				data.real_string_size_delayed = 0;
				data.curr_string_keylen.clear();
				data.curr_string_keylen_val = 1;
				break;
			case Constants::ro__db_thread_string::KEY_ADD_SET_LEN:
				data.curr_string_keylen_val = v;
				break;
			}
		}

		void Database::set(const Constants::io__db_functional_opt u, const int p, const std::function<void(void)> f)
		{
			switch (u) {
			case Constants::io__db_functional_opt::MOUSE_KEY:
				if (p != +Constants::ro__db_mouse_boolean::size && p > 0) data.func_mb[p] = f;
				break;
			case Constants::io__db_functional_opt::KEYBOARD_KEY:
				if (p < ALLEGRO_KEY_MAX && p > 0) {
					data.func_kb[p] = f;					
				}
				break;
			case Constants::io__db_functional_opt::MOUSE_LEFT:
				if (p != +Constants::ro__db_mouse_boolean::size && p > 0) data.func_mb_u[p] = f;
				break;
			case Constants::io__db_functional_opt::KEYBOARD_LEFT:
				if (p < ALLEGRO_KEY_MAX && p > 0) {
					data.func_kb_u[p] = f;
				}
				break;
			}
		}

		bool Database::check(const Constants::io__conf_boolean e)
		{
			bool t;
			return get(e, t);
		}

		bool Database::check(const Constants::io__conf_double e)
		{
			double t;
			return get(e, t);
		}

		bool Database::check(const Constants::io__conf_integer e)
		{
			int t;
			return get(e, t);
		}

		bool Database::check(const Constants::io__conf_longlong e)
		{
			long long t;
			return get(e, t);
		}

		bool Database::check(const Constants::io__conf_string e)
		{
			std::string t;
			return get(e, t);
		}

		bool Database::check(const Constants::io__conf_color e)
		{
			ALLEGRO_COLOR t;
			return get(e, t);
		}

		bool Database::get(const std::string e, std::string& v)
		{
			if (!data.c) throw Abort::abort(__FUNCSIG__, "Cannot get \"" + e + "\" as \"" + v + "\".");
			const char* chh = al_get_config_value(data.c, Constants::conf_string_default_txt.c_str(), e.c_str());
			if (!chh) return false;
			v = chh;
			return true;
		}
		bool Database::get(const Constants::io__conf_boolean e, bool& v)
		{
			std::string output;
			bool b = get(Constants::ro__conf_boolean_str[+e], output);
			v = (output == Constants::ro__conf_truefalse_str[1 /*true*/]);
			return b;
		}
		bool Database::get(const Constants::io__conf_double e, double& v)
		{
			std::string output;
			bool b = get(Constants::ro__conf_double_str[+e], output);
			v = 0.0;
			size_t div = 0;
			bool nowdiv = false;
			for (auto& i : output) {
				if (i >= '0' && i <= '9') {
					v *= 10.0;
					v += i - '0';
					if (nowdiv) div++;
				}
				else {
					nowdiv = true;
				}
			}
			v /= (pow(10.0, div));
			return b;
		}
		bool Database::get(const Constants::io__conf_integer e, int& v)
		{
			std::string output;
			bool b = get(Constants::ro__conf_integer_str[+e], output);
			v = atoi(output.c_str());
			return b;
		}
		bool Database::get(const Constants::io__conf_longlong e, long long& v)
		{
			std::string output;
			bool b = get(Constants::ro__conf_longlong_str[+e], output);
			v = atoll(output.c_str());
			return b;
		}
		bool Database::get(const Constants::io__conf_string e, std::string& v)
		{
			std::string output;
			bool b = get(Constants::ro__conf_string_str[+e], output);
			v = output;
			return b;
		}

		bool Database::get(const Constants::io__conf_color e, ALLEGRO_COLOR& v)
		{
			if (e == Constants::io__conf_color::LAST_COLOR_TRANSLATE) {
				v = data.color;
				return true;
			}
			return false;
		}

		bool Database::get(const Constants::io__db_boolean e, bool& v)
		{
			if (e != Constants::io__db_boolean::size) {
				v = data.b[+e];
				return true;
			}
			return false;
		}

		bool Database::get(const Constants::io__db_sizet e, size_t& v)
		{
			if (e != Constants::io__db_sizet::size) {
				v = data.sz[+e];
				return true;
			}
			return false;
		}

		bool Database::get(const Constants::ro__db_string e, std::string& v)
		{
			switch (e) {
			case Constants::ro__db_string::CURRENT_STRING:
				v = data.curr_string;
				return true;
			case Constants::ro__db_string::LAST_STRING:
				v = data.last_string;
				return true;
			}
			return false;
		}

		bool Database::get(const int al_keycod, bool& v, const bool defaul)
		{
			if (al_keycod >= 0 && al_keycod < ALLEGRO_KEY_MAX) {
				v = data.keys[al_keycod];
				return true;
			}
			return false;
		}
		bool Database::get(const Constants::ro__db_mouse_boolean e, bool& v)
		{
			bool good = false;
			if (e != Constants::ro__db_mouse_boolean::size) {
				v = data.mouse[+e];
				good = true;
			}
			if (e == Constants::ro__db_mouse_boolean::IS_ANY_PRESSED) {
				v = false;
				for (auto& i : data.mouse) v |= i;
				good = true;
			}
			return good;
		}
		bool Database::get(const Constants::ro__db_mouse_double e, double& v)
		{
			if (e != Constants::ro__db_mouse_double::size) {
				v = +data.db_mouse_axes[+e];
				return true;
			}
			return false;
		}

		bool Database::get(const Constants::ro__db_statistics_sizet e, size_t& v)
		{
			if (e != Constants::ro__db_statistics_sizet::size) {
				v = data.db_statistics_sizet[+e];
				return true;
			}
			return false;
		}

		bool Database::get(const Constants::ro__db_statistics_double e, double& v)
		{
			if (e != Constants::ro__db_statistics_double::size) {
				v = data.db_statistics_double[+e];
				return true;
			}
			return false;
		}


		bool Database::isEq(const std::string e, const std::string v)
		{
			std::string oth;
			get(e, oth); // meh
			return oth == v;
		}
		bool Database::isEq(const Constants::io__conf_boolean e, const bool v)
		{
			bool oth;
			get(e, oth); // meh
			return oth == v;
		}
		bool Database::isEq(const Constants::io__conf_double e, const double v)
		{
			double oth;
			get(e, oth); // meh
			return oth == v;
		}
		bool Database::isEq(const Constants::io__conf_integer e, const int v)
		{
			int oth;
			get(e, oth); // meh
			return oth == v;
		}
		bool Database::isEq(const Constants::io__conf_longlong e, const long long v)
		{
			long long oth;
			get(e, oth); // meh
			return oth == v;
		}
		bool Database::isEq(const Constants::io__conf_string e, const std::string v)
		{
			std::string oth;
			get(e, oth); // meh
			return oth == v;
		}

		bool Database::isEq(const Constants::io__conf_color e, const ALLEGRO_COLOR v)
		{
			ALLEGRO_COLOR oth;
			get(e, oth); // meh
			return (oth.r == v.r) && (oth.g == v.g) && (oth.b == v.b) && (oth.a == v.a);
		}

		bool Database::isEq(const Constants::io__db_boolean e, const bool v)
		{
			bool oth;
			get(e, oth); // meh
			return oth == v;
		}

		bool Database::isEq(const Constants::io__db_sizet e, const size_t v)
		{
			size_t oth;
			get(e, oth); // meh
			return oth == v;
		}

		bool Database::isEq(const Constants::ro__db_string e, const std::string v)
		{
			std::string oth;
			get(e, oth); // meh
			return oth == v;
		}

		bool Database::isEq(const int e, const bool v)
		{
			bool oth;
			get(e, oth, v); // meh
			return oth == v;
		}

		bool Database::isEq(const Constants::ro__db_mouse_boolean e, const bool v)
		{
			bool oth;
			get(e, oth); // meh
			return oth == v;
		}

		bool Database::isEq(const Constants::ro__db_mouse_double e, const double v)
		{
			double oth;
			get(e, oth); // meh
			return oth == v;
		}

		bool Database::isEq(const Constants::ro__db_statistics_sizet e, const size_t v)
		{
			size_t oth;
			get(e, oth);
			return oth == v;
		}

		bool Database::isEq(const Constants::ro__db_statistics_double e, const double v)
		{
			double oth;
			get(e, oth);
			return oth == v;
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

			if (hz <= 0) {
				hz = 0;
				for (auto& i : u) {
					bool higher = ((i.x > tx&& i.y >= ty) || (i.x >= tx && i.y > ty));
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
					logg << L::SLF << fsr(__FUNCSIG__) << "Created display successfully: " << x << "x" << y << "@" << (hz == 0 ? "auto" : std::to_string(hz)) << ";flags=" << flag << L::ELF;
				}
				else {
					throw Abort::abort(__FUNCSIG__, "Could not create display!");
				}
				al_convert_bitmaps();
				al_hide_mouse_cursor(d);

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
			auto u = al_get_target_bitmap();
			std::string path;
			Database db;
			db.get(Constants::io__conf_string::PRINT_PATH, path);
			Tools::interpretPath(path);
			Tools::createPath(path);

			auto rt = time(NULL);
			tm tt;
			auto v = localtime_s(&tt, &rt);

			if (v != 0 || path.length() == 0) throw Abort::abort(__FUNCSIG__, "Can't set a proper name to save the screenshot!");

			path = path + std::to_string(tt.tm_year + 1900) + "_" + std::to_string(tt.tm_mon + 1) + "_" + std::to_string(tt.tm_mday) + "-" + std::to_string(tt.tm_hour) + "_" + std::to_string(tt.tm_min) + "_" + std::to_string(tt.tm_sec) + "-" + std::to_string((int)(GetTickCount64() % 1000)) + ".jpg";

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

		void Display::__check_acknowledge_n_buf()
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__) << "Acknowledge and updated internally buffer and screen." << L::ELF;

			al_acknowledge_resize(d);

			Database db;

			db.get(Constants::io__conf_double::FX_AMOUNT, quick_fx);

			if (db.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, true)) {

				double prop = 1.0;
				db.get(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION, prop);

				ALLEGRO_BITMAP* newbmp = al_create_bitmap(al_get_display_width(d) * prop, al_get_display_height(d) * prop);
				al_set_target_bitmap(newbmp);
				if (buf_) {
					al_draw_scaled_bitmap(buf_, 0, 0, al_get_bitmap_width(buf_), al_get_bitmap_height(buf_), 0, 0, al_get_bitmap_width(newbmp), al_get_bitmap_height(newbmp), 0);
					al_destroy_bitmap(buf_);
				}
				buf_ = newbmp;

				logg << L::SLF << fsr(__FUNCSIG__) << "Double buffering is now " << (buf_ ? "ENABLED SUCCESSFULLY" : "NOT ENABLED, FAILED") << L::ELF;
			}
			else if (buf_) {
				al_destroy_bitmap(buf_);
				buf_ = nullptr;

				logg << L::SLF << fsr(__FUNCSIG__) << "Double buffering disabled" << L::ELF;
			}
		}

		Display::Display() // automatic from config
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__) << "Creating new display (blank)" << L::ELF;

			int flag = 0;

			Constants::display_mode md;

			Database config;
			config.get(Constants::io__conf_integer::SCREEN_X, md.x);
			config.get(Constants::io__conf_integer::SCREEN_Y, md.y);
			config.get(Constants::io__conf_integer::SCREEN_PREF_HZ, md.hz);
			config.get(Constants::io__conf_integer::SCREEN_FLAGS, flag);
			flag |= Constants::start_display_obligatory_flag_mode; // obligatory flags like OpenGL and Resizable

			__g_sys.setNewDisplayMode(flag);
			auto u = __g_sys.getAvailableResolutions();


			if (md.x > 16 && md.y > 9) {
				logg << L::SLF << fsr(__FUNCSIG__) << "Found config from config file." << (md.hz > 0 ? " Got refresh rate set!" : "Getting automatic refresh rate information...") << L::ELF;

				if (md.hz == 0) {
					for (auto& i : u) {
						if ((i.x == md.x && i.y == md.y)) {
							if (i.hz > md.hz) {
								md.hz = i.hz;
								config.set(Constants::io__conf_integer::SCREEN_PREF_HZ, md.hz);
							}
						}
					}
				}
			}
			else if (Constants::start_force_720p)
			{
				logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "No config file found  or invalid one. 720P is set to default, so, searching available refresh rates..." << L::ELF;
				md.x = 1280;
				md.y = 720;
				md.hz = 0;
				config.set(Constants::io__conf_integer::SCREEN_X, md.x);
				config.set(Constants::io__conf_integer::SCREEN_Y, md.y);
				for (auto& i : u) {
					if ((i.x == md.x && i.y == md.y)) {
						if (i.hz > md.hz) {
							md.hz = i.hz;
							config.set(Constants::io__conf_integer::SCREEN_PREF_HZ, md.hz);
						}
					}
				}
			}
			if (md.hz == 0) { // both before failed, search for another option afaik
				logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Failed to find a exact value for refresh rate. Trying automatic instead." << L::ELF;
				md.hz = 0;
			}


			_init(md.x, md.y, -1, md.hz);

			if (d) config.flush();

			should_check_acknowledge_and_prop_buf = al_get_time() + 0.5;
		}

		Display::Display(const int x, const int y, const int flag, int hz)
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__) << "Creating new custom display [" << x << "x" << y << "@" << hz << " w/ " << flag << "]" << L::ELF;
			_init(x, y, flag, hz);

			should_check_acknowledge_and_prop_buf = al_get_time() + 0.5;
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
			if (buf_) {
				al_set_target_backbuffer(d);

				ALLEGRO_TRANSFORM defaul;
				al_identity_transform(&defaul);
				al_use_transform(&defaul);

				if (quick_fx == 0.0) {
					al_clear_to_color(al_map_rgb(0, 0, 0));
					al_draw_scaled_bitmap(buf_, 0, 0, al_get_bitmap_width(buf_), al_get_bitmap_height(buf_), 0, 0, al_get_display_width(d), al_get_display_height(d), 0);
				}
				else {
					double conv_quick_fx = pow(quick_fx, 2.5);
					double conv_quick_fx2 = pow(quick_fx, 0.8);
					double offsets[3];
					double scl = 10.0 * sqrt(al_get_bitmap_width(buf_) * al_get_bitmap_height(buf_)) / sqrt(Constants::conf_default_screen_siz[0] * Constants::conf_default_screen_siz[1]);
					offsets[0] = cos(al_get_time() * 0.40)       * scl * conv_quick_fx;
					offsets[1] = sin(al_get_time() * 0.32)       * scl * conv_quick_fx;
					offsets[2] = cos(al_get_time() * 0.29 + 0.5) * scl * conv_quick_fx;

					al_draw_filled_rectangle(0, 0, al_get_display_width(d), al_get_display_height(d), al_map_rgba_f(0.0, 0.0, 0.0, 1.0 - 0.952 * conv_quick_fx2));

					al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);

					double qfbl = 1.0 - 0.95 * conv_quick_fx2;

					al_draw_tinted_scaled_bitmap(buf_, al_map_rgba_f(qfbl, 0.000,0.000,qfbl ), 0, 0, al_get_bitmap_width(buf_), al_get_bitmap_height(buf_), offsets[0], offsets[1], al_get_display_width(d) + offsets[1], al_get_display_height(d) + offsets[2], 0);
					al_draw_tinted_scaled_bitmap(buf_, al_map_rgba_f(0.000,qfbl, 0.000,qfbl ), 0, 0, al_get_bitmap_width(buf_), al_get_bitmap_height(buf_), offsets[1], offsets[2], al_get_display_width(d) + offsets[2], al_get_display_height(d) + offsets[0], 0);
					al_draw_tinted_scaled_bitmap(buf_, al_map_rgba_f(0.000,0.000,qfbl, qfbl ), 0, 0, al_get_bitmap_width(buf_), al_get_bitmap_height(buf_), offsets[2], offsets[0], al_get_display_width(d) + offsets[0], al_get_display_height(d) + offsets[1], 0);
					al_draw_tinted_scaled_bitmap(buf_, al_map_rgba_f(0.015,0.015,0.015,0.015), 0, 0, al_get_bitmap_width(buf_), al_get_bitmap_height(buf_), 0, 0, al_get_display_width(d), al_get_display_height(d), 0);

					al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
				}

				// COMING
				/*quick_fx*/
			}

			if (printing) _print();

			al_flip_display();

			if (should_check_acknowledge_and_prop_buf != 0.0 && al_get_time() - should_check_acknowledge_and_prop_buf > 0.0) {
				__check_acknowledge_n_buf();
				should_check_acknowledge_and_prop_buf = 0.0;
			}

			if (buf_) al_set_target_bitmap(buf_);
			else al_set_target_backbuffer(d);
		}
		void Display::clearTo(ALLEGRO_COLOR v)
		{
			al_clear_to_color(v);
		}
		void Display::close()
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__) << "Deleting display..." << L::ELF;

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
		void Display::setToCheckBufferResConfig()
		{
			should_check_acknowledge_and_prop_buf = al_get_time() + 1.0;
		}
		double Display::getAppliedProportion()
		{
			if (buf_) {
				return al_get_bitmap_width(buf_) * 1.0 / al_get_display_width(d);
			}
			else return 1.0;
		}
		void Display::print()
		{
			printing = true;
		}
		ALLEGRO_DISPLAY*& Display::getDisplay()
		{
			return d;
		}

		void Display::acknowledgeResize()
		{
			if (d) {
				Database db;

				double prop = 1.0;
				db.get(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION, prop);
				if (prop < 0.3) prop = 0.3;
				if (prop > 2.0) prop = 2.0;
				db.set(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION, prop);
				db.set(Constants::io__conf_boolean::DOUBLEBUFFERING, prop != 1.0 || db.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, true));

				should_check_acknowledge_and_prop_buf = al_get_time() + 0.5;
			}
		}



		void forceExit(const char* windw, const char* title, const char* ext)
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__, windw ? E::ERRR : E::WARN) << "FORCEEXIT: " << (windw ? windw : "NULL") << ": " << (title ? title : "NULL") << "; " << (ext ? ext : "NULL") << L::ELF;
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
		void askForceExit(const char* windw, const char* title, const char* ext)
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "askEXIT: " << (windw ? windw : "NULL") << ": " << (title ? title : "NULL") << "; " << (ext ? ext : "NULL") << L::ELF;
			logg.flush();
			if (windw) {
				int e = al_show_native_message_box(
					nullptr,
					windw,
					title,
					((ext ? ext : "") + std::string("\n\nContinue anyway? (NO = force exit the game)")).c_str(),
					NULL,
					ALLEGRO_MESSAGEBOX_YES_NO);
				if (e == 2) { // NO
					exit(EXIT_FAILURE);
				}
			}
		}
		void alert(const char* windw, const char* title, const char* ext)
		{
			gfile logg;
			logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "ALERT: " << (windw ? windw : "NULL") << ": " << (title ? title : "NULL") << "; " << (ext ? ext : "NULL") << L::ELF;
			logg.flush();
			if (windw) {
				al_show_native_message_box(
					nullptr,
					windw,
					title,
					ext,
					NULL,
					ALLEGRO_MESSAGEBOX_WARN);
			}
		}
	}
}