#include "stacker.h"

namespace LSW {
	namespace v2 {
		namespace Stacker {


			const bool extractor::interpret(const std::string path)
			{
				Log::gfile logg;

				stack.clear();
				FILE* fp = nullptr;
				if (fopen_s(&fp, path.c_str(), "rb") != 0) {
					logg << Log::ERRDV << Log::NEEDED_START << "[EXTR:INTRP][ERRR] Failed on extractor::interpret = no file found / file is null." << Log::NEEDED_ENDL;
					return false;
				}

				char line[96];
				fgets(line, 96, fp);

				for (auto& i : line) {
					if (i == '\n') i = '\0';
				}

				if (std::string(line) != "%LSW_C")
				{
					logg << Log::ERRDV << Log::NEEDED_START << "[EXTR:INTRP][ERRR] Failed on extractor::interpret = This file is not supported!" << Log::NEEDED_ENDL;
					return false;
				}

				fgets(line, 96, fp);

				for (auto& i : line) {
					if (i == '\n') i = '\0';
				}

				__int64 amountofd = 0;

				if (memcmp(line, "#SS=", 4) == 0)
				{
					sscanf_s(line + 4, "%I64d", &amountofd);
				}

				if (amountofd <= 0) {
					logg << Log::ERRDV << Log::NEEDED_START << "[EXTR:INTRP][ERRR] Failed on extractor::interpret = Amount of files detected not valid!" << Log::NEEDED_ENDL;
					return false;
				}

				for (__int64 u = 0; u < amountofd; u++)
				{
					_stack_assist sa;
					char buf[256];
					fgets(buf, 256, fp);

					fscanf_s(fp, "%I64d\n", &sa.siz_o_file);
					sa.filename = buf;
					if (sa.filename.length() == 0) {
						logg << Log::ERRDV << Log::NEEDED_START << "[EXTR:INTRP][ERRR] Failed on extractor::interpret = One file read was bad formatted!" << Log::NEEDED_ENDL;
						return false;
					}
					sa.filename.pop_back();

					if (sa.filename.length() == 0 || sa.siz_o_file <= 0) {
						logg << Log::ERRDV << Log::NEEDED_START << "[EXTR:INTRP][ERRR] Failed on extractor::interpret = One file read was bad formatted!" << Log::NEEDED_ENDL;
						return false;
					}
					stack.push_back(sa);
				}

				orig = path;
				fclose(fp);
				return true;
			}
			const bool extractor::extractAll(std::string at)
			{
				perc = 0.0;
				Log::gfile logg;

				FILE* fp = nullptr;
				if (fopen_s(&fp, orig.c_str(), "rb") != 0) {
					logg << Log::ERRDV << Log::NEEDED_START << "[EXTR:EXALL][ERRR] Failed on extractor::extractAll = Could not re-open file / file is null!" << Log::NEEDED_ENDL;
					return false;
				}

				char line[96];
				while (std::string(line) != "%LSW_E") {
					fgets(line, 96, fp);

					for (auto& i : line) {
						if (i == '\n') i = '\0';
					}

					if (feof(fp)) {
						logg << Log::ERRDV << Log::NEEDED_START << "[EXTR:EXALL][ERRR] Failed on compactor::extractAll = End of index not found / corrupted!" << Log::NEEDED_ENDL;
					}
				}

				bool useat = false;
				if (at.length() > 0)
				{
					if ((at.back() == '\\') || (at.back() == '/')) at.pop_back();
					useat = true;
				}

				bool printed_err = false;
				size_t max = stack.size(), now = 0;
				for (auto& i : stack) {
					perc = 1.0 * now / max;
					FILE* fl = nullptr;

					std::string tmps = i.filename;
					if (useat) {
						if (tmps.length() >= 2) {
							if (tmps[0] == '.')
							{
								tmps.erase(tmps.begin());
							}
						}
						tmps = at + tmps;
					}


					Tools::interpret_path(tmps);

					if (fopen_s(&fl, tmps.c_str(), "wb") != 0) {

						Tools::createPath(tmps);
						std::experimental::filesystem::remove(tmps);

						if (fopen_s(&fl, tmps.c_str(), "wb") != 0) {
							if (!printed_err) {
								logg << Log::ERRDV;
								printed_err = true;
							}
							logg << Log::NEEDED_START << "[EXTR:EXALL][ERRR] Failed on compactor::extractAll = One of the files could not be created! Skipping..." << Log::NEEDED_ENDL;
							continue;
						}
					}

					__int64 readd = 0;

					for (__int64 u = 0; u < i.siz_o_file; u += readd)
					{
						perc = 1.0 * now / max + ((1.0 / max) * u / i.siz_o_file);

						char buf[stack_size_default];

						__int64 expected;
						if (i.siz_o_file - u < stack_size_default) expected = i.siz_o_file - u;
						else expected = stack_size_default;

						readd = fread(buf, sizeof(char), expected, fp);
						fwrite(buf, sizeof(char), (size_t)readd, fl);
					}

					fclose(fl);
				}

				fclose(fp);
				perc = 1.0;

				return true;
			}


			void compactor::insert(std::string nuev)
			{
				Log::gfile logg;

				if (nuev.length() > 1)
				{
					if (nuev[0] == '\"' && nuev[nuev.length() - 1] == '\"')
					{
						nuev.pop_back();
						nuev.erase(nuev.begin() + 0);
					}
				}

				_stack_assist sa;
				sa.siz_o_file = _getFileSize(nuev);
				if (sa.siz_o_file <= 0) {
					logg << Log::ERRDV << Log::NEEDED_START << "[COMP:INSRT][ERRR] Failed on compactor::insert = no file found / file is null." << Log::NEEDED_ENDL;
					return;
				}
				sa.filename = nuev;
				stack.push_back(sa);
			}
			void compactor::insertF(std::string fold)
			{
				std::list<std::string> l;
				_folderSearch(l, fold);
				for (auto& i : l)
				{
					if (i.length() > 0) insert(i);
				}
			}
			void compactor::setOut(const std::string outt)
			{
				orig = outt;
			}
			void compactor::compactAll()
			{
				Log::gfile logg;

				FILE* fp = nullptr;
				if (fopen_s(&fp, orig.c_str(), "wb") != 0) {
					logg << Log::ERRDV << Log::NEEDED_START << "[COMP:CPALL][ERRR] Failed on compactor::compactAll = Cannot open the file for writing!" << Log::NEEDED_ENDL;
					return;
				}

				fprintf_s(fp, "%cLSW_C\n", '%');
				fprintf_s(fp, "#SS=%zu\n", stack.size());

				for (auto& i : stack)
				{
					fprintf_s(fp, "%s\n%I64d\n", i.filename.c_str(), i.siz_o_file);
				}

				fprintf_s(fp, "%cLSW_E\n", '%');

				size_t max = stack.size(), now = 0;
				for (auto& i : stack) {
					perc = 1.0 * now / max;

					FILE* fl = nullptr;
					if (fopen_s(&fl, i.filename.c_str(), "rb") != 0) {
						logg << Log::ERRDV << Log::NEEDED_START << "[COMP:CPALL][ERRR]Failed on compactor::compactAll = One of the files doesn't exist! (how?)" << Log::NEEDED_ENDL;
						return;
					}

					__int64 readd = 0;

					for (__int64 u = 0; u < i.siz_o_file; u += readd)
					{
						perc = 1.0 * now / max + ((1.0 / max) * u / i.siz_o_file);

						char buf[stack_size_default];
						readd = fread(buf, sizeof(char), stack_size_default, fl);
						fwrite(buf, sizeof(char), readd, fp);
					}

					fclose(fl);
				}
				stack.clear();

				fclose(fp);
				perc = 1.0;
			}

			void _createPath(std::string s)
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
			__int64 _getFileSize(const std::string& s)
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

			void _folderSearch(std::list<std::string>& lista, std::string local)
			{
				DIR *dir;
				struct dirent *item;

				//    std::cout << "lendo pasta " << local << std::endl;
				if (!(dir = opendir(local.c_str())))
				{
					if (!(dir = opendir(std::string(".\\" + std::string(local)).c_str())))
					{
						throw "Failed on _folderSearch: Cannot verify folders and files from here! - " + local;
						return;
					}
				}
				while ((item = readdir(dir)))
				{
					//      std::cout << "  lendo item " << item->d_name << std::endl;
					if (item->d_name[0] == '.')
						continue;
					if (item->d_type != DT_DIR)
					{
						//        std::cout << "    adicionando " << std::string(local) + "\\" + item->d_name << std::endl;
						lista.push_back(".\\" + std::string(local) + "\\" + item->d_name);
						continue;
					}
					else
					{
						//      std::cout << "    vai ler pasta " << std::string(std::string(local) + "\\" + item->d_name).c_str() << std::endl;
						_folderSearch(lista, local + "\\" + item->d_name);
					}
				}
				closedir(dir);
			}
			const size_t _stack_f::size()
			{
				return stack.size();
			}
			const float _stack_f::getPerc()
			{
				return perc;
			}
		}
	}
}