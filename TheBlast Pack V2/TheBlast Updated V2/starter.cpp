#include <iostream>
#include <string>
#include <Windows.h>

#include "LSW/organizer/organizer.h"

using namespace LSW::v4;

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}


enum class main_gamemodes { LOADING = -1, MENU, OPTIONS, PAUSE, GAMING };

int main(int argc, const char* argv[])
{
	try {
		/*************************************************************************************

			# Start stuff:
			> Variables, some tests, initializing the basics

		**************************************************************************************/

		gfile logg;

		alert("BETA/ALPHA", "This is a beta or alpha version of the game.", "There might be some issues here and there, so be nice and report to @Lohkdesgds at Twitter, Gamejolt or GitHub! Thanks!");

		logg << L::SLF << fsr(__FUNCSIG__) << "App version: " << Constants::version_app << L::ELF;

		logg << L::SLF << fsr(__FUNCSIG__) << "Initializing game..." << L::ELF;

		lswInit();

		// raw allegro stuff
		Textures textures;
		Fonts fonts;
		Samples samples;

		// controllers
		Sprites sprites;
		Texts texts;
		Tracks tracks;

		// others
		Camera gcam;
		//camera_preset cp;

		Mixer mixer; // init audio mixer

		Console consol;
		Database conf;

		logg << L::SLF << fsr(__FUNCSIG__) << "Setting up template functions..." << L::ELF;

		/*************************************************************************************

			# Functions part:
			> This is the part where we tell how to create some stuff.
			> Yes, it is fully customizable!

		**************************************************************************************/


		textures.set(Constants::lambda_bitmap_load, Constants::lambda_bitmap_unload);
		fonts.set(Constants::lambda_font_load, Constants::lambda_font_unload);
		samples.set(Constants::lambda_sample_load, Constants::lambda_sample_unload);

		sprites.set(Constants::lambda_default_load<Sprite>, Constants::lambda_default_unload<Sprite>);
		texts.set(Constants::lambda_default_load<Text>, Constants::lambda_default_unload<Text>);
		tracks.set(Constants::lambda_default_load<Track>, Constants::lambda_default_unload<Track>);


		logg << L::SLF << fsr(__FUNCSIG__) << "Setting up load/unload function... (faster unload)" << L::ELF;

		consol.setUnloadWay([&textures, &sprites, &texts, &fonts]() { sprites.clear(); texts.clear(); fonts.clear(); textures.clear(); });
		consol.throwToLoad(Assistance::io__thread_ids::DRAWING, []() {

			const double animsize[2] = { 0.35, 0.07 };

			// raw allegro stuff
			Textures textures;
			Fonts fonts;
			Samples samples;

			// controllers
			Tracks tracks;

			gfile logg;

			logg << L::SLF << fsr(__FUNCSIG__) << "Loading resources (lambda after function)" << L::ELF;

			__g_sys.setInterface();


			/*************************************************************************************

				# Loading stuff:
				> Load textures, sprites, musics, and have a progress bar while doing it hahaha

			**************************************************************************************/


			// REALLY IMPORTANT TO LOAD BEFORE EVERYTHING, REALLY!
			auto ff = fonts.load("DEFAULT", "font.ttf");
			samples.load("LOADING", "musics/loading.ogg");


			draw_simple_bar(0.05, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading basics...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();


			// need this to draw progress bar
			auto __introtrack = tracks.create("INTRO0");
			__introtrack->set(Assistance::io__track_string::LOADID, "LOADING");
			__introtrack->set(Assistance::io__track_boolean::PLAYING, true);
			__introtrack->set(Assistance::io__track_integer::PLAYMODE, Assistance::io__track_integer_modes::LOOP);


			//float __progress = 0.00f;
			//bool hasProgressBarWorked = false;
			/*std::thread __progress_bar([&__progress, &fonts, &hasProgressBarWorked] {
				Display* myd = new Display(300, 40, ALLEGRO_WINDOWED | ALLEGRO_NOFRAME, 60);
				ALLEGRO_FONT* f;
				float __smoothp = 0.00;
				size_t frames_already_done = 0;
				bool good = false;
				for (size_t k = 0; !(good = fonts.get("DEFAULT", f)) && k < 10; k++) Sleep(50);
				if (good) {
					hasProgressBarWorked = true;
					while (frames_already_done < 60 && (100.0f - __smoothp > 0.02f)) {
						if (__progress == 1.00) frames_already_done++;
						__smoothp = (__smoothp * 16.0 + __progress) / 17.0;
						char tb[16];
						sprintf_s(tb, "%02.2f%c", 100.0f * __smoothp, '%');
						draw_simple_bar(__smoothp);
						draw_simple_txt(f, tb);
						myd->flip();
						Sleep(20);
					}
					draw_simple_bar(1.0);
					draw_simple_txt(f, "100%");
					myd->flip();
					Sleep(100);
				}
				else {
					hasProgressBarWorked = false;
					gfile logg;
					logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "FAILED TO RUN CUSTOM PROGRESS BAR! Trying to continue anyway." << L::ELF;
					logg.flush();
					while (frames_already_done < 60 && (100.0f - __smoothp > 0.02f)) {
						if (__progress == 1.00) frames_already_done++;
						__smoothp = (__smoothp * 16.0 + __progress) / 17.0;
						draw_simple_bar(__smoothp);
						myd->flip();
						Sleep(20);
					}
					draw_simple_bar(1.0);
					myd->flip();
					Sleep(100);
				}
				delete myd;
				});*/

				///__progress = 0.01f;

			logg << L::SLF << fsr(__FUNCSIG__) << "Initializing display, events and stuff..." << L::ELF;

			/*textures.load("BACKGROUND_START", "background_gameplay_start.png");
			textures.load("BAR_OFF", "bar_single_one.png");
			textures.load("BAR_ON", "bar_single_one_on.png");
			textures.load("MOUSE", "mouse.png");
			textures.load("MAIN_LOGO", "the_storm.png");

			std::thread* lthr = nullptr;
			float thrfloat = 0.00;
			__progress = 0.03f;

			lthr = new std::thread([&thrfloat, &__progress] {while (thrfloat != 1.00) { __progress = 0.03f + 0.05f * thrfloat; }});
			textures.load(Tools::genStrFormat("BLOCK_##", 10), Tools::genStrFormat("anim/bloco##.png", 10), &thrfloat);
			lthr->join(); delete lthr; lthr = nullptr; thrfloat = 0.00;

			lthr = new std::thread([&thrfloat, &__progress] {while (thrfloat != 1.00) { __progress = 0.09f + 0.69f * thrfloat; }});
			textures.load(Tools::genStrFormat("LOGO_##", 115), Tools::genStrFormat("logo/frame##.png", 115), &thrfloat);
			lthr->join(); delete lthr; lthr = nullptr; thrfloat = 0.00;

			lthr = new std::thread([&thrfloat, &__progress] {while (thrfloat != 1.00) { __progress = 0.79f + 0.20f * thrfloat; }});
			textures.load(Tools::genStrFormat("PAUSE_##", 29), Tools::genStrFormat("pause/pause_##.png", 29), &thrfloat);
			lthr->join(); delete lthr; lthr = nullptr; thrfloat = 0.00;

			*/

			draw_simple_bar(0.10, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading atlas...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();

			auto atlas_wild = textures.load("ATLAS0", "atlas0.png");

			for (int q = 0; q < 8; q++) {
				for (int p = 0; p < 4; p++) {
					if ((q < 7) || (p == 0)) {
						textures.customLoad("PAUSE_" + std::string((p + q * 4) < 10 ? "0" : "") + std::to_string(p + q * 4),
							[&atlas_wild, &p, &q](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, p * 512, q * 128, 512, 128)); }
						);

						draw_simple_bar(0.10 + ((p + q * 4) * 0.25f / 31), al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading pause animation #" + std::string((p + q * 4) < 10 ? "0" : "") + std::to_string(p + q * 4) + "...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
					}
				}
			}
			for (int q = 0; q < 22; q++) {
				for (int p = 0; p < 4; p++) {
					if ((q < 21) || (p == 0)) {
						textures.customLoad("LOGO_" + std::string((p + q * 4) < 10 ? "0" : "") + std::to_string(p + q * 4),
							[&atlas_wild, &p, &q](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, p * 512 + 2048, q * 128, 512, 128)); }
						);

						draw_simple_bar(0.35 + ((p + q * 4) * 0.45f / 87), al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading logo animation #" + std::string((p + q * 4) < 10 ? "0" : "") + std::to_string(p + q * 4) + "...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
					}
				}
			}

			draw_simple_bar(0.80, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading main textures...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();


			textures.customLoad("BACKGROUND_START", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 2560, 2048, 1154)); });
			textures.customLoad("BAR_OFF", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512, 1408, 1024, 128)); });
			textures.customLoad("BAR_ON", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512, 1280, 1024, 128)); });
			textures.customLoad("MOUSE", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1536, 1024, 256, 256)); });
			textures.customLoad("MAIN_LOGO", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512, 1024, 1024, 512)); });


			draw_simple_bar(0.85, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading blocks...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();

			textures.customLoad("BLOCK_00", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 1024, 512, 512)); }); /// DIRT
			textures.customLoad("BLOCK_01", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 1536, 512, 512)); }); /// BLOCK
			textures.customLoad("BLOCK_02", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 2048, 1536, 512, 512)); }); ///	LIFE
			textures.customLoad("BLOCK_03", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512, 2048, 512, 512)); }); /// ?
			textures.customLoad("BLOCK_04", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512, 2048, 512, 512)); }); /// ?
			textures.customLoad("BLOCK_05", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1024, 2048, 512, 512)); }); /// X
			textures.customLoad("BLOCK_06", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512, 1536, 512, 512)); }); /// DESTR 1
			textures.customLoad("BLOCK_07", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1024, 1536, 512, 512)); }); /// DESTR 2
			textures.customLoad("BLOCK_08", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1536, 1536, 512, 512)); }); /// DESTR 3
			textures.customLoad("BLOCK_09", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 2048, 512, 512)); }); /// DESTR 4



			draw_simple_bar(0.87, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading jump...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
			samples.load("JUMP", "musics/jump_01.wav");
			draw_simple_bar(0.89, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading main menu music...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
			samples.load("MUSIC_0", "musics/music_01.ogg");
			draw_simple_bar(0.91, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading alt menu music...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
			samples.load("MUSIC_1", "musics/music_02.ogg");
			draw_simple_bar(0.93, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading game music...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
			samples.load("MUSIC_2", "musics/music_03.ogg");
			draw_simple_bar(0.95, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading walk...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
			samples.load("WALK", "musics/walk_01.wav");


			draw_simple_bar(0.98, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Verifying self existance...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();

			al_convert_bitmaps();

			logg << L::SLF << fsr(__FUNCSIG__) << "Loaded everything." << L::ELF;

			draw_simple_bar(0.99, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Starting game...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();


			});

		logg << L::SLF << fsr(__FUNCSIG__) << "Launching threads..." << L::ELF;

		consol.start();

		logg << L::SLF << fsr(__FUNCSIG__) << "Waiting initialization to be done..." << L::ELF;
		while (!consol.isOpen()) Sleep(20);


		logg << L::SLF << fsr(__FUNCSIG__) << "Waiting lambda load to end..." << L::ELF;
		while (consol.hasSmthToLoad()) Sleep(20);
		logg << L::SLF << fsr(__FUNCSIG__) << "Preparing sprites and starting game... (finally)" << L::ELF;


		/*************************************************************************************

			# Main thread stuff:
			> Loading last resources...

		**************************************************************************************/

		/*cp.setLayer(2, true);
		cp.setLayer(3, true);
		cp.setLayer(0, true);
		cp.setLayer(1, true);
		cp.setLayer(99, true);*/
		//cp.set(Assistance::io__camera_float::OFFSET_Y, 2.6);


		consol.pauseThread();
		
		{
			camera_preset cp = camera_preset();

			// loading
			cp.setLayer(-50, true);
			gcam.set(cp, +main_gamemodes::LOADING);

			// menu
			cp = camera_preset();
			cp.setLayer(-10, true);
			cp.setLayer(99, true); // mouse
			//cp.set(Assistance::io__camera_float::OFFSET_X, 0.4);
			gcam.set(cp, +main_gamemodes::MENU);

			// options
			cp = camera_preset();
			cp.setLayer(-9, true);
			cp.setLayer(99, true); // mouse
			gcam.set(cp, +main_gamemodes::OPTIONS);

			// pause
			cp = camera_preset();
			cp.setLayer(-1, true);
			cp.setLayer(99, true); // mouse
			gcam.set(cp, +main_gamemodes::PAUSE);

			// game
			cp = camera_preset();
			cp.setLayer(0, true);
			gcam.set(cp, +main_gamemodes::GAMING);
		}



		{
			Sprite* s = sprites.create("MOUSE");
			s->set(Assistance::io__sprite_boolean::FOLLOWMOUSE, true);
			s->set(Assistance::io__sprite_boolean::AFFECTED_BY_CAM, false);
			s->set(Assistance::io__sprite_boolean::DRAW, true); // no draw for now
			s->set(Assistance::io__sprite_double::SCALEG, 0.2);
			s->set(Assistance::io__sprite_string::ADD, "MOUSE");
			s->set(Assistance::io__sprite_integer::LAYER, 99);
			s->set(Assistance::io__sprite_double::SPEEDROT, 15.0 / 10);
		}

		{
			Sprite* s = sprites.create("PAUSE_ANIM");
			s->set(Assistance::io__sprite_string_vector::ADDMULTIPLE, Tools::genStrFormat("PAUSE_##", 29));
			s->set(Assistance::io__sprite_string::ID, "PAUSE_ANIM");
			s->set(Assistance::io__sprite_boolean::DRAW, true);
			s->set(Assistance::io__sprite_double::SCALEG, 0.55);
			s->set(Assistance::io__sprite_double::POSY, -0.35);
			s->set(Assistance::io__sprite_double::ANIMATION_FPS, 1.0/24);
			s->set(Assistance::io__sprite_integer::LAYER, -10);
		}

		{
			Sprite* s = sprites.create("LOADING_ANIM");
			s->set(Assistance::io__sprite_string_vector::ADDMULTIPLE, Tools::genStrFormat("LOGO_##", 85));
			s->set(Assistance::io__sprite_string::ID, "LOADING_ANIM");
			s->set(Assistance::io__sprite_boolean::DRAW, true);
			s->set(Assistance::io__sprite_double::SCALEG, 0.4);
			s->set(Assistance::io__sprite_double::SCALEX, 1.4);
			s->set(Assistance::io__sprite_integer::LAYER, -50);
			s->set(Assistance::io__sprite_double::ANIMATION_FPS, 1.0/20);
			s->set(Assistance::io__sprite_boolean::LOOPFRAMES, false);
		}



		auto mytext = texts.create("randomtext");
		mytext->set(Assistance::io__text_string::FONT, "DEFAULT");
		mytext->set(Assistance::io__text_string::STRING, "TEST");
		mytext->set(Assistance::io__text_string::ID, "randomtext");
		mytext->set(Assistance::io__text_boolean::SHOW, true);
		mytext->set(Assistance::io__text_double::SCALEG, 0.1);
		mytext->set(Assistance::io__text_double::POSY, 0.65);
		mytext->set(Assistance::io__text_integer::LAYER, -10);
		mytext->set(Assistance::io__text_double::UPDATETIME, 1.0/20);


		auto mytrack = tracks.create("randomtrack");
		mytrack->set(Assistance::io__track_string::LOADID, "MUSIC_0");

		consol.resumeThread();


		/*************************************************************************************

			# Main thread ready:
			> Game has "started"!

		**************************************************************************************/

		main_gamemodes modern = main_gamemodes::LOADING; // CHANGE TO LOADING ON FINAL VERSION


		logg << L::SLF << fsr(__FUNCSIG__) << "Ready to go! Starting main!" << L::ELF;

		// LOADING PART
		if (modern == main_gamemodes::LOADING) // shh skip
		{
			Sprite* s = nullptr;
			if (!sprites.get("LOADING_ANIM", s)) throw Abort::abort(__FUNCSIG__, "It wasn't supposed to not find a sprite here!");

			bool b = false;
			do {
				gcam.apply(+modern); // first
				s->get(Assistance::io__sprite_boolean::HAS_DONE_LOOPONCE, b);
			} while (!b && consol.isRunning() && modern == main_gamemodes::LOADING);

			Sleep(2000);
			gcam.apply(+main_gamemodes::MENU);
			modern = main_gamemodes::MENU;
		}


		size_t counttt = 0;
		auto copy = gcam.get(); // later idk just have a copy here k?

		//__introtrack->set(io__track_boolean::PLAYING, false);

		while (consol.isRunning()) {
			Sleep(7);


			mytext->set(Assistance::io__text_string::STRING, "Counting here lmao " + std::to_string(counttt++) + "!");
			//mytext->set(Assistance::io__text_string::STRING, "FPS: %fps% | TPS: %tps% | UPS: %ups%");

			//mysprite->apply(Assistance::io__sprite_double::POSX, 0.3 * sin(0.7 + 0.91 * al_get_time()));
			//mysprite->apply(Assistance::io__sprite_double::POSY, - 1.5 + 0.2 * cos(0.4 + 0.65 * al_get_time()));

			//cp.set(Assistance::io__camera_float::ROTATION_RAD, al_get_time() * 0.25);
			//logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "ANG=" << (float)((int)(0.3 * (1800.0 / ALLEGRO_PI) * al_get_time())%3600)/10.0 << L::EL;
			//cp.set(Assistance::io__camera_float::ROTATION_RAD, 0.3 * al_get_time());
			gcam.get().set(Assistance::io__camera_float::OFFSET_X, 0.45 * cos(0.3*al_get_time()));
			//gcam.get().set(Assistance::io__camera_float::OFFSET_Y, 0.14 * sin(0.2*al_get_time()));
			gcam.get().set(Assistance::io__camera_float::SCALE_G, 0.7 + 0.4 * sin(al_get_time() * 0.33));
			gcam.get().set(Assistance::io__camera_float::ROTATION_RAD, (al_get_time() * 0.03));
			gcam.apply(+modern);

			//gcam.set(cp, 0);
			//gcam.apply(0);
		}
		

		logg << L::SLF << fsr(__FUNCSIG__) << "Closing game..." << L::ELF;

		textures.clear();
		sprites.clear();
		fonts.clear();
		texts.clear();
		conf.flush();
	}
	catch (Abort::abort a)
	{
		forceExit("Something went wrong at MAIN!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
	}

	

	return 0;
}
