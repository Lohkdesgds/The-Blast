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
	};
}

int sub_main(const int, Display::display&);
void temp_thr_load(float*, bool*, Display::display*);
void killall();

int main(int argc, char* argv[])
{
	local::initt strt;
	if (argc > 1)
	{
		for (int u = 0; u < argc; u++)
		{
			Safer::safe_string wrk = argv[u];
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
			if (wrk == "-fixedmemory")
			{
				strt.fixmem = true;
				strt.changed = true;
			}
		}
	}

	prepare_commom_templates

	d_sprite_database spr_data; // ya, does need to draw!
	d_images_database img_data; // does not need to draw.
	d_texts_database  txt_data; // ya, does need to draw!
	d_musics_database msk_data; // there's no way to draw.

	Display::display disp;
	Log::gfile logg;
	Camera::camera_g cam; // cam stuff
	Layer::layerer lyr; // layering stuff
	Events::big_event bev;

	logg << "[" << Log::TIMEF << "]" << "[INFO] Starting the app..." << Log::ENDL;

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

		lyr.getOne(0).set(1).colliding[2] = true;


		/* CREATING LAYERS FOR CAMERA...? */

		// LAYER -2
		cam.set(-2, -2, true);
		cam.set(-2, Camera::ZOOM, 1.0);
		cam.set(-2, Camera::OFFY, 2.7);

		// LAYER -1
		cam.set(-1, -1, true);
		cam.set(-1, Camera::ZOOM, 1.1);

		// LAYER 0
		cam.set(0, Camera::ZOOM, 1.0);
		for (int p = 0; p < 4; p++) cam.set(0, p, true);




		float perc = 0.0;
		bool isdone = false;
		bool hasbeenkilledwhile = false;

		Text::text* wt = nullptr;
		Text::text* wt2 = nullptr;

		txt_data.create(wt2);
		wt2->loadFromDatabase();
		wt2->set(Text::SETSTRING, "Please don't close the game until full load!");
		wt2->set(Text::USEBUFOPT, true);
		wt2->set(Text::POSX, 0.0);
		wt2->set(Text::POSY, 3.26);
		wt2->set(Text::MODE, Text::ALIGN_CENTER);
		wt2->set(Text::AFFECTED_BY_CAM, false);
		wt2->set(Text::UPDATETIME, 1.00);
		wt2->set(Text::LAYER, -2);
		wt2->setID("LOADING_1");

		txt_data.create(wt);
		wt->set(Text::SETSTRING, "Loading...");
		wt->set(Text::USEBUFOPT, true);
		wt->set(Text::POSX, 0.0);
		wt->set(Text::POSY, 3.2);
		wt->set(Text::MODE, Text::ALIGN_CENTER);
		wt->set(Text::AFFECTED_BY_CAM, false);
		wt->setMainDisplay(&disp);
		wt->set(Text::UPDATETIME, 0.05);
		wt->set(Text::LAYER, -2);
		wt->setID("LOADING_0");

		disp.checkUpImages();

		std::thread thr(temp_thr_load, &perc, &isdone, &disp);

		disp.capFPS(60);
		cam.apply(-2);

		while (!isdone)
		{
			if (!hasbeenkilledwhile) {

				cam.set(-2, Camera::ROTATION, cos(al_get_time())*0.04);
				cam.apply();

				//spr_data.draw();
				wt->set(Text::SETSTRING, "Loading [" + std::to_string(100.0*perc) + "\\%]");

				txt_data.draw();
				spr_data.draw();

				if (!disp.flip()) {
					hasbeenkilledwhile = true;
				}
			}
			else {
				al_rest(0.1);
			}
		}

		wt->set(Text::SETSTRING, "Loaded!");

		for (double d = al_get_time(); (al_get_time() - d < 3.0) && !hasbeenkilledwhile;) {
			cam.set(-2, Camera::ROTATION, cos(al_get_time())*0.04);
			cam.apply();

			wt->set(Text::COLOR, al_map_rgba_f(1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0));
			wt2->set(Text::COLOR, al_map_rgba_f(1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0, 1.0 - (al_get_time() - d) / 3.0));

			txt_data.draw();
			spr_data.draw();

			if (!disp.flip()) {
				hasbeenkilledwhile = true;
			}
		}

		//wt = nullptr;

		thr.join();

		disp.checkUpImages();

		txt_data.remove("LOADING_1");

		if (hasbeenkilledwhile) {
			killall();
			return -2;
		}


		cam.set(-2, -2, true);
		cam.set(-2, Camera::ZOOM, 1.0);
		cam.set(-2, Camera::OFFY, 0.0);
		cam.set(-2, Camera::ROTATION, 0.0);
		cam.apply(-2);

		wt->set(Text::SETSTRING, "Checking...");
		wt->set(Text::USEBUFOPT, true);
		wt->set(Text::POSX, 0.98);
		wt->set(Text::POSY, 0.87);
		wt->set(Text::MODE, Text::ALIGN_RIGHT);
		wt->set(Text::COLOR, al_map_rgba_f(0.5,0.5,0.5,0.5));
		wt->set(Text::UPDATETIME, 0.2);
		disp.capFPS(30);


		img_data.work().lock();
		for (auto& i : img_data.work().work())
		{
			if (disp.flip(false)) {
				txt_data.draw();
				spr_data.draw();
			}					

			i->reload();

			wt->set(Text::SETSTRING, "Checking: " + i->whoAmI().g());
		}
		img_data.work().unlock();

		//al_rest(0.5);
	}


	if (strt.fixmem) {
		Image::image_low* nulp;
		img_data.get(nulp, 0);
		assert(nulp);
		nulp->_setKeepOnMemory(strt.fixmem);
	}

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

	for (int res = -1; bev.g()._keep() && res != -10;)
	{
		res = sub_main(res, disp);
	}

	killall();
	

	return 0;
}

void temp_thr_load(float* f, bool* b, Display::display* disp)
{
	if (!f || !b || !disp) throw "Wasn't supposed to work this way! Error on Line 1 temp_thr_load";

	*b = false;
	*f = 0.0;

	//al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

	d_sprite_database spr_data;
	d_images_database img_data;
	d_texts_database  txt_data;
	d_musics_database msk_data;

	Sprite::sprite*   ws = nullptr;
	Image::image_low* wi = nullptr;
	Text::text*       wt = nullptr;
	Sound::track*	  wm = nullptr;

	/* DOWNLOADING TEXTURES */
	//Tools::saveFromCloud("https://www.dropbox.com/s/y3vgmzb6qhyqf5i/mouse.png?dl=1", "mouse.png");


	/* LOADING TEXTURES...? */
	float o = 0.0;

	std::thread thr1(Image::multipleCloudLoad, "DEFAULT", "pause/pause_", 29, 2, ".png", nullptr, &o);
	while (o != 1.0) {
		*f = o * 0.48;
	}

	o = 0.0;
	thr1.join();

	std::thread thr2(Image::multipleCloudLoad,"INTRO", "logo/frame", 115, 2, ".png", nullptr, &o);
	while (o != 1.0){
		*f = 0.48 + o * 0.48;
	}

	thr2.join();

	img_data.create(wi);
	wi->load("mouse.png");
	wi->loadFromDatabase(/*"https://www.dropbox.com/s/y3vgmzb6qhyqf5i/mouse.png?dl=1",*/ "mouse.png");
	wi->setID("MOUSE");
	wi = nullptr;

	*f = 0.96;

	/* CREATING MUSICS...? */

	msk_data.create(wm);
	wm->loadFromDatabase("musics/music_01.wav");
	wm->set(Sound::ID, "MUSIC_0");
	//wm->set(Sound::PLAYING, true);
	wm->set(Sound::PLAYMODE, Sound::LOOP);

	*f = 0.965;

	/* CREATING SPRITES...? */

	spr_data.create(ws);
	ws->add("DEFAULT", 29); // adds texture called DEFAULT
	ws->setID("DEFAULT"); // set sprite name DEFAULT
	ws->set(Sprite::ANIMATIONTIME, 24.0);
	ws->set(Sprite::POSX, 0.5);
	ws->set(Sprite::POSY, 0.5);
	ws->set(Sprite::LAYER, 1);
	ws->set(Sprite::SCALEG, 0.2);
	ws->set(Sprite::COLLIDE, true);
	ws->set(Sprite::AFFECTED_BY_COLLISION, true);
	ws->set(Sprite::SHOWBOX, true);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;

	*f = 0.97;

	spr_data.create(ws);
	ws->add("DEFAULT", 29); // adds texture called DEFAULT
	ws->setID("DEFAULT3"); // set sprite name DEFAULT
	ws->set(Sprite::ANIMATIONTIME, 40.0);
	ws->set(Sprite::SCALEG, 0.4);
	ws->set(Sprite::POSX, 0.0);
	ws->set(Sprite::POSY, 0.7);
	ws->set(Sprite::COLLIDE, true);
	ws->set(Sprite::AFFECTED_BY_COLLISION, true);
	ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 1);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;

	*f = 0.975;

	spr_data.create(ws);
	ws->add("DEFAULT", 29); // adds texture called DEFAULT
	ws->setID("DEFAULT2"); // set sprite name DEFAULT
	ws->set(Sprite::ANIMATIONTIME, 30.0);
	ws->set(Sprite::SCALEG, 0.6);
	ws->set(Sprite::POSX, 0.0);
	ws->set(Sprite::POSY, 0.0);
	ws->set(Sprite::COLLIDE, true);
	ws->set(Sprite::AFFECTED_BY_COLLISION, true);
	ws->set(Sprite::SHOWBOX, true);
	ws->set(Sprite::LAYER, 1);
	//ws->set(Sprite::AFFECTED_BY_CAM, false);
	ws = nullptr;

	*f = 0.98;

	spr_data.create(ws);
	ws->add("INTRO", 115); // adds texture called DEFAULT
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

	*f = 0.985;

	spr_data.create(ws);
	ws->add("MOUSE");
	ws->setID("MOUSE");
	ws->set(Sprite::LAYER, 2);
	ws->set(Sprite::SCALEX, 0.60);
	//ws->set(Sprite::SCALEY, 1.6);
	ws->set(Sprite::SCALEG, 0.07);
	ws->set(Sprite::FOLLOWMOUSE, true);
	ws->set(Sprite::COLLIDE, true);
	ws = nullptr;

	*f = 0.99;


	/* CREATING A TEXT...? */

	txt_data.create(wt);
	//wt->loadFromDatabase();
	wt->set(Text::SETSTRING, "0 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::SETFOLLOW, "DEFAULT");
	wt->setID("FOLLOW_0");
	//wt->set(Text::USEBUFOPT, true);
	wt = nullptr;

	*f = 0.993;

	txt_data.create(wt);
	wt->loadFromDatabase();
	wt->set(Text::SETSTRING, "1 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::SETFOLLOW, "DEFAULT2");
	wt->setID("FOLLOW_1");
	//wt->set(Text::USEBUFOPT, true);
	wt = nullptr;

	*f = 0.996;

	txt_data.create(wt);
	wt->loadFromDatabase();
	wt->set(Text::SETSTRING, "2 [%pos_x% %pos_y%] at XY %sprite_speed_x% : %sprite_speed_y%");
	wt->set(Text::MODE, Text::ALIGN_CENTER);
	wt->set(Text::SETFOLLOW, "DEFAULT3");
	wt->setID("FOLLOW_2");
	//wt->set(Text::USEBUFOPT, true);
	wt = nullptr;

	*f = 0.999;

	txt_data.create(wt);
	wt->set(Text::SETSTRING, "%fps%/%tps% | CAM: %cam_x% : %cam_y% @ %cam_zoom% | %curr_string% | %mouse_x%x%mouse_y%");
	wt->set(Text::POSX, -1.0);
	wt->set(Text::POSY, -1.0);
	wt->set(Text::MODE, Text::ALIGN_LEFT);
	wt->setMainDisplay(disp);
	wt->set(Text::UPDATETIME, 0.20);
	wt->setID("OSD_0");
	wt = nullptr;

	//al_rest(4.0);

	*f = 1.0;
	*b = true;
}


int sub_main(const int l, Display::display& disp)
{
	d_sprite_database spr_data;
	d_texts_database  txt_data;
	d_images_database img_data;
	Events::big_event bev;

	Log::gfile logg;
	Camera::camera_g cam;
	logg.flush();

	
	switch (l)
	{
	case -1:
		//return 1; // remove later
		{
			// -------------------- LOAD OF THE SCENE -------------------- //

			Sprite::sprite*   sprite_0 = nullptr;

			if (!spr_data.get(sprite_0, "INTRO")) throw "FAILED TO LOAD INTRO";

			sprite_0->set(Sprite::ANIMATIONTIME, -1.0);
			sprite_0->set(Sprite::ANIMATIONTIME, 20.0);
			sprite_0->set(Sprite::TINT, al_map_rgb(255,255,255));

			disp.capFPS(60);
			cam.apply(-1);

			double start_point = al_get_time();

			// -------------------- THE SCENE -------------------- //

			while (!sprite_0->gotToEnd())
			{
				if ((al_get_time() - start_point) < 1.0) cam.set(-1, Camera::OFFY, pow(2.0*cos((al_get_time() - start_point)*ALLEGRO_PI*0.5),1.5));
				else cam.set(-1, Camera::OFFY, 0);
				cam.apply();

				spr_data.draw();
				txt_data.draw();


				if (!disp.flip()) return -1;
			}

			sprite_0->set(Sprite::ANIMATIONTIME, 0.0);

			for (double n = al_get_time(); al_get_time() - n < 3.0;)
			{
				sprite_0->set(Sprite::TINT, al_map_rgba_f(1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0, 1.0 - (al_get_time() - n) / 3.0));

				spr_data.draw();
				txt_data.draw();
				if (!disp.flip()) return -1;
			}

			// -------------------- END OF THE SCENE -------------------- //

			// nothing has to be done here
		}
		return 0;
	case 0:
		{
			disp.capFPS();
			cam.apply(0);
			Sprite::sprite*		sprite_0 = nullptr;
			Sprite::sprite*		sprite_1 = nullptr;

			if (!spr_data.get(sprite_0, "MOUSE")) throw "FAILED TO LOAD MOUSE";
			if (!spr_data.get(sprite_1, "DEFAULT2")) throw "FAILED TO LOAD DEFAULT2";

			Fx::bubbles imkr(300, 60.0);

			for (double n = al_get_time(); al_get_time() - n < 5.0;)
			{
				sprite_0->set(Sprite::ROTATION, bev.g().getFunctionValNow(0));
				sprite_0->set(Sprite::SCALEG, 0.07*bev.g().getFunctionValNow(1));
				//sprite_1->set(Sprite::SCALEG, bev.g().getFunctionValNow(2));
				//cam.set(Camera::ZOOM, 0.75 + 0.3*cos(al_get_time()*0.65));
				//cam.set(Camera::OFFX, 0.6*cos(al_get_time()*0.41) * 1.0 / (fabs(cam.get(Camera::ZOOM))) + 0.01);
				//cam.set(Camera::OFFY, 0.6*sin(al_get_time()*0.53) * 1.0 / (fabs(cam.get(Camera::ZOOM))) + 0.01);

				//cam.apply();

				imkr.think();
				imkr.draw();

				img_data.draw();
				spr_data.draw();
				txt_data.draw();
				if (!disp.flip()) return -1;
			}
			return 1;
		}
	case 1:
		{
			Fx::lines lines;

			Sprite::sprite*		sprite_0 = nullptr;

			if (!spr_data.get(sprite_0, "MOUSE")) throw "FAILED TO LOAD MOUSE";

			for (double n = al_get_time(); al_get_time() - n < 5.0;) {
				sprite_0->set(Sprite::ROTATION, bev.g().getFunctionValNow(0));

				/*cam.set(cam.getLastApplyID(), Camera::ZOOM, 0.75 + 0.3*cos(al_get_time()*0.65));
				cam.set(cam.getLastApplyID(), Camera::OFFX, 0.6*cos(al_get_time()*0.41) * 1.0 / (fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM))) + 0.01);
				cam.set(cam.getLastApplyID(), Camera::OFFY, 0.6*sin(al_get_time()*0.53) * 1.0 / (fabs(cam.get(cam.getLastApplyID(), Camera::ZOOM))) + 0.01);

				cam.apply();*/

				lines.draw();

				img_data.draw();
				spr_data.draw();
				txt_data.draw();

				if (!disp.flip()) return -1;
			}
			return -1;
		}
	}

	return -10;
}

void killall()
{
	Events::big_event bev;
	d_sprite_database spr_data;
	d_images_database img_data;
	d_texts_database  txt_data;
	d_musics_database msk_data;

	bev.g().delAllFunctions();

	spr_data.clear();
	img_data.clear();
	txt_data.clear();
	msk_data.clear();
}