#pragma once

#include <string>
#include <Windows.h>
#include <ShlObj.h>
#include <sstream>
#include <vector>
#include <functional>

namespace LSW {
	namespace v4 {

		namespace Tools {

			const std::string paths_known[] = { "%appdata%", "%win_fonts%", "%win_photos_path%" }; //CSIDL_APPDATA, CSIDL_FONTS, CSIDL_MYPICTURES

			void createPath(std::string);
			LONGLONG getFileSize(const std::string&);
			bool getFolderCSIDL(std::string&, const int&);
			void clearPath(std::string&, const bool = false);
			void interpretPath(std::string&);

			std::vector<std::string> genStrFormat(const std::string, const size_t, const size_t = 0); // FORMAT = "STARTSTUFF######ENDSTUFF" (# = number space)

			std::vector<bool> translBinary(const int, const size_t); // value, how many bytes to test?
		}
	}
}