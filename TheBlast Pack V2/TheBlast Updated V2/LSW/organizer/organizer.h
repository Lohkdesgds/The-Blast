#pragma once

/*
Namespaces:
- Constants: the constant values, default values, objects and stuff that won't change while running
- Assistance: the structs, enums and classes that are not directly used by the user (may be used on a method)
*/

// debugging
#include "..\debug\debug.h"

// barebones (each)
#include "..\tools\tools.h"
#include "..\custom_abort\abort.h"

// helper (TEMPLATES DON'T KNOW WHAT THEY ARE)
#include "..\big_templates\big_templates.h"

// default
#include "..\system\system.h"
#include "..\drawing\drawing.h"


// the big thing
#include "..\console\console.h"