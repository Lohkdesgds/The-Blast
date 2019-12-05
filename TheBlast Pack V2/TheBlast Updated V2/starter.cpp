#include <iostream>
#include <string>
#include <Windows.h>

#include "LSW/organizer/organizer.h"

using namespace LSW::v4;

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

// example FS
/*static void show_image(ALLEGRO_BITMAP* bmp)
{
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_EVENT event;

	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_keyboard_event_source());

	while (true) {
		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_draw_bitmap(bmp, 0, 0, 0);
		al_flip_display();
		if (al_get_next_event(queue, &event)) {
			if (event.type == ALLEGRO_EVENT_KEY_DOWN
				&& event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				break;
			}
		}
	}

	al_destroy_event_queue(queue);
}

static void print_file(ALLEGRO_FS_ENTRY* entry)
{
	int mode = al_get_fs_entry_mode(entry);
	time_t now = time(NULL);
	time_t atime = al_get_fs_entry_atime(entry);
	time_t ctime = al_get_fs_entry_ctime(entry);
	time_t mtime = al_get_fs_entry_mtime(entry);
	const char* name = al_get_fs_entry_name(entry);
	off_t size = al_get_fs_entry_size(entry);

	printf_s("%-36s %s%s%s%s%s%s %8u %8u %8u %8u\n",
		name,
		mode & ALLEGRO_FILEMODE_READ ? "r" : ".",
		mode & ALLEGRO_FILEMODE_WRITE ? "w" : ".",
		mode & ALLEGRO_FILEMODE_EXECUTE ? "x" : ".",
		mode & ALLEGRO_FILEMODE_HIDDEN ? "h" : ".",
		mode & ALLEGRO_FILEMODE_ISFILE ? "f" : ".",
		mode & ALLEGRO_FILEMODE_ISDIR ? "d" : ".",
		(unsigned)(now - ctime),
		(unsigned)(now - mtime),
		(unsigned)(now - atime),
		(unsigned)size);
}

static void listdir(ALLEGRO_FS_ENTRY* entry)
{
	ALLEGRO_FS_ENTRY* next;

	al_open_directory(entry);
	while (1) {
		next = al_read_directory(entry);
		if (!next)
			break;

		print_file(next);
		if (al_get_fs_entry_mode(next) & ALLEGRO_FILEMODE_ISDIR)
			listdir(next);
		al_destroy_fs_entry(next);
	}
	al_close_directory(entry);
}

// main
ALLEGRO_FS_ENTRY* entry;


	 //Make future calls to al_fopen() on this thread go to the PhysicsFS
	 //backend.


	 //List the contents of our example zip recursively.

printf_s("%-36s %-6s %8s %8s %8s %8s\n",
	"name", "flags", "ctime", "mtime", "atime", "size");
printf_s(
	"------------------------------------ "
	"------ "
	"-------- "
	"-------- "
	"-------- "
	"--------\n");
entry = al_create_fs_entry("");
listdir(entry);
al_destroy_fs_entry(entry);


*/

int main(int argc, const char* argv[])
{
	try {
		/*************************************************************************************

			# Start stuff:
			> Variables, some tests, initializing the basics

		**************************************************************************************/

		gfile logg;

		logg << L::SLL << fsr(__FUNCSIG__) << "App version: " << Constants::version_app << L::BLL;

		logg << L::SLL << fsr(__FUNCSIG__) << "Initializing game..." << L::BLL;

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
		camera_preset cp;
		Mixer mixer; // init audio mixer

		logg << L::SLL << fsr(__FUNCSIG__) << "Setting up template functions..." << L::BLL;

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


		/*************************************************************************************

			# Loading stuff:
			> Load textures, sprites, musics, and have a progress bar while doing it hahaha

		**************************************************************************************/


		// need this to draw progress bar
		fonts.load("DEFAULT", "font.ttf");
		samples.load("LOADING", "musics/loading.ogg");
		auto __introtrack = tracks.create("INTRO0");
		__introtrack->set(Assistance::io__track_string::LOADID, "LOADING");
		__introtrack->set(Assistance::io__track_boolean::PLAYING, true);
		__introtrack->set(Assistance::io__track_integer::PLAYMODE, Assistance::io__track_integer_modes::LOOP);


		float __progress = 0.00f;
		std::thread __progress_bar([&__progress, &fonts] {
			Display* myd = new Display(300, 40, ALLEGRO_WINDOWED | ALLEGRO_NOFRAME, 60);
			ALLEGRO_FONT* f;
			float __smoothp = 0.00;
			size_t frames_already_done = 0;
			assert(fonts.get("DEFAULT", f));
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
			delete myd;
			});

		__progress = 0.01f;



		//cp.set(Assistance::io__camera_float::OFFSET_Y, 2.6);


		logg << L::SLL << fsr(__FUNCSIG__) << "Initializing display, events and stuff..." << L::BLL;

		textures.load("BACKGROUND_START", "background_gameplay_start.png");
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

		// 0.97
		__progress = 0.990f;
		samples.load("JUMP", "musics/jump_01.wav");
		__progress = 0.9915f;
		samples.load("MUSIC_0", "musics/music_01.ogg");
		__progress = 0.993f;
		samples.load("MUSIC_1", "musics/music_02.ogg");
		__progress = 0.9945f;
		samples.load("MUSIC_2", "musics/music_03.ogg");
		__progress = 0.9960f;
		samples.load("WALK", "musics/walk_01.wav");

		//draw_simple_bar(250, 45, 0.92f); _progress_bar->flip();
		__progress = 0.997f;


		auto mysprite = sprites.create("randomsprite");

		lthr = new std::thread([&thrfloat, &__progress] {while (thrfloat != 1.00) { __progress = 0.87f + 0.10f * thrfloat; }});
		mysprite->set(Assistance::io__sprite_string_vector::ADDMULTIPLE, Tools::genStrFormat("PAUSE_##", 29), &thrfloat);
		lthr->join(); delete lthr; lthr = nullptr; thrfloat = 0.00;

		mysprite->set(Assistance::io__sprite_string::ID, "randomsprite");
		mysprite->set(Assistance::io__sprite_boolean::DRAW, true);
		mysprite->set(Assistance::io__sprite_double::SCALEG, 0.7);
		//mysprite->apply(Assistance::io__sprite_double::POSX, 0.3 * sin(0.7 + 0.91 * al_get_time()));
		//mysprite->apply(Assistance::io__sprite_double::POSY, -1.5 + 0.2 * cos(0.4 + 0.65 * al_get_time()));
		//mysprite->apply(Assistance::io__sprite_double::POSY, 2.80); // 3.20
		mysprite->set(Assistance::io__sprite_double::POSY, 0.50); // 3.20

		//draw_simple_bar(250, 45, 0.95f); _progress_bar->flip();
		__progress = 0.998f;


		auto mytext = texts.create("randomtext");
		mytext->set(Assistance::io__text_string::FONT, "DEFAULT");
		mytext->set(Assistance::io__text_string::STRING, "TEST");
		mytext->set(Assistance::io__text_string::ID, "randomtext");
		mytext->set(Assistance::io__text_boolean::SHOW, true);
		mytext->set(Assistance::io__text_double::SCALEG, 0.1);
		//mytext->set(Assistance::io__text_double::POSY, 3.3);
		mytext->set(Assistance::io__text_double::POSY, 0.75);

		//draw_simple_bar(250, 45, 0.98f); _progress_bar->flip();
		__progress = 0.999f;

		auto mytrack = tracks.create("randomtrack");
		mytrack->set(Assistance::io__track_string::LOADID, "MUSIC_0");

		//draw_simple_bar(250, 45, 1.00f); _progress_bar->flip();


		//Sleep(2000);

		/*delete _progress_bar;
		_progress_bar = nullptr;*/

		__progress = 1.00f;
		__progress_bar.join(); // sync loading screen, can die


		/*************************************************************************************

			# Main thread stuff:
			> Game has started!

		**************************************************************************************/


		Console consol;
		consol.start();

		logg << L::SLL << fsr(__FUNCSIG__) << "Waiting the end of initialization..." << L::BLL;
		while (!consol.isOpen()) Sleep(20);
		//mytrack->set(io__track_boolean::PLAYING, true);
		logg << L::SLL << fsr(__FUNCSIG__) << "Started main loop." << L::BLL;

		size_t counttt = 0;
		//__introtrack->set(io__track_boolean::PLAYING, false);

		while (consol.isRunning()) {
			Sleep(25);


			mytext->set(Assistance::io__text_string::STRING, "Main loop counter:" + std::to_string(counttt++));

			//mysprite->apply(Assistance::io__sprite_double::POSX, 0.3 * sin(0.7 + 0.91 * al_get_time()));
			//mysprite->apply(Assistance::io__sprite_double::POSY, - 1.5 + 0.2 * cos(0.4 + 0.65 * al_get_time()));

			cp.set(Assistance::io__camera_float::ROTATION, al_get_time() * 0.25);
			//logg << L::SL << fsr(__FUNCSIG__, E::DEBUG) << "ANG=" << (float)((int)(0.3 * (1800.0 / ALLEGRO_PI) * al_get_time())%3600)/10.0 << L::BL;
			cp.set(Assistance::io__camera_float::ROTATION, 0.3 * al_get_time());

			gcam.set(cp, 0);
			//gcam.apply(0);
		}



		logg << L::SLL << fsr(__FUNCSIG__) << "Closing game..." << L::BLL;

		textures.clear();
		sprites.clear();
		fonts.clear();
		texts.clear();
	}
	catch (Abort::abort a)
	{
		forceExit("Something went wrong at MAIN!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
	}

	return 0;
}
