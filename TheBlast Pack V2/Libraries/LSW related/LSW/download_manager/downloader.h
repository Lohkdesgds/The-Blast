/*
THIS IS AN ADAPTED VERSION OF DOWNLOADER_V2 MADE FOR THIS GAME!
*/

#pragma once

#include <windows.h>
#include <wininet.h>
#include <ShlObj.h>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <thread>
#include <queue>
#include <vector>
#include <algorithm>
#include <mutex>
#include <time.h>
#include <sstream>

#include "..\shared_constants\constants.h"
#include "..\tools\tools.h"

#pragma comment (lib, "wininet.lib")

#define MAXDOWNLOADSTEPSIZE 8192
//#define USEDEFINEDVALIFUNKNOWN true

typedef LPCSTR LP;

void remover(const char*, std::string&);
long getSize(const char*);

namespace LSW {
	namespace v4 {

		class Download {
			std::string buf;
		public:
			bool get(const char*);
			std::string& read();
		};

	}
}