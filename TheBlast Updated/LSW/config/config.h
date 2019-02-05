#pragma once

#include "..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Config {

			enum conf_b{HAD_ERROR,WAS_OSD_ON,WAS_FULLSCREEN};
			enum conf_f{LAST_VOLUME};
			enum conf_i{SCREEN_X,SCREEN_Y,KEY_FULLSCREEN,KEY_OSD};
			enum conf_ll{_TIMES_LIT};
			enum conf_s{LAST_VERSION,LAST_PLAYERNAME,LAST_COLOR};

			const Safer::safe_string conf_b_str[] = { "had_error", "was_osd_on", "fullscreen" };
			const Safer::safe_string conf_f_str[] = { "last_volume" };
			const Safer::safe_string conf_i_str[] = { "screen_width","screen_height","key_fullscreen","key_osd" };
			const Safer::safe_string conf_ll_str[] = { "times_open" };
			const Safer::safe_string conf_s_str[] = { "last_version","playername","playercolor" };

			// tool
			const Safer::safe_string bool_s[] = { "false", "true" };
			
			class config {
				static ALLEGRO_CONFIG* c;
				static std::mutex m;
				Log::gfile logg;
			public:
				config();
				~config();

				void set(const Safer::safe_string, const Safer::safe_string);
				void set(const conf_b, const bool);
				void set(const conf_f, const float);
				void set(const conf_i, const int);
				void set(const conf_ll, const long long);
				void set(const conf_s, const Safer::safe_string);

				void get(const Safer::safe_string, Safer::safe_string&, const Safer::safe_string);
				void get(const conf_b, bool&, const bool);
				void get(const conf_f, float&, const float);
				void get(const conf_i, int&, const int);
				void get(const conf_ll, long long&, const long long);
				void get(const conf_s, Safer::safe_string&, const Safer::safe_string);

				const bool isEq(const Safer::safe_string, const Safer::safe_string);
				const bool isEq(const conf_b, const bool);
				const bool isEq(const conf_f, const float);
				const bool isEq(const conf_i, const int);
				const bool isEq(const conf_ll, const long long);
				const bool isEq(const conf_s, const Safer::safe_string);
			};
		}
	}
}