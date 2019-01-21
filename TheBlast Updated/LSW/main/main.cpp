#include "..\all\enable_all.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

using namespace LSW::v2;

//enum gamescreens{MENU};
namespace local {
	struct initt {
		int res[2] = { -1,-1 };
		int hz = -1;
		int mode = Defaults::default_display_settings;
		bool changed = false;
		bool fixmem = false;
		bool skipdown = false;
	};
}

int sub_main(const int, Display::display&);
void temp_thr_load(float*, bool*, Display::display*, bool);
void temp_xtract(Stacker::extractor*, Safer::safe_string, Safer::safe_string);
void killall();

int main(int argc, char* argv[])
{
	local::initt strt;
	Log::gfile logg;
	
	logg << Log::START << "[THR0:MAIN_][WARN] Detected command entry:{" << Log::ENDL;

	if (argc > 1)
	{
		for (int u = 0; u < argc; u++)
		{
			Safer::safe_string wrk = argv[u];

			logg << Log::START << "[THR0:MAIN_][ARGS] " << u << "> " << wrk << Log::ENDL;

			if (u + 1 < argc) { // tags can be just -offline or whatever later
				if (wrk == "-w")
				{
					strt.changed = true;
					strt.res[0] = atoi(argv[u + 1]);

				}
				if (wrk == "-h")
				{
					strt.changed = true;
					strt.res[1] = atoi(argv[u + 1]);
				}
				if (wrk == "-hz")
				{
					strt.changed = true;
					strt.hz = atoi(argv[u + 1]);
				}
			}
			if (wrk == "-fullscreen")
			{
				strt.changed = true;
				strt.mode = Defaults::default_display_settings;
			}
			if (wrk == "-windowed")
			{
				strt.changed = true;
				strt.mode = ALLEGRO_OPENGL | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE;
			}
			if (wrk == "-skipdownload")
			{
				strt.skipdown = true;
				strt.changed = true;
			}
			if (wrk == "-fixedmemory")
			{
				strt.fixmem = true;
				strt.changed = true;
			}
			if (wrk == "-dev")
			{
				logg.showOnConsole(true);
			}
			if (wrk == "-longlog")
			{
				logg.longLog(true);
				logg << Log::NEEDED_START << "[THR0:MAIN_][WARN] LongLog enabled. Everything is going to be saved!" << Log::NEEDED_ENDL;
			}
		}
	}

	logg << Log::NEEDED_START << "[THR0:MAIN_][WARN]}" << Log::NEEDED_ENDL;

	prepare_commom_templates

	d_sprite_database spr_data; // ya, does need to draw!
	d_images_database img_data; // does not need to draw.
	d_texts_database  txt_data; // ya, does need to draw!
	d_musics_database msk_data; // there's no way to draw.

	Display::display disp;
	Camera::camera_g cam; // cam stuff
	Layer::layerer lyr; // layering stuff
	Events::big_event bev;


	logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Starting the app..." << Log::NEEDED_ENDL;

	if (strt.changed) disp.custom_launch(strt.res[0], strt.res[1], strt.mode);
	else disp.launch();

	cam.setup();
	bev.initInstance(); // create object
	bev.g().setMultiplierForUpdatingImg(disp._get().getMode(-1).hz);
	bev.initSetupAndRun(); // init thread and start working on events
	disp.setKeep(bev.g().getKeep());
	disp._get().setTarget();

	// global always-in-use variables (textures, texts, etc)
	{
		/* CREATING LAYERS FOR COLLISION...? */


		// setting 0, layer 0 collide and (probably) is affected by 100
		lyr.getOne(0).getR(0).colliding[100] = true;
		lyr.getOne(2).getR(2).colliding[100] = true;
		//lyr.getOne(3).set(Defaults::map_default_layer).colliding[Defaults::user_default_layer] = true;
		//lyr.getOne(3).set(Defaults::map_default_layer).colliding[Defaults::badboys_default_layer] = true;
		//lyr.getOne(3).set(Defaults::badboys_default_layer).colliding[Defaults::user_default_layer] = true;


		/* CREATING LAYERS FOR CAMERA...? */

		// LAYER -2 // before intro
		cam.set(-2, -2, true);
		cam.set(-2, Camera::OFFY, 2.7);
		cam.set(-2, -9999, true); // EARLY ACCESS
		cam.set(-2, Camera::ZOOM, 1.2);

		// LAYER -1 // intro
		cam.set(-1, -1, true);
		cam.set(-1, -9999, true); // EARLY ACCESS
		cam.set(-1, Defaults::default_font_foreground_layer, true);
		cam.set(-1, Camera::ZOOM, 1.15);
		//cam.set(-1, 100, true);

		// LAYER 0 // menu
		cam.apply(0);
		cam.set(0, -20, true);
		cam.set(0, 0, true);
		cam.set(0, 99, true);
		cam.set(0, 100, true);
		cam.set(0, -9999, true); // EARLY ACCESS
		cam.set(0, Defaults::default_font_foreground_layer, true);

		// LAYER 2
		cam.set(2, Camera::ZOOM, 1.0);
		cam.set(2, 100, true);
		cam.set(2, Defaults::default_font_foreground_layer, true);
		for (int p = 1; p < 4; p++) cam.set(2, p, true);

		// LAYER 3
		//cam.set(3, 100, true);
		cam.set(3, Defaults::default_font_foreground_layer, true);
		cam.set(3, Defaults::map_default_layer, true);
		cam.set(3, Defaults::user_default_layer, true);
		cam.set(3, Defaults::badboys_default_layer, true);

		//for (int p = 1; p < 4; p++) cam.set(2, p, true);

		float perc = 0.0;
		bool isdone = false;
		bool hasbeenkilledwhile = false;

		Text::text* wt = nullptr;
		Text::text* wt2 = nullptr;


		// EARLY ACCESS
		txt_data.create(wt);
		wt->set(Text::SETSTRING, ("Early Access ") + Defaults::version_app.g());
		wt->set(Text::POSX, 1.0);
		wt->set(Text::POSY, -1.0);
		wt->set(Text::MODE, Text::ALIGN_RIGHT);
		wt->set(Text::AFFECTED_BY_CAM, false);
		wt->set(Text::COLOR, al_map_rgba_f(0.2, 0.2, 0.2, 0.2));
		wt->set(Text::SCALEG, 1.05);
		wt->setMainDisplay(&disp);
		wt->set(Text::UPDATETIME, 0.05);
		wt->set(Text::LAYER, -9999);
		wt->set(Text::SETID,"EARLY_ACCESS_00");
		wt = nullptr;

		txt_data.create(wt);
		wt->set(Text::SETSTRING, "Lohk's Studios Works 2019");
		wt->set(Text::POSX, 1.0);
		wt->set(Text::POSY, -0.95);
		wt->set(Text::MODE, Text::ALIGN_RIGHT);
		wt->set(Text::AFFECTED_BY_CAM, false);
		wt->set(Text::COLOR, al_map_rgba_f(0.15, 0.15, 0.15, 0.15));
		wt->set(Text::SCALEG, 0.8);
		wt->setMainDisplay(&disp);
		wt->set(Text::UPDATETIME, 0.05);
		wt->set(Text::LAYER, -9999);
		wt->set(Text::SETID,"EARLY_ACCESS_01");
		wt = nullptr;


		txt_data.create(wt2);
		wt2->set(Text::SETGLOBALPATH, Defaults::font_altern_name_full);
		wt2->set(Text::IS_GLOBALPATH_RAW, true);
		wt2->set(Text::SETSTRING, "Please don't close the game until full load!");
		wt2->set(Text::POSX, 0.0);
		wt2->set(Text::POSY, 3.26);
		wt2->set(Text::MODE, Text::ALIGN_CENTER);
		wt2->set(Text::UPDATETIME, 1.00);
		wt2->set(Text::LAYER, -2);
		wt2->set(Text::SETID,"LOADING_1");

		txt_data.create(wt);
		wt->set(Text::SETSTRING, "Loading...");
		wt->set(Text::POSX, 0.0);
		wt->set(Text::POSY, 3.20);
		wt->set(Text::MODE, Text::ALIGN_CENTER);
		wt->setMainDisplay(&disp);
		wt->set(Text::UPDATETIME, 0.05);
		wt->set(Text::LAYER, -2);
		wt->set(Text::SETID,"LOADING_0");



		//disp.checkUpImages();


		logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Calling THR1:LOAD_..." << Log::NEEDED_ENDL;

		std::thread thr(temp_thr_load, &perc, &isdone, &disp, strt.skipdown);

		disp.capFPS(60);
		cam.apply(-2);

		logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Showing loading screen. Registering time..." << Log::NEEDED_ENDL;

		double took_time = al_get_time();
		int step = 0;

		while (!isdone)
		{
			if (!hasbeenkilledwhile) {

				cam.set(-2, Camera::ROTATION, cos(al_get_time())*0.04);
				cam.apply();

				wt->set(Text::SETSTRING, "Loading [" + std::to_string(100.0*perc) + "\\%]");

				spr_data.draw();
				txt_data.draw();

				if (!disp.flip()) {
					hasbeenkilledwhile = true;
				}

				if (al_get_time() - took_time > 15.0 && step == 0)
				{
					wt2->set(Text::SETSTRING, "It may take a while! Please wait...");
					step++;
				}
				else if(al_get_time() - took_time > 30.0 && step == 1)
				{
					wt2->set(Text::SETSTRING, "It is slow right? Sorry about that... :x");
					step++;
				}
				else if (al_get_time() - took_time > 45.0 && step == 2)
				{
					wt2->set(Text::SETSTRING, "Yeah yeah that's big. Is it worth it? I don't really know...");
					step++;
				}
				else if (al_get_time() - took_time > 60.0 && step == 3)
				{
					wt2->set(Text::SETSTRING, "Dropbox seems slow today... Maybe it's your internet... Hmm...");
					step++;
				}
				else if (al_get_time() - took_time > 75.0 && step == 4)
				{
					wt2->set(Text::SETSTRING, "Hm. Let's talk about you. How is it going?");
					step++;
				}
				else if (al_get_time() - took_time > 85.0 && step == 5)
				{
					wt2->set(Text::SETSTRING, "Did you ask me? I'm fine. I guess...");
					step++;
				}
				else if (al_get_time() - took_time > 95.0 && step == 6)
				{
					wt2->set(Text::SETSTRING, "Yeah yeah. I'm tired of waiting too.");
					step++;
				}
				else if (al_get_time() - took_time > 120.0 && step == 7)
				{
					wt2->set(Text::SETSTRING, "Almost done... I guess?");
					step++;
				}
				else if (al_get_time() - took_time > 150.0 && step == 8)
				{
					wt2->set(Text::SETSTRING, "... <you'll have to kill me in task manager if you want to leave :x> ...");
					step++;
				}
				else if (al_get_time() - took_time > 180.0 && step > 8)
				{
					wt2->set(Text::SETSTRING, std::to_string((int)(1.0*(al_get_time() - took_time)/60.0)) + " minutes...");
				}
			}
			else {
				al_rest(0.1);
			}
		}		

		wt->set(Text::SETGLOBALPATH, Defaults::font_default_name);
		wt->set(Text::IS_GLOBALPATH_RAW, false);

		txt_data.work().lock();
		for (auto& i : txt_data.work().work()) {
			i->unload();
			i->verify();
		}
		txt_data.work().unlock();

		wt->set(Text::SETSTRING, "Loaded!");
		wt2->set(Text::SETSTRING, "YAY! Finally DONE! :3");

		for (double d = al_get_time(); (al_get_time() - d < 3.0) && !hasbeenkilledwhile;) {
			cam.set(-2, Camera::ROTATION, cos(al_get_time())*0.04);
			cam.apply();

			wt->set(Text::COLOR, al_map_rgba_f(1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0));
			wt2->set(Text::COLOR, al_map_rgba_f(1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0));

			spr_data.draw();
			txt_data.draw();

			if (!disp.flip()) {
				hasbeenkilledwhile = true;
			}
		}

		thr.join();

		logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Time taken: " << (al_get_time() - took_time) << " second(s)." << Log::NEEDED_ENDL;

		logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Reloading optimized version of the textures..." << Log::NEEDED_ENDL;

		took_time = al_get_time();
		
		disp.checkUpImages();

		txt_data.remove("LOADING_1");

		if (hasbeenkilledwhile) {
			logg << Log::NEEDED_START << "[THR0:MAIN_][WARN] User left before game start. It's not recommended to exit this way." << Log::NEEDED_ENDL;
			logg.flush();

			killall();
			exit(EXIT_FAILURE);
		}


		cam.set(-2, -2, true);
		cam.set(-2, Camera::ZOOM, 1.0);
		cam.set(-2, Camera::OFFY, 0.0);
		cam.set(-2, Camera::ROTATION, 0.0);
		cam.apply(-2);

		wt->set(Text::SETSTRING, "Checking...");
		//wt->set(Text::USEBUFOPT, true);
		wt->set(Text::POSX, 0.95);
		wt->set(Text::POSY, 0.87);
		wt->set(Text::MODE, Text::ALIGN_RIGHT);
		wt->set(Text::COLOR, al_map_rgba_f(0.5,0.5,0.5,0.5));
		wt->set(Text::UPDATETIME, 0.2);
		disp.capFPS(30);

		img_data.work().lock();
		size_t nnow = 0, sizz = img_data.work().getMax();
		for (auto& i : img_data.work().work())
		{
			if (disp.flip(false)) {
				spr_data.draw();
				txt_data.draw();
			}					

			i->reload();

			wt->set(Text::SETSTRING, std::string("Checking [") + std::to_string(nnow) + "/" + std::to_string(sizz) + "]: " + i->whoAmI().g());
			nnow++;
		}
		img_data.work().unlock();


		logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Time taken: " << (al_get_time() - took_time) << " second(s)." << Log::NEEDED_ENDL;

		//al_rest(0.5);
	}

	logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Preparing functions to start..." << Log::NEEDED_ENDL;


	if (strt.fixmem) {
		Image::image_low* nulp;
		img_data.get(nulp, 0);
		assert(nulp);
		nulp->_setKeepOnMemory(strt.fixmem);
	}

	// LAST SETTINGS

	// VARS
	//Sprite::sprite* spr_t = nullptr;
	Text::text* wtt = nullptr;

	// BAR_00_TEXT
	if (!txt_data.get(wtt, "BAR_00_TEXT")) throw "FAILED TO LOAD BAR_00_TEXT";
	wtt->verify();
	/*wtt->get(Text::SPRITE, spr_t);

	if (spr_t){
		spr_t->set(Sprite::SCALEY, 1.5);
	}
	spr_t = nullptr;*/
	wtt = nullptr;


	// BAR_01_TEXT
	if (!txt_data.get(wtt, "BAR_01_TEXT")) throw "FAILED TO LOAD BAR_01_TEXT";
	wtt->verify();
	/*wtt->get(Text::SPRITE, spr_t);

	if (spr_t){
		spr_t->set(Sprite::SCALEY, 1.5);
	}
	spr_t = nullptr;*/
	wtt = nullptr;



	// start functions
	bev.g().addFunction(NEWF(Events::add_t_s, 0), &Events::defaultfunction_add);
	bev.g().addFunction(NEWF(Events::cos_t_s, 1), &Events::defaultfunction_cos);
	bev.g().addFunction(NEWF(Events::cos_t_s, 2), &Events::defaultfunction_cos);

	// get instances
	Events::add_t_s* a1 = nullptr;
	Events::cos_t_s* b1 = nullptr;
	Events::cos_t_s* b2 = nullptr;

	a1 = (Events::add_t_s*)bev.g().getFVoidArg(0);
	b1 = (Events::cos_t_s*)bev.g().getFVoidArg(1);
	b2 = (Events::cos_t_s*)bev.g().getFVoidArg(2);

	// verify
	assert(a1);
	assert(b1);
	assert(b2);

	// settings for each one
	a1->adding = 2.0;
	bev.g().setTimerForF(1.0 / 90, 0);

	b1->ampl = 0.2;
	b1->default_val = 1.0;
	b1->time_multiplier = 8.0;

	b2->ampl = 0.12;
	b2->default_val = 1.0;
	b2->time_multiplier = 1.0;
	// done

	//disp._get().enableBlur(); // not working as expected hmm
	logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Now running!" << Log::NEEDED_ENDL;
	logg.flush();

	srand(al_get_time());

	for (int res = -1; bev.g()._keep() && res != -10;)
	{
		res = sub_main(res, disp);
	}

	logg << Log::NEEDED_START << "[THR0:MAIN_][INFO] Now exiting the game the usual way!" << Log::NEEDED_ENDL;
	logg.flush();

	killall();

	return 0;
}

void temp_thr_load(float* f, bool* b, Display::display* disp, bool skipdownload)
{
	if (!f || !b || !disp) throw "Wasn't supposed to work this way! Error on Line 1 temp_thr_load";

	*b = false;
	*f = 0.0;

	d_sprite_database spr_data;
	d_images_database img_data;
	d_texts_database  txt_data;
	d_musics_database msk_data;

	Sprite::sprite*   ws = nullptr;
	Image::image_low* wi = nullptr;
	Text::text*       wt = nullptr;
	Sound::track*	  wm = nullptr;

	Log::gfile logg;

	logg << Log::NEEDED_START << "[THR1:LOAD_][INFO] Loading resources from the internet..." << Log::NEEDED_ENDL;
	logg.flush();
	   
	/* LOADING TEXTURES...? */
	float o = 0.01;


	if (!skipdownload){

		Safer::safe_string path_d = Defaults::default_onefile_path;
		Tools::interpret_path(path_d);

		logg << Log::START << "[THR1:LOAD_][INFO] Downloading..." << Log::ENDL;

		std::thread thr0(Downloader::easyDownload, Defaults::call_url_file.g().c_str(), path_d.g().c_str(), &o);

		while (o != 1.0 && disp->isOpen()) {
			*f = o * 0.90;
		}

		o = 0.0;
		thr0.join();

		logg << Log::START << "[THR1:LOAD_][INFO] Extracting..." << Log::ENDL;

		Stacker::extractor ext;
		std::thread thr3(temp_xtract,&ext, path_d, Defaults::default_root_path);

		while ((o = ext.getPerc()) != 1.0 && disp->isOpen()) {
			*f = o * 0.05 + 0.90;
		}
		thr3.join();

		std::experimental::filesystem::remove(path_d.g());
	}
	else {
		logg << Log::NEEDED_START << "[THR1:LOAD_][WARN] SkipDownload flag skipped auto download." << Log::NEEDED_ENDL;
	}

	logg << Log::NEEDED_START << "[THR1:LOAD_][INFO] Pre-loading textures..." << Log::NEEDED_ENDL;
	

	/* LOADING TEXTURES...? */


	std::thread thr1(Image::multipleLoad, "PAUSE", "pause/pause_", 29, 2, ".png", &o, true);
	while (o != 1.0 && disp->isOpen()) {
		*f = 0.95 + o * 0.01;
	}

	o = 0.0;
	thr1.join();

	std::thread thr2(Image::multipleLoad,"LOGO", "logo/frame", 115, 2, ".png", &o, true);
	while (o != 1.0 && disp->isOpen()){
		*f = 0.96 + o * 0.01;
	}

	thr2.join();

	img_data.create(wi);
	wi->load("mouse.png");
	wi->setID("MOUSE");
	wi = nullptr;
	
	img_data.create(wi);
	wi->load("the_storm.png");
	wi->setID("BLAST_LOGO");
	wi = nullptr;

	img_data.create(wi);
	wi->load("bar_single_one_on.png");
	wi->setID("BAR_ON");
	wi = nullptr;

	img_data.create(wi);
	wi->load("bar_single_one.png");
	wi->setID("BAR_OFF");
	wi = nullptr;

	img_data.create(wi);
	wi->load("background_gameplay_start.png");
	wi->setID("BG_INTRO");
	wi = nullptr;

	/// anim
	
	std::thread thr4(Image::multipleLoad, Defaults::map_default_start, "anim/bloco", 10, Defaults::map_default_len_name_int, ".png", &o, false);
	while (o != 1.0 && disp->isOpen()) {
		*f = 0.97 + o * 0.01;
	}

	thr4.join();



	*f = 0.97;

	/* LOADING MUSICS...? */

	msk_data.create(wm);
	wm->load("musics/music_01.ogg");
	wm->set(Sound::ID, "MUSIC_0");
	wm->set(Sound::PLAYING, false);
	wm->set(Sound::PLAYMODE, Sound::LOOP);
	wm = nullptr;

	msk_data.create(wm);
	wm->load("musics/music_02.ogg");
	wm->set(Sound::ID, "MUSIC_1");
	wm->set(Sound::PLAYING, false);
	wm->set(Sound::PLAYMODE, Sound::LOOP);
	wm = nullptr;
	

	msk_data.create(wm);
	wm->load("musics/music_03.ogg");
	wm->set(Sound::ID, "MUSIC_2");
	wm->set(Sound::PLAYING, false);
	wm->set(Sound::PLAYMODE, Sound::LOOP);
	wm = nullptr;

	msk_data.create(wm);
	wm->load("musics/jump_01.wav");
	wm->set(Sound::ID, "JUMP_FX");
	wm->set(Sound::PLAYING, false);
	wm->set(Sound::PLAYMODE, Sound::ONCE);
	wm = nullptr;
	
	msk_data.create(wm);
	wm->load("musics/walk_01.wav");
	wm->set(Sound::ID, "WALK_FX");
	wm->set(Sound::PLAYING, false);
	wm->set(Sound::PLAYMODE, Sound::ONCE);
	wm = nullptr;

	*f = 0.98;

	/* CREATING SPRITES...? */

	spr_data.create(ws);
	ws->add("BLAST_LOGO"); // adds texture called DEFAULT
	ws->setID("BLAST_LOGO"); // set sprite name DEFAULT
	ws->set(Sprite::POSX, 0.0);
	ws->set(Sprite::POSY, -0.45);
	ws->set(Sprite::SCALEG, 1.0);
	ws->set(Sprite::SCALEY, 0.6);
	//ws->set(Sprite::COLLIDE, true);
	//ws->set(Sprite::AFFECTED_BY_COLLISION, false);
	//ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 0);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;

	spr_data.create(ws);
	ws->add("BAR_OFF"); // adds texture called DEFAULT
	ws->add("BAR_ON"); // adds texture called DEFAULT
	ws->setID("BAR_00"); // set sprite name DEFAULT
	ws->set(Sprite::POSX, 0.0);
	ws->set(Sprite::POSY, 0.4);
	ws->set(Sprite::SCALEG, 0.6);
	ws->set(Sprite::SCALEY, 0.22);
	ws->set(Sprite::COLLIDE, true);
	//ws->set(Sprite::DOES_COLLISION_ROTATE_OBJECT, true); // not implemented yet (probably wont)
	ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 0);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;

	spr_data.create(ws);
	ws->add("BAR_OFF"); // adds texture called DEFAULT
	ws->add("BAR_ON"); // adds texture called DEFAULT
	ws->setID("BAR_01"); // set sprite name DEFAULT
	ws->set(Sprite::POSX, 0.0);
	ws->set(Sprite::POSY, 0.60);
	ws->set(Sprite::SCALEG, 0.6);
	ws->set(Sprite::SCALEY, 0.22);
	ws->set(Sprite::COLLIDE, true);
	//ws->set(Sprite::DOES_COLLISION_ROTATE_OBJECT, true) ;// not implemented yet (probably wont)
	ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 0);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;

	spr_data.create(ws);
	ws->add("BAR_OFF"); // adds texture called DEFAULT
	ws->add("BAR_ON"); // adds texture called DEFAULT
	ws->setID("BAR_02"); // set sprite name DEFAULT
	ws->set(Sprite::POSX, 0.0);
	ws->set(Sprite::POSY, 0.8);
	ws->set(Sprite::SCALEG, 0.6);
	ws->set(Sprite::SCALEY, 0.22);
	ws->set(Sprite::COLLIDE, true);
	//ws->set(Sprite::DOES_COLLISION_ROTATE_OBJECT, true);
	ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 0);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;


	spr_data.create(ws);
	ws->add("PAUSE", 29); // adds texture called DEFAULT
	ws->setID("DEFAULT"); // set sprite name DEFAULT
	ws->set(Sprite::ANIMATIONTIME, 24.0);
	ws->set(Sprite::POSX, 0.5);
	ws->set(Sprite::POSY, 0.5);
	ws->set(Sprite::SCALEG, 0.2);
	ws->set(Sprite::COLLIDE, true);
	ws->set(Sprite::AFFECTED_BY_COLLISION, true);
	ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 2);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;
	
	spr_data.create(ws);
	ws->add("PAUSE", 29); // adds texture called DEFAULT
	ws->setID("DEFAULT3"); // set sprite name DEFAULT
	ws->set(Sprite::ANIMATIONTIME, 40.0);
	ws->set(Sprite::SCALEG, 0.4);
	ws->set(Sprite::POSX, 0.0);
	ws->set(Sprite::POSY, 0.7);
	ws->set(Sprite::COLLIDE, true);
	ws->set(Sprite::AFFECTED_BY_COLLISION, true);
	ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 2);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;
	
	spr_data.create(ws);
	ws->add("PAUSE", 29); // adds texture called DEFAULT
	ws->setID("DEFAULT2"); // set sprite name DEFAULT
	ws->set(Sprite::ANIMATIONTIME, 30.0);
	ws->set(Sprite::LAYER, 100);
	ws->set(Sprite::SCALEG, 0.6);
	ws->set(Sprite::POSX, 0.0);
	ws->set(Sprite::POSY, 0.0);
	ws->set(Sprite::COLLIDE, true);
	ws->set(Sprite::AFFECTED_BY_COLLISION, true);
	//ws->set(Sprite::DOES_COLLISION_ROTATE_OBJECT, true); // not implemented yet (probably wont)
	ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 2);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;

	*f = 0.99;

	spr_data.create(ws);
	ws->add("LOGO", 115); // adds texture called DEFAULT
	ws->setID("INTRO"); // set sprite name DEFAULT
	ws->set(Sprite::ANIMATIONTIME, 0.0);
	ws->set(Sprite::SCALEX, 1.6);
	ws->set(Sprite::SCALEY, 0.7);
	ws->set(Sprite::SCALEG, 0.6);
	//ws->set(Sprite::DRAW, false);
	ws->set(Sprite::LAYER, -1);
	ws->set(Sprite::LOOPING_IMAGES, false);
	ws->set(Sprite::USE_TINTED_DRAWING, true);
	ws = nullptr;

	/*spr_data.create(ws);
	ws->add("BAR_ON");
	ws->setID("PLAYER");
	ws->set(Sprite::LAYER, 70);
	ws->set(Sprite::SCALEG, 0.08);
	ws->set(Sprite::FOLLOWKEYBOARD, true);
	ws->set(Sprite::COLLIDE, true);
	ws->set(Sprite::AFFECTED_BY_COLLISION, true);
	ws = nullptr;*/

	spr_data.create(ws);
	ws->add("MOUSE");
	ws->setID("MOUSE");
	ws->set(Sprite::LAYER, 100);
	ws->set(Sprite::SCALEX, 0.60);
	//ws->set(Sprite::SCALEY, 1.6);
	ws->set(Sprite::SCALEG, 0.07);
	ws->set(Sprite::FOLLOWMOUSE, true);
	ws->set(Sprite::COLLIDE, true);
	ws = nullptr;

	/* CREATING A TEXT...? */

	txt_data.create(wt);
	wt->set(Text::SETSTRING, "< TEST 1 >");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::LAYER, 99);
	wt->set(Text::SETFOLLOW, "BAR_00");
	wt->set(Text::UPDATETIME, 1.0);
	wt->set(Text::SCALEG, 1.35);
	wt->set(Text::POSY, -0.053);
	wt->set(Text::POSX, 0.002);
	wt->set(Text::SETID,"BAR_00_TEXT");
	wt = nullptr;

	txt_data.create(wt);
	wt->set(Text::SETSTRING, "< TEST 2 >");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::LAYER, 99);
	wt->set(Text::SETFOLLOW, "BAR_01");
	wt->set(Text::UPDATETIME, 1.0);
	wt->set(Text::SCALEG, 1.35);
	wt->set(Text::POSY, -0.053);
	wt->set(Text::POSX, 0.002);
	wt->set(Text::SETID,"BAR_01_TEXT");
	wt = nullptr;

	txt_data.create(wt);
	wt->set(Text::SETSTRING, "< EXIT >");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::LAYER, 99);
	wt->set(Text::SETFOLLOW, "BAR_02");
	wt->set(Text::UPDATETIME, 1.0);
	wt->set(Text::SCALEG, 1.35);
	wt->set(Text::POSY, -0.053);
	wt->set(Text::POSX, 0.002);
	wt->set(Text::SETID,"BAR_02_TEXT");
	wt = nullptr;


	/*txt_data.create(wt);
	//wt->loadFromDatabase();
	wt->set(Text::SETSTRING, "PLAYER [%pos_x% : %pos_y% | %sprite_speed_x% : %sprite_speed_y%]");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::LAYER, 70);
	wt->set(Text::SETFOLLOW, "PLAYER");
	wt->set(Text::SETID,"PLAYER_TEXT");
	//wt->set(Text::USEBUFOPT, true);
	wt = nullptr;*/

	txt_data.create(wt);
	//wt->loadFromDatabase();
	wt->set(Text::SETSTRING, "0 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::LAYER, 2);
	wt->set(Text::SETFOLLOW, "DEFAULT");
	wt->set(Text::SETID,"FOLLOW_0");
	//wt->set(Text::USEBUFOPT, true);
	wt = nullptr;
	
	txt_data.create(wt);
	//wt->load();
	wt->set(Text::SETSTRING, "1 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::LAYER, 2);
	wt->set(Text::SETFOLLOW, "DEFAULT2");
	wt->set(Text::SETID,"FOLLOW_1");
	//wt->set(Text::USEBUFOPT, true);
	wt = nullptr;
	
	txt_data.create(wt);
	//wt->load();
	wt->set(Text::SETSTRING, "2 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::LAYER, 2);
	wt->set(Text::SETFOLLOW, "DEFAULT3");
	wt->set(Text::SETID,"FOLLOW_2");
	//wt->set(Text::USEBUFOPT, true);
	wt = nullptr;
	
	txt_data.create(wt);
	wt->set(Text::SETSTRING, "%fps%/%tps% | CAM: %cam_x% : %cam_y% @ %cam_zoom% | %curr_string% | %mouse_x%x%mouse_y%");
	wt->set(Text::POSX, -1.0);
	wt->set(Text::POSY, -1.0);
	wt->set(Text::SCALEG, 0.8);
	wt->set(Text::MODE, Text::ALIGN_LEFT);
	wt->set(Text::LAYER, Defaults::default_font_foreground_layer);
	wt->setMainDisplay(disp);
	wt->set(Text::UPDATETIME, 0.20);
	//wt->set(Text::COLOR, al_map_rgba_f(0.9, 0.9, 0.0, 0.9));
	wt->set(Text::AFFECTED_BY_CAM, false);
	wt->set(Text::SETID,"OSD_0");
	wt = nullptr;

	txt_data.create(wt);
	wt->set(Text::SETSTRING,
		"{COL=[%tps_col%/" + std::to_string((int)(1.0/Defaults::collision_timer)) +
		"];FUNC=[%tps_funcs%/" + std::to_string((int)(1.0 / Defaults::functions_timer)) +
		"];SEC=[%tps_second%/" + std::to_string((int)(1.0 / Defaults::calcLoops_timer)) +
		"];POSUPD=[%tps_posupd%/" + std::to_string((int)(1.0 / Defaults::updatepos_timer)) + "]}");
	wt->set(Text::POSX, -1.0);
	wt->set(Text::POSY, -0.97);
	wt->set(Text::SCALEG, 0.7);
	wt->set(Text::MODE, Text::ALIGN_LEFT);
	wt->set(Text::LAYER, Defaults::default_font_foreground_layer);
	wt->setMainDisplay(disp);
	wt->set(Text::UPDATETIME, 0.50);
	//wt->set(Text::COLOR, al_map_rgba_f(0.9, 0.9, 0.0, 0.9));
	wt->set(Text::AFFECTED_BY_CAM, false);
	wt->set(Text::SETID,"OSD_1");
	wt = nullptr;

	//al_rest(4.0);

	*f = 1.0;
	*b = true;
	logg << Log::NEEDED_START << "[THR1:LOAD_][INFO] Done." << Log::NEEDED_ENDL;
}


int sub_main(const int l, Display::display& disp)
{
	d_sprite_database spr_data;
	d_texts_database  txt_data;
	d_images_database img_data;
	d_musics_database msk_data;
	Events::big_event bev;

	Log::gfile logg;
	Camera::camera_g cam;
	Layer::layerer lyr;
	logg.flush();

	int nexte = -999999999;
	
	switch (l)
	{
	case -1:
		//return 1; // remove later
		{
			// -------------------- LOAD OF THE SCENE -------------------- //

			Sprite::sprite*   sprite_0 = nullptr;
			//Sound::track* tck = nullptr;
			
			if (!spr_data.get(sprite_0, "INTRO")) throw "FAILED TO LOAD INTRO";
			/*if (!msk_data.get(tck, ((rand()%2) ? "MUSIC_0" : "MUSIC_1"))) throw "FAILED TO LOAD MUSIC_0";*/

			sprite_0->set(Sprite::ANIMATIONTIME, -1.0);
			sprite_0->set(Sprite::ANIMATIONTIME, 20.0);
			sprite_0->set(Sprite::TINT, al_map_rgb(255,255,255));

			//tck->set(Sound::PLAYING, true);

			disp.capFPS(60);
			cam.apply(-1);
			lyr.setUsingNow(-1);

			double start_point = al_get_time();

			// -------------------- THE SCENE -------------------- //

			while (!sprite_0->gotToEnd() && (nexte == -999999999))
			{
				if ((al_get_time() - start_point) < 1.0) cam.set(-1, Camera::OFFY, pow(2.0*cos((al_get_time() - start_point)*ALLEGRO_PI*0.5),1.5));
				else cam.set(-1, Camera::OFFY, 0);
				cam.apply();

				spr_data.draw();
				txt_data.draw();


				if (!disp.flip()) return -10;

				//if (bev.g().getKey(Events::KEY_ESCAPE)) nexte = 0;
			}

			sprite_0->set(Sprite::ANIMATIONTIME, 0.0);

			for (double n = al_get_time(); al_get_time() - n < 3.0 && (nexte == -999999999);)
			{
				sprite_0->set(Sprite::TINT, al_map_rgba_f(1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0));

				spr_data.draw();
				txt_data.draw();
				if (!disp.flip()) return -10;

				//if (bev.g().getKey(Events::KEY_ESCAPE)) return 0;
			}

			// -------------------- END OF THE SCENE -------------------- //

			if (nexte == -999999999) nexte = 0;

			// nothing has to be done here
		}
		break;
	case 0:
		{
			disp.capFPS();
			cam.apply(0);
			lyr.setUsingNow(0);

			Sprite::sprite*		sprite_m = nullptr;
			Sprite::sprite*     sprite_b0 = nullptr;
			Sprite::sprite*     sprite_b1 = nullptr;
			Sprite::sprite*     sprite_b2 = nullptr;

			Sound::track* tck = nullptr;
			Sound::track* tck2 = nullptr;
			Sound::track* tck3 = nullptr;

			if (!spr_data.get(sprite_m, "MOUSE")) throw "FAILED TO LOAD MOUSE";
			if (!spr_data.get(sprite_b0, "BAR_00")) throw "FAILED TO LOAD BAR_00";
			if (!spr_data.get(sprite_b1, "BAR_01")) throw "FAILED TO LOAD BAR_01";
			if (!spr_data.get(sprite_b2, "BAR_02")) throw "FAILED TO LOAD BAR_02";

			if (!msk_data.get(tck, "MUSIC_0")) throw "FAILED TO LOAD MUSIC_0";
			if (!msk_data.get(tck2, "MUSIC_1")) throw "FAILED TO LOAD MUSIC_1";
			if (!msk_data.get(tck3, "MUSIC_2")) throw "FAILED TO LOAD MUSIC_2";

			tck->set(Sound::PLAYING, false);
			tck2->set(Sound::PLAYING, false);
			tck3->set(Sound::PLAYING, false);

			if (rand()%2) tck->set(Sound::PLAYING, true);
			else tck2->set(Sound::PLAYING, true);

			Fx::bubbles imkrA(300, 60.0, -20);

			bool mk = false;
			bool fx_wk = true;

			for (bool t = true; (t || mk) && (nexte == -999999999);)
			{
				if (fx_wk) {
					cam.set(0, Camera::ZOOM, 0.99 + 0.01*cos(al_get_time()*0.65));
					cam.set(0, Camera::OFFX, /*0.90 + */ 0.01*cos(al_get_time()*1.5));
					cam.set(0, Camera::OFFY, /*0.90 + */ 0.01*cos(al_get_time()*0.9));
				}
				else {
					cam.set(0, Camera::ZOOM, 1.0);
					cam.set(0, Camera::OFFX, 0.0);
					cam.set(0, Camera::OFFY, 0.0);
				}
				cam.apply();

				bool b0, b1, b2;
				sprite_b0->get(Sprite::_IS_COLLIDING, b0);
				sprite_b1->get(Sprite::_IS_COLLIDING, b1);
				sprite_b2->get(Sprite::_IS_COLLIDING, b2);

				mk = bev.g().getKey(Events::MOUSE_0, false);
				
				if (bev.g().getKey(Events::KEY_F, true)) fx_wk = !fx_wk;
				if (bev.g().getKey(Events::KEY_T, true)) nexte = 2;


				if (mk) {
					if (b0) { // test 1
						nexte = 1;
						t = false;
					}
					if (b1) { // test 2
						nexte = 2;
						t = false;
					}
					if (b2) { // exit
						nexte = -10;
						t = false;
					}
				}
				if (bev.g().getKey(Events::KEY_F11, true)) disp.toggleFS();

				sprite_m->set(Sprite::ROTATION, bev.g().getFunctionValNow(0));
				sprite_m->set(Sprite::SCALEG, 0.07*bev.g().getFunctionValNow(1));

				imkrA.think();
				imkrA.draw();

				img_data.draw();
				spr_data.draw();
				txt_data.draw();
				if (!disp.flip()) return -10;

				//if (bev.g().getKey(Events::KEY_ESCAPE)) return -10;
			}
		}
		break;
	/*case 1:
		{
			cam.apply(2);
			lyr.setUsingNow(2);

			Fx::lines lines(1);

			Sprite::sprite*		sprite_0 = nullptr;
			Sprite::sprite*		sprite_10 = nullptr;
			Sprite::sprite*		sprite_11 = nullptr;
			Sprite::sprite*		sprite_12 = nullptr;

			if (!spr_data.get(sprite_0, "MOUSE")) throw "FAILED TO LOAD MOUSE";
			if (!spr_data.get(sprite_10, "DEFAULT")) throw "FAILED TO LOAD DEFAULT";
			if (!spr_data.get(sprite_11, "DEFAULT2")) throw "FAILED TO LOAD DEFAULT2";
			if (!spr_data.get(sprite_12, "DEFAULT3")) throw "FAILED TO LOAD DEFAULT3";

			sprite_10->set(Sprite::POSX, 0.5);
			sprite_10->set(Sprite::POSY, 0.5);
			sprite_11->set(Sprite::POSX, 0.0);
			sprite_11->set(Sprite::POSY, 0.0);
			sprite_12->set(Sprite::POSX, 0.0);
			sprite_12->set(Sprite::POSY, 0.7);

			for (double n = al_get_time(); al_get_time() - n < 30.0 && (nexte == -999999999);) {
				sprite_0->set(Sprite::ROTATION, bev.g().getFunctionValNow(0));

				//cam.set(cam.getLastApplyID(), Camera::ZOOM, 0.75 + 0.3*cos(al_get_time()*0.65));
				//cam.set(cam.getLastApplyID(), Camera::OFFX, 0.6*cos(al_get_time()*0.41) * 1.0 / (fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM))) + 0.01);
				//cam.set(cam.getLastApplyID(), Camera::OFFY, 0.6*sin(al_get_time()*0.53) * 1.0 / (fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM))) + 0.01);

				//cam.apply();

				lines.draw();

				img_data.draw();
				spr_data.draw();
				txt_data.draw();

				if (!disp.flip()) return -10;

				if (bev.g().getKey(Events::KEY_ESCAPE)) nexte = 0;
				if (bev.g().getKey(Events::KEY_F11, true)) disp.toggleFS();
			}
			nexte = 0;
		}
		break;*/
	/*case 2:
		{
			disp.capFPS();
			cam.apply(2);
			lyr.setUsingNow(2);

			Sprite::sprite*		sprite_m = nullptr;
			Sprite::sprite*		sprite_10 = nullptr;
			Sprite::sprite*		sprite_11 = nullptr;
			Sprite::sprite*		sprite_12 = nullptr;

			if (!spr_data.get(sprite_m, "MOUSE")) throw "FAILED TO LOAD MOUSE";
			if (!spr_data.get(sprite_10, "DEFAULT")) throw "FAILED TO LOAD DEFAULT";
			if (!spr_data.get(sprite_11, "DEFAULT2")) throw "FAILED TO LOAD DEFAULT2";
			if (!spr_data.get(sprite_12, "DEFAULT3")) throw "FAILED TO LOAD DEFAULT3";

			sprite_10->set(Sprite::POSX, 0.5);
			sprite_10->set(Sprite::POSY, 0.5);
			sprite_11->set(Sprite::POSX, 0.0);
			sprite_11->set(Sprite::POSY, 0.0);
			sprite_12->set(Sprite::POSX, 0.0);
			sprite_12->set(Sprite::POSY, 0.7);

			Fx::bubbles imkrB(300, 60.0, 1);

			for (double n = al_get_time(); al_get_time() - n < 30.0 && (nexte == -999999999);)
			{
				sprite_m->set(Sprite::ROTATION, bev.g().getFunctionValNow(0));
				sprite_m->set(Sprite::SCALEG, 0.07*bev.g().getFunctionValNow(1));
				//sprite_1->set(Sprite::SCALEG, bev.g().getFunctionValNow(2));
				//cam.set(Camera::ZOOM, 0.75 + 0.3*cos(al_get_time()*0.65));
				//cam.set(Camera::OFFX, 0.6*cos(al_get_time()*0.41) * 1.0 / (fabs(cam.get(Camera::ZOOM))) + 0.01);
				//cam.set(Camera::OFFY, 0.6*sin(al_get_time()*0.53) * 1.0 / (fabs(cam.get(Camera::ZOOM))) + 0.01);

				//cam.apply();

				imkrB.think();
				imkrB.draw();

				img_data.draw();
				spr_data.draw();
				txt_data.draw();
				if (!disp.flip()) return -10;

				if (bev.g().getKey(Events::KEY_ESCAPE)) nexte = 0;
				if (bev.g().getKey(Events::KEY_F11, true)) disp.toggleFS();
			}
			nexte = 0;
		}
		break;*/
	case 1:
		{
			d_entity_database ent;
			Map::map* mapp = nullptr;
			Sprite::sprite*		sprite_m = nullptr;
			Sprite::sprite*		sprite_p = nullptr;

			Sound::track* tck = nullptr;
			Sound::track* tck2 = nullptr;
			Sound::track* tck3 = nullptr;

			if (!spr_data.get(sprite_m, "MOUSE")) throw "FAILED TO LOAD MOUSE";
			//if (!spr_data.get(sprite_p, "PLAYER")) throw "FAILED TO LOAD PLAYER";
			
			if (!msk_data.get(tck, "MUSIC_0")) throw "FAILED TO LOAD MUSIC_0";
			if (!msk_data.get(tck2, "MUSIC_1")) throw "FAILED TO LOAD MUSIC_1";
			if (!msk_data.get(tck3, "MUSIC_2")) throw "FAILED TO LOAD MUSIC_2";

			tck->set(Sound::PLAYING, false);
			tck2->set(Sound::PLAYING, false);
			tck3->set(Sound::PLAYING, true);

			if (!(mapp = new Map::map())) return 0;

			mapp->launch_player("BAR_ON");
			mapp->launch_badboys("BAR_OFF", 4);

			cam.apply(3);
			lyr.setUsingNow(3);

			mapp->setLayer(Defaults::map_default_layer);
			mapp->setSeed(rand());

			mapp->start_cpu_thr();
			mapp->start_draw_thr();

			lyr.getNow().save_package(mapp);
			lyr.getNow().setMode(Layer::USEMAP);

			double zoom = 1.0;
			double pos_x = 0.0, pos_y = 0.0;
			bool is_following_player = false;

			while (!bev.g().getKey(Events::KEY_ESCAPE) && (nexte == -999999999))
			{
				if (bev.g().getKey(Events::KEY_PAD_PLUS)) zoom *= 1.1;
				if (bev.g().getKey(Events::KEY_PAD_MINUS)) zoom *= 0.9;
				if (bev.g().getKey(Events::KEY_PAD_ENTER)) zoom = 1.0;
				if (bev.g().getKey(Events::KEY_PAD_2)) pos_y += 0.1;
				if (bev.g().getKey(Events::KEY_PAD_8)) pos_y -= 0.1;
				if (bev.g().getKey(Events::KEY_PAD_6)) pos_x += 0.1;
				if (bev.g().getKey(Events::KEY_PAD_4)) pos_x -= 0.1;
				if (bev.g().getKey(Events::KEY_PAD_5)) pos_x = pos_y = 0.0;
				if (bev.g().getKey(Events::KEY_NUMLOCK)) is_following_player = !is_following_player;

				if (is_following_player) {
					if ((sprite_p = mapp->_getPlayerSprite())) {
						sprite_p->get(Sprite::POSX, pos_x);
						sprite_p->get(Sprite::POSY, pos_y);
						zoom = 1.6;
					}
				}

				cam.set(3, Camera::ZOOM, zoom);
				cam.set(3, Camera::OFFX, pos_x);
				cam.set(3, Camera::OFFY, pos_y);
				cam.apply(3);

				mapp->checkDraw();
				mapp->checkPositionChange();

				if (mapp->hasReachedEnd()) { // NOT WORKING SOMEHOW
					//lyr.setUsingNow(0);

					mapp->setCPULock(true);
					while (mapp->isCPUtasking());

					mapp->reset_cpu_thr();
					mapp->reset_draw_thr();
					/*delete mapp;

					if (!(mapp = new Map::map())) return 0;

					mapp->launch_player("BAR_ON");
					mapp->launch_badboys("BAR_OFF", 4, Defaults::badboys_default_layer);

					mapp->setLayer(Defaults::map_default_layer);
					mapp->setSeed(rand());*/

					mapp->setSeed(rand());

					mapp->start_cpu_thr();
					mapp->start_draw_thr();

					mapp->setCPULock(false);
				}

				img_data.draw();
				spr_data.draw();
				txt_data.draw();
				if (!disp.flip()) return -10;

				if (bev.g().getKey(Events::KEY_F11, true)) disp.toggleFS();

				//ent.draw();
			}

			if (mapp) {
				mapp->setCPULock(true);
				while (mapp->isCPUtasking());

				delete mapp;
				mapp = nullptr;
			}

			if (nexte == -999999999) nexte = 0;
		}
		break;

	case 2:
		{
			// copy of case 1, but more bots and corrosion

			d_entity_database ent;
			Map::map* mapp = nullptr;
			Sprite::sprite*		sprite_m = nullptr;
			Sprite::sprite*		sprite_p = nullptr;

			Sound::track* tck = nullptr;
			Sound::track* tck2 = nullptr;
			Sound::track* tck3 = nullptr;

			if (!spr_data.get(sprite_m, "MOUSE")) throw "FAILED TO LOAD MOUSE";
			//if (!spr_data.get(sprite_p, "PLAYER")) throw "FAILED TO LOAD PLAYER";

			if (!msk_data.get(tck, "MUSIC_0")) throw "FAILED TO LOAD MUSIC_0";
			if (!msk_data.get(tck2, "MUSIC_1")) throw "FAILED TO LOAD MUSIC_1";
			if (!msk_data.get(tck3, "MUSIC_2")) throw "FAILED TO LOAD MUSIC_2";

			tck->set(Sound::PLAYING, false);
			tck2->set(Sound::PLAYING, false);
			tck3->set(Sound::PLAYING, true);

			if (!(mapp = new Map::map())) return 0;

			mapp->launch_player("BAR_ON");
			mapp->launch_badboys("BAR_OFF", 12);

			cam.apply(3);
			lyr.setUsingNow(3);

			mapp->setLayer(Defaults::map_default_layer);
			mapp->setSeed(rand());

			mapp->start_cpu_thr();
			mapp->start_draw_thr();

			lyr.getNow().save_package(mapp);
			lyr.getNow().setMode(Layer::USEMAP);

			double zoom = 1.0;
			double pos_x = 0.0, pos_y = 0.0;
			bool is_following_player = false;

			double last_corrosion = al_get_time();

			while (!bev.g().getKey(Events::KEY_ESCAPE) && (nexte == -999999999))
			{
				if (bev.g().getKey(Events::KEY_PAD_PLUS)) zoom *= 1.1;
				if (bev.g().getKey(Events::KEY_PAD_MINUS)) zoom *= 0.9;
				if (bev.g().getKey(Events::KEY_PAD_ENTER)) zoom = 1.0;
				if (bev.g().getKey(Events::KEY_PAD_2)) pos_y += 0.1;
				if (bev.g().getKey(Events::KEY_PAD_8)) pos_y -= 0.1;
				if (bev.g().getKey(Events::KEY_PAD_6)) pos_x += 0.1;
				if (bev.g().getKey(Events::KEY_PAD_4)) pos_x -= 0.1;
				if (bev.g().getKey(Events::KEY_PAD_5)) pos_x = pos_y = 0.0;
				if (bev.g().getKey(Events::KEY_NUMLOCK)) is_following_player = !is_following_player;

				if (is_following_player) {
					if ((sprite_p = mapp->_getPlayerSprite())) {
						sprite_p->get(Sprite::POSX, pos_x);
						sprite_p->get(Sprite::POSY, pos_y);
						zoom = 1.6;
					}
				}

				cam.set(3, Camera::ZOOM, zoom);
				cam.set(3, Camera::OFFX, pos_x);
				cam.set(3, Camera::OFFY, pos_y);
				cam.apply(3);

				mapp->checkDraw();
				mapp->checkPositionChange();

				if (mapp->hasReachedEnd()) { // NOT WORKING SOMEHOW
					//lyr.setUsingNow(0);

					mapp->setCPULock(true);
					while (mapp->isCPUtasking());

					mapp->reset_cpu_thr();
					mapp->reset_draw_thr();
					/*delete mapp;

					if (!(mapp = new Map::map())) return 0;

					mapp->launch_player("BAR_ON");
					mapp->launch_badboys("BAR_OFF", 4, Defaults::badboys_default_layer);

					mapp->setLayer(Defaults::map_default_layer);
					mapp->setSeed(rand());*/

					mapp->setSeed(rand());

					mapp->start_cpu_thr();
					mapp->start_draw_thr();

					mapp->setCPULock(false);
				}

				if (al_get_time() - last_corrosion > 3.0) {
					last_corrosion += 1.0;
					mapp->corruptWorldTick();
				}

				img_data.draw();
				spr_data.draw();
				txt_data.draw();
				if (!disp.flip()) return -10;

				if (bev.g().getKey(Events::KEY_F11, true)) disp.toggleFS();

				//ent.draw(); // THISSSSSSSSSSSSSSSSSSSSSSSSSSSS
			}

			if (mapp) {
				mapp->setCPULock(true);
				while (mapp->isCPUtasking());

				delete mapp;
				mapp = nullptr;
			}

			if (nexte == -999999999) nexte = 0;
		}
		break;
	}
	return nexte;
}

void temp_xtract(Stacker::extractor* xtract, Safer::safe_string interp, Safer::safe_string saveon)
{
	if (xtract) {
		xtract->interpret(interp.g());
		xtract->extractAll(saveon.g());
	}
	else throw "NULL POINTER AT TEMP_XTRACT!";
}

void killall()
{
	Events::big_event bev;

	d_sprite_database spr_data;
	d_images_database img_data;
	d_texts_database  txt_data;
	d_musics_database msk_data;

	bev.g().delAllFunctions();
	bev.g()._setShutDownNow();

	spr_data.clear();
	img_data.clear();
	txt_data.clear();
	msk_data.clear();

	while (!bev.g().isSafeToQuit());
}