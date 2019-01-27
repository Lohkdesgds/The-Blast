#pragma once

#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Image {

			enum _img_mode {
				CREATED,
				CREATED_PAINTED,
				LOADED
			};

			enum vals_string{RAW_PATH,PATH,ID};
			enum vals_int{CREATE_X,CREATE_Y};
			enum vals_bool{HAS_LOADED,HAS_RELOADED, GLOBAL_SET_NO_OPTIMIZING_SETTING,FORCE_ON_MEMORY,USE_COLOR,LOAD_LATER};
			enum vals_double{GLOBAL_OPTIMIZATION_TIMING};
			enum vals_color{COLOR};
			enum vals_bmpp{BMP};
			enum vals_x_enum1{MODE};



			class image_low {
				// flag to all image_low, used with arg calling no optimization
				static bool global_no_optimization;
				static double global_timing_optimization;

				// easier
				Log::gfile logg;

				// its bitmap
				ALLEGRO_BITMAP* bmp = nullptr;
				bool is_loaded = false;

				// path(raw), ID and size for (re)load, identification and creation
				Safer::safe_string path_raw, path;
				Safer::safe_string id;
				int size_for_creation[2] = { 0,0 }; // -1 means screen size, so it should not start with this value
				int size[2] = { 0,0 }; // used internally
				_img_mode mode = CREATED; // mode, internal use normally (can be set manually, but not recommended
				ALLEGRO_COLOR color = al_map_rgba_f(0.0, 0.0, 0.0, 0.0); // color used to paint bitmap
				bool using_color = false;

				bool load_later = false;
				double lastcall = 0;
				bool optimized = false;
				bool created_itself = false;
				bool has_reloaded = false;

				bool no_optimization = false;
				std::mutex optimizing;


				//double proportion = 1.0; // when resized for low memory usage if not using or when drawing is half or so
				
				//Log::gfile logg;
				//int orig_siz[2] = { -1, -1 };

				//const bool _load_NOADJUST(const Safer::safe_string);
				void paint();
			public:
				image_low();
				~image_low();

				// --------------- HEAD FUNCTIONS --------------- //

				void set(const vals_string, const Safer::safe_string);
				void set(const vals_int, const int);
				void set(const vals_bool, const bool);
				void set(const vals_double, const double);
				void set(const vals_color, const ALLEGRO_COLOR);
				void set(const vals_bmpp, ALLEGRO_BITMAP* const);
				void set(const vals_x_enum1, const _img_mode);

				void get(const vals_string, Safer::safe_string&);
				void get(const vals_int, int&);
				void get(const vals_bool, bool&);
				void get(const vals_double, double&);
				void get(const vals_color, ALLEGRO_COLOR&);
				void get(const vals_bmpp, ALLEGRO_BITMAP*&);
				void get(const vals_x_enum1, _img_mode&);

				const bool isEq(const vals_string, const Safer::safe_string);
				const bool isEq(const vals_int, const int);
				const bool isEq(const vals_bool, const bool);
				const bool isEq(const vals_double, const double);
				const bool isEq(const vals_color, const ALLEGRO_COLOR);
				const bool isEq(const vals_bmpp, const ALLEGRO_BITMAP*);
				const bool isEq(const vals_x_enum1, const _img_mode);

				// --------------- ASSISTANCE FUNCTIONS --------------- //

				const bool load();
				const bool reload();
				void unload();
				
				void verify();
				void checkMemory();
			};

			size_t _find(const Safer::safe_string, Safer::safe_vector<image_low*>&, bool&);
			void _draw();


			void multipleLoad(const Safer::safe_string, Safer::safe_string, const size_t, const unsigned, Safer::safe_string, float* = nullptr, const bool = true);


			image_low* getOrCreate(const Safer::safe_string, const bool = false); // create?
			void easyRemove(const Safer::safe_string);
		}
	}
}