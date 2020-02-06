#pragma once

// just another cleaner way to say this:
#include "..\LSW\organizer\organizer.h"
#include <shellapi.h>


/*
[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>---  LSW  GAMES LAUNCHER  ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<--------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
*/


// this project only constants
const std::string start_zip_default_extract_path = "%appdata%/Lohk's Studios/Launcher/data/data.zip"; // yos
const std::string config_default_file = "%appdata%/Lohk's Studios/Launcher/config/config.lohk"; // yus
const std::string default_file_global_path = "%appdata%/Lohk's Studios/Launcher/logs/latest.log"; // yes
const std::string default_hash_check_url = "https://www.dropbox.com/s/ccg2cxgd5mafi1y/latest_game_hash.hash?dl=0";

using namespace LSW::v4;

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

	void _init();
public:
	__auto_initializer_do_not_touch();
};

static const __auto_initializer_do_not_touch __i_auto_init; // static can only be accessed here lmao