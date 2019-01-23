#include "..\all\enable_all.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

using namespace LSW::v2;

int main(int argc, char* argv[])
{
	/*int button = al_show_native_message_box(
		nullptr,
		"Hey!",
		"This is a pre-release version! (useful warn)",
		"Keep in mind that pre-releases normally are incomplete.\n\n"
		"Some features are not available in this version:\n"
		"- \"Configuration file (save configuration)\"\n\n"
		"If you want to continue (people does that), please click YES.",
		NULL,
		ALLEGRO_MESSAGEBOX_YES_NO
	);
	if (button != 1) return 0;*/

	Log::gfile logg;
	logg << Log::NEEDED_START << "[MAIN:_____][INFO] Initializing..." << Log::NEEDED_ENDL;

	Main::main cmain;

	try {
		Main::initialization init = Main::interpret_console_entry(argc, argv);

		// set configuration
		cmain.__saveConfig(init);

		// initialize display, events, camera and layers configuration
		logg << Log::NEEDED_START << "[MAIN:_____][INFO] Starting display, events, camera and layers..." << Log::NEEDED_ENDL;
		cmain.init();

		// show Loading %% thing and load
		logg << Log::NEEDED_START << "[MAIN:_____][INFO] Loading resources..." << Log::NEEDED_ENDL;
		cmain.load();

		// READY
		logg << Log::NEEDED_START << "[MAIN:_____][INFO] Ready to play! Playing now!" << Log::NEEDED_ENDL;
		while (cmain.play());

		cmain.unloadAll();
	}
	catch (const Safer::safe_string& s)
	{
		logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:_____][ERROR] Caught throw at main: " << s << Log::NEEDED_ENDL;
		al_show_native_message_box(nullptr, "ERROR", "Error: ",
			s.g().c_str(),
			NULL,
			ALLEGRO_MESSAGEBOX_ERROR
		);
	}
	catch (const std::string& s)
	{
		logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:_____][ERROR] Caught throw at main: " << s << Log::NEEDED_ENDL;
		al_show_native_message_box(nullptr, "ERROR", "Error: ",
			s.c_str(),
			NULL,
			ALLEGRO_MESSAGEBOX_ERROR
		);
	}
	catch (const char* s)
	{
		logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:_____][ERROR] Caught throw at main: " << s << Log::NEEDED_ENDL;
		al_show_native_message_box(nullptr, "ERROR", "Error: ",
			s,
			NULL,
			ALLEGRO_MESSAGEBOX_ERROR
		);
	}
	catch (const int i)
	{
		if (i != 0) {
			logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:_____][ERROR] Caught throw at main: Error code #" << i << Log::NEEDED_ENDL;
			al_show_native_message_box(nullptr, "ERROR", "Error: ",
				("Returned: " + std::to_string(i)).c_str(),
				NULL,
				ALLEGRO_MESSAGEBOX_ERROR
			);
		}
	}
	catch (...)
	{
		logg << Log::ERRDV << Log::NEEDED_START << "[MAIN:_____][ERROR] Caught throw at main: Unknown error." << Log::NEEDED_ENDL;
		al_show_native_message_box(nullptr, "ERROR", "Error: ",
			"UNKNOWN?",
			NULL,
			ALLEGRO_MESSAGEBOX_ERROR
		);
	}
	logg.flush();
	try{
		cmain.unloadAll();
	}
	catch(...){
		return 0;
	}

	return 0;
}