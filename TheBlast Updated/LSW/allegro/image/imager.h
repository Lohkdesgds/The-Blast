#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Image {

			enum _img_mode {
				CREATED,
				LOADED
			};

			class image_low {
				//static double lastimagereload;
				static bool keep_on_memory_always;

				ALLEGRO_BITMAP* bmp = nullptr;
				Safer::safe_string path;
				Safer::safe_string id;
				double lastcall = 0;
				//double lastproportion = 1.0;
				bool optimized = false;
				bool created_itself = false;
				double check_time = 0.0;
				bool has_Reloaded = false;

				bool noOptimization = false;
				std::mutex optimizing;

				ALLEGRO_COLOR lastpaint = al_map_rgba_f(0.0, 0.0, 0.0, 0.0);

				//double proportion = 1.0; // when resized for low memory usage if not using or when drawing is half or so
				_img_mode mode = CREATED;
				Log::gfile logg;
				int orig_siz[2] = { -1, -1 };

				const bool _load_NOADJUST(const Safer::safe_string);
			public:
				~image_low();

				const bool load(const Safer::safe_string);
				void setToLoad(const Safer::safe_string);

				const bool create(int, int);
				void setID(const Safer::safe_string);

				void paint(const ALLEGRO_COLOR);

				const bool amI(const Safer::safe_string);
				const bool amI(const _img_mode);
				const Safer::safe_string whoAmI();
				const bool isLoaded();

				void unload();

				const bool hasReloaded(const bool = false);
				const bool reload();

				void shouldOptimize(const bool);
				
				void verify();
				void checkMemory();
				
				void get(ALLEGRO_BITMAP*&);
				const bool checkExistance();

				void _setKeepOnMemory(const bool);
			};

			size_t _find(const Safer::safe_string, Safer::safe_vector<image_low*>&, bool&);
			void _draw();

			// its name, initial path, how many, how many 0's, end (format), perc, optimize those bitmaps?
			void multipleLoad(const Safer::safe_string, Safer::safe_string, const size_t, const unsigned, Safer::safe_string, float* = nullptr, const bool = true);
			// its name, initial path, how many, how many 0's, end (format), urls
			//void multipleCloudLoad(const Safer::safe_string, Safer::safe_string, const size_t, const unsigned, Safer::safe_string, const Safer::safe_string* = nullptr, float* = nullptr); // NULLPTR == from database on Defaults::initial_call_url

			image_low* getOrCreate(const Safer::safe_string, const bool = false); // create?
			void easyRemove(const Safer::safe_string);
		}
	}
}