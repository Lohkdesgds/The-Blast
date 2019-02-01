#include "..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Tools {

			void throw_at_screen(const Safer::safe_string thrown, const bool diee) {
				al_show_native_message_box(nullptr, "ERROR", "Error: ",
					(thrown.g() + "\nPLEASE RESTART THE GAME!").c_str(),
					NULL,
					ALLEGRO_MESSAGEBOX_ERROR
				);
				Config::config conf;
				Log::gfile logg;

				conf.set(Config::HAD_ERROR, true);
				conf.~config();

				logg << Log::ERRDV << Log::NEEDED_START << "[__________][FATAL] THROWN ERROR SOMEWHERE: " << thrown << Log::NEEDED_ENDL;
				logg.flush();

				if (diee) exit(EXIT_FAILURE);
			}
		}
	}
}