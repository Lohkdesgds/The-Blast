#include "..\LSW\organizer\organizer.h"

using namespace LSW::v4;

int main() {
	try {
		gfile logg;
		ResourceOf<ALLEGRO_FONT> fonts;
		ResourceOf<Sprite> sprites;
		ResourceOf<Text> texts;
		ResourceOf<Button> buttons;
		ResourceOf<SliderX> sliderxs;
		Camera gcam;
		Mixer mixer;
		Manager consol;
		Database conf;

		const std::function<void(void)> set_consol_mainthread_start = []() {
			ResourceOf<ALLEGRO_BITMAP> textures;

			__g_sys.setInterface();

			textures.create("CLEAR", "clear.jpg");
			textures.create("CLEARING", "clearing.jpg");
			textures.create("INSTALL", "install.jpg");
			textures.create("INSTALLING", "installing.jpg");
			textures.create("PLAY", "play.jpg");
			textures.create("UNINSTALL", "uninstall.jpg");
			textures.create("UNINSTALLING", "uninstalling.jpg");
		};


		consol.setSimpleTask(Constants::io__thread_ids::DRAWING, Constants::io__threads_taskid::START, set_consol_mainthread_start);

		// STARTUP
		logg << L::SLF << fsr(__FUNCSIG__) << "Launching..." << L::ELF;

		consol.start();

		logg << L::SLF << fsr(__FUNCSIG__) << "Waiting Manager to start..." << L::ELF;
		logg.flush();

		while (!consol.isOpen()) Sleep(20);
		if (!consol.isRunning()) return -1;

		// LOCK GAME TO SET UP EVERYTHING
		consol.pauseThread();
		while (!consol.hasThreadPaused()) Sleep(10);

		logg << L::SLF << fsr(__FUNCSIG__) << "Setting up menu stuff..." << L::ELF;


		{
			camera_preset cp;
			cp.setLayer(0, true);
			cp.set(Constants::io__camera_boolean::RESPECT_LIMITS, false);
			cp.set(Constants::io__camera_boolean::READONLY_NOW, true);
			gcam.set(cp, 0);
			gcam.apply(0);
		}

		{
			Button* but = buttons.create("");
			but->setText("");
			but->setLayers({ 0});
			//but->hook(Constants::io__buttons_shared_tie_func::MOUSE_CLICK, [&versatile_select](Sprite* sp, Text* tp, std::string& str) { if (sp) versatile_select(main_gamemodes::GAMING); });
			but->hook(Constants::io__buttons_shared_tie_func::MOUSE_ON, [&conf](Sprite* sp, Text* tp, std::string& str) { if (tp) std::transform(str.begin(), str.end(), str.begin(), ::toupper); });
			//but->setPosY(0.25);
			but->addBitmaps(true, { "INSTALL", "INSTALLING" });
			but->setLinks({ { Constants::io__sprite_tie_func_to_state::COLLISION_NONE, 0 },{ Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_ON, 1 },{ Constants::io__sprite_tie_func_to_state::COLLISION_MOUSE_CLICK, 1 } });
			but->setPropBasedScale(1.125);
		}


		// UNLOCK
		consol.resumeThread();

		while (consol.isRunning()) Sleep(20);
	}
	catch (Abort::abort a)
	{
		forceExit("Something went wrong at MAIN!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
	}
	return 0;
}