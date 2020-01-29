#include "downloader.h"

void remover(const char* chars, std::string& str)
{
	for (unsigned int i = 0; i < strlen(chars); ++i)
	{
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}
}
long getSize(const char* road)
{
	FILE* fp = NULL;
	if (fopen_s(&fp, road, "r") != 0)
		return -1;

	fseek(fp, 0, SEEK_END);

	long a = ftell(fp);
	fclose(fp);
	return a;
}


namespace LSW {
	namespace v4 {
		bool download::get(const char* url)
		{
			HINTERNET connect = InternetOpen("LSW Downloader V4", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
			if (!connect) return false;
			HINTERNET OpenAddress = InternetOpenUrlA(connect, url, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
			if (!OpenAddress) return false;
			
			char DataReceived[MAXDOWNLOADSTEPSIZE];
			DWORD NumberOfBytesRead = 0;

			buf.clear();

			while (InternetReadFile(OpenAddress, DataReceived, MAXDOWNLOADSTEPSIZE, &NumberOfBytesRead) && NumberOfBytesRead) {
				for (DWORD a = 0; a < NumberOfBytesRead; a++)
				{
					buf += DataReceived[a];
				}
			}

			InternetCloseHandle(OpenAddress);
			InternetCloseHandle(connect);
			return true;
		}
		std::string& download::read()
		{
			return buf;
		}
}
}