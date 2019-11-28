#include "system.h"

namespace LSW {
	namespace v4 {
		std::vector<std::shared_ptr<__raw_image>> Textures::imgs; // static

		void __systematic::__extract_package(float* perc)
		{
			if (perc) *perc = 0.00;

			FILE* me, * fi;
			std::string work = Constants::start_zip_default_extract_path;
			char myself[1024];
			GetModuleFileNameA(NULL, myself, 1024);

			Tools::interpret_path(work);
			Tools::createPath(work);

			auto err0 = fopen_s(&me, myself, "rb");
			auto err1 = fopen_s(&fi, work.c_str(), "wb");

			__ensure_warn_package();

			LONGLONG totalsiz = Tools::getFileSize(myself) - 2; // adjust
			LONGLONG nowstep = 0;

			if (err0 || err1) {
				throw Abort::abort("fopen_s", "::lsw_extract_merge", "Cannot proceed with temp file to zip");
				if (me) fclose(me);
				if (fi) fclose(fi);
				return;
			}

			size_t p = 0;
			char ch[1];
			bool still = true;
			bool found = false;

			while (still)
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
					throw Abort::abort("totalsiz", "Tools::getFileSize", "File size isn't right!");
					return;
				}
			}

			if (found) {
				char buf[512];
				for (size_t s = 0;;) {
					s = fread_s(buf, sizeof(buf), sizeof(char), 512, me);
					fwrite(buf, sizeof(char), s, fi);
					nowstep += s;

					if (perc) *perc = (1.0 * nowstep / totalsiz);
					if (s != 512) break;
				}
			}
			else {
				throw Abort::abort("null", "::lsw_extract_merge", "data pack not available inside .exe file");
			}

			if (me) fclose(me);
			if (fi) fclose(fi);

			if (perc) *perc = 1.0;
			extracted_zip_at = work;
		}

		void __systematic::__ensure_warn_package()
		{
			std::string work = Constants::start_zip_warning_file_txt;

			Tools::interpret_path(work);
			Tools::createPath(work);

			FILE* fp = nullptr;
			auto err = fopen_s(&fp, work.c_str(), "wb");

			if (err == 0) {
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

			if (!PHYSFS_addToSearchPath(extracted_zip_at.c_str(), 1)) throw Abort::abort("PHYSFS_addToSearchPath", "system::__loadPackage", "Can't add datapack to internal search");
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

			if (!PHYSFS_init(myself))
				throw Abort::abort("PHYSFS_init", "system::system", "Failed to set the datapack to run");

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

		void __raw_display::_init(const int x, const int y, const int flag, int hz)
		{
			if (d) {
				al_destroy_display(d);
				d = nullptr;
			}

			if (flag > 0) __g_sys.__set_new_display_mode(flag);

			auto u = __g_sys.__get_available_res();

			if (hz == 0) {
				for (auto& i : u) {
					if (i.hz > hz) hz = i.hz;
				}
			}

			if (__g_sys.__check_resolution_existance(x, y, hz) || (flag & ALLEGRO_WINDOWED)) {
				al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
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

		__raw_display::__raw_display()
		{
			int flag = Constants::start_display_default_mode;
			__g_sys.__set_new_display_mode(flag);
			auto u = __g_sys.__get_available_res();

			Assistance::__submode md;

			for (auto& i : u) {
				if ((i.x >= md.x || i.y >= md.y)) {
					if (i.hz > md.hz) {
						md.hz = i.hz;
					}
					md.x = i.x;
					md.y = i.y;
				}
			}

			_init(md.x, md.y, -1, md.hz);
		}

		__raw_display::__raw_display(const int x, const int y, const int flag, int hz)
		{
			_init(x, y, flag, hz);
		}
		__raw_display::~__raw_display()
		{
			close();
		}
		void __raw_display::restart()
		{
			close();
			_init(this->x, this->y, this->f, this->h);
		}
		void __raw_display::flip()
		{
			al_set_target_backbuffer(d);
			al_flip_display();
		}
		void __raw_display::clear_to(const ALLEGRO_COLOR v)
		{
			al_clear_to_color(v);
		}
		void __raw_display::close()
		{
			if (d) {
				al_destroy_display(d);
				d = nullptr;
			}
		}
		bool __raw_display::exist()
		{
			return (d);
		}
		__raw_image::~__raw_image()
		{
			if (bmp && al_is_system_installed()) {
				al_destroy_bitmap(bmp);
				bmp = nullptr;
				path.clear();
				id.clear();
			}
		}
		void Textures::load(const std::string str, const std::string path)
		{
			std::weak_ptr<__raw_image> i;

			if (!get(str, i)) {
				std::shared_ptr<__raw_image> j(new __raw_image());

				j->path = path;
				j->id = str;
				if (!(j->bmp = al_load_bitmap(path.c_str()))) throw Abort::abort("al_load_bitmap", "images::load", "Couldn't load '" + str + "' @ path '" + path + "'");
				imgs.push_back(j);
			}
		}
		bool Textures::get(const std::string str, std::weak_ptr<__raw_image>& ime)
		{
			for (auto& i : imgs) {
				if (str == i->id) {
					ime = i;
					return true;
				}
			}
			//throw Abort::abort("null", "images::get", "Couldn't find '" + str + "'");
			return false;
		}
		void Textures::del(const std::string str)
		{
			for (size_t p = 0; p < imgs.size(); p++)
			{
				if (imgs[p]->id == str) {
					imgs.erase(imgs.begin() + p);
					return;
				}
			}
		}




		void lsw_init()
		{
			try {
				__g_sys.init_system();
			}
			catch (Abort::abort a) {
				al_show_native_message_box(
					nullptr,
					"Something went wrong!",
					"Please report the following:",
					std::string("Function gone wrong: " + a.function() + "\n\nFrom what exactly: " + a.from() + "\n\nExtended explanation: " + a.details()).c_str(),
					NULL,
					ALLEGRO_MESSAGEBOX_ERROR
				);
				exit(EXIT_FAILURE);
			}
		}
	}
}