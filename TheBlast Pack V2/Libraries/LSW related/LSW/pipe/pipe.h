#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <functional>
#include <thread>
#include <string>
#include <sstream>
#include <Windows.h>
#include <algorithm>

#include "..\tools\tools.h"	

namespace LSW {
	namespace v4 {

		class Pipe {
			HANDLE g_hChildStd_OUT_Rd = nullptr;
			HANDLE g_hChildStd_OUT_Wr = nullptr;
			SECURITY_ATTRIBUTES saAttr;

			PROCESS_INFORMATION piProcInfo; // handle = piProcInfo.hProcess
			STARTUPINFO siStartInfo;

			char inter_buf[8];

			std::function<void(const std::string)> prunt = std::function<void(const std::string)>();
			std::thread* autosav = nullptr;
			bool keep = false;

			void keep_reading();
		public:
			~Pipe();

			void hookPrint(const std::function<void(const std::string)>);
			bool launch(char*);
			void kill();
		};
	}
}
