#pragma once

#include <string>
#include <Windows.h>
#include <ShlObj.h>
#include <sstream>
#include <vector>

namespace LSW {
	namespace v4 {

		template <typename T>
		constexpr auto operator+(T e) noexcept
			-> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
		{
			return static_cast<std::underlying_type_t<T>>(e);
		}

		namespace Tools {

			const std::string paths_known[] = { "%appdata%", "%win_fonts%", "%win_photos_path%" }; //CSIDL_APPDATA, CSIDL_FONTS, CSIDL_MYPICTURES

			void createPath(std::string);
			LONGLONG getFileSize(const std::string&);
			bool GetFolderCSIDL(std::string&, const int&);
			void clearPath(std::string&, const bool = false);
			void interpret_path(std::string&);
			void createPath(std::string);
		}
	}
}