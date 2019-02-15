#include "..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Config {

			ALLEGRO_CONFIG* config::c = nullptr;
			std::mutex config::m;
			unsigned config::counter = 0;
			
			config::config()
			{
				m.lock();
				if (counter++ == 0) {
					logg << Log::START << Log::_func("config", "config") << "Registered spawn of config #" + std::to_string((size_t)this) << " (handler)" << Log::ENDL;
					Safer::safe_string temporary = Defaults::Config::config_file;
					Tools::interpret_path(temporary);
					Tools::createPath(temporary);

					logg << Log::NEEDED_START << Log::_func("config", "config") << "Registered loading of Config file (global)" << Log::NEEDED_ENDL;
					if (!(c = al_load_config_file(temporary.g().c_str())))
					{
						c = al_create_config();
						if (!c) throw "at config::config [#" + std::to_string((size_t)this) + "]: Cannot load/create config file!";
						logg << Log::NEEDED_START << Log::_func("config", "config") << "Is this the first time you're opening this game? Registered first Config load." << Log::NEEDED_ENDL;

						// set or load first values
						set(Config::HAD_ERROR, true); // then texture download starts			// USED
						set(Config::WAS_OSD_ON, false);											// USED
						set(Config::WAS_FULLSCREEN, true);										// USED
						set(Config::LAST_VOLUME, 0.5);
						set(Config::LAST_VERSION, Defaults::version_app);						// nah
						set(Config::LAST_PLAYERNAME, "Player");
						set(Config::LAST_COLOR, "GREEN");
						set(Config::SCREEN_X, Defaults::Config::screen_siz[0]);
						set(Config::SCREEN_Y, Defaults::Config::screen_siz[1]);
						set(Config::_TIMES_LIT, 0LL);
						set(Config::WAS_FX_ON, true);

						if (!al_save_config_file(temporary.g().c_str(), c)) {
							throw "at config::config [#" + std::to_string((size_t)this) + "]: Cannot save config file!";
						}
					}
					else {
						long long ll;
						get(Config::_TIMES_LIT, ll, 0LL);
						ll++;
						set(Config::_TIMES_LIT, ll);
					}
				}
				else if (!c) {
					throw "at config::config [#" + std::to_string((size_t)this) + "]: Unexpected situation! Why isn't config loaded if there's more than one config object?";
				}
				m.unlock();
			}
			config::~config()
			{
				if (--counter == 0) {
					logg << Log::START << Log::_func("config", "~config") << "Registered despawn of config #" + std::to_string((size_t)this) << " (handler)" << Log::ENDL;
					m.lock();
					Safer::safe_string temporary = Defaults::Config::config_file;
					Tools::interpret_path(temporary);
					Tools::createPath(temporary);
					al_save_config_file(temporary.g().c_str(), c);
					m.unlock();
				}
			}

			void config::set(const Safer::safe_string e, const Safer::safe_string v)
			{
				if (!c) throw "at config::set [#" + std::to_string((size_t)this) + "]: Cannot set \"" + e.g() + "\" as \"" + v.g() + "\".";
				al_set_config_value(c, Defaults::Config::strt_txt.g().c_str(), e.g().c_str(), v.g().c_str());
			}
			void config::set(const conf_b e, const bool v)
			{
				set(conf_b_str[e].g(), bool_s[(int)v]);
			}
			void config::set(const conf_f e, const float v)
			{
				set(conf_f_str[e].g(), std::to_string(v));
			}
			void config::set(const conf_i e, const int v)
			{
				set(conf_i_str[e].g(), std::to_string(v));
			}
			void config::set(const conf_ll e, const long long v)
			{
				set(conf_ll_str[e].g(), std::to_string(v));
			}
			void config::set(const conf_s e, const Safer::safe_string v)
			{
				set(conf_s_str[e].g(), v);
			}

			void config::get(const Safer::safe_string e, Safer::safe_string& v, const Safer::safe_string defaul)
			{
				if (!c) throw "at config::get [#" + std::to_string((size_t)this) + "]: Cannot get \"" + e.g() + "\" as \"" + v.g() + "\".";
				const char* chh = al_get_config_value(c, Defaults::Config::strt_txt.g().c_str(), e.g().c_str());
				if (!chh) {
					logg << Log::NEEDED_START << Log::_func("config", "get", Log::WARN) << "There was no value to " << e << ", so the default value was set (" << defaul << ")." << Log::NEEDED_ENDL;
					set(e, defaul);
					v = defaul;
					return;
				}
				v = chh;
			}
			void config::get(const conf_b e, bool& v, const bool defaul)
			{
				Safer::safe_string output;
				get(conf_b_str[e], output, bool_s[(int)defaul]);
				v = (output == bool_s[1 /*true*/]);
			}
			void config::get(const conf_f e, float& v, const float defaul)
			{
				Safer::safe_string output;
				get(conf_f_str[e], output, std::to_string(defaul));
				v = atof(output.g().c_str());
			}
			void config::get(const conf_i e, int& v, const int defaul)
			{
				Safer::safe_string output;
				get(conf_i_str[e], output, std::to_string(defaul));
				v = atoi(output.g().c_str());
			}
			void config::get(const conf_ll e, long long& v, const long long defaul)
			{
				Safer::safe_string output;
				get(conf_ll_str[e], output, std::to_string(defaul));
				v = atoll(output.g().c_str());
			}
			void config::get(const conf_s e, Safer::safe_string& v, const Safer::safe_string defaul)
			{
				Safer::safe_string output;
				get(conf_s_str[e], output, defaul);
				v = output;
			}


			const bool config::isEq(const Safer::safe_string e, const Safer::safe_string v)
			{
				Safer::safe_string oth;
				get(e, oth, v); // meh
				return oth == v;
			}
			const bool config::isEq(const conf_b e, const bool v)
			{
				bool oth;
				get(e, oth, v); // meh
				return oth == v;
			}
			const bool config::isEq(const conf_f e, const float v)
			{
				float oth;
				get(e, oth, v); // meh
				return oth == v;
			}
			const bool config::isEq(const conf_i e, const int v)
			{
				int oth;
				get(e, oth, v); // meh
				return oth == v;
			}
			const bool config::isEq(const conf_ll e, const long long v)
			{
				long long oth;
				get(e, oth, v); // meh
				return oth == v;
			}
			const bool config::isEq(const conf_s e, const Safer::safe_string v)
			{
				Safer::safe_string oth;
				get(e, oth, v); // meh
				return oth == v;
			}
		}
	}
}