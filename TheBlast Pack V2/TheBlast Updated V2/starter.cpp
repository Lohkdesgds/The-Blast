#define GAME_THEBLAST_CLIENT
#include "self_only.h"

#include <iostream>
#include <string>
#include <Windows.h>

// nvidia stuff
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}


using namespace LSW::v4;


enum class main_gamemodes { HASNT_STARTED_YET = -2, LOADING, MENU, OPTIONS, PAUSE, GAMING, MULTIPLAYER };
enum class my_custom_funcs { LOADING_ANIMATION/*at: setup_animation_functional (starter.cpp)*/, BUBBLE_TASK/*at: set_consol_mainthread_start*/, GAMING_PLAYER_EXIT_TASK, WALK_TASK };

int main(int argc, const char* argv[])
{
	try {
		// this has to be like the first one
		gfile logg;
		DiscordConnection* connection = nullptr;

		try {
			connection = new DiscordConnection();
			connection->lock();
			connection->getActivity().GetAssets().SetLargeImage("icon_large");
			connection->getActivity().GetAssets().SetSmallImage("icon_large");
			connection->getActivity().GetAssets().SetLargeText("The Blast V2");
			connection->getActivity().GetAssets().SetSmallText("by LSW");
			connection->getActivity().SetName("Loading");
			connection->getActivity().SetDetails("");
			connection->unlock();
		}
		catch (Abort::warn w) {
			logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Got Discord integration error: " << w.from() << " -> " << w.details() << "; errID: " << w.getErrN() << L::ELF;
		}
		catch (Abort::abort a) {
			throw a; // must close the game, so throw to main try
		}

		/*************************************************************************************

			# Start stuff:
			> Variables, some tests, initializing the basics

		**************************************************************************************/


		// used by lambdas
		main_gamemodes modern = main_gamemodes::HASNT_STARTED_YET;
		bool __assist[3] = { false, false, false }; // 1: LOADING_ANIMATION, 2: FULLSCREEN/WINDOW TOGGLE, 3: GAMEBOXMAIN Sprite down below on loop
		main_gamemodes __assist_g = modern;
		double __assist_t_button = 0;


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
		Mixer mixer;
		Manager consol;
		Database conf;

		BubblesFX bubble;
		LiningFX lines;

		if (conf.isEq(Constants::io__conf_boolean::ENABLE_SECOND_DEBUGGING_SCREEN, true)) logg.callDebugScreen(); // debugging lmao

		logg << L::SLF << fsr(__FUNCSIG__) << "Setting up dynamic functions..." << L::ELF;


		// quick var setup (fullscreen / windowed)
		{
			int __s;
			conf.get(Constants::io__conf_integer::SCREEN_FLAGS, __s);
			__assist[1] = (__s & ALLEGRO_FULLSCREEN_WINDOW) || (__s & ALLEGRO_FULLSCREEN);
		}

		// here quick test
		const std::function<bool(const main_gamemodes)> isGamingLocally = [](const main_gamemodes u)->bool {return (u == main_gamemodes::PAUSE || u == main_gamemodes::GAMING); };

		// functional-only lambdas
		const std::function< int(void)> reapply_cam_settings					= [&gcam, &modern] {
			gcam.apply(+modern);
			return 0;
		};
		const std::function< int(void)> bubble_think_task						= [&bubble, &gcam]()->int {
			auto cpy = gcam.get();
			for (auto& o : cpy) if (bubble.think(o)) return 0;
			return 0;
		};
		const std::function< int(void)> fxs_draw_task							= [&bubble, &lines, &gcam]()->int {
			auto cpy = gcam.get();
			bool hasdone[2] = { false,false };
			for (auto& o : cpy) {
				if (!hasdone[0]) hasdone[0] |= bubble.draw(o);
				if (!hasdone[1]) hasdone[1] |= lines.draw(o);
				if (hasdone[0] && hasdone[1]) break;
			}
			return 0;
		};
		const std::function< int(void)> guarantee_walk_task						= [&modern, &isGamingLocally, &sprites, &tracks]{
			Track* t;
			if (!tracks.get("WALK", t)) {
				return 0;
			}
			if (isGamingLocally(modern) && modern != main_gamemodes::PAUSE) {
				for (auto& i : sprites) {
					double vel[2];
					i->self->get(Constants::io__sprite_double::RO_SPEEDX, vel[0]);
					i->self->get(Constants::io__sprite_double::RO_SPEEDY, vel[1]);

					if (fabs(vel[0]) > 0.002 || fabs(vel[1]) > 0.002) {

						t->set(Constants::io__track_boolean::PLAYING, true);
						return 0;
					}
				}
			}
			t->set(Constants::io__track_boolean::PLAYING, false);
			return 0;
		};

		// used ONCE lambdas (pre-determined normally huge start and end lambdas
		const std::function<void(void)> set_consol_mainthread_end				= [&textures, &sprites, &texts, &fonts]() {
			sprites.clear(); texts.clear(); fonts.clear(); textures.clear();
		};
		const std::function<void(void)> set_consol_mainthread_start				= [&bubble_think_task, &bubble, &lines, &consol]() {

			const int default_bitmap_size = 4096; // 4096x4096

			const double animsize[2] = { 0.35, 0.07 };

			// raw allegro stuff
			Textures textures;
			Fonts fonts;
			Samples samples;

			// controllers
			Tracks tracks;

			gfile logg;

			logg << L::SLF << fsr(__FUNCSIG__) << "Got OPENGL version " << al_get_new_display_option(ALLEGRO_OPENGL_MAJOR_VERSION, 0) << "." << al_get_new_display_option(ALLEGRO_OPENGL_MINOR_VERSION, 0) << L::ELF;

			logg << L::SLF << fsr(__FUNCSIG__) << "Loading resources (lambda after function)" << L::ELF;

			__g_sys.setInterface();

			bubble.init(200, -1, -10001); // MENU
			lines.init(-10002); // PAUSE
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
			auto __introtrack = tracks.create("LOADING_MUSIC");
			__introtrack->set(Constants::io__track_string::ID, "LOADING_MUSIC");
			__introtrack->set(Constants::io__track_string::LOADID, "LOADING");
			__introtrack->set(Constants::io__track_integer::PLAYMODE, Constants::io__track_integer_modes::LOOP);
			__introtrack->set(Constants::io__track_boolean::PLAYING, true);


			logg << L::SLF << fsr(__FUNCSIG__) << "Initializing display, events and stuff..." << L::ELF;

			draw_simple_bar(0.10, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading atlas...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();

			auto atlas_wild = textures.load("ATLAS0", "atlas0.png");

			double prop[2] = { al_get_bitmap_width(atlas_wild) * 1.0 / default_bitmap_size ,al_get_bitmap_height(atlas_wild) * 1.0 / default_bitmap_size };

			for (int q = 0; q < 8; q++) {
				for (int p = 0; p < 4; p++) {
					if ((q < 7) || (p == 0)) {
						textures.customLoad("PAUSE_" + std::string((p + q * 4) < 10 ? "0" : "") + std::to_string(p + q * 4),
							[&atlas_wild, &p, &q, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, p * 512 * prop[0], q * 128 * prop[1], 512, 128)); }
						);

						draw_simple_bar(0.10 + ((p + q * 4) * 0.25f / 31), al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading pause animation #" + std::string((p + q * 4) < 10 ? "0" : "") + std::to_string(p + q * 4) + "...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
					}
				}
			}
			for (int q = 0; q < 22; q++) {
				for (int p = 0; p < 4; p++) {
					if ((q < 21) || (p == 0)) {
						textures.customLoad("LOGO_" + std::string((p + q * 4) < 10 ? "0" : "") + std::to_string(p + q * 4),
							[&atlas_wild, &p, &q, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, (p * 512 + 2048) * prop[0], q * 128 * prop[1], 512, 128)); }
						);

						draw_simple_bar(0.35 + ((p + q * 4) * 0.45f / 87), al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading logo animation #" + std::string((p + q * 4) < 10 ? "0" : "") + std::to_string(p + q * 4) + "...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();
					}
				}
			}

			draw_simple_bar(0.80, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading main textures...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();


			textures.customLoad("BACKGROUND_START", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 2560 * prop[1], 2048 * prop[0], 1154 * prop[1])); });
			textures.customLoad("BAR_OFF", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512 * prop[0], 1408 * prop[1], 1024 * prop[0], 128 * prop[1])); });
			textures.customLoad("BAR_ON", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512 * prop[0], 1280 * prop[1], 1024 * prop[0], 128 * prop[1])); });
			textures.customLoad("MOUSE", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1536 * prop[0], 1024 * prop[1], 256 * prop[0], 256 * prop[1])); });
			textures.customLoad("MAIN_LOGO", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512 * prop[0], 1024 * prop[1], 1024 * prop[0], 256 * prop[1])); });


			draw_simple_bar(0.85, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Loading blocks...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();

			textures.customLoad("BLOCK_00", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 1536 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// BLOCK
			textures.customLoad("BLOCK_01", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 1024 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// DIRT
			textures.customLoad("BLOCK_02", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1536 * prop[0], 2048 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// LIFE
			textures.customLoad("BLOCK_03", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512 * prop[0], 2048 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// ?
			textures.customLoad("BLOCK_05", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1024 * prop[0], 2048 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// X
			textures.customLoad("BLOCK_06", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 512 * prop[0], 1536 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// DESTR 1
			textures.customLoad("BLOCK_07", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1024 * prop[0], 1536 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// DESTR 2
			textures.customLoad("BLOCK_08", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 1536 * prop[0], 1536 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// DESTR 3
			textures.customLoad("BLOCK_09", [&atlas_wild, &prop](ALLEGRO_BITMAP*& b) -> bool {return (b = al_create_sub_bitmap(atlas_wild, 0, 2048 * prop[1], 512 * prop[0], 512 * prop[1])); }); /// DESTR 4



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

			draw_simple_txt(ff, "Preparing soundtrack...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();

			// already loaded: LOADING_MUSIC
			// loaded later: JUMP, MUSIC_0, MUSIC_1, MUSIC_GAMING, WALK

			Track* tr = tracks.create("JUMP");
			tr->set(Constants::io__track_string::LOADID, "JUMP");
			tr->set(Constants::io__track_string::ID, "JUMP");

			tr = tracks.create("MUSIC_0");
			tr->set(Constants::io__track_string::LOADID, "MUSIC_0");
			tr->set(Constants::io__track_string::ID, "MUSIC_0");
			tr->set(Constants::io__track_integer::PLAYMODE, Constants::io__track_integer_modes::LOOP);

			tr = tracks.create("MUSIC_1");
			tr->set(Constants::io__track_string::LOADID, "MUSIC_1");
			tr->set(Constants::io__track_string::ID, "MUSIC_1");
			tr->set(Constants::io__track_integer::PLAYMODE, Constants::io__track_integer_modes::LOOP);

			tr = tracks.create("MUSIC_GAMING");
			tr->set(Constants::io__track_string::LOADID, "MUSIC_2");
			tr->set(Constants::io__track_string::ID, "MUSIC_GAMING");
			tr->set(Constants::io__track_integer::PLAYMODE, Constants::io__track_integer_modes::LOOP);

			tr = tracks.create("WALK");
			tr->set(Constants::io__track_string::LOADID, "WALK");
			tr->set(Constants::io__track_string::ID, "WALK");
			tr->set(Constants::io__track_integer::PLAYMODE, Constants::io__track_integer_modes::LOOP);
			tr->set(Constants::io__track_double::VOLUME, 1.4);



			draw_simple_bar(0.98, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Verifying self existance...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();

			al_convert_bitmaps();

			logg << L::SLF << fsr(__FUNCSIG__) << "Loaded everything." << L::ELF;

			draw_simple_bar(0.99, al_map_rgb(0, 0, 0), animsize[0], animsize[1]); draw_simple_txt(ff, "Starting game...", al_map_rgb(255, 255, 255), 1, 0.04); al_flip_display();


		};

		// button related (also quick function call)
		const std::function<void(void)> setup_animation_functional				= [&consol, &modern, &sprites, &__assist, &logg]()->void {
			logg << L::SL << fsr(__FUNCSIG__) << "Setting up LOADING_ANIM animation" << L::EL;
			consol.addCustomTask([&]()->int {
				//logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "(lambda animation task)" << L::EL;
				Sprite* s = nullptr; if (!sprites.get("LOADING_ANIM", s)) return 0; // if running while program is being closed it can hurt
				if (modern != main_gamemodes::LOADING) {
					__assist[0] = false;
					modern = main_gamemodes::LOADING;
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Resetting LOADING_ANIM animation to then run LOADING animation" << L::EL;
				}
				else if (s->isEq(Constants::io__sprite_boolean::IS_IT_ON_LAST_FRAME, true) && !__assist[0])
				{
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Done doing LOADING animation" << L::EL;
					__assist[0] = true;
					return -40; // 40 cycles because it updates 40 per sec //Sleep(2000);
				}
				else if (__assist[0]) {
					modern = main_gamemodes::MENU;
					s->set(Constants::io__sprite_double::ANIMATION_FPS, 0);
					logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "Finally removed itself" << L::EL;
					consol.removeCustomTask(+my_custom_funcs::LOADING_ANIMATION);
				}
				return 0;
				}, +my_custom_funcs::LOADING_ANIMATION, 1.0 / 20);
			logg << L::SL << fsr(__FUNCSIG__) << "Has set LOADING_ANIM to run" << L::EL;
		};
		const std::function<void(void)> skip_animation_functional				= [&__assist] {
			__assist[0] = true;
		};
		const std::function<void(void)> exit_game_versatile						= [&consol, &__assist_t_button] {
			if (al_get_time() - __assist_t_button > Constants::default_delay_click) {
				consol.shouldStop();
			}
		};

		const std::function<void(const main_gamemodes)> versatile_select		= [&modern, &__assist_g, &__assist_t_button](const main_gamemodes gt) {
			if (al_get_time() - __assist_t_button > Constants::default_delay_click) {
				modern = gt;
				__assist_t_button = al_get_time();
			}
		};

		// keybinds
		const std::function<void(void)> keyb_fullscreen_window					= [&consol, &__assist]() {
			consol.sendEvent(Constants::ro__my_events::CUSTOM_EVENT_CALL_FULLSCREEN, (intptr_t)(__assist[1] = !__assist[1]));
		};
		const int						keyb_fullscreen_window_key				= ALLEGRO_KEY_F11;
		const std::function<void(void)> keyb_printscreen						= [&consol]() {
			if (consol._display_handle()) consol._display_handle()->print();
		};
		const int						keyb_printscreen_key					= ALLEGRO_KEY_F2;

		const std::function<void(void)> keyb_kbf_go_north						= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::GO_NORTH);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_south						= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::GO_SOUTH);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_west						= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::GO_WEST);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_east						= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::GO_EAST);
				}
			}
		};

		const std::function<void(void)> keyb_kbf_go_reset_north					= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::STOP_NORTH);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_reset_south					= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::STOP_SOUTH);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_reset_west					= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::STOP_WEST);
				}
			}
		};
		const std::function<void(void)> keyb_kbf_go_reset_east					= [&sprites]() {
			for (auto& i : sprites) {
				if (i->self->isEq(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true)) {
					i->self->set(Constants::cl__sprite_direction_mult::STOP_EAST);
				}
			}
		};

		const std::function<void(void)> keyb_kbf_pause_switch					= [&modern, &__assist_g]() {

			if (modern == main_gamemodes::GAMING || modern == main_gamemodes::MULTIPLAYER) {
				__assist_g = modern;
				modern = main_gamemodes::PAUSE;
			}
			else if (modern == main_gamemodes::PAUSE) {
				modern = __assist_g;
			}
		};

		const std::function<void(void)> keyb_kbf_osd_switch						= [&texts, &conf]() {
			auto lit = texts.getList([](const std::string a)->bool {return a.find("osd_stuff") == 0; });

			if (lit.size() > 0) {
				bool def = false;
				lit[0]->get(Constants::io__text_boolean::SHOW, def);
				for (auto& i : lit) i->set(Constants::io__text_boolean::SHOW, !def);
				conf.set(Constants::io__conf_boolean::WAS_OSD_ON, !def);
			}
		};

		const int						keyb_kbf_go_north_key					= ALLEGRO_KEY_W;
		const int						keyb_kbf_go_south_key					= ALLEGRO_KEY_S;
		const int						keyb_kbf_go_west_key					= ALLEGRO_KEY_A;
		const int						keyb_kbf_go_east_key					= ALLEGRO_KEY_D;

		const int						keyb_kbf_go_north_key_a					= ALLEGRO_KEY_UP;
		const int						keyb_kbf_go_south_key_a					= ALLEGRO_KEY_DOWN;
		const int						keyb_kbf_go_west_key_a					= ALLEGRO_KEY_LEFT;
		const int						keyb_kbf_go_east_key_a					= ALLEGRO_KEY_RIGHT;

		const int						keyb_kbf_pause_switch_key				= ALLEGRO_KEY_ESCAPE;

		const int						keyb_kbf_osd_switch_key					= ALLEGRO_KEY_F3;


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

			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_pause_switch_key, keyb_kbf_pause_switch);

			conf.set(Constants::io__db_functional_opt::KEYBOARD_KEY, keyb_kbf_osd_switch_key, keyb_kbf_osd_switch);
		}


		consol.setSimpleTask(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::END, set_consol_mainthread_end);
		consol.setSimpleTask(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::START, set_consol_mainthread_start);
		consol.setSimpleTask(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::LOOP, [&reapply_cam_settings, &fxs_draw_task]() {reapply_cam_settings(); fxs_draw_task(); });
		consol.addCustomTask(guarantee_walk_task, +my_custom_funcs::WALK_TASK, 1.0 / 20);
		//consol.addCustomTask(reapply_cam_settings, +my_custom_funcs::UPDATE_CAMERA, 1.0 / 20);

		reapply_cam_settings(); // apply once to just not blink


		/*************************************************************************************

			# Starting up screen and stuff:
			> Start consol so things can actually happen
			> It has to start to work

		**************************************************************************************/


		logg << L::SLF << fsr(__FUNCSIG__) << "Launching..." << L::ELF;

		consol.start();

		logg << L::SLF << fsr(__FUNCSIG__) << "Waiting Manager to start..." << L::ELF;
		logg.flush();

		while (!consol.isOpen()) Sleep(20);
		if (!consol.isRunning()) return -1;

		al_set_window_title(consol._display_handle()->getDisplay(), "The Blast V2.0");

		while (consol.hasTasked(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::START)) Sleep(20);
		logg << L::SLF << fsr(__FUNCSIG__) << "Time to work on Sprites, Texts and stuff." << L::ELF;


		/*************************************************************************************

			# Creating variables part:
			> Creating the actual game here (obviously after loading the texture as you can see)
			> Sprites, Texts, Music and stuff

		**************************************************************************************/


		// LOCK GAME TO SET UP EVERYTHING
		consol.pauseThread();
		while (!consol.hasThreadPaused()) Sleep(10);

		// USED BY MANY
		Entity* this_is_the_player = nullptr;

		/// >>>>>>> GAME SPRITES, TEXTS AND STUFF BRACKET <<<<<<< ///
		{
			/*
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>--- Camera preset loading ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			*/

			{
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
					gcam.set(cp, +main_gamemodes::HASNT_STARTED_YET); // same
					gcam.set(cp, +main_gamemodes::LOADING, [&connection](camera_preset& mee)->void {
						connection->lock();
						connection->getActivity().SetDetails("on Menu");
						connection->getActivity().GetTimestamps().SetStart(0);
						connection->unlock();
						});
				}
				{
					// menu
					camera_preset cp;
					cp.setInternalID("Menu preset");
					cp.setLayer(-10001, true); // bubbles
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
					gcam.set(cp, +main_gamemodes::MENU, [&this_is_the_player,&connection](camera_preset& mee)->void {
						if (this_is_the_player) {
							this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, false);
						}
						connection->lock();
						connection->getActivity().SetDetails("on Menu");
						connection->getActivity().GetTimestamps().SetStart(0);
						connection->unlock();
						});
				}
				{
					// options
					camera_preset cp;
					cp.setInternalID("Options preset");
					cp.setLayer(-10001, true); // bubbles
					cp.setLayer(-9, true);
					cp.setLayer(99, true); // mouse
					cp.setLayer(-99, true); // bubble animation
					cp.setLayer(100, true); // DEBUG LINE
					cp.set(Constants::io__camera_boolean::RESPECT_LIMITS, true);
					cp.set(Constants::io__camera_double::LIMIT_MIN_X, -0.51);
					cp.set(Constants::io__camera_double::LIMIT_MIN_Y, 0.052);
					cp.set(Constants::io__camera_double::LIMIT_MAX_X, 0.51);
					cp.set(Constants::io__camera_double::LIMIT_MAX_Y, 0.542);
					cp.set(Constants::io__camera_boolean::READONLY_NOW, true);
					gcam.set(cp, +main_gamemodes::OPTIONS, [&conf,&this_is_the_player,&connection](camera_preset& mee)->void {
						double dy = 0;
						conf.get(Constants::ro__db_mouse_double::MOUSE_Y, dy);
						camera_preset cpy = mee;
						cpy.set(Constants::io__camera_boolean::READONLY_NOW, false); // override
						cpy.set(Constants::io__camera_double::OFFSET_Y, 0.45 * dy - 0.35);
						cpy.set(Constants::io__camera_boolean::READONLY_NOW, true);
						mee = cpy; // always locked
						if (this_is_the_player) {
							this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true);
							this_is_the_player->set(Constants::io__sprite_double::SCALEG, 0.068);
						}
						connection->lock();
						connection->getActivity().SetDetails("on Options");
						connection->getActivity().GetTimestamps().SetStart(0);
						connection->unlock();
						});
				}
				{
					// pause
					camera_preset cp;
					cp.setInternalID("Pause preset");
					cp.setLayer(-10002, true); // lines
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
					gcam.set(cp, +main_gamemodes::PAUSE, [&this_is_the_player,&connection](camera_preset& mee)->void {
						if (this_is_the_player) {
							this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, false);
						}
						connection->lock();
						connection->getActivity().SetDetails("Paused the game");
						connection->unlock();
						});
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
					gcam.set(cp, +main_gamemodes::GAMING, [&this_is_the_player,&connection](camera_preset& mee)->void {
						if (this_is_the_player) {
							this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true);
						}
						connection->lock();
						connection->getActivity().SetDetails("Singleplayer");
						connection->unlock();
						});
				}
				{
					// multiplayer
					camera_preset cp;
					cp.setInternalID("Multiplayer preset (dynamic)");
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
					gcam.set(cp, +main_gamemodes::MULTIPLAYER, [&this_is_the_player,&connection](camera_preset& mee)->void {
						if (this_is_the_player) {
							this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true);
						}
						connection->lock();
						connection->getActivity().SetDetails("Multiplayer");
						connection->unlock();
						});
				}
			}

			/*
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>--- Sprite preset loading ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			*/

			// > > > > > |[}-----> UNDOUBTLY GLOBAL UNIQUE PPL
			{
				{
					Sprite* s = sprites.create("MOUSE");
					s->set(Constants::io__sprite_string::ID, "MOUSE");
					s->set(Constants::io__sprite_boolean::FOLLOWMOUSE, true);
					s->set(Constants::io__sprite_collision_mode::COLLISION_ANY_HOLD_STATIC);
					s->set(Constants::io__sprite_boolean::DRAW, true); // no draw for now
					s->set(Constants::io__sprite_boolean::SHOWBOX, false); // no draw for now
					s->set(Constants::io__sprite_boolean::SHOWDOT, false); // no draw for now
					s->set(Constants::io__sprite_double::SCALEG, 0.1);
					s->set(Constants::io__sprite_string::ADD, "MOUSE");
					s->set(Constants::io__sprite_integer::LAYER, 99);
					//s->set(Constants::io__sprite_integer::LAYER, -10); // TEMP
					s->set(Constants::io__sprite_double::SPEEDROT, 15.0 / Constants::__i_col_pos_t_update);
				}

				{
					ALLEGRO_COLOR cl;
					conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
					{
						ALLEGRO_BITMAP* s_bmp = textures.customLoad("PLAYER_BMP", [&](ALLEGRO_BITMAP*& b) -> bool { return (b = al_create_bitmap(64, 64)); }); // don't worry, it won't create if it is already there
						ALLEGRO_BITMAP* curr = al_get_target_bitmap();
						al_set_target_bitmap(s_bmp);
						al_clear_to_color(al_map_rgb(255,255,255));
						al_set_target_bitmap(curr);
					}

					this_is_the_player = (Entity*)sprites.customLoad("PLAYER", [](Sprite*& u)->bool {return (u = (Sprite*)new Entity()); });//sprites.create("PLAYER");
					this_is_the_player->set(Constants::io__sprite_string::ID, "PLAYER");
					this_is_the_player->set(Constants::io__sprite_boolean::DRAW, true);
					this_is_the_player->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
					this_is_the_player->set(Constants::io__sprite_collision_mode::COLLISION_SAMELAYER_ROUGH);
					///this_is_the_player->set(Constants::io__sprite_boolean::FOLLOWKEYBOARD, true); // enabled in game only
					//this_is_the_player->set(Constants::io__sprite_boolean::SHOWDOT, true);
					this_is_the_player->set(Constants::io__sprite_boolean::SHOWBOX, false);
					this_is_the_player->set(Constants::io__sprite_double::SCALEG, 0.065);
					this_is_the_player->set(Constants::io__sprite_double::POSX, 1.0);
					this_is_the_player->set(Constants::io__sprite_double::POSY, -1.0);
					this_is_the_player->set(Constants::io__sprite_double::SCALEY, 1.77777778);
					this_is_the_player->set(Constants::io__sprite_string::ADD, "PLAYER_BMP");
					this_is_the_player->set(Constants::io__sprite_integer::LAYER, 1);
					this_is_the_player->set(Constants::io__sprite_integer::ADD_ALT_LAYER, -9);
					this_is_the_player->set(Constants::io__sprite_color::TINT, cl);

					std::string its_name = "Player";
					conf.get(Constants::io__conf_string::LAST_PLAYERNAME, its_name);
					this_is_the_player->set(Constants::io__entity_string::NICKNAME, its_name);


					Text* t = texts.create("PLAYER_T");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "%entity_name% [%entity_health%]");
					t->set(Constants::io__text_string::ID, "PLAYER_T");
					t->set(Constants::io__text_boolean::SHOW, true);
					t->set(Constants::io__text_boolean::USE_SPRITE_TINT_INSTEAD, true);
					t->set(Constants::io__text_double::SHADOW_DIST_X, 0.00175);
					t->set(Constants::io__text_double::SHADOW_DIST_Y, 0.0025);
					t->set(Constants::io__text_double::SCALEG, 0.041);
					t->set(Constants::io__text_double::SCALEX, 0.7);
					t->set(Constants::io__text_double::POSY, -0.145);
					t->set(Constants::io__text_string::FOLLOW_SPRITE, "PLAYER");
					t->set(Constants::io__text_integer::LAYER, 1); 
					t->set(Constants::io__text_integer::ADD_ALT_LAYER, -9);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 5);
					t->set(Constants::io__text_color::COLOR, al_map_rgb(255, 125, 125));
				}
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


			// LOADING PART
			{
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
					s->set(Constants::io__sprite_collision_mode::COLLISION_TRANSPARENT);
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

			// MENU PART
			{
				{
					Sprite* s = sprites.create("MENU_LOGO");
					s->set(Constants::io__sprite_string::ADD, "MAIN_LOGO");
					s->set(Constants::io__sprite_string::ID, "MENU_LOGO");
					s->set(Constants::io__sprite_boolean::DRAW, true);
					s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
					s->set(Constants::io__sprite_collision_mode::COLLISION_TRANSPARENT);
					s->set(Constants::io__sprite_double::SCALEG, 1.20);
					s->set(Constants::io__sprite_double::SCALEY, 0.5);
					s->set(Constants::io__sprite_double::POSY, -0.55);
					s->set(Constants::io__sprite_double::ANIMATION_FPS, 1.0 / 24);
					s->set(Constants::io__sprite_integer::LAYER, -10);
				}
				{
					Sprite* s = sprites.create("BUTTON_MENU_0");
					s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
					s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
					s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
					s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
					s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05,1.0,0.45,1.0));
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
					s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&versatile_select]() {versatile_select(main_gamemodes::GAMING); }); // [&modern]()->void {modern = main_gamemodes::GAMING; });
					s->set(Constants::io__sprite_string::ID, "BUTTON_MENU_0");
					s->set(Constants::io__sprite_boolean::DRAW, true);
					s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
					s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
					s->set(Constants::io__sprite_double::SCALEG, 0.18);
					s->set(Constants::io__sprite_double::SCALEX, 5.5);
					s->set(Constants::io__sprite_double::POSY, 0.25);
					s->set(Constants::io__sprite_integer::LAYER, -10);

					Text* t = texts.create("BUTTON_MENU_0_T");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "Start Game");
					t->set(Constants::io__text_string::ID, "BUTTON_MENU_0_T");
					t->set(Constants::io__text_boolean::SHOW, true);
					t->set(Constants::io__text_double::SCALEG, 0.08);
					t->set(Constants::io__text_double::SCALEX, 0.7);
					t->set(Constants::io__text_double::POSY, -0.055);
					t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_MENU_0");
					t->set(Constants::io__text_integer::LAYER, -10);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 20);
					t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
					t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [](std::string& str) {std::transform(str.begin(), str.end(), str.begin(), ::toupper); });
				}
				{
					Sprite* s = sprites.create("BUTTON_MENU_1");
					s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
					s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
					s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
					s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
					s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
					s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&versatile_select]() {versatile_select(main_gamemodes::OPTIONS); }); // setup_animation_functional);
					s->set(Constants::io__sprite_string::ID, "BUTTON_MENU_1");
					s->set(Constants::io__sprite_boolean::DRAW, true);
					s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
					s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
					s->set(Constants::io__sprite_double::SCALEG, 0.18);
					s->set(Constants::io__sprite_double::SCALEX, 5.5);
					s->set(Constants::io__sprite_double::POSY, 0.45);
					s->set(Constants::io__sprite_integer::LAYER, -10);

					Text* t = texts.create("BUTTON_MENU_1_T");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "Options & About");
					t->set(Constants::io__text_string::ID, "BUTTON_MENU_1_T");
					t->set(Constants::io__text_boolean::SHOW, true);
					t->set(Constants::io__text_double::SCALEG, 0.08);
					t->set(Constants::io__text_double::SCALEX, 0.7);
					t->set(Constants::io__text_double::POSY, -0.055);
					t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_MENU_1");
					t->set(Constants::io__text_integer::LAYER, -10);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 20);
					t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
					t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [](std::string& str) {std::transform(str.begin(), str.end(), str.begin(), ::toupper); });
				}
				{
					Sprite* s = sprites.create("BUTTON_MENU_2");
					s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
					s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
					s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
					s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
					s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
					s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, exit_game_versatile);
					s->set(Constants::io__sprite_string::ID, "BUTTON_MENU_2");
					s->set(Constants::io__sprite_boolean::DRAW, true);
					s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
					s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
					s->set(Constants::io__sprite_double::SCALEG, 0.18);
					s->set(Constants::io__sprite_double::SCALEX, 5.5);
					s->set(Constants::io__sprite_double::POSY, 0.65);
					s->set(Constants::io__sprite_integer::LAYER, -10);

					Text* t = texts.create("BUTTON_MENU_2_T");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "Exit Game");
					t->set(Constants::io__text_string::ID, "BUTTON_MENU_2_T");
					t->set(Constants::io__text_boolean::SHOW, true);
					t->set(Constants::io__text_double::SCALEG, 0.08);
					t->set(Constants::io__text_double::SCALEX, 0.7);
					t->set(Constants::io__text_double::POSY, -0.055);
					t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_MENU_2");
					t->set(Constants::io__text_integer::LAYER, -10);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 20);
					t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
					t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [](std::string& str) {std::transform(str.begin(), str.end(), str.begin(), ::toupper); });
				}
			}

			// OPTIONS PART
			{ // -09
				{
					size_t counter = 0;
					double _posy = -1.95;

					// > > > > > |[}-----> OSD SWITCH
					{
						Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
						s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
						s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
						s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
						s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
						s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&keyb_kbf_osd_switch]() { keyb_kbf_osd_switch(); }); // [&modern,&__assist_g]()->void {modern = __assist_g; });
						s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::DRAW, true);
						s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
						s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
						s->set(Constants::io__sprite_double::SCALEG, 0.16);
						s->set(Constants::io__sprite_double::SCALEX, 5.5);
						s->set(Constants::io__sprite_integer::LAYER, -9);
						s->set(Constants::io__sprite_double::POSY, _posy);

						Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
						t->set(Constants::io__text_string::FONT, "DEFAULT");
						t->set(Constants::io__text_string::STRING, "Switch OSD");
						t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
						t->set(Constants::io__text_boolean::SHOW, true);
						t->set(Constants::io__text_double::SCALEG, 0.07);
						t->set(Constants::io__text_double::SCALEX, 0.7);
						t->set(Constants::io__text_double::POSY, -0.055);
						t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
						t->set(Constants::io__text_integer::LAYER, -9);
						t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
						t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
						t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [&conf](std::string& str) {
							bool is_osd_enabled = true;
							conf.get(Constants::io__conf_boolean::WAS_OSD_ON, is_osd_enabled);
							str = std::string("Right now it is: ") + (is_osd_enabled ? "enabled" : "disabled");
							});
						t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK, [](std::string& str) { str = "Switched!"; });
					}

					_posy += 0.2;
					// > > > > > |[}-----> FULLSCREEN SWITCH
					{
						Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
						s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
						s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
						s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
						s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
						s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&keyb_fullscreen_window]() {keyb_fullscreen_window(); }); // [&modern,&__assist_g]()->void {modern = __assist_g; });
						s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::DRAW, true);
						s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
						s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
						s->set(Constants::io__sprite_double::SCALEG, 0.16);
						s->set(Constants::io__sprite_double::SCALEX, 5.5);
						s->set(Constants::io__sprite_integer::LAYER, -9);
						s->set(Constants::io__sprite_double::POSY, _posy);

						Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
						t->set(Constants::io__text_string::FONT, "DEFAULT");
						t->set(Constants::io__text_string::STRING, "Toggle Fullscreen");
						t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
						t->set(Constants::io__text_boolean::SHOW, true);
						t->set(Constants::io__text_double::SCALEG, 0.07);
						t->set(Constants::io__text_double::SCALEX, 0.7);
						t->set(Constants::io__text_double::POSY, -0.055);
						t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
						t->set(Constants::io__text_integer::LAYER, -9);
						t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
						t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
						t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [&conf, &__assist](std::string& str) {
							bool is_osd_enabled = true;
							conf.get(Constants::io__conf_boolean::WAS_OSD_ON, is_osd_enabled);
							str = std::string("Right now it is: ") + (__assist[1] ? "Fullscreen" : "Windowed");
							});
						t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK, [](std::string& str) { str = "Switched!"; });
					}

					_posy += 0.2;
					// > > > > > |[}-----> VOLUME CONTROL
					{
						// lower
						{
							Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
							s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
							s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
							s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_boolean::DRAW, true);
							s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
							s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
							s->set(Constants::io__sprite_double::SCALEG, 0.16);
							s->set(Constants::io__sprite_double::SCALEX, 5.5);
							s->set(Constants::io__sprite_integer::LAYER, -9);
							s->set(Constants::io__sprite_double::POSY, _posy);
							s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&conf, &mixer]() {
								double dx = 0;
								conf.get(Constants::ro__db_mouse_double::MOUSE_X, dx);
								if (dx > 0.342) dx = 0.342;
								if (dx < -0.342) dx = -0.342;
								double vol = (dx + 0.342) / (0.342 * 2.0);
								mixer.volume(vol);
								}, true);

							Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
							t->set(Constants::io__text_string::FONT, "DEFAULT");
							t->set(Constants::io__text_string::STRING, "Volume: %volume_perc%%");
							t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
							t->set(Constants::io__text_boolean::SHOW, true);
							t->set(Constants::io__text_double::SCALEG, 0.07);
							t->set(Constants::io__text_double::SCALEX, 0.7);
							t->set(Constants::io__text_double::POSY, -0.055);
							t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
							t->set(Constants::io__text_integer::LAYER, -9);
							t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
							t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK, [t, &mixer](std::string& str) {
								str = "%volume_perc%%";
								t->set(Constants::io__text_double::POSX, mixer.getVolume() * (0.342 * 2.0) - 0.342);
								});
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [t](std::string& str) {
								t->set(Constants::io__text_double::POSX, 0.0);
								});
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_NONE, [t](std::string& str) {
								t->set(Constants::io__text_double::POSX, 0.0);
								});

						}
						// higher (dot)
						{
							Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
							s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
							s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
							s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [s, &mixer]() {	s->set(Constants::io__sprite_double::POSX, mixer.getVolume() * (0.342 * 2.0) - 0.342); }); // [&modern,&__assist_g]()->void {modern = __assist_g; });
							s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter++));
							s->set(Constants::io__sprite_boolean::DRAW, true);
							s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
							s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
							s->set(Constants::io__sprite_double::SCALEG, 0.16);
							s->set(Constants::io__sprite_double::SCALEX, 1.2);
							s->set(Constants::io__sprite_integer::LAYER, -9);
							s->set(Constants::io__sprite_double::POSY, _posy);
							s->set(Constants::io__sprite_double::POSX, mixer.getVolume() * (0.342 * 2.0) - 0.342);
						}
					}

					_posy += 0.2;
					// > > > > > |[}-----> FX CONTROL
					{
						// lower
						{
							Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
							s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
							s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
							s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_boolean::DRAW, true);
							s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
							s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
							s->set(Constants::io__sprite_double::SCALEG, 0.16);
							s->set(Constants::io__sprite_double::SCALEX, 5.5);
							s->set(Constants::io__sprite_integer::LAYER, -9);
							s->set(Constants::io__sprite_double::POSY, _posy);
							s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&conf, &consol, s]() {
								double dx = 0;
								conf.get(Constants::ro__db_mouse_double::MOUSE_X, dx);
								if (dx > 0.342) dx = 0.342;
								if (dx < -0.342) dx = -0.342;
								double vol = (dx + 0.342) / (0.342 * 2.0);
								if (conf.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, false)) vol = 0.0; // has to have doublebuffer
								consol.sendEvent(Constants::ro__my_events::CUSTOM_EVENT_DISPLAY_CHROMA_FX, +(vol * 1000));
								//conf.set(Constants::io__conf_double::FX_AMOUNT, vol);
								}, true);

							Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
							t->set(Constants::io__text_string::FONT, "DEFAULT");
							t->set(Constants::io__text_string::STRING, "Chroma failure FX: %screen_chroma_fx%");
							t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
							t->set(Constants::io__text_boolean::SHOW, true);
							t->set(Constants::io__text_double::SCALEG, 0.07);
							t->set(Constants::io__text_double::SCALEX, 0.7);
							t->set(Constants::io__text_double::POSY, -0.055);
							t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
							t->set(Constants::io__text_integer::LAYER, -9);
							t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
							t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK, [t, &conf](std::string& str) {
								double dd = 1.0;
								conf.get(Constants::io__conf_double::FX_AMOUNT, dd);
								double tx = (dd * (0.342 * 2.0) - 0.342);
								dd *= 100.0;
								if (conf.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, false)) {
									tx = -0.342;		/// TIED WITH UP VAL
									dd = -0.1;
								}

								char perc[32];
								if (dd >= 0.0) sprintf_s(perc, "%03.2lf%c", dd, '%');
								else sprintf_s(perc, "Needs Double Render!");
								str = perc;
								t->set(Constants::io__text_double::POSX, tx);
								});
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [t](std::string& str) {
								t->set(Constants::io__text_double::POSX, 0.0);
								});
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_NONE, [t](std::string& str) {
								t->set(Constants::io__text_double::POSX, 0.0);
								});

						}
						// higher (dot)
						{
							Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
							s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
							s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
							s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [s, &conf]() {
								double dd = 1.0;
								conf.get(Constants::io__conf_double::FX_AMOUNT, dd);
								double tx = (dd * (0.342 * 2.0) - 0.342);
								dd *= 100.0;
								if (conf.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, false)) tx = -0.342;		/// TIED WITH UP VAL
								s->set(Constants::io__sprite_double::POSX, tx);
								});
							s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter++));
							s->set(Constants::io__sprite_boolean::DRAW, true);
							s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
							s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
							s->set(Constants::io__sprite_double::SCALEG, 0.16);
							s->set(Constants::io__sprite_double::SCALEX, 1.2);
							s->set(Constants::io__sprite_integer::LAYER, -9);
							s->set(Constants::io__sprite_double::POSY, _posy);
							s->set(Constants::io__sprite_double::POSX, [&conf]()->double{
								double dd = 1.0;
								conf.get(Constants::io__conf_double::FX_AMOUNT, dd);
								double tx = (dd * (0.342 * 2.0) - 0.342);
								dd *= 100.0;
								if (conf.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, false)) tx = -0.342;		/// TIED WITH UP VAL
								return tx;
								}());
						}
					}

					_posy += 0.2;
					// > > > > > |[}-----> RESOLUTION CONTROL
					{
						// lower
						{
							Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
							s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
							s->set(Constants::io__sprite_color::TINT, [&conf]() {if (conf.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, true)) return  al_map_rgba_f(0.05, 1.0, 0.75, 1.0); else return al_map_rgba_f(0.85, 0.85, 0.05, 0.85); }()); // tied BELOW*
							s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_boolean::DRAW, true);
							s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
							s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
							s->set(Constants::io__sprite_double::SCALEG, 0.16);
							s->set(Constants::io__sprite_double::SCALEX, 5.5);
							s->set(Constants::io__sprite_integer::LAYER, -9);
							s->set(Constants::io__sprite_double::POSY, _posy);
							s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&conf, &consol, s]() {

								double dx = 0;
								conf.get(Constants::ro__db_mouse_double::MOUSE_X, dx);
								if (dx > 0.342) dx = 0.342;
								if (dx < -0.242) dx = -0.342;  /// TIED TO DOWN BELOW
								double vol = 0.2 + 1.8 * (dx + 0.242) / (0.342 + 0.242);
								bool is_enabled = (vol > 0.2);
								if (!is_enabled) vol = 1.0;

								consol.sendEvent(Constants::ro__my_events::CUSTOM_EVENT_DISPLAY_UPDATE_RESOLUTION_SCALE, +(vol * 100), is_enabled);
								if (is_enabled) s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.75, 1.0));  /// *
								else s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.85, 0.85, 0.05, 0.85));  /// *

								}, true);

							Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
							t->set(Constants::io__text_string::FONT, "DEFAULT");
							t->set(Constants::io__text_string::STRING, "Double render scale: %screen_buf_proportion%");
							t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
							t->set(Constants::io__text_boolean::SHOW, true);
							t->set(Constants::io__text_double::SCALEG, 0.07);
							t->set(Constants::io__text_double::SCALEX, 0.7);
							t->set(Constants::io__text_double::POSY, -0.055);
							t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
							t->set(Constants::io__text_integer::LAYER, -9);
							t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
							t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK, [t, &conf](std::string& str) {
								double dd = 1.0;
								conf.get(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION, dd);
								double tx = ((dd - 0.2) / 1.8) * (0.342 + 0.242) - 0.242;
								dd *= 100.0;
								if (conf.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, false)) {
									tx = -0.342;		/// TIED WITH UP VAL
									dd = 0.0;
								}

								char perc[32];
								if (dd > 0.0) sprintf_s(perc, "%03.2lf%c", dd, '%');
								else sprintf_s(perc, "Disabled");
								str = perc;
								t->set(Constants::io__text_double::POSX, tx);
								});
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [t](std::string& str) {
								t->set(Constants::io__text_double::POSX, 0.0);
								});
							t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_NONE, [t](std::string& str) {
								t->set(Constants::io__text_double::POSX, 0.0);
								});

						}
						// higher (dot)
						{
							Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
							s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
							s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
							s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
							s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [s, &conf]() {
								double dd = 1.0;
								conf.get(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION, dd);
								double tx = ((dd - 0.2) / 1.8) * (0.342 + 0.242) - 0.242;
								if (conf.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, false)) tx = -0.342;		/// TIED WITH UP VAL
								s->set(Constants::io__sprite_double::POSX, tx);
								});
							s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter++));
							s->set(Constants::io__sprite_boolean::DRAW, true);
							s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
							s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
							s->set(Constants::io__sprite_double::SCALEG, 0.16);
							s->set(Constants::io__sprite_double::SCALEX, 1.2);
							s->set(Constants::io__sprite_integer::LAYER, -9);
							s->set(Constants::io__sprite_double::POSY, _posy);
							s->set(Constants::io__sprite_double::POSX, [&conf]()->double{
								double dd = 1.0;
								conf.get(Constants::io__conf_double::RESOLUTION_BUFFER_PROPORTION, dd);
								double tx = ((dd - 0.2) / 1.8) * (0.342 + 0.242) - 0.242;
								if (conf.isEq(Constants::io__conf_boolean::DOUBLEBUFFERING, false)) tx = -0.342;		/// TIED WITH UP VAL
								return tx;
								}());
						}
					}

					_posy += 0.2;
					// > > > > > |[}-----> PLAYER NAMING
					{
						Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
						s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
						s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
						s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
						s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
						s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&conf]() {
							bool ua = false;
							conf.get(Constants::io__db_boolean::SAVING_STRING_INPUT, ua);
							conf.set(Constants::io__db_boolean::SAVING_STRING_INPUT, !ua);
							if (!ua) conf.set(Constants::io__db_sizet::MAXIMUM_STRING_INPUT_LEN, 15);
							}); // switch string input
						s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::DRAW, true);
						s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
						s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
						s->set(Constants::io__sprite_double::SCALEG, 0.16);
						s->set(Constants::io__sprite_double::SCALEX, 5.5);
						s->set(Constants::io__sprite_integer::LAYER, -9);
						s->set(Constants::io__sprite_double::POSY, _posy);

						Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
						t->set(Constants::io__text_string::FONT, "DEFAULT");
						t->set(Constants::io__text_string::STRING, "Nickname: ...");
						t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
						t->set(Constants::io__text_boolean::SHOW, true);
						t->set(Constants::io__text_double::SCALEG, 0.07);
						t->set(Constants::io__text_double::SCALEX, 0.7);
						t->set(Constants::io__text_double::POSY, -0.055);
						t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
						t->set(Constants::io__text_integer::LAYER, -9);
						t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
						t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
						t->hook(Constants::io__text_tie_func_to_state::ON_UPDATE, [&conf, &sprites, this_is_the_player](std::string& str) {
							bool ua = false;
							conf.get(Constants::io__db_boolean::SAVING_STRING_INPUT, ua);

							if (ua) {
								std::string has_enter;
								conf.get(Constants::ro__db_string::LAST_STRING, has_enter);
								if (has_enter.length() == 0) {
									str = "New nickname: %curr_string%";
								}
								else {
									str = "Saving...";
									this_is_the_player->set(Constants::io__entity_string::NICKNAME, has_enter);
									conf.set(Constants::io__db_boolean::SAVING_STRING_INPUT, false);
									conf.set(Constants::io__conf_string::LAST_PLAYERNAME, has_enter);
								}
							}
							else {
								std::string ent_str;
								this_is_the_player->get(Constants::io__entity_string::NICKNAME, ent_str);
								str = "Nickname: " + (ent_str);
							}
							});
					}

					// RGB
					{
						ALLEGRO_COLOR initclr;
						conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, initclr);

						_posy += 0.2;
						// > > > > > |[}-----> PLAYER COLOR R
						{
							// lower
							{
								Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_string::ADD, "BAR_ON"); // clearly 0
								s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_boolean::DRAW, true);
								s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
								s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
								s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
								s->set(Constants::io__sprite_double::SCALEG, 0.16);
								s->set(Constants::io__sprite_double::SCALEX, 5.5);
								s->set(Constants::io__sprite_integer::LAYER, -9);
								s->set(Constants::io__sprite_double::POSY, _posy);


								s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&conf, s, this_is_the_player]() {
									double dx = 0;
									conf.get(Constants::ro__db_mouse_double::MOUSE_X, dx);
									if (dx > 0.342) dx = 0.342;
									if (dx < -0.342) dx = -0.342;
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									cl.r = (dx + 0.342) / (0.342 * 2.0);																							// R
									conf.set(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									this_is_the_player->set(Constants::io__sprite_color::TINT, cl);  // R
								}, true);
								s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [&conf, s]() {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.25f + 0.45f * cl.r, 0.1f + 0.12f * cl.r, 0.1f + 0.12f * cl.r, 1.0));
									});



								Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
								t->set(Constants::io__text_string::FONT, "DEFAULT");
								t->set(Constants::io__text_string::STRING, "RED");
								t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
								t->set(Constants::io__text_boolean::SHOW, true);
								t->set(Constants::io__text_double::SCALEG, 0.07);
								t->set(Constants::io__text_double::SCALEX, 0.7);
								t->set(Constants::io__text_double::POSY, -0.055);
								t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
								t->set(Constants::io__text_integer::LAYER, -9);
								t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
								t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK, [t, &conf](std::string& str) {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									t->set(Constants::io__text_double::POSX, cl.r * (0.342 * 2.0) - 0.342);															// R
									});
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [t](std::string& str) {
									t->set(Constants::io__text_double::POSX, 0.0);
									});
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_NONE, [t](std::string& str) {
									t->set(Constants::io__text_double::POSX, 0.0);
									});
								t->hook(Constants::io__text_tie_func_to_state::ON_UPDATE, [&conf](std::string& str) {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									str = "RED: " + std::to_string((int)(cl.r * 100)) + std::string("%");															// R
									});

							}
							// higher (dot)
							{
								Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
								s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [s, &conf]() {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									s->set(Constants::io__sprite_double::POSX, cl.r * (0.342 * 2.0) - 0.342);														// R						
									}); // [&modern,&__assist_g]()->void {modern = __assist_g; });
								s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter++));
								s->set(Constants::io__sprite_boolean::DRAW, true);
								s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
								s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(1.0, 0.2, 0.2, 1.0));
								s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
								s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
								s->set(Constants::io__sprite_double::SCALEG, 0.16);
								s->set(Constants::io__sprite_double::SCALEX, 1.2);
								s->set(Constants::io__sprite_integer::LAYER, -9);
								s->set(Constants::io__sprite_double::POSY, _posy);
								s->set(Constants::io__sprite_double::POSX, initclr.r * (0.342 * 2.0) - 0.342);
							}
						}

						_posy += 0.2;
						// > > > > > |[}-----> PLAYER COLOR G
						{
							// lower
							{
								Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_string::ADD, "BAR_ON"); // clearly 0
								s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_boolean::DRAW, true);
								s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
								s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
								s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
								s->set(Constants::io__sprite_double::SCALEG, 0.16);
								s->set(Constants::io__sprite_double::SCALEX, 5.5);
								s->set(Constants::io__sprite_integer::LAYER, -9);
								s->set(Constants::io__sprite_double::POSY, _posy);

								s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&conf, s, this_is_the_player]() {
									double dx = 0;
									conf.get(Constants::ro__db_mouse_double::MOUSE_X, dx);
									if (dx > 0.342) dx = 0.342;
									if (dx < -0.342) dx = -0.342;
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									cl.g = (dx + 0.342) / (0.342 * 2.0);																							// R
									conf.set(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									this_is_the_player->set(Constants::io__sprite_color::TINT, cl);  // R
								}, true);
								s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [&conf, s]() {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.1f + 0.12f * cl.g, 0.25f + 0.45f * cl.g, 0.1f + 0.12f * cl.g, 1.0));
									});

								Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
								t->set(Constants::io__text_string::FONT, "DEFAULT");
								t->set(Constants::io__text_string::STRING, "GREEN");
								t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
								t->set(Constants::io__text_boolean::SHOW, true);
								t->set(Constants::io__text_double::SCALEG, 0.07);
								t->set(Constants::io__text_double::SCALEX, 0.7);
								t->set(Constants::io__text_double::POSY, -0.055);
								t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
								t->set(Constants::io__text_integer::LAYER, -9);
								t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
								t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK, [t, &conf](std::string& str) {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									t->set(Constants::io__text_double::POSX, cl.g * (0.342 * 2.0) - 0.342);															// R
									});
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [t](std::string& str) {
									t->set(Constants::io__text_double::POSX, 0.0);
									});
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_NONE, [t](std::string& str) {
									t->set(Constants::io__text_double::POSX, 0.0);
									});
								t->hook(Constants::io__text_tie_func_to_state::ON_UPDATE, [&conf](std::string& str) {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									str = "GREEN: " + std::to_string((int)(cl.g * 100)) + std::string("%");															// R
									});

							}
							// higher (dot)
							{
								Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
								s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [s, &conf]() {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									s->set(Constants::io__sprite_double::POSX, cl.g * (0.342 * 2.0) - 0.342);														// R						
									}); // [&modern,&__assist_g]()->void {modern = __assist_g; });
								s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter++));
								s->set(Constants::io__sprite_boolean::DRAW, true);
								s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
								s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.2, 1.0, 0.2, 1.0));
								s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
								s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
								s->set(Constants::io__sprite_double::SCALEG, 0.16);
								s->set(Constants::io__sprite_double::SCALEX, 1.2);
								s->set(Constants::io__sprite_integer::LAYER, -9);
								s->set(Constants::io__sprite_double::POSY, _posy);
								s->set(Constants::io__sprite_double::POSX, initclr.g* (0.342 * 2.0) - 0.342);
							}
						}

						_posy += 0.2;
						// > > > > > |[}-----> PLAYER COLOR B
						{
							// lower
							{
								Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_string::ADD, "BAR_ON"); // clearly 0
								s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_boolean::DRAW, true);
								s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
								s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
								s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
								s->set(Constants::io__sprite_double::SCALEG, 0.16);
								s->set(Constants::io__sprite_double::SCALEX, 5.5);
								s->set(Constants::io__sprite_integer::LAYER, -9);
								s->set(Constants::io__sprite_double::POSY, _posy);

								s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&conf, s, this_is_the_player]() {
									double dx = 0;
									conf.get(Constants::ro__db_mouse_double::MOUSE_X, dx);
									if (dx > 0.342) dx = 0.342;
									if (dx < -0.342) dx = -0.342;
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									cl.b = (dx + 0.342) / (0.342 * 2.0);																							// R
									conf.set(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									this_is_the_player->set(Constants::io__sprite_color::TINT, cl);  // R
									}, true);
								s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [&conf, s]() {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.1f + 0.12f * cl.b, 0.1f + 0.12f * cl.b, 0.25f + 0.45f * cl.b, 1.0));
									});


								Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
								t->set(Constants::io__text_string::FONT, "DEFAULT");
								t->set(Constants::io__text_string::STRING, "BLUE");
								t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
								t->set(Constants::io__text_boolean::SHOW, true);
								t->set(Constants::io__text_double::SCALEG, 0.07);
								t->set(Constants::io__text_double::SCALEX, 0.7);
								t->set(Constants::io__text_double::POSY, -0.055);
								t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
								t->set(Constants::io__text_integer::LAYER, -9);
								t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
								t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_CLICK, [t, &conf](std::string& str) {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									t->set(Constants::io__text_double::POSX, cl.b * (0.342 * 2.0) - 0.342);															// R
									});
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [t](std::string& str) {
									t->set(Constants::io__text_double::POSX, 0.0);
									});
								t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_NONE, [t](std::string& str) {
									t->set(Constants::io__text_double::POSX, 0.0);
									});
								t->hook(Constants::io__text_tie_func_to_state::ON_UPDATE, [&conf](std::string& str) {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									str = "BLUE: " + std::to_string((int)(cl.b * 100)) + std::string("%");															// R
									});

							}
							// higher (dot)
							{
								Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
								s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
								s->hook(Constants::io__sprite_tie_func_to_state::ON_DRAW, [s, &conf]() {
									ALLEGRO_COLOR cl;
									conf.get(Constants::io__conf_color::LAST_COLOR_TRANSLATE, cl);
									s->set(Constants::io__sprite_double::POSX, cl.b * (0.342 * 2.0) - 0.342);														// R						
									}); // [&modern,&__assist_g]()->void {modern = __assist_g; });
								s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter++));
								s->set(Constants::io__sprite_boolean::DRAW, true);
								s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
								s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.2, 0.2, 1.0, 1.0));
								s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
								s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
								s->set(Constants::io__sprite_double::SCALEG, 0.16);
								s->set(Constants::io__sprite_double::SCALEX, 1.2);
								s->set(Constants::io__sprite_integer::LAYER, -9);
								s->set(Constants::io__sprite_double::POSY, _posy);
								s->set(Constants::io__sprite_double::POSX, initclr.b* (0.342 * 2.0) - 0.342);
							}
						}
					}

					_posy += 0.65;
					// > > > > > |[}-----> CREDITS AND STUFF
					{
						ALLEGRO_BITMAP* lmao = textures.customLoad("BUTTON_OPTIONS_IMG_" + std::to_string(counter), [](ALLEGRO_BITMAP*& b)->bool {return (b = al_create_bitmap(720, 480)); });
						{
							ALLEGRO_BITMAP* trg = al_get_target_bitmap();
							int xx = al_get_bitmap_width(lmao);
							int yy = al_get_bitmap_height(lmao);
							al_set_target_bitmap(lmao);
							al_clear_to_color(al_map_rgba_f(0, 0.13f, 0.2f, 0.2f));
							al_draw_rounded_rectangle(15, 15, xx - 15, yy - 15, 20, 20, al_map_rgba_f(0, 0.3f, 0.3f, 0.3f), 27);
							al_set_target_bitmap(trg);
						}
						Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_string::ADD, "BUTTON_OPTIONS_IMG_" + std::to_string(counter));
						s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::DRAW, true);
						s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
						s->set(Constants::io__sprite_collision_mode::COLLISION_TRANSPARENT);
						s->set(Constants::io__sprite_double::SCALEG, 1.2);
						s->set(Constants::io__sprite_double::SCALEY, 0.6);
						s->set(Constants::io__sprite_boolean::SHOWBOX, false);
						s->set(Constants::io__sprite_boolean::SHOWDOT, false);
						s->set(Constants::io__sprite_integer::LAYER, -9);
						s->set(Constants::io__sprite_double::POSY, _posy);

						std::string strings[] = {
							"The Blast version %version%",
							"LSW 2019",
							"- - - - - - - - - - - - - -",
							"Thanks for downloading the game!",
							"- - - - - - - - - - - - - -",
							"Beta testers:",
							"Lohk, Lupspie, Lyvon",
							". . .",
							". ."
						};

						for (int u = 0; u < 9; u++)
						{
							Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T_") + std::to_string(u));
							t->set(Constants::io__text_string::FONT, "DEFAULT");
							t->set(Constants::io__text_string::STRING, strings[u]);
							t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
							t->set(Constants::io__text_boolean::SHOW, true);
							t->set(Constants::io__text_double::SCALEG, 0.069);
							t->set(Constants::io__text_double::SCALEX, 0.7);
							t->set(Constants::io__text_double::POSY, -0.316 + 0.0671 * u/*-0.055*/);
							t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter));
							t->set(Constants::io__text_integer::LAYER, -9);
							t->set(Constants::io__text_double::UPDATETIME, 1.0 / 2);
							t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
						}
						counter++;
					}


					_posy += 0.55;
					// > > > > > |[}-----> GO BACK TO MENU (lower part)
					{
						Sprite* s = sprites.create("BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
						s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
						s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
						s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
						s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
						s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
						s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&__assist_g, &versatile_select]() {versatile_select(main_gamemodes::MENU); }); // [&modern,&__assist_g]()->void {modern = __assist_g; });
						s->set(Constants::io__sprite_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter));
						s->set(Constants::io__sprite_boolean::DRAW, true);
						s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
						s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
						s->set(Constants::io__sprite_double::SCALEG, 0.16);
						s->set(Constants::io__sprite_double::SCALEX, 3.5);
						s->set(Constants::io__sprite_integer::LAYER, -9);
						//s->set(Constants::io__sprite_double::POSX, -0.7);
						s->set(Constants::io__sprite_double::POSY, _posy);

						Text* t = texts.create("BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
						t->set(Constants::io__text_string::FONT, "DEFAULT");
						t->set(Constants::io__text_string::STRING, "Go back to Menu");
						t->set(Constants::io__text_string::ID, "BUTTON_OPTIONS_" + std::to_string(counter) + std::string("_T"));
						t->set(Constants::io__text_boolean::SHOW, true);
						t->set(Constants::io__text_double::SCALEG, 0.07);
						t->set(Constants::io__text_double::SCALEX, 0.7);
						t->set(Constants::io__text_double::POSY, -0.055);
						t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_OPTIONS_" + std::to_string(counter++));
						t->set(Constants::io__text_integer::LAYER, -9);
						t->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);
						t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
						t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [](std::string& str) {std::transform(str.begin(), str.end(), str.begin(), ::toupper); });
					}
				}
			}

			// PAUSE PART
			{
				{
					Sprite* s = sprites.create("PAUSE_ANIM");
					s->set(Constants::io__sprite_string_vector::ADDMULTIPLE, Tools::genStrFormat("PAUSE_##", 29));
					s->set(Constants::io__sprite_string::ID, "PAUSE_ANIM");
					s->set(Constants::io__sprite_boolean::DRAW, true);
					s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
					s->set(Constants::io__sprite_collision_mode::COLLISION_TRANSPARENT);
					s->set(Constants::io__sprite_double::SCALEG, 0.80);
					s->set(Constants::io__sprite_double::SCALEY, 0.65);
					s->set(Constants::io__sprite_double::POSY, -0.25);
					s->set(Constants::io__sprite_double::ANIMATION_FPS, 1.0 / 24);
					s->set(Constants::io__sprite_integer::LAYER, -1);
				}

				{
					Sprite* s = sprites.create("BUTTON_GAMING_3");
					s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
					s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
					s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
					s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
					s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
					s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&__assist_g, &versatile_select]() {versatile_select(__assist_g); }); // [&modern,&__assist_g]()->void {modern = __assist_g; });
					s->set(Constants::io__sprite_string::ID, "BUTTON_GAMING_3");
					s->set(Constants::io__sprite_boolean::DRAW, true);
					s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
					s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
					s->set(Constants::io__sprite_double::SCALEG, 0.17);
					s->set(Constants::io__sprite_double::SCALEX, 4.8);
					s->set(Constants::io__sprite_double::POSY, 0.4);
					s->set(Constants::io__sprite_integer::LAYER, -1);

					Text* t = texts.create("BUTTON_GAMING_3_T");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "Continue");
					t->set(Constants::io__text_string::ID, "BUTTON_GAMING_3_T");
					t->set(Constants::io__text_boolean::SHOW, true);
					t->set(Constants::io__text_double::SCALEG, 0.07);
					t->set(Constants::io__text_double::SCALEX, 0.7);
					t->set(Constants::io__text_double::POSY, -0.055);
					t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_GAMING_3");
					t->set(Constants::io__text_integer::LAYER, -1);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 2);
					t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
					t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [](std::string& str) {std::transform(str.begin(), str.end(), str.begin(), ::toupper); });
				}

				{
					Sprite* s = sprites.create("BUTTON_GAMING_4");
					s->set(Constants::io__sprite_boolean::USE_STATE_AS_BITMAP, true);
					s->set(Constants::io__sprite_string::ADD, "BAR_OFF"); // clearly 0
					s->set(Constants::io__sprite_string::ADD, "BAR_ON");  // clearly 1
					s->set(Constants::io__sprite_boolean::USE_TINTED_DRAWING, true);
					s->set(Constants::io__sprite_color::TINT, al_map_rgba_f(0.05, 1.0, 0.45, 1.0));
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1);
					s->set(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1);
					s->hook(Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, [&versatile_select]() {versatile_select(main_gamemodes::MENU); }); //  [&modern]()->void {modern = main_gamemodes::MENU; });
					s->set(Constants::io__sprite_string::ID, "BUTTON_GAMING_4");
					s->set(Constants::io__sprite_boolean::DRAW, true);
					s->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
					s->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
					s->set(Constants::io__sprite_double::SCALEG, 0.17);
					s->set(Constants::io__sprite_double::SCALEX, 4.8);
					s->set(Constants::io__sprite_double::POSY, 0.6);
					s->set(Constants::io__sprite_integer::LAYER, -1);

					Text* t = texts.create("BUTTON_GAMING_4_T");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "Go back to Menu");
					t->set(Constants::io__text_string::ID, "BUTTON_GAMING_4_T");
					t->set(Constants::io__text_boolean::SHOW, true);
					t->set(Constants::io__text_double::SCALEG, 0.07);
					t->set(Constants::io__text_double::SCALEX, 0.7);
					t->set(Constants::io__text_double::POSY, -0.055);
					t->set(Constants::io__text_string::FOLLOW_SPRITE, "BUTTON_GAMING_4");
					t->set(Constants::io__text_integer::LAYER, -1);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 2);
					t->set(Constants::io__text_color::COLOR, al_map_rgb(180, 255, 225));
					t->hook(Constants::io__text_tie_func_to_state::SPRITE_COLLISION_MOUSE_ON, [](std::string& str) {std::transform(str.begin(), str.end(), str.begin(), ::toupper); });
				}
			}

			/*
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<-------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>--- Texts preset loading ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
			[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<-------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
			*/

			// OSD
			{
				bool is_osd_enabled = true;
				conf.get(Constants::io__conf_boolean::WAS_OSD_ON, is_osd_enabled);

				{
					Text* const t = texts.create("osd_stuff0");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "M:%mouse_x%,%mouse_y% | C:%cam_x%,%cam_y%@~%cam_zoom_combined% | S:%curr_string%[%last_string%]");
					t->set(Constants::io__text_string::ID, "osd_stuff0");
					t->set(Constants::io__text_boolean::SHOW, is_osd_enabled);
					t->set(Constants::io__text_double::SCALEG, 0.025);
					t->set(Constants::io__text_double::SCALEX, 0.6);
					t->set(Constants::io__text_double::POSY, -1.00);
					t->set(Constants::io__text_double::POSX, -1.0);
					t->set(Constants::io__text_integer::MODE, +Constants::io__text_alignment::ALIGN_LEFT);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 4);
					t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
					t->set(Constants::io__text_integer::LAYER, 100);
				}
				{
					Text* const t = texts.create("osd_stuff1");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "FPS: %int_fps%|%instant_fps% [%ms_fps% ms]");
					t->set(Constants::io__text_string::ID, "osd_stuff1");
					t->set(Constants::io__text_boolean::SHOW, is_osd_enabled);
					t->set(Constants::io__text_double::SCALEG, 0.025);
					t->set(Constants::io__text_double::SCALEX, 0.6);
					t->set(Constants::io__text_double::POSY, -0.97);
					t->set(Constants::io__text_double::POSX, -1.0);
					t->set(Constants::io__text_integer::MODE, +Constants::io__text_alignment::ALIGN_LEFT);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 30);
					t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
					t->set(Constants::io__text_integer::LAYER, 100);
				}
				{
					Text* const t = texts.create("osd_stuff2");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "TPS: %int_tps%|%instant_tps% [%ms_tps% ms]");
					t->set(Constants::io__text_string::ID, "osd_stuff2");
					t->set(Constants::io__text_boolean::SHOW, is_osd_enabled);
					t->set(Constants::io__text_double::SCALEG, 0.025);
					t->set(Constants::io__text_double::SCALEX, 0.6);
					t->set(Constants::io__text_double::POSY, -0.94);
					t->set(Constants::io__text_double::POSX, -1.0);
					t->set(Constants::io__text_integer::MODE, +Constants::io__text_alignment::ALIGN_LEFT);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 3);
					t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
					t->set(Constants::io__text_integer::LAYER, 100);
				}
				{
					Text* const t = texts.create("osd_stuff3");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "UPS: %int_ups%|%instant_ups% [%ms_ups% ms]");
					t->set(Constants::io__text_string::ID, "osd_stuff3");
					t->set(Constants::io__text_boolean::SHOW, is_osd_enabled);
					t->set(Constants::io__text_double::SCALEG, 0.025);
					t->set(Constants::io__text_double::SCALEX, 0.6);
					t->set(Constants::io__text_double::POSY, -0.91);
					t->set(Constants::io__text_double::POSX, -1.0);
					t->set(Constants::io__text_integer::MODE, +Constants::io__text_alignment::ALIGN_LEFT);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 4);
					t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
					t->set(Constants::io__text_integer::LAYER, 100);
				}
				{
					Text* const t = texts.create("osd_stuff4");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "APS: %int_aps%|%instant_aps% [%ms_aps% ms]");
					t->set(Constants::io__text_string::ID, "osd_stuff4");
					t->set(Constants::io__text_boolean::SHOW, is_osd_enabled);
					t->set(Constants::io__text_double::SCALEG, 0.025);
					t->set(Constants::io__text_double::SCALEX, 0.6);
					t->set(Constants::io__text_double::POSY, -0.88);
					t->set(Constants::io__text_double::POSX, -1.0);
					t->set(Constants::io__text_integer::MODE, +Constants::io__text_alignment::ALIGN_LEFT);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 6);
					t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
					t->set(Constants::io__text_integer::LAYER, 100);
				}
				{
					Text* const t = texts.create("osd_stuff5");
					t->set(Constants::io__text_string::FONT, "DEFAULT");
					t->set(Constants::io__text_string::STRING, "D:%curr_res_x%:%curr_res_y%@%curr_refresh_rate% | I:%num_images%,S:%num_sprites%,T:%num_texts%,A:%num_tracks%");
					t->set(Constants::io__text_string::ID, "osd_stuff5");
					t->set(Constants::io__text_boolean::SHOW, is_osd_enabled);
					t->set(Constants::io__text_double::SCALEG, 0.025);
					t->set(Constants::io__text_double::SCALEX, 0.6);
					t->set(Constants::io__text_double::POSY, -0.85);
					t->set(Constants::io__text_double::POSX, -1.0);
					t->set(Constants::io__text_integer::MODE, +Constants::io__text_alignment::ALIGN_LEFT);
					t->set(Constants::io__text_double::UPDATETIME, 1.0 / 3);
					t->set(Constants::io__text_boolean::AFFECTED_BY_CAM, false);
					t->set(Constants::io__text_integer::LAYER, 100);
				}
			}
		}

		/*************************************************************************************

			# Main thread variables only:
			> Stuff mostly used here at the bottom.

		**************************************************************************************/


		/*Text* temporary_fun_text = texts.create("randomtext");
		temporary_fun_text->set(Constants::io__text_string::FONT, "DEFAULT");
		temporary_fun_text->set(Constants::io__text_string::STRING, "TEST");
		temporary_fun_text->set(Constants::io__text_string::ID, "randomtext");
		temporary_fun_text->set(Constants::io__text_boolean::SHOW, true);
		temporary_fun_text->set(Constants::io__text_double::SCALEG, 0.1);
		temporary_fun_text->set(Constants::io__text_double::SCALEX, 0.8);
		temporary_fun_text->set(Constants::io__text_double::POSY, 0.85);
		temporary_fun_text->set(Constants::io__text_integer::LAYER, -10);
		temporary_fun_text->set(Constants::io__text_double::UPDATETIME, 1.0 / 10);*/






		logg << L::SLF << fsr(__FUNCSIG__) << "Ready to go! Starting main!" << L::ELF;

		setup_animation_functional();

		// UNLOCK
		consol.resumeThread();


		/*************************************************************************************

			# Main thread ongoing:
			> Main Thread will:
			* Check if other threads are working as expected			(to be done)


		**************************************************************************************/




		GamingStorm* game_0 = nullptr;

		// already loaded: LOADING_MUSIC
		// loaded later: JUMP, MUSIC_0, MUSIC_1, MUSIC_GAMING, WALK

		Track *loading_music, *music_0, *music_1, *music_gaming;

		if (!tracks.get("LOADING_MUSIC", loading_music)) throw Abort::abort(__FUNCSIG__, "Failed to get track later on");
		if (!tracks.get("MUSIC_0", music_0)) throw Abort::abort(__FUNCSIG__, "Failed to get track later on");
		if (!tracks.get("MUSIC_1", music_1)) throw Abort::abort(__FUNCSIG__, "Failed to get track later on");
		if (!tracks.get("MUSIC_GAMING", music_gaming)) throw Abort::abort(__FUNCSIG__, "Failed to get track later on");



		main_gamemodes last_modern = main_gamemodes::HASNT_STARTED_YET;

		while (consol.isRunning()) {// (size_t counttt = 0; consol.isRunning(); counttt++) {

			if (last_modern != modern) {
				switch (modern) {
				case main_gamemodes::MENU:
				case main_gamemodes::OPTIONS:
				{
					loading_music->set(Constants::io__track_boolean::PLAYING, false);
					if (!(last_modern == main_gamemodes::MENU || last_modern == main_gamemodes::OPTIONS)) {
						bool switched = rand() % 2;
						music_0->set(Constants::io__track_boolean::PLAYING, switched);
						music_1->set(Constants::io__track_boolean::PLAYING, !switched);
					}
					music_gaming->set(Constants::io__track_boolean::PLAYING, false);
				}
					break;
				case main_gamemodes::GAMING:
				{
					loading_music->set(Constants::io__track_boolean::PLAYING, false);
					music_0->set(Constants::io__track_boolean::PLAYING, false);
					music_1->set(Constants::io__track_boolean::PLAYING, false);
					music_gaming->set(Constants::io__track_boolean::PLAYING, true);
				}
					break;
				}
				last_modern = modern;
			}

			if (isGamingLocally(modern)) {
				if (!game_0) {
					game_0 = new GamingStorm(&consol, this_is_the_player, music_gaming, +my_custom_funcs::GAMING_PLAYER_EXIT_TASK);
					game_0->tieToGamingTime([&connection](const std::chrono::milliseconds ms) { connection->getActivity().GetTimestamps().SetStart(ms.count()); });
					game_0->tieToLevelName([&connection](const std::string lv) { connection->getActivity().SetDetails(lv.c_str()); });
					game_0->startAutomatically();
				}
				else {
					game_0->pauseTask((modern == main_gamemodes::PAUSE));
					if (game_0->hasEnded()) modern = main_gamemodes::MENU;
				}
			}
			else {
				if (game_0) {
					game_0->stopNow();
					delete game_0;
					game_0 = nullptr;
					connection->getActivity().GetTimestamps().SetStart(0);
				}
			}

			Sleep(1000 / 30);			
		}


		logg << L::SLF << fsr(__FUNCSIG__) << "Closing game..." << L::ELF;

		logg.killDebugScreen();
		textures.clear();
		sprites.clear();
		fonts.clear();
		texts.clear();
		conf.flush();

		if (connection) delete connection;
	}
	catch (Abort::abort a)
	{
		forceExit("Something went wrong at MAIN!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
	}
	return 0;
}
