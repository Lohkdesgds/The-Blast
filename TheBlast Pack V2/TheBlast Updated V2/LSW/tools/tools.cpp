#include "tools.h"

namespace LSW {
	namespace v4 {
		namespace Tools {
			void createPath(std::string s)
			{
				std::string str;
				std::stringstream ss(s);
				std::string token;
				std::vector<std::string> paths;

				while (std::getline(ss, token, '\\'))
				{
					str += token;
					paths.push_back(str);
					str += '\\';
				}

				std::string u = paths.back();
				paths.pop_back();
				if (u.rfind(".") == std::string::npos) paths.push_back(u);

				for (auto& i : paths)
				{
					CreateDirectoryA(i.c_str(), NULL);
				}
			}
			LONGLONG getFileSize(const std::string& s)
			{
				HANDLE hFile = CreateFile(s.c_str(), GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == INVALID_HANDLE_VALUE)
					return -1; // error condition, could call GetLastError to find out more

				LARGE_INTEGER size;
				if (!GetFileSizeEx(hFile, &size))
				{
					CloseHandle(hFile);
					return -1; // error condition, could call GetLastError to find out more
				}

				CloseHandle(hFile);
				return size.QuadPart;
			}
			bool GetFolderCSIDL(std::string& s, const int& u)
			{
				wchar_t Folder[1024];
				HRESULT hr = SHGetFolderPathW(0, u, 0, 0, Folder);
				if (SUCCEEDED(hr))
				{
					char str[1024];
					size_t i;
					wcstombs_s(&i, str, Folder, 1023);
					s = str + '\0';
					return true;
				}
				return false;
			}
			void clearPath(std::string& s, const bool inverted) // true = '/'
			{
				std::string str;

				for (auto& i : s) {
					if (!inverted && (i == '/')) str += '\\';
					else if (inverted && (i == '\\')) str += '/';
					else str += i;
				}

				s = str;
				str.clear();
			}

			void interpret_path(std::string& local_t)
			{
				for (size_t p = local_t.find('%'); (p != std::string::npos); p = local_t.find('%'))
				{
					if (p == 0 || (local_t[p - 1] != '\\')) {
						std::string elsee = local_t.substr(p);
						std::string path_to_add;
						std::string change;

						size_t found = 0;
						bool rfound = false;
						for (auto& i : paths_known)
						{
							if (elsee.find(i) == 0)
							{
								rfound = true;
								break;
							}
							found++;
						}

						if (!rfound) {
							if (p > 0)
							{
								change = local_t.substr(0, p - 1);
							}
							change += "\\";
							change += local_t.substr(p);
							local_t = change;

							continue;
						}

						switch (found)
						{
						case 0:
							GetFolderCSIDL(path_to_add, CSIDL_APPDATA);
							break;
						case 1:
							GetFolderCSIDL(path_to_add, CSIDL_FONTS);
							break;
						case 2:
							GetFolderCSIDL(path_to_add, CSIDL_MYPICTURES);
							break;
						}

						if (p > 0)
						{
							change = local_t.substr(0, p);
						}
						change += path_to_add + local_t.substr(p + paths_known[found].length());
						local_t = change;
					}
				}

				clearPath(local_t);
			}
		}
	}
}