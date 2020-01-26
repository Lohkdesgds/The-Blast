#include <iostream>
#include <string>
#include <Windows.h>

#include "LSW/lsw.h"

using namespace LSW::v4;


enum class main_gamemodes { HASNT_STARTED_YET=-2, LOADING, MENU, OPTIONS, PAUSE, GAMING };
enum class my_custom_funcs { LOADING_ANIMATION/*at: setup_animation_functional (starter.cpp)*/,BUBBLE_TASK/*at: set_consol_mainthread_start*/,UPDATE_CAMERA };

int main(int argc, const char* argv[])
{
	try {
		/*************************************************************************************

			# Start stuff:
			> Variables, some tests, initializing the basics

		**************************************************************************************/


		// used by lambdas
		main_gamemodes modern = main_gamemodes::HASNT_STARTED_YET;
		bool assist[3] = { false, false, false }; // 1: LOADING_ANIMATION, 2: FULLSCREEN/WINDOW TOGGLE, 3: GAMEBOXMAIN Sprite down below on loop

		// this has to be like the first one
		gfile logg;

		// needed initialization
		alert("BETA/ALPHA", "This is a beta or alpha version of the game.", "There might be some issues here and there, so be nice and report to @Lohkdesgds at Twitter, Gamejolt or GitHub! Thanks!");
		

		// things being used
		Textures textures;
		Fonts fonts;
		//Samples samples;
		Sprites sprites;
		Texts texts;
		Tracks tracks;
		Camera gcam;
		Bubbles bubble;
		Mixer mixer;
		Manager consol;
		Database conf;

		logg << L::SLF << fsr(__FUNCSIG__) << "Setting up dynamic functions..." << L::ELF;

		// temporary
		conf.set(Constants::io__db_boolean::SAVING_STRING_INPUT, true);


		// quick var setup
		{
			int __s;
			conf.get(Constants::io__conf_integer::SCREEN_FLAGS, __s, 0);
			assist[1] = (__s & ALLEGRO_FULLSCREEN_WINDOW) || (__s & ALLEGRO_FULLSCREEN);
		}

		// functional-only lambdas
		const std::function< int(void)> reapply_cam_settings			= [&gcam, &modern] {
			gcam.apply(+modern);
			return 0;
		};
		const std::function< int(void)> bubble_think_task				= [&bubble]()->int {
			bubble.think(); return 0; 
		};
		const std::function< int(void)> bubble_draw_task				= [&bubble]()->int {
			bubble.draw(); 
			return 0;
		};

		// used ONCE lambdas (pre-determined normally huge start and end lambdas
		const std::function<void(void)> set_consol_mainthread_end		= [&textures, &sprites, &texts, &fonts]() {
			sprites.clear(); texts.clear(); fonts.clear(); textures.clear();
		};
		const std::function<void(void)> set_consol_mainthread_start		= [&bubble_think_task, &bubble_draw_task, &bubble, & consol]() {

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

			bubble.init(200, -1, -99);
			consol.setSimpleTask(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::LOOP, bubble_draw_task);
			consol.addCustomTask(bubble_think_task, +my_custom_funcs::BUBBLE_TASK, 1.0 / 24);


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
			__introtrack->set(Constants::io__track_string::LOADID, "LOADING");
			__introtrack->set(Constants::io__track_boolean::PLAYING, true);
			__introtrack->set(Constants::io__track_integer::PLAYMODE, Constants::io__track_integer_modes::LOOP);


			logg << L::SLF << fsr(__FUNCSIG__) << "Initializing display, events and stuff..." << L::ELF;

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

			textures.customLoad("BLOCK_00", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 1536, 512, 512)); }); /// BLOCK
			textures.customLoad("BLOCK_01", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 1024, 512, 512)); }); /// DIRT
			textures.customLoad("BLOCK_02", [&atlas_wild](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1536, 1536, 512, 512)); }); ///	LIFE
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


		};

		// button related (also quick function call)
		const std::function<void(void)> setup_animation_functional		= [&consol, &modern, &sprites, &assist, &logg]()->void {
			logg << L::SL << fsr(__FUNCSIG__) << "Setting up LOADING_ANIM animation" << L::EL;
			consol.addCustomTask([&]()->int {
				//logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "(lambda animation task)" << L::EL;
				Sprite* s = nullptr; if (!sprites.get("LOADING_ANIM", s)) return 0; // if running while program is being closed it can hurt
				if (modern != main_gamemodes::LOADING) {
					assist[0] = false;
					modern = main_gamemodes::LOADING;
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Resetting LOADING_ANIM animation to then run LOADING animation" << L::EL;
				}
				else if (s->isEq(Constants::io__sprite_boolean::IS_IT_ON_LAST_FRAME, true) && !assist[0])
				{
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Done doing LOADING animation" << L::EL;
					assist[0] = true;
					return -40; // 40 cycles because it updates 40 per sec //Sleep(2000);
				}
				else if (assist[0]) {
					modern = main_gamemodes::MENU;
					s->set(Constants::io__sprite_double::ANIMATION_FPS, 0);
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Finally removed itself" << L::EL;
					consol.removeCustomTask(+my_custom_funcs::LOADING_ANIMATION);
				}
				return 0;
				}, +my_custom_funcs::LOADING_ANIMATION, 1.0 / 20);
			logg << L::SL << fsr(__FUNCSIG__) << "Has set LOADING_ANIM to run" << L::EL;
		};
		const std::function<void(void)> skip_animation_functional		= [&assist] {
			assist[0] = true;
		};
		const std::function<void(void)> exit_game						= [&consol] {
			consol.shouldStop();
		};

		// keybinds
		const std::function<void(void)> keyb_fullscreen_window			= [&consol, &assist]() {
			consol.sendEvent(Constants::ro__my_events::THRDRW_GOT_FORCED_RESIZE, (intptr_t)(assist[1] = !assist[1]));
		};
		const int						keyb_fullscreen_window_key		= ALLEGRO_KEY_F11;
		const std::function<void(void)> keyb_printscreen				= [&consol]() {
			if (consol._display_handle()) consol._display_handle()->print();
		};
		const int						keyb_printscreen_key			= ALLEGRO_KEY_F2;

		const std::function<void(void)> keyb_kbf_go_north				= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::GO_NORTH);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_south				= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::GO_SOUTH);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_west				= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::GO_WEST);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_east				= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::GO_EAST);
				}
			}
		};

		const std::function<void(void)> keyb_kbf_go_reset_north			= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::STOP_NORTH);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_reset_south			= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::STOP_SOUTH);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_reset_west			= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::STOP_WEST);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_reset_east			= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::STOP_EAST);
				}
			}
		};

		const int						keyb_kbf_go_north_key			= ALLEGRO_KEY_W;
		const int						keyb_kbf_go_south_key			= ALLEGRO_KEY_S;
		const int						keyb_kbf_go_west_key			= ALLEGRO_KEY_A;
		const int						keyb_kbf_go_east_key			= ALLEGRO_KEY_D;

		const int						keyb_kbf_go_north_key_a			= ALLEGRO_KEY_UP;
		const int						keyb_kbf_go_south_key_a			= ALLEGRO_KEY_DOWN;
		const int						keyb_kbf_go_west_key_a			= ALLEGRO_KEY_LEFT;
		const int						keyb_kbf_go_east_key_a			= ALLEGRO_KEY_RIGHT;

		
		logg << L::SLF << fsr(__FUNCSIG__) << "Setting up load/unload base..." << L::ELF;


		/*************************************************************************************

			# Setting up things part:
			> This is the part where we tell how to create some stuff.
			> Yes, it is fully customizable!

		**************************************************************************************/

		// KEYBINDING
		{
			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_fullscreen_window_key, keyb_fullscreen_window);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_printscreen_key, keyb_printscreen);

			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_go_north_key, keyb_kbf_go_north);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_go_south_key, keyb_kbf_go_south);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_go_west_key, keyb_kbf_go_west);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_go_east_key, keyb_kbf_go_east);

			conf.set(Constants::io__db_functional_opt::KEYBOARD_LEFT, keyb_kbf_go_north_key, keyb_kbf_go_reset_north);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_LEFT, keyb_kbf_go_south_key, keyb_kbf_go_reset_south);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_LEFT, keyb_kbf_go_west_key, keyb_kbf_go_reset_west);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_LEFT, keyb_kbf_go_east_key, keyb_kbf_go_reset_east);

			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_go_north_key_a, keyb_kbf_go_north);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_go_south_key_a, keyb_kbf_go_south);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_go_west_key_a, keyb_kbf_go_west);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_go_east_key_a, keyb_kbf_go_east);

			conf.set(Constants::io__db_functional_opt::KEYBOARD_LEFT, keyb_kbf_go_north_key_a, keyb_kbf_go_reset_north);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_LEFT, keyb_kbf_go_south_key_a, keyb_kbf_go_reset_south);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_LEFT, keyb_kbf_go_west_key_a, keyb_kbf_go_reset_west);
			conf.set(Constants::io__db_functional_opt::KEYBOARD_LEFT, keyb_kbf_go_east_key_a, keyb_kbf_go_reset_east);
		}


		consol.setSimpleTask(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::END, set_consol_mainthread_end);
		consol.setSimpleTask(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::START, set_consol_mainthread_start);

		consol.addCustomTask(reapply_cam_settings, +my_custom_funcs::UPDATE_CAMERA, 1.0 / 20);




		/*************************************************************************************

			# Starting up screen and stuff:
			> Start consol so things can actually happen
			> It has to start to work

		**************************************************************************************/


		logg << L::SLF << fsr(__FUNCSIG__) << "Launching..." << L::ELF;

		consol.start();

		logg << L::SLF << fsr(__FUNCSIG__) << "Want some tea? It's tea time (waiting game launch)" << L::ELF;
		logg.flush();

		while (!consol.isOpen()) Sleep(20);

		logg << L::SLF << fsr(__FUNCSIG__) << "The tea time continues until all textures and stuff are loaded..." << L::ELF;
		while (consol.hasTasked(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::START)) Sleep(20);
		logg << L::SLF << fsr(__FUNCSIG__) << "Done drinking tea. Time to work on Sprites, Texts and stuff." << L::ELF;


		/*************************************************************************************

			# Creating variables part:
			> Creating the actual game here (obviously after loading the texture as you can see)
			> Sprites, Texts, Music and stuff

		**************************************************************************************/


		// LOCK GAME TO SET UP EVERYTHING
		consol.pauseThread();
		while (!consol.hasThreadPaused()) Sleep(10);
		
		/// >>>>>>> GAME SPRITES, TEXTS AND STUFF BRACKET <<<<<<< ///
		{
			/*
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>--- Camera preset loading ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			*/

			{
				// loading
				camera_preset cp;
				cp.setInternalID("Loading preset");
				cp.setLayer(-50, true);
				cp.setLayer(100, true); // DEBUG LINE
				cp.set(Constants::io__camera_boolean::RESPECT_LIMITS, true);
				cp.set(Constants::io__camera_double::LIMIT_MIN_X, -0.95);
				cp.set(Constants::io__camera_double::LIMIT_MIN_Y, -0.95);
				cp.set(Constants::io__camera_double::LIMIT_MAX_X, 0.95);
				cp.set(Constants::io__camera_double::LIMIT_MAX_Y, 0.95);
				cp.set(Constants::io__camera_boolean::READONLY_NOW, true);
				gcam.set(cp, +main_gamemodes::LOADING);
			}
			{
				// menu
				camera_preset cp;
				cp.setInternalID("Menu preset");
				cp.setLayer(-10, true);
				cp.setLayer(-99, true); // bubble animation
				cp.setLayer(99, true); // mouse
				cp.setLayer(100, true); // DEBUG LINE
				cp.set(Constants::io__camera_boolean::RESPECT_LIMITS, true);
				cp.set(Constants::io__camera_double::LIMIT_MIN_X, -0.95);
				cp.set(Constants::io__camera_double::LIMIT_MIN_Y, -0.95);
				cp.set(Constants::io__camera_double::LIMIT_MAX_X, 0.95);
				cp.set(Constants::io__camera_double::LIMIT_MAX_Y, 0.95);
				cp.set(Constants::io__camera_boolean::READONLY_NOW, true);
				gcam.set(cp, +main_gamemodes::MENU);
			}
			{
				// options
				camera_preset cp;
				cp.setInternalID("Options preset");
				cp.setLayer(-9, true);
				cp.setLayer(99, true); // mouse
				cp.setLayer(-99, true); // bubble animation
				cp.setLayer(100, true); // DEBUG LINE
				cp.set(Constants::io__camera_boolean::RESPECT_LIMITS, true);
				cp.set(Constants::io__camera_double::LIMIT_MIN_X, -0.95);
				cp.set(Constants::io__camera_double::LIMIT_MIN_Y, -0.95);
				cp.set(Constants::io__camera_double::LIMIT_MAX_X, 0.95);
				cp.set(Constants::io__camera_double::LIMIT_MAX_Y, 0.95);
				cp.set(Constants::io__camera_boolean::READONLY_NOW, true);
				gcam.set(cp, +main_gamemodes::OPTIONS);
			}
			{
				// pause
				camera_preset cp;
				cp.setInternalID("Pause preset");
				cp.setLayer(-1, true);
				cp.setLayer(99, true); // mouse
				cp.setLayer(-98, true); // lines animation
				cp.setLayer(100, true); // DEBUG LINE
				cp.set(Constants::io__camera_boolean::RESPECT_LIMITS, true);
				cp.set(Constants::io__camera_double::LIMIT_MIN_X, -0.95);
				cp.set(Constants::io__camera_double::LIMIT_MIN_Y, -0.95);
				cp.set(Constants::io__camera_double::LIMIT_MAX_X, 0.95);
				cp.set(Constants::io__camera_double::LIMIT_MAX_Y, 0.95);
				cp.set(Constants::io__camera_boolean::READONLY_NOW, true);
				gcam.set(cp, +main_gamemodes::PAUSE);
			}
			{
				// game
				camera_preset cp;
				cp.setInternalID("Gaming preset (dynamic)");
				cp.setLayer(0, true);
				cp.setLayer(1, true);
				cp.setLayer(100, true); // DEBUG LINE
				cp.set(Constants::io__camera_boolean::RESPECT_LIMITS, true);
				cp.set(Constants::io__camera_double::LIMIT_MIN_X, -1.05);
				cp.set(Constants::io__camera_double::LIMIT_MIN_Y, -1.05);
				cp.set(Constants::io__camera_double::LIMIT_MAX_X, 1.05);
				cp.set(Constants::io__camera_double::LIMIT_MAX_Y, 1.05);
				//cp.set(Constants::io__camera_double::SLIPPERINESS, 1.0);
				//cp.set(Constants::io__camera_double::SCALE_G, 0.95);
				//cp.set(Constants::io__camera_double::SCALE_X, 1.0175);
				cp.set(Constants::io__camera_boolean::READONLY_NOW, true);
				gcam.set(cp, +main_gamemodes::GAMING);
			}

			/*
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>--- Sprite preset loading ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			*/

			{
				Sprite* s = sprites.create("MOUSE");
				s->set(Constants::io__sprite_string::ID, "MOUSE");
				s->set(Constants::io__sprite_boolean::FOLLOWMOUSE, true);
				s->set(Constants::io__sprite_collision_mode::COLLISION_ANY_HOLD_STATIC);
				s->set(Constants::io__sprite_boolean::DRAW, true); // no draw for now
				s->set(Constants::io__sprite_double::SCALEG, 0.2);
				s->set(Constants::io__sprite_string::ADD, "MOUSE");
				s->set(Constants::io__sprite_integer::LAYER, 99);
				//s->set(Constants::io__sprite_integer::LAYER, -10); // TEMP
				s->set(Constants::io__sprite_double::SPEEDROT, 15.0 / Constants::__i_col_pos_t_update);
			}

			/*{
				Sprite* s = sprites.create("PAUSE_ANIM");
				s->set(Constants::io__sprite_string_vector::ADDMULTIPLE, Tools::genStrFormat("PAUSE_##", 29));
				s->set(Constants::io__sprite_string::ID, "PAUSE_ANIM");
				s->set(Constants::io__sprite_boolean::DRAW, true);
				s->set(Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ELASTIC);
				s->set(Constants::io__sprite_double::SCALEG, 0.55);
				s->set(Constants::io__sprite_double::POSY, 0.45);
				s->set(Constants::io__sprite_double::ANIMATION_FPS, 1.0 / 24);
				s->set(Constants::io__sprite_integer::LAYER, 0);
			}

			{
				Sprite* s = sprites.create("PAUSE_ANIM2");
				s->set(Constants::io__sprite_string_vector::ADDMULTIPLE, Tools::genStrFormat("PAUSE_##", 29));
				s->set(Constants::io__sprite_string::ID, "PAUSE_ANIM2");
				s->set(Constants::io__sprite_boolean::DRAW, true);
				s->set(Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH);
				s->set(Constants::io__sprite_double::SCALEG, 0.35);
				s->set(Constants::io__sprite_double::POSY, -0.45);
				s->set(Constants::io__sprite_double::ANIMATION_FPS, 1.0 / 15);
				s->set(Constants::io__sprite_integer::LAYER, 0);
			}*/

			{
				Sprite* s = sprites.create("LOADING_ANIM");
				s->set(Constants::io__sprite_string_vector::ADDMULTIPLE, Tools::genStrFormat("LOGO_##", 85));
				s->set(Constants::io__sprite_string::ID, "LOADING_ANIM");
				s->set(Constants::io__sprite_boolean::DRAW, true);
				s->set(Constants::io__sprite_double::SCALEG, 0.4);
				s->set(Constants::io__sprite_double::SCALEX, 1.4);
				s->set(Constants::io__sprite_integer::LAYER, -50);
				s->set(Constants::io__sprite_double::ANIMATION_FPS, 1.0 / 20);
				s->set(Constants::io__sprite_boolean::LOOPFRAMES, false);
				s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				s->set(Constants::io__sprite_boolean::ZERO_RESETS_POSITION_INSTEAD_OF_FREEZING, true);
				//s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&modern]()->void {modern = main_gamemodes::MENU; });
			}
			{
				Sprite* s = sprites.create("LOADING_SKIP_ALL_SCREEN");
				s->set(Constants::io__sprite_string::ID, "LOADING_SKIP_ALL_SCREEN");
				s->set(Constants::io__sprite_boolean::DRAW, false);
				s->set(Constants::io__sprite_double::SCALEG, 2.0);
				s->set(Constants::io__sprite_integer::LAYER, -50);
				s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, skip_animation_functional);
			}

			/*
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>--- Sprite-Text Button preset loading ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			*/

			// BUTTONS

			/*
			LOADING:	-50
			MENU:		-10
			OPTIONS:	-09
			PAUSE:		-01
			GAME:		 00
			all:		 99 (mouse)
			*/

			{
				Sprite* s = sprites.create("BUTTON_MENU_0");
				s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
				s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
				s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
				s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&modern]()->void {modern = main_gamemodes::GAMING; });
				s->set(Constants::io__sprite_string::ID, "BUTTON_MENU_0");
				s->set(Constants::io__sprite_boolean::DRAW, true);
				s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				s->set(Constants::io__sprite_double::SCALEG, 0.18);
				s->set(Constants::io__sprite_double::SCALEX, 5.5);
				s->set(Constants::io__sprite_double::POSY, -0.35);
				s->set(Constants::io__sprite_integer::LAYER, -10);

				Text* t = texts.create("BUTTON_MENU_0_T");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "GO GAMING | %sprite_state%");
				t->set(Constants::io__text_string::ID, "BUTTON_MENU_0_T");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.08);
				t->set(Constants::io__text_double::SCALEX, 0.7);
				t->set(Constants::io__text_double::POSY, -0.060);
				t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_MENU_0");
				t->set(Constants::io__text_integer::LAYER, -10);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 20);
				t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
			}
			{
				Sprite* s = sprites.create("BUTTON_MENU_1");
				s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
				s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
				s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
				s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, setup_animation_functional);
				s->set(Constants::io__sprite_string::ID, "BUTTON_MENU_1");
				s->set(Constants::io__sprite_boolean::DRAW, true);
				s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				s->set(Constants::io__sprite_double::SCALEG, 0.18);
				s->set(Constants::io__sprite_double::SCALEX, 5.5);
				s->set(Constants::io__sprite_double::POSY, -0.05);
				s->set(Constants::io__sprite_integer::LAYER, -10);

				Text* t = texts.create("BUTTON_MENU_1_T");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "GO LOADING | %sprite_state%");
				t->set(Constants::io__text_string::ID, "BUTTON_MENU_1_T");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.08);
				t->set(Constants::io__text_double::SCALEX, 0.7);
				t->set(Constants::io__text_double::POSY, -0.060);
				t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_MENU_1");
				t->set(Constants::io__text_integer::LAYER, -10);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 20);
				t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
			}
			{
				Sprite* s = sprites.create("BUTTON_MENU_2");
				s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
				s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
				s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
				s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, exit_game);
				s->set(Constants::io__sprite_string::ID, "BUTTON_MENU_2");
				s->set(Constants::io__sprite_boolean::DRAW, true);
				s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				s->set(Constants::io__sprite_double::SCALEG, 0.18);
				s->set(Constants::io__sprite_double::SCALEX, 5.5);
				s->set(Constants::io__sprite_double::POSY, 0.25);
				s->set(Constants::io__sprite_integer::LAYER, -10);

				Text* t = texts.create("BUTTON_MENU_2_T");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "EXIT | %sprite_state%");
				t->set(Constants::io__text_string::ID, "BUTTON_MENU_2_T");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.08);
				t->set(Constants::io__text_double::SCALEX, 0.7);
				t->set(Constants::io__text_double::POSY, -0.060);
				t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_MENU_2");
				t->set(Constants::io__text_integer::LAYER, -10);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 20);
				t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
			}
			{
				Sprite* s = sprites.create("BUTTON_GAMING_0");
				s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
				s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
				s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
				s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
				s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&modern]()->void {modern = main_gamemodes::MENU; });
				s->set(Constants::io__sprite_string::ID, "BUTTON_GAMING_0");
				s->set(Constants::io__sprite_boolean::DRAW, true);
				s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				s->set(Constants::io__sprite_double::SCALEG, 0.14);
				s->set(Constants::io__sprite_double::SCALEX, 5.5);
				s->set(Constants::io__sprite_double::POSY, -0.9);
				s->set(Constants::io__sprite_double::POSX, 0.45);
				s->set(Constants::io__sprite_integer::LAYER, 1);

				Text* t = texts.create("BUTTON_GAMING_0_T");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "GO MENU | %sprite_state%");
				t->set(Constants::io__text_string::ID, "BUTTON_GAMING_0_T");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.07);
				t->set(Constants::io__text_double::SCALEX, 0.7);
				t->set(Constants::io__text_double::POSY, -0.060);
				t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_GAMING_0");
				t->set(Constants::io__text_integer::LAYER, 1);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 20);
				t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
			}

			/*
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<-------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>--- Texts preset loading ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<-------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			*/

			{
				Text* const t = texts.create("osd_stuff0");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "M:%mouse_x%,%mouse_y% | C:%cam_x%,%cam_y%@~%cam_zoom_combined% | S:%curr_string%[%last_string%]");
				t->set(Constants::io__text_string::ID, "osd_stuff0");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.025);
				t->set(Constants::io__text_double::SCALEX, 0.6);
				t->set(Constants::io__text_double::POSY, -1.00);
				t->set(Constants::io__text_double::POSX, -1.0);
				t->set(Constants::io__text_integer::MODE, +Constants::io__alignment_text::ALIGN_LEFT);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 4);
				t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
				t->set(Constants::io__text_integer::LAYER, 100);
			}
			{
				Text* const t = texts.create("osd_stuff1");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "FPS: %int_fps%|%instant_fps% [%ms_fps% ms]");
				t->set(Constants::io__text_string::ID, "osd_stuff1");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.025);
				t->set(Constants::io__text_double::SCALEX, 0.6);
				t->set(Constants::io__text_double::POSY, -0.97);
				t->set(Constants::io__text_double::POSX, -1.0);
				t->set(Constants::io__text_integer::MODE, +Constants::io__alignment_text::ALIGN_LEFT);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 12);
				t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
				t->set(Constants::io__text_integer::LAYER, 100);
			}
			{
				Text* const t = texts.create("osd_stuff2");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "TPS: %int_tps%|%instant_tps% [%ms_tps% ms]");
				t->set(Constants::io__text_string::ID, "osd_stuff2");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.025);
				t->set(Constants::io__text_double::SCALEX, 0.6);
				t->set(Constants::io__text_double::POSY, -0.94);
				t->set(Constants::io__text_double::POSX, -1.0);
				t->set(Constants::io__text_integer::MODE, +Constants::io__alignment_text::ALIGN_LEFT);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 3);
				t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
				t->set(Constants::io__text_integer::LAYER, 100);
			}
			{
				Text* const t = texts.create("osd_stuff3");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "UPS: %int_ups%|%instant_ups% [%ms_ups% ms]");
				t->set(Constants::io__text_string::ID, "osd_stuff3");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.025);
				t->set(Constants::io__text_double::SCALEX, 0.6);
				t->set(Constants::io__text_double::POSY, -0.91);
				t->set(Constants::io__text_double::POSX, -1.0);
				t->set(Constants::io__text_integer::MODE, +Constants::io__alignment_text::ALIGN_LEFT);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 4);
				t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
				t->set(Constants::io__text_integer::LAYER, 100);
			}
			{
				Text* const t = texts.create("osd_stuff4");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "APS: %int_aps%|%instant_aps% [%ms_aps% ms]");
				t->set(Constants::io__text_string::ID, "osd_stuff4");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.025);
				t->set(Constants::io__text_double::SCALEX, 0.6);
				t->set(Constants::io__text_double::POSY, -0.88);
				t->set(Constants::io__text_double::POSX, -1.0);
				t->set(Constants::io__text_integer::MODE, +Constants::io__alignment_text::ALIGN_LEFT);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 6);
				t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
				t->set(Constants::io__text_integer::LAYER, 100);
			}
			{
				Text* const t = texts.create("osd_stuff5");
				t->set(Constants::io__text_string::FONT, "DEFAULT");
				t->set(Constants::io__text_string::STRING, "D:%curr_res_x%:%curr_res_y%@%curr_refresh_rate% | I:%num_images%,S:%num_sprites%,T:%num_texts%,A:%num_tracks%");
				t->set(Constants::io__text_string::ID, "osd_stuff5");
				t->set(Constants::io__text_boolean::SHOW, true);
				t->set(Constants::io__text_double::SCALEG, 0.025);
				t->set(Constants::io__text_double::SCALEX, 0.6);
				t->set(Constants::io__text_double::POSY, -0.85);
				t->set(Constants::io__text_double::POSX, -1.0);
				t->set(Constants::io__text_integer::MODE, +Constants::io__alignment_text::ALIGN_LEFT);
				t->set(Constants::io__text_double::UPDATETIME, 1.0 / 3);
				t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
				t->set(Constants::io__text_integer::LAYER, 100);
			}
		}


		/*************************************************************************************

			# Main thread variables only:
			> Stuff mostly used here at the bottom.

		**************************************************************************************/


		Text* temporary_fun_text = texts.create("randomtext");
		temporary_fun_text->set(Constants::io__text_string::FONT, "DEFAULT");
		temporary_fun_text->set(Constants::io__text_string::STRING, "TEST");
		temporary_fun_text->set(Constants::io__text_string::ID, "randomtext");
		temporary_fun_text->set(Constants::io__text_boolean::SHOW, true);
		temporary_fun_text->set(Constants::io__text_double::SCALEG, 0.1);
		temporary_fun_text->set(Constants::io__text_double::SCALEX, 0.8);
		temporary_fun_text->set(Constants::io__text_double::POSY, 0.65);
		temporary_fun_text->set(Constants::io__text_integer::LAYER, -10);
		temporary_fun_text->set(Constants::io__text_double::UPDATETIME, 1.0/10);


		auto mytrack = tracks.create("randomtrack");
		mytrack->set(Constants::io__track_string::LOADID, "MUSIC_0");




		logg << L::SLF << fsr(__FUNCSIG__) << "Ready to go! Starting main!" << L::ELF;

		setup_animation_functional();

		// UNLOCK
		consol.resumeThread();


		/*************************************************************************************

			# Main thread ongoing:
			> Main Thread will:
			* Check if other threads are working as expected			(to be done)
			

		**************************************************************************************/


		/*{
			Sprite* s = sprites.create("PLAYER");
			s->set(Constants::io__sprite_string::ID, "PLAYER");
			s->set(Constants::io__sprite_boolean::DRAW, true);
			s->set(Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH);
			s->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true);
			//s->set(Constants::io__sprite_boolean::SHOWDOT, true);
			s->set(Constants::io__sprite_boolean::SHOWBOX, false);
			s->set(Constants::io__sprite_double::SCALEG, 0.065);
			s->set(Constants::io__sprite_double::SCALEY, 1.81);
			s->set(Constants::io__sprite_string::ADD, "BLOCK_05");
			s->set(Constants::io__sprite_integer::LAYER, 1);


			Text* t = texts.create("PLAYER_T");
			t->set(Constants::io__text_string::FONT, "DEFAULT");
			t->set(Constants::io__text_string::STRING, "%sprite_debug%");
			t->set(Constants::io__text_string::ID, "PLAYER_T");
			t->set(Constants::io__text_boolean::SHOW, true);
			t->set(Constants::io__text_double::SCALEG, 0.06);
			t->set(Constants::io__text_double::SCALEX, 0.7);
			t->set(Constants::io__text_double::POSY, 0.060);
			t->set(Constants::io__text_string::FOLLOW_SPRITE, "PLAYER");
			t->set(Constants::io__text_integer::LAYER, 1);
			t->set(Constants::io__text_double::UPDATETIME, 1.0 / 4);
			t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
		}*/



		bool has_map_gen = false;
		World* wd = nullptr;


		for (size_t counttt = 0; consol.isRunning(); counttt++) {
			Sleep(15);


			if (modern == main_gamemodes::MENU) temporary_fun_text->set(Constants::io__text_string::STRING, "Counting here lmao " + std::to_string(counttt) + "!");
			if (modern == main_gamemodes::GAMING) {
				if (!has_map_gen)
				{
					int mult = rand() % 4 + 1;

					wd = new World(16.0* +mult, 9.0* +mult);

					consol.pauseThread();
					while (!consol.hasThreadPaused()) Sleep(10);

					Sprite* this_is_the_player = nullptr;

					{
						Sprite*& s = this_is_the_player;
						s = sprites.create("PLAYER");
						s->set(Constants::io__sprite_string::ID, "PLAYER");
						s->set(Constants::io__sprite_boolean::DRAW, true);
						s->set(Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH);
						s->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true);
						//s->set(Constants::io__sprite_boolean::SHOWDOT, true);
						s->set(Constants::io__sprite_boolean::SHOWBOX, false);
						s->set(Constants::io__sprite_double::SCALEG, 0.065 / mult);
						s->set(Constants::io__sprite_double::SCALEY, 1.81);
						s->set(Constants::io__sprite_string::ADD, "BLOCK_05");
						s->set(Constants::io__sprite_integer::LAYER, 1);


						Text* t = texts.create("PLAYER_T");
						t->set(Constants::io__text_string::FONT, "DEFAULT");
						t->set(Constants::io__text_string::STRING, "%sprite_debug%");
						t->set(Constants::io__text_string::ID, "PLAYER_T");
						t->set(Constants::io__text_boolean::SHOW, true);
						t->set(Constants::io__text_double::SCALEG, 0.06);
						t->set(Constants::io__text_double::SCALEX, 0.7);
						t->set(Constants::io__text_double::POSY, 0.060);
						t->set(Constants::io__text_string::FOLLOW_SPRITE, "PLAYER");
						t->set(Constants::io__text_integer::LAYER, 1);
						t->set(Constants::io__text_double::UPDATETIME, 1.0 / 4);
						t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
					}

					wd->generate(al_get_time()*20.0);

					for (int y = 0; y < wd->getLen(1); y++) {
						for (int x = 0; x < wd->getLen(0); x++)
						{
							if (wd->readPos((int)x, (int)y) == +blocks::BLOCK) {
								double pss[2];
								pss[0] = 2.0 * (1.0 * x / (wd->getLen(0) - 1) - 0.5);
								pss[1] = 2.0 * (1.0 * y / (wd->getLen(1) - 1) - 0.5);

								Sprite* s = sprites.create("GAMEBOX_" + std::to_string(y) + "_" + std::to_string(x));
								s->set(Constants::io__sprite_string::ID, "GAMEBOX_" + std::to_string(y) + "_" + std::to_string(x));
								s->set(Constants::io__sprite_boolean::DRAW, false); // no draw for now
								s->set(Constants::io__sprite_collision_mode::COLLISION_ANY_HOLD_STATIC);
								//s->set(Constants::io__sprite_boolean::SHOWDOT, true);
								s->set(Constants::io__sprite_boolean::SHOWBOX, false);
								s->set(Constants::io__sprite_double::SCALEG, 0.132 / mult);
								s->set(Constants::io__sprite_double::SCALEY, 1.81);
								s->set(Constants::io__sprite_string::ADD, "BLOCK_0" + std::string(wd->readPos(x, y) ? "1" : "0"));
								s->set(Constants::io__sprite_integer::LAYER, 0);
								s->set(Constants::io__sprite_double::POSX, pss[0]);
								s->set(Constants::io__sprite_double::POSY, pss[1]);
							}
						}
					}
					{
						bool _don = false;
						for (unsigned a = 0; a < 20; a++) {

							int px, py;
							px = rand() % wd->getLen(0);
							py = rand() % wd->getLen(1);

							if (wd->readPos(px, py) == +blocks::EMPTY) {
								double pss[2];
								pss[0] = 2.0 * (1.0 * px / (wd->getLen(0) - 1) - 0.5);
								pss[1] = 2.0 * (1.0 * py / (wd->getLen(1) - 1) - 0.5);

								this_is_the_player->set(Constants::io__sprite_double::POSX, pss[0]);
								this_is_the_player->set(Constants::io__sprite_double::POSY, pss[1]);
								_don = true;
							}
						}

						if (!_don) {
							for (int py = 0; py < wd->getLen(1); py++) {
								for (int px = 0; px < wd->getLen(0); px++) {
									if (wd->readPos(px, py) == +blocks::EMPTY) {
										double pss[2];
										pss[0] = 2.0 * (1.0 * px / (wd->getLen(0) - 1) - 0.5);
										pss[1] = 2.0 * (1.0 * py / (wd->getLen(1) - 1) - 0.5);

										this_is_the_player->set(Constants::io__sprite_double::POSX, pss[0]);
										this_is_the_player->set(Constants::io__sprite_double::POSY, pss[1]);
										_don = true;
									}
								}
							}
						}
					}

					{
						textures.customLoad("GAMEBOXMAIN_I", [](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_bitmap(1600, 900)); });

						Sprite* s = sprites.create("GAMEBOXMAIN");
						s->set(Constants::io__sprite_string::ID, "GAMEBOX_MAIN");
						s->set(Constants::io__sprite_boolean::DRAW, true); // no draw for now
						s->set(Constants::io__sprite_collision_mode::COLLISION_TRANSPARENT);
						s->set(Constants::io__sprite_boolean::SHOWDOT, false);
						s->set(Constants::io__sprite_boolean::SHOWBOX, false);
						s->set(Constants::io__sprite_double::SCALEG, 2.0);
						s->set(Constants::io__sprite_boolean::TIE_SIZE_TO_DISPLAY, true);
						s->set(Constants::io__sprite_double::RO_HAPPENED_RESIZE_DISPLAY, al_get_time());
						s->set(Constants::io__sprite_string::ADD, "GAMEBOXMAIN_I");
						s->set(Constants::io__sprite_integer::LAYER, 0);
						s->hook(Constants::io__sprite_tie_func_to_state::WHEN_BITMAPS_RESIZED_AUTO, [&assist]() {
							assist[2] = false;
							});
						s->hook(Constants::io__sprite_tie_func_to_state::WHEN_DRAWING, [&sprites, &gcam, s, &assist]() {
							if (!assist[2]) {
								auto vct = sprites.getList([](const std::string a)->bool {return a.find("GAMEBOX_") == 0; });
								ALLEGRO_BITMAP* trg = nullptr;
								s->get(trg);
								ALLEGRO_BITMAP* old_trg = al_get_target_bitmap();
								al_set_target_bitmap(trg);
								al_clear_to_color(al_map_rgb(0, 0, 0));

								for (auto& i : vct) {
									i->set(Constants::io__sprite_boolean::DRAW, true);
									i->draw(0);
									i->set(Constants::io__sprite_boolean::DRAW, false);
								}

								al_set_target_bitmap(old_trg);
								assist[2] = true;
							}
							});
					}
					
					consol.resumeThread();
					has_map_gen = true;
				}
			}
			else {
				if (has_map_gen) {
					consol.pauseThread();
					while (!consol.hasThreadPaused()) Sleep(10);
					delete wd;
					wd = nullptr;
					sprites.remove([](const std::string a)->bool {return a.find("GAMEBOX_") == 0; });
					sprites.remove("GAMEBOXMAIN");
					sprites.remove("PLAYER");
					texts.remove("PLAYER_T");
					consol.resumeThread();
					has_map_gen = false;
				}
			}
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
