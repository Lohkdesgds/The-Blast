#pragma once

/*
Namespaces:
- Constants: the constant values, default values, objects and stuff that won't change while running
- Assistance: the structs, enums and classes that are not directly used by the user (may be used on a method)
*/

// barebones (each)
#include "..\shared_constants\constants.h"			// SOLO
#include "..\tools\tools.h"							// SOLO
#include "..\custom_abort\abort.h"					// SOLO
#include "..\big_templates\small_templates.h"       // SOLO
#include "..\logger\logger.h"						// DEP: CONST, TOOLS, SMALLTEMP
#include "..\big_templates\big_templates.h"			// DEP: ABORT, SMALLTEMP


// default
#include "..\system\system.h"						// DEP: CONST, TOOLS, ABORT, LOGG
#include "..\sound\sound.h"							// DEP: ABORT, SMALLTEMP, TEMPLATE, LOGG, SYSTEM
#include "..\drawing\drawing.h"						// DEP: ABORT, SMALLTEMP, TEMPLATE, LOGG, SYSTEM, SOUND


// the big thing
#include "..\console\console.h"						// DEP: @everyone (this)