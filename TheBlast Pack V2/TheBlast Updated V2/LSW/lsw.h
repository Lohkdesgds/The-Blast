#pragma once

// nvidia stuff
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

// just another cleaner way to say this:
#include "console/console.h"


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