#include "self_only.h"

/*
[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>---     THE BLAST GAME    ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
*/

void __auto_initializer_do_not_touch::_init()
{
	gfile logg;
	Database conf;

	try {
		conf.load(config_default_file);
		__g_sys.setZipLocation(start_zip_default_extract_path);
		__g_sys.initSystem();
	}
	catch (Abort::abort a) {
		if (a.getErrN() == 1) {

			logg << L::SLF << fsr(__FUNCSIG__, E::WARN) << "Internal datapack wasn't found." << L::ELF;
			logg.flush();

			int res = al_show_native_message_box(
				nullptr,
				"Internal datapack not found!",
				"Your file may be broken!",
				"You can still run the app, but there will be no verification about the resource pack. Click OK if you want to continue anyway.",
				NULL,
				ALLEGRO_MESSAGEBOX_OK_CANCEL);

			if (res == 1) {
				try {
					__g_sys.posInit_forceWithNoZipAnyway();
				}
				catch (Abort::abort a) {

					std::string ext_exp = std::string("Function gone wrong: " + a.from() + "\n\nExplanation: " + a.details());

					logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "User tried to continue, but something went wrong anyway." << L::ELF;
					logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << ext_exp << L::ELF;
					logg.flush();

					forceExit("Something went wrong anyway!", "Please report the following:", ext_exp.c_str());
				}
			}
			else {
				logg << L::SLF << fsr(__FUNCSIG__) << "Failed with error " << res << L::ELF;
				logg.flush();

				forceExit();
			}
		}
		else {
			std::string ext_exp = std::string("Function gone wrong: " + a.from() + "\n\nExplanation: " + a.details());

			logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Something went wrong opening the game." << L::ELF;
			logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << ext_exp << L::ELF;
			logg.flush();

			forceExit("Something went wrong anyway!", "Please report the following:", ext_exp.c_str());
		}
	}
}

__auto_initializer_do_not_touch::__auto_initializer_do_not_touch() {
	logg.setPath(default_file_global_path);

	logg << L::SLF << fsr(__FUNCSIG__) << "[>~v~~~~~~~~~~v~ PRE-INIT ~v~~~~~~~~~~v~<]" << L::ELF;
	logg << L::SLF << fsr(__FUNCSIG__) << "Initializing stuff in the background..." << L::ELF;
	logg << L::SLF << fsr(__FUNCSIG__) << "App version: " << Constants::version_app << L::ELF;
	logg << L::SLF << fsr(__FUNCSIG__) << "Initializing libraries..." << L::ELF;
	_init();

	logg << L::SLF << fsr(__FUNCSIG__) << "Verifying updates..." << L::ELF;

#ifndef _DEBUG
	{
		download d;
		d.get(default_hash_check_url.c_str());

		char webhash_raw[96], url[96];
		sscanf_s(d.read().c_str(), "HASH: %s\nURL_: %s", webhash_raw, (unsigned int)sizeof(webhash_raw), url, (unsigned int)sizeof(url));

		char filee[1024];
		GetModuleFileNameA(NULL, filee, 1024);

		FILE* a = nullptr;
		if (fopen_s(&a, filee, "rb") == 0) {
			std::string myself;
			while (!feof(a)) {
				char ch;
				fread_s(&ch, 1, sizeof(char), 1, a);
				myself += ch;
			}
			fclose(a);

			std::string myhash = imported::sha256(myself);

			logg << L::SLF << fsr(__FUNCSIG__) << ". . . . . My hash: " << myhash << L::ELF;
			logg << L::SLF << fsr(__FUNCSIG__) << "Updated Hash file: " << webhash_raw << L::ELF;

			if (myhash != webhash_raw) {
				int e = al_show_native_message_box(
					nullptr,
					"Update available!",
					"There's an update available! Would you like to download it?",
					"Your browser will be opened with the link to download it and the game will be closed.",
					NULL,
					ALLEGRO_MESSAGEBOX_YES_NO);
				if (e == 1) { // YES
					ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
					exit(EXIT_SUCCESS);
				}

			}
		}
		else {
			logg << L::SLF << fsr(__FUNCSIG__) << "Failed checking hash :(" << L::ELF;
		}
	}

#else
	logg << L::SLF << fsr(__FUNCSIG__) << "Debug version detected. Not checking updates." << L::ELF;

#endif // !_DEBUG



	logg << L::SLF << fsr(__FUNCSIG__) << "Initializing variables..." << L::ELF;
	init = new all;

	logg << L::SLF << fsr(__FUNCSIG__) << "Setting up custom vector behaviours..." << L::ELF;

	init->textures.set(Constants::lambda_bitmap_load, Constants::lambda_bitmap_unload);
	init->fonts.set(Constants::lambda_font_load, Constants::lambda_font_unload);
	init->samples.set(Constants::lambda_sample_load, Constants::lambda_sample_unload);

	init->sprites.set(Constants::lambda_default_load<Sprite>, Constants::lambda_default_unload<Sprite>);
	init->texts.set(Constants::lambda_default_load<Text>, Constants::lambda_default_unload<Text>);
	init->tracks.set(Constants::lambda_default_load<Track>, Constants::lambda_default_unload<Track>);

	logg << L::SLF << fsr(__FUNCSIG__) << "[>~^~~~~~~~~~~^~ PRE-INIT ~^~~~~~~~~~~^~<]" << L::ELF;
}