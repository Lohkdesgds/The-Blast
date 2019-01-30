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

#include "..\defaults\defaults.h"
#include "..\tools\tools.h"

#pragma comment (lib, "wininet.lib")

#define MAXDOWNLOADSTEPSIZE 8192
//#define USEDEFINEDVALIFUNKNOWN true

typedef LPCSTR LP;

void remover(const char*, std::string&);
long getSize(const char*);

namespace LSW {
	namespace v2 {
		namespace Downloader {

			const char removable[] = "\n\t\r\"";

			enum commaPossib { FAIL, FUNCTIONENDD, CREATEFOLDER, DOWNLOADFILE, CALLASSYSTEM, DELETETHATFP, VERIFYDAFILE };

			const commaPossib iCmdP(const std::string);

			// imported necessary.h
			void lsw_copy_f(const std::string, const std::string);

			std::string GetSystemFolderPaths(int);

			const wchar_t* converter(const char*);
			const std::wstring converter(const std::string);
			const std::string converter(const std::wstring);

			void processList(std::string, float*);

			

			class downloader {
				static std::string mainfolder;
				std::mutex m;
				std::wstring connection_id = L"LSW V1.1 WebDownloader";
				std::queue<std::string> q;

				void push(const std::string);
			public:
				bool download(const char*, const char*, float* const = nullptr);
				bool downloadAsString(const char*, std::string&, float* const = nullptr);
				void setmainfolder(const std::string fold) { mainfolder = fold; }

				const bool getQ(std::string&);
			};

			void easyDownload(const char*, const char*, float* const = nullptr);
			void easyDownloadAsString(const char*, std::string&, float* const = nullptr);


			class lineupD {
				std::queue<std::queue<std::string>> commands;
				size_t lastsize = 0;
			public:
				const float percDone();
				void loadcommands(const std::string);
				// returns true if there are commands
				const bool nextCommand();
				const std::string nextCommaArg();
			};
		}
	}
}