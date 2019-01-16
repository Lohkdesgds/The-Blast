#include "stacker.h"


namespace LSW {
	namespace v2 {
		namespace Stacker {

			
			const bool extractor::interpret(const std::string path)
			{
				stack.clear();
				FILE* fp = nullptr;
				if (fopen_s(&fp, path.c_str(), "rb") != 0) {
					throw "Failed on extractor::interpret = no file found / file is null.";
					return false;
				}

				char line[96];
				fgets(line, 96, fp);

				for (auto& i : line) {
					if (i == '\n') i = '\0';
				}

				if (std::string(line) != "%LSW_C")
				{
					throw "Failed on extractor::interpret = This file is not supported!";
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
					throw "Failed on extractor::interpret = Amount of files detected not valid!";
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
						throw "Failed on extractor::interpret = One file read was bad formatted!";
						return false;
					}
					sa.filename.pop_back();

					if (sa.filename.length() == 0 || sa.siz_o_file <= 0) {
						throw "Failed on extractor::interpret = One file read was bad formatted!";
						return false;
					}
					stack.push_back(sa);
				}

				orig = path;
				fclose(fp);
				return true;
			}
			const bool extractor::extractAll()
			{
				FILE* fp = nullptr;
				if (fopen_s(&fp, orig.c_str(), "rb") != 0) {
					throw "Failed on extractor::extractAll = Could not re-open file / file is null.";
					return false;
				}

				char line[96];
				while (std::string(line) != "%LSW_E") {
					fgets(line, 96, fp);

					for (auto& i : line) {
						if (i == '\n') i = '\0';
					}

					if (feof(fp)) {
						throw "Failed on compactor::extractAll = End of index not found!";
						return false;
					}
				}
				

				for (auto& i : stack) {
					FILE* fl = nullptr;
					if (fopen_s(&fl, i.filename.c_str(), "wb") != 0) {
						throw "Failed on compactor::extractAll = One of the files could not be created!";
						return false;
					}

					__int64 readd = 0;

					for (__int64 u = 0; u < i.siz_o_file; u += readd)
					{
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
				return true;
			}

			
			void compactor::insert(std::string nuev)
			{
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
					throw "Failed on compactor::insert = no file found / file is null.";
					return;
				}
				sa.filename = nuev;
				stack.push_back(sa);
			}
			void compactor::setOut(const std::string outt)
			{
				orig = outt;
			}
			void compactor::compactAll()
			{
				FILE* fp = nullptr;
				if (fopen_s(&fp, orig.c_str(), "wb") != 0) {
					throw "Failed on compactor::compactAll = Cannot open the file for writing!";
					return;
				}

				fprintf_s(fp, "%cLSW_C\n",'%');
				fprintf_s(fp, "#SS=%zu\n", stack.size());

				for (auto& i : stack)
				{
					fprintf_s(fp, "%s\n%I64d\n", i.filename.c_str(), i.siz_o_file);
				}

				fprintf_s(fp, "%cLSW_E\n", '%');

				for (auto& i : stack) {
					FILE* fl = nullptr;
					if (fopen_s(&fl, i.filename.c_str(), "rb") != 0) {
						throw "Failed on compactor::compactAll = One of the files doesn't exist! (how?)";
						return;
					}
					
					__int64 readd = 0;

					for (__int64 u = 0; u < i.siz_o_file; u += readd)
					{
						char buf[stack_size_default];
						readd = fread(buf, sizeof(char), stack_size_default, fl);
						fwrite(buf, sizeof(char), readd, fp);
					}

					fclose(fl);
				}
				stack.clear();

				fclose(fp);
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
		}
	}
}