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

			inline const char* paths_known[] = { "%appdata%", "%win_fonts%", "%win_photos_path%" }; //CSIDL_APPDATA, CSIDL_FONTS, CSIDL_MYPICTURES

			void createPath(std::string);
			LONGLONG getFileSize(const std::string&);
			bool getFolderCSIDL(std::string&, const int&);
			void clearPath(std::string&, const bool = false);
			void interpretPath(std::string&); // calls clearPath in the end, so it's good

			std::vector<std::string> genStrFormat(const std::string, const size_t, const size_t = 0); // FORMAT = "STARTSTUFF######ENDSTUFF" (# = number space)

			std::vector<bool> translBinary(const int, const size_t); // value, how many bytes to test?

			std::vector<std::pair<std::string, std::string>> breakLines(const std::string, const std::string = ":= ", const std::string = "#", const std::string = "\r\n"); // the string, the separator (like :, =, ...) and characters in the end aka endline
		}
	}
}