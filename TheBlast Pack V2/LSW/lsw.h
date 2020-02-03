#pragma once

// just another cleaner way to say this:
#include "organizer/organizer.h"
#include <shellapi.h>

// nvidia stuff
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

namespace LSW {
	namespace v4 {
		class __auto_initializer_do_not_touch {
			gfile logg;

			struct all {
				Textures textures;
				Fonts fonts;
				Samples samples;
				Sprites sprites;
				Texts texts;
				Tracks tracks;
				Camera gcam = Camera();
				Mixer mixer;
				Manager consol;
				Database conf;
			};

			all* init = nullptr;
		public:
			__auto_initializer_do_not_touch() {
				logg << L::SLF << fsr(__FUNCSIG__) << "[>~v~~~~~~~~~~v~ PRE-INIT ~v~~~~~~~~~~v~<]" << L::ELF;
				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing stuff in the background..." << L::ELF;
				logg << L::SLF << fsr(__FUNCSIG__) << "App version: " << Constants::version_app << L::ELF;
				logg << L::SLF << fsr(__FUNCSIG__) << "Initializing libraries..." << L::ELF;
				lswInit();

				logg << L::SLF << fsr(__FUNCSIG__) << "Verifying updates..." << L::ELF;

#ifndef _DEBUG
				{
					download d;
					d.get(Constants::default_hash_check_url.c_str());

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
		};

		static const __auto_initializer_do_not_touch __i_auto_init;
	}
}