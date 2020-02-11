#pragma once

#include <string>
#include "big_templates\small_templates.h"

/*

[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<------------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]
[}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{>--- LSW CUSTOMIZABLE TOP PART ---<}[=]:[=]:[=]:[=]:[=]:[=]:[=]:[=]{]
[#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#<------------------------->#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#$:$#]

> This is the file where you set up the top of the game, aka project folders and links.
> 'default_hash_check_url' must be a link to a file with:
  > HASH: a SHA256 hash (can be generated right here when the game launches, it is saved on log
  > URL_: the URL for the new game. It will open the page on your browser.

*/

// THIS enables automatic launch (aka the game will load the things automatically from this data). If you don't want that, comment this macro (why wouldn't you want this feature?)
#define AUTOMATIC_LAUNCH
// THIS you can disable if you want (need to disable this when compiling TheBlast Addon (lmao)
#define USE_DISCORD_API_EXTERNAL


namespace LSW {
	namespace v4 {
		namespace Constants {

			inline const char project_name[]						= PROJECTNAME;		// properties, c++, preprocessor definitions. Default autogen: 'PROJECTNAME="$(ProjectName)";%(PreprocessorDefinitions)'
			inline const char project_company[]						= "Lohk's Studios";	// manually put
			
			inline const std::string discord_api_extract			= strcatfast({ "%appdata%/", project_company, "/", project_name, "/data/discordapi.exe" });
			inline const std::string discord_api_extract_dll		= strcatfast({ "%appdata%/", project_company, "/", project_name, "/data/discord_game_sdk.dll" });
			inline const std::string start_zip_default_extract_path = strcatfast({ "%appdata%/", project_company, "/", project_name, "/data/data.zip" });
			inline const std::string config_default_file			= strcatfast({ "%appdata%/", project_company, "/", project_name, "/config/config.lohk" });
			inline const std::string default_file_global_path		= strcatfast({ "%appdata%/", project_company, "/", project_name, "/logs/latest.log" });

			inline const std::string default_hash_check_url			= "https://www.dropbox.com/s/o811cb6jpymicc1/latest_game_hash.hash?dl=1";
		}
	}
}