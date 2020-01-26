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
#include "..\drawing\drawing.h"						// DEP: ABORT, TOOLS, ABORT, SMALLTEMP, TEMPLATE, LOGG, SYSTEM, SOUND

// some typedefs here
namespace LSW::v4 {
	typedef __template_static_vector<ALLEGRO_BITMAP>		Textures;
	typedef __template_static_vector<ALLEGRO_FONT>			Fonts;
	typedef __template_static_vector<ALLEGRO_SAMPLE>		Samples;

	typedef __template_static_vector<Track>					Tracks;
	typedef __template_static_vector<Sprite>				Sprites;
	typedef __template_static_vector<Text>					Texts;
}

// gaming
#include "..\map_gen\map.h"							// DEP: CONST, ABORT, SMALLTEMP

// the big thing
//#include "..\console\console.h"						// DEP: @everyone (this) (THIS IS THE MANAGER ITSELF, DON'T NEED TO SELF INCLUDE, IT IS THE MAIN .H)