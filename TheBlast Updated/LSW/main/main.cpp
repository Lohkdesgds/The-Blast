#include "..\all\enable_all.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

using namespace LSW::v2;

int main(int argc, char* argv[])
{
	Main::main cmain;
	Main::initialization init = Main::interpret_console_entry(argc, argv);

	// set configuration
	cmain.__saveConfig(init);

	// initialize display, events, camera and layers configuration
	cmain.init();

	// show Loading %% thing and load
	cmain.load();

	// READY
	while (cmain.play());

	cmain.unloadAll();

	return 0;
}