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
	gfile logg;

	logg << L::SLL << freg("main", "main") << "Initializing game..." << L::BLL;

	lsw_init();

	// raw allegro stuff
	Textures textures;
	Fonts fonts;
	Samples samples;

	// controllers
	Sprites sprites;
	Texts texts;
	Tracks tracks;


	logg << L::SLL << freg("main", "main") << "Setting up template functions..." << L::BLL;

	textures.setFuncs(Constants::lambda_bitmap_load, Constants::lambda_bitmap_unload);
	fonts.setFuncs(Constants::lambda_font_load, Constants::lambda_font_unload);
	samples.setFuncs(Constants::lambda_sample_load, Constants::lambda_sample_unload);

	sprites.setFuncs(Constants::lambda_default_load<Sprite>, Constants::lambda_default_unload<Sprite>);
	texts.setFuncs(Constants::lambda_default_load<Text>, Constants::lambda_default_unload<Text>);
	tracks.setFuncs(Constants::lambda_default_load<Track>, Constants::lambda_default_unload<Track>);


	logg << L::SLL << freg("main", "main") << "Initializing display, events and stuff..." << L::BLL;


	textures.load("BACKGROUND_START", "background_gameplay_start.png");
	textures.load("BAR_OFF", "bar_single_one.png");
	textures.load("BAR_ON", "bar_single_one_on.png");
	textures.load("MOUSE", "mouse.png");
	textures.load("MAIN_LOGO", "the_storm.png");
	textures.load(Tools::generateStringsFormat("BLOCK_##", 10), Tools::generateStringsFormat("anim/bloco##.png", 10));
	textures.load(Tools::generateStringsFormat("LOGO_##", 115), Tools::generateStringsFormat("logo/frame##.png", 115));
	textures.load(Tools::generateStringsFormat("PAUSE_##", 29), Tools::generateStringsFormat("pause/pause_##.png", 29));
	fonts.load("DEFAULT", "font.ttf");
	samples.load("JUMP", "musics/jump_01.wav");
	samples.load("MUSIC_0", "musics/music_01.ogg");
	samples.load("MUSIC_1", "musics/music_02.ogg");
	samples.load("MUSIC_2", "musics/music_03.ogg");
	samples.load("WALK", "musics/walk_01.wav");


	auto mysprite = sprites.create("randomsprite");
	mysprite->apply(Assistance::io___vecstring_sprite::ADDMULTIPLE, Tools::generateStringsFormat("PAUSE_##", 29));
	mysprite->apply(Assistance::io___string_sprite::SPRITE_ID, "randomsprite");
	mysprite->apply(Assistance::io___boolean_sprite::DRAW, true);
	mysprite->apply(Assistance::io___double_sprite::SCALEG, 0.7);
	//mysprite->apply(Assistance::io___double_sprite::POSX, 0.3 * sin(0.7 + 0.91 * al_get_time()));
	//mysprite->apply(Assistance::io___double_sprite::POSY, -1.5 + 0.2 * cos(0.4 + 0.65 * al_get_time()));
	mysprite->apply(Assistance::io___double_sprite::POSY, 2.80); // 3.20

	auto mytext = texts.create("randomtext");
	mytext->set(Assistance::_text_opt_str::SETFONT, "DEFAULT");
	mytext->set(Assistance::_text_opt_str::SETSTRING, "TEST");
	mytext->set(Assistance::_text_opt_str::SETID, "randomtext");
	mytext->set(Assistance::_text_opt_bool::SHOW, true);
	mytext->set(Assistance::_text_opt_db::SCALEG, 0.1);
	mytext->set(Assistance::_text_opt_db::POSY, 3.3);

	auto mytrack = tracks.create("randomtrack");
	mytrack->set(track_s::LOADID, "MUSIC_0");
	mytrack->set(track_p::PLAYING, true);


	Console consol;
	consol.launch();
	
	Camera gcam;

	camera_preset cp;
	cp.set(Assistance::io___float_camera::OFFSET_Y, 2.6);




	logg << L::SLL << freg("main", "main") << "Waiting the end of initialization..." << L::BLL;
	while (!consol.awakened()) Sleep(20);
	logg << L::SLL << freg("main", "main") << "Started main loop." << L::BLL;

	size_t counttt = 0;

	while (consol.running()) {
		Sleep(10);
		

		mytext->set(Assistance::_text_opt_str::SETSTRING, "Main loop counter:" + std::to_string(counttt++));

		//mysprite->apply(Assistance::io___double_sprite::POSX, 0.3 * sin(0.7 + 0.91 * al_get_time()));
		//mysprite->apply(Assistance::io___double_sprite::POSY, - 1.5 + 0.2 * cos(0.4 + 0.65 * al_get_time()));

		cp.set(Assistance::io___float_camera::ROTATION, cos(al_get_time()) * 0.05);

		gcam.set(cp, 0);
		gcam.apply();
	}



	logg << L::SLL << freg("main", "main") << "Closing game..." << L::BLL;
	   
	textures.clear();
	sprites.clear();
	fonts.clear();
	texts.clear();

	return 0;
}
