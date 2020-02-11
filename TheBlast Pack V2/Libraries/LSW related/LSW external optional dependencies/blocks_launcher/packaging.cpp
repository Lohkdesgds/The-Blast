#include "packaging.h"


namespace LSW {
	namespace v4 {

		size_t LaunchBlock::counter = 0;

		/*
		std::string main_url_ = "";	// the main URL (aka game hash file with all data)

		std::string game_hash = ""; // HASH: ______... (txt, RAW) // hash of the game itself					--- CHECK GAME UPDATE
		std::string game_url_ = ""; // URL_: ______... (exe, EXE) // the game itself							--- SAVE AS GNAM/GAME.EXE
		std::string game_gnam = ""; // GNAM: ______... (txt, RAW) // the game's name							--- GNAM
		std::string game_bash = ""; // BASH: ______... (txt, RAW) // the game's art hash						--- SAVE AS GNAM/ART.PNG
		std::string game_bart = ""; // BART: ______... (png, PNG) // the game's art itself, 960x480 (normally)	--- CAN TEST IF PNG ALREADY DOWNLOADED IS IN FACT THE SAME!
		*/

		void LaunchBlock::checkExtractable()
		{
			is_extractable = (game_hash.length() && game_url_.length() && game_gnam.length() && game_bash.length() && game_bart.length() && exe_local.length() && art_local.length());
		}

		void LaunchBlock::createBackgroundSprites()
		{
			ResourceOf<ALLEGRO_BITMAP> bitmaps;
			if (!(bitmap_handle = bitmaps.create(automatic_string_creation + "BITMAP", art_local))) {
				throw Abort::abort(__FUNCSIG__, "Unexpected failure when loading art into memory! There should have been a file there! '" + art_local + "'");
			}
			else {
				ResourceOf<Sprite> sprites;

				banner = sprites.create(automatic_string_creation + "SPRITE_BANNER");
				button_0 = sprites.create(automatic_string_creation + "SPRITE_0");
				button_1 = sprites.create(automatic_string_creation + "SPRITE_1");

				banner->set(Constants::io__sprite_string::ID, automatic_string_creation + "SPRITE_BANNER");
				banner->set(Constants::io__sprite_boolean::DRAW, true);
				banner->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
				banner->set(Constants::io__sprite_collision_mode::COLLISION_TRANSPARENT);
				banner->set(Constants::io__sprite_integer::LAYER, 0);
				banner->set(Constants::io__sprite_string::ADD, automatic_string_creation + "BITMAP");

				button_0->set(Constants::io__sprite_string::ID, automatic_string_creation + "SPRITE_BANNER");
				button_0->set(Constants::io__sprite_boolean::DRAW, true);
				button_0->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
				button_0->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				button_0->set(Constants::io__sprite_integer::LAYER, 0);
				button_0->set(Constants::io__sprite_double::POSX, -0.5);
				button_0->set(Constants::io__sprite_double::SCALEG, 0.5);
				button_0->set(Constants::io__sprite_string::ADD, "UNINSTALL");
				button_0->set(Constants::io__sprite_string::ADD, "UNINSTALLING");

				button_1->set(Constants::io__sprite_string::ID, automatic_string_creation + "SPRITE_BANNER");
				button_1->set(Constants::io__sprite_boolean::DRAW, true);
				button_1->set(Constants::io__sprite_boolean::RESPECT_CAMERA_LIMITS, false);
				button_1->set(Constants::io__sprite_collision_mode::COLLISION_MOUSEONLY);
				button_1->set(Constants::io__sprite_integer::LAYER, 0);
				button_1->set(Constants::io__sprite_double::POSX, 0.5);
				button_1->set(Constants::io__sprite_double::SCALEG, 0.5);
				button_1->set(Constants::io__sprite_string::ADD, "INSTALL");
				button_1->set(Constants::io__sprite_string::ADD, "INSTALLING");
				button_1->set(Constants::io__sprite_string::ADD, "PLAY");
			}
		}

		Constants::err_packg LaunchBlock::loadStuffBG(const bool force_download_art_anyway)
		{
			auto siz = Tools::getFileSize(art_local);
			gfile logg;

			if (!force_download_art_anyway && siz != 0) {
				FILE* a = nullptr;
				std::string content;

				auto err = fopen_s(&a, art_local.c_str(), "rb");
				if (err) return Constants::err_packg::FILESYSTEM_BAD;

				for (char ubuf; fread_s(&ubuf, 1, sizeof(char), 1, a);) content += ubuf;

				std::string thisart_hash = imported::sha256(content);

				if (thisart_hash != game_bash) {
					logg << L::SLF << fsr(__FUNCSIG__) << "Had to update art of '" << game_gnam << "'" << L::ELF;
					return loadStuffBG(true);
				}
				else return Constants::err_packg::NONE;
			}
			else { // has to download
				Download art_;

				art_.get(game_bart.c_str());

				std::string thisart_hash = imported::sha256(art_.read());

				if (thisart_hash != game_bash) return Constants::err_packg::HASH_MISMATCH;

				FILE* endf = nullptr;
				auto err = fopen_s(&endf, art_local.c_str(), "wb+");

				if (err) return Constants::err_packg::FILESYSTEM_BAD;

				for (auto& i : art_.read()) fwrite(&i, sizeof(char), 1, endf);

				fclose(endf);
			}
			return Constants::err_packg::NONE;
		}

		LaunchBlock::LaunchBlock()
		{
			automatic_string_creation = "__I_LAUNCHBLOCK_ID_" + std::to_string(counter++) + "_";
		}

		bool LaunchBlock::loadGameUrl(const std::string url)
		{
			main_url_ = url;

			gfile logg;
			Download down;
			if (!down.get(main_url_.c_str())) throw Abort::warn(__FUNCSIG__, "Cannot get game on path " + url);

			auto vectu = Tools::breakLines(down.read(), ":= "); // default values are mine lmao haha

			for (auto& i : vectu) {
				if (i.first == "HASH") {
					game_hash = i.second;
				}
				if (i.first == "URL_") {
					game_url_ = i.second;
				}
				if (i.first == "GNAM") {
					game_gnam = i.second;
				}
				if (i.first == "BASH") {
					game_bash = i.second;
				}
				if (i.first == "BART") {
					game_bart = i.second;
				}
			}

			loadGameLocal(game_gnam);
			checkExtractable();
			if (loadStuffBG() == Constants::err_packg::NONE) {
				logg << L::SLF << fsr(__FUNCSIG__) << "Added/Loaded successfully game named '" << game_gnam << "'" << L::ELF;
				createBackgroundSprites();
				return true;
			}
			else return false;
			
		}

		void LaunchBlock::loadGameLocal(const std::string nam)
		{
			game_gnam = nam;

			exe_local = Constants::always_path_init + game_gnam + "/game.exe";
			Tools::interpretPath(exe_local);
			Tools::createPath(exe_local);

			art_local = Constants::always_path_init + game_gnam + "/art.png";
			Tools::interpretPath(art_local);
			Tools::createPath(art_local);
		}

		Constants::err_packg LaunchBlock::extractGame()
		{
			if (is_extractable) {
				gfile logg;
				Download game;

				logg << L::SLF << fsr(__FUNCSIG__) << "Downloading game '" << game_gnam << "' ..." << L::ELF;

				game.get(game_url_.c_str());
				
				logg << L::SLF << fsr(__FUNCSIG__) << "Verifying package '" << game_gnam << "' ..." << L::ELF;

				std::string thisgamehash = imported::sha256(game.read());

				if (thisgamehash != game_hash) return Constants::err_packg::HASH_MISMATCH;

			}
			return Constants::err_packg::FILESYSTEM_BAD;
		}

	}
}