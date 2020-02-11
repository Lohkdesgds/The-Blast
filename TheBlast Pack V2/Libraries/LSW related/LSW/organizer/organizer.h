#pragma once


// SO SOLO, MUST BE FIRST LMAO
#include "..\package_raw\package_raw.h"				// SOLO (HAS TO BE FIRST BECAUSE OF DEFINE WINDOWS LEAN AND MEAN

// barebones (each)
#include "..\shared_constants\constants.h"			// SOLO
#include "..\tools\tools.h"							// SOLO
#include "..\custom_abort\abort.h"					// SOLO
#include "..\big_templates\small_templates.h"       // SOLO
#include "..\hash\sha256.h"						    // SOLO
#include "..\pipe\pipe.h"							// DEP: TOOLS
#include "..\logger\logger.h"						// DEP: CONST, TOOLS, SMALLTEMP
#include "..\big_templates\big_templates.h"			// DEP: CONST, ABORT, SMALLTEMP
#include "..\download_manager\downloader.h"			// DEP: CONST, TOOLS

// default
#include "..\system\system.h"						// DEP: CONST, TOOLS, ABORT, LOGG, DOWN, SHA
#include "..\sound\sound.h"							// DEP: ABORT, SMALLTEMP, TEMPLATE, LOGG, SYSTEM
#include "..\drawing\drawing.h"						// DEP: ABORT, TOOLS, ABORT, SMALLTEMP, TEMPLATE, LOGG, SYSTEM, SOUND
#include "..\menutton\menutton.h"					// DEP: ABORT, TOOLS, ABORT, SMALLTEMP, TEMPLATE, LOGG, SYSTEM, SOUND, DRAWING (DRAWING + "drawing")

// gaming
#include "..\map_gen\map.h"							// DEP: CONST, ABORT, SMALLTEMP

// the big thing
#include "..\console\console.h"						// DEP: @everyone (this) (THIS IS THE MANAGER ITSELF, DON'T NEED TO SELF INCLUDE, IT IS THE MAIN .H)