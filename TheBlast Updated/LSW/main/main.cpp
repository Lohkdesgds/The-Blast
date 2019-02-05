#include "..\all\enable_all.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

using namespace LSW::v2;

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	int button = al_show_native_message_box(
		nullptr,
		"Hey!",
		"This is a pre-release version! (useful warn)",
		"Keep in mind that pre-releases normally are incomplete.\n\n"
		"Some features are not stable in this version:\n"
		"- \"Map loading / collision\"\n\n"
		"If you want to continue (people does that), please click YES.",
		NULL,
		ALLEGRO_MESSAGEBOX_YES_NO
	);
	if (button != 1) return 0;
#endif

	Log::gfile logg;
	logg << Log::NEEDED_START << "[MAIN:_____][INFO] Initializing..." << Log::NEEDED_ENDL;

	Main::main cmain;
	Config::config conf; // has to be after cmain (allegro initialization)

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
		Tools::throw_at_screen(s, false);
	}
	catch (const std::string& s)
	{
		Tools::throw_at_screen(s, false);
	}
	catch (const char* s)
	{
		Tools::throw_at_screen(s, false);
	}
	catch (const int i)
	{
		if (i != 0) {
			Tools::throw_at_screen(std::to_string(i), false);
		}
	}
	catch (...)
	{
		Tools::throw_at_screen("UNKNOWN at MAIN", false);
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