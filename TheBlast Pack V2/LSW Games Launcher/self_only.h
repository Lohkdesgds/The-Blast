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
const std::string default_hash_check_url = "https://www.dropbox.com/s/48jyu9t93rroknx/latest_launcher_hash.hash?dl=1";

using namespace LSW::v4;

class __auto_initializer_do_not_touch {
	gfile logg;

	struct prelaunch {
		Mixer mixer; // has to be initialized once
		Database conf;
	};

	static prelaunch* init;
	static size_t counter;

	void _init();
public:
	__auto_initializer_do_not_touch();
};

inline __auto_initializer_do_not_touch::prelaunch* __auto_initializer_do_not_touch::init = nullptr;
inline size_t __auto_initializer_do_not_touch::counter = 0;

static inline const __auto_initializer_do_not_touch __i_auto_init; // static can only be accessed here lmao