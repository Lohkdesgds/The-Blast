#include <Lunaris/all.h>
#include "Modules/gamedraw.h"
#include "Modules/host.h"


using namespace Lunaris;


void host_mode();
void client_mode();


int main(int argc, char* argv[])
{
	if (argc != 2) {
		cout << console::color::YELLOW << "Please do 'APP.exe --host' or 'APP.exe --client'";
		return 0;
	}

	if (strcmp("--host", argv[1]) == 0) {
		host_mode();
	}
	else {
		client_mode();
	}
}


void host_mode()
{
	Host host;
	if (!host.start()) {
		cout << console::color::RED << "Can't start host.";
		return;
	}
	else {
		cout << console::color::GREEN << "Started host for map " << world_map_width << "x" << world_map_height << " with player limit: " << max_users_amount;
	}

	host.yield();

	// probably never here lol
	host.close();
}

void client_mode()
{
	display_async mdisp;
	if (!mdisp.create(display_config().set_framerate_limit(120).set_fullscreen(false).set_wait_for_display_draw(true).set_display_mode(display_options().set_width(1280).set_height(720)).set_extra_flags(ALLEGRO_OPENGL)))
	{
		cout << console::color::RED << "Can't start display.";
		return;
	}





	std::vector<TextureMap> textures;

	for (uint32_t p = 0; p <= static_cast<uint32_t>(World::block_types::__LAST); p++)
	{
		auto nt = make_hybrid<texture>();

		if (!mdisp.add_run_once_in_drawing_thread([&] {
			if (!nt->create(256, 256)) return false;
			ALLEGRO_BITMAP* bck = al_get_target_bitmap();
			nt->set_as_target();
			color((int)(random() % 256), random() % 256, random() % 256).clear_to_this();
			al_set_target_bitmap(bck);
			return true;
		}).get()) {
			cout << console::color::RED << "Can't create bitmap.";
			return;
		}

		textures.push_back(TextureMap{ p, nt });
	}

	config conf;
	conf.load("config.ini");
	conf.auto_save("config.ini");

	GameDraw gd(mdisp, conf);
	for (const auto& it : textures) gd.add_texture(it.bmp, it.block_id);
	gd.set_user_name("Player");
	gd.check_config();
	if (!gd.connect_and_enable_draw_and_keyboard())
	{
		cout << console::color::RED << "Can't start connection.";
		return;
	}

	while (1) std::this_thread::sleep_for(std::chrono::seconds(1));

}