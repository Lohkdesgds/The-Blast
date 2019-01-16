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

#pragma comment (lib, "wininet.lib")

#define MAXDOWNLOADSTEPSIZE 8192

typedef LPCSTR LP;

void remover(const char*, std::string&);
long getSize(const char*);

namespace LSW {

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
		//bool download(LP, const char*);
		bool download(const char*, const char*);
		//bool downloadAsString(LP, std::string&);
		bool downloadAsString(const char*, std::string&);
		void setmainfolder(const std::string fold) { mainfolder = fold; }

		const bool getQ(std::string&);
	};

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