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

#ifdef DEBUG
	PRINTDEBUG("[INFO] Initializing game...")
#endif

	lsw_init();

#ifdef DEBUG
	PRINTDEBUG("[INFO] Setting up textures base...")
#endif

	Textures textures;
	textures.setFuncs(Constants::lambda_bitmap_load, Constants::lambda_bitmap_unload);

#ifdef DEBUG
	PRINTDEBUG("[INFO] Setting up sprites base...")
#endif

	Sprites sprites;
	sprites.setFuncs(Constants::lambda_default_load<Sprite>, Constants::lambda_default_unload<Sprite>);

#ifdef DEBUG
	PRINTDEBUG("[INFO] Setting up fonts base...")
#endif

	Fonts fonts;
	fonts.setFuncs(Constants::lambda_font_load, Constants::lambda_font_unload);


	//__raw_display disp;
	/*Textures imgs;

	imgs.load("temp", "pause/pause_00.png");


	Camera tf;
	tf.apply(); // reset to a default situation

	Sprite spr;
	spr.apply(Assistance::_sprite_opt_strg::ADD, "temp");
	spr.apply(Assistance::in___boolean_sprite::SHOWBOX, true);
	spr.apply(Assistance::in___double_sprite::CENTERX, 0.0);*/


	/*disp.print();

	for (auto u = GetTickCount64(); GetTickCount64() - u < 10000;) {
		disp.clear_to(al_map_rgb(0, 0, 0));
		spr.draw();
		disp.flip();
	}*/


#ifdef DEBUG
	PRINTDEBUG("[INFO] Setting up console...")
#endif

	Console consol;
	consol.launch();


#ifdef DEBUG
	PRINTDEBUG("[INFO] Loading textures...")
#endif

	textures.load(Tools::generateStringsFormat("PAUSE_##", 29), Tools::generateStringsFormat("pause/pause_##.png", 29));


#ifdef DEBUG
	PRINTDEBUG("[INFO] Loading sprites...")
#endif

	auto mysprite = sprites.create("randomsprite");
	mysprite->apply(Assistance::in___string_sprite::ADD, "PAUSE_00");
	mysprite->apply(Assistance::in___string_sprite::SPRITE_ID, "MySprite");
	mysprite->apply(Assistance::in___boolean_sprite::DRAW, true);


#ifdef DEBUG
	PRINTDEBUG("[INFO] Main thread chillin'")
#endif


	while (consol.running()) {
		Sleep(100);
	}

#ifdef DEBUG
	PRINTDEBUG("[INFO] Closing textures, sprites, fonts and the game...")
#endif


	textures.clear();
	sprites.clear();
	fonts.clear();

#ifdef DEBUG
	PRINTDEBUG("[INFO] Ended successfully")
#endif

	return 0;
}
