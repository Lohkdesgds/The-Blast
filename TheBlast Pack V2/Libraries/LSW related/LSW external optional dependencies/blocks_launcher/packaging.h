#pragma once

#include "..\..\LSW\drawing\drawing.h"
#include "..\..\LSW\menutton\menutton.h"
#include "..\..\LSW\download_manager\downloader.h"
#include "..\..\LSW\tools\tools.h"
#include "..\..\LSW\hash\sha256.h"

namespace LSW {
	namespace v4 {

		namespace Constants {
			const std::string always_path_init = "%appdata%/Lohk's Studios/Games/";

			enum class err_packg {NONE,HASH_MISMATCH,INTERNET_DOWN,FILESYSTEM_BAD};
		}

		class LaunchBlock {
			ALLEGRO_BITMAP* bitmap_handle = nullptr;
			Sprite* banner = nullptr;
			Sprite* button_0 = nullptr;
			Sprite* button_1 = nullptr;
			
			std::string main_url_ = "";	// the main URL (aka game hash file with all data)

			std::string game_hash = ""; // HASH: ______... (txt, RAW) // hash of the game itself					--- CHECK GAME UPDATE
			std::string game_url_ = ""; // URL_: ______... (exe, EXE) // the game itself							--- SAVE AS GNAM/GAME.EXE
			std::string game_gnam = ""; // GNAM: ______... (txt, RAW) // the game's name							--- GNAM
			std::string game_bash = ""; // BASH: ______... (txt, RAW) // the game's art hash						--- SAVE AS GNAM/ART.PNG
			std::string game_bart = ""; // BART: ______... (png, PNG) // the game's art itself, 960x480 (normally)	--- CAN TEST IF PNG ALREADY DOWNLOADED IS IN FACT THE SAME!

			std::string exe_local = ""; // path to save extracted EXE
			std::string art_local = ""; // path to save extracted PNG

			std::string automatic_string_creation; // generated string from counter for stuff around

			static size_t counter;		// counter to automatically name banner, install_play and uninstall

			bool is_extractable = false;

			void checkExtractable();
			void createBackgroundSprites();																	// STEP 2: PUT THEM TO SHOW!
			Constants::err_packg loadStuffBG(const bool = false); // force download of art?					// STEP 1: DOWNLOAD AND HASH THE ART.PNG!
		public:
			LaunchBlock();

			bool loadGameUrl(const std::string);															// STEP 0: DOWNLOAD MAIN HASH AND STUFF!
			void loadGameLocal(const std::string); // game name

			Constants::err_packg extractGame();

		};

	}
}