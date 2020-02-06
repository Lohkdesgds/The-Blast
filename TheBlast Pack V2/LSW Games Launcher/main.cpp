#include "..\LSW\organizer\organizer.h"

using namespace LSW::v4;

int main() {
	try {
		gfile logg;
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


		Sleep(3000);



		// UNLOCK
		consol.resumeThread();
	}
	catch (Abort::abort a)
	{
		forceExit("Something went wrong at MAIN!", "Please report the following:", (a.from() + " -> " + a.details()).c_str());
	}
	return 0;
}