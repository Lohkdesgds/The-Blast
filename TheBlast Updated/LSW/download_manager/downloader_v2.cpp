#include "downloader_v2.h"

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
	namespace v2 {
		namespace Downloader {

			std::string downloader::mainfolder;

			using namespace std;

			/// CODE ///
			const wchar_t* converter(const char* chr)
			{
				return converter(string(chr)).c_str();
			}
			const wstring converter(const string raw)
			{
				wstring work;
				size_t point = 0;
				while (point < raw.length())
					work += raw[point++];

				return work;
			}
			const string converter(const wstring wraw)
			{
				string work;
				size_t point = 0;
				while (point < wraw.length())
					work += wraw[point++];

				return work;
			}
			void lsw_copy_f(const std::string from, const std::string to)
			{
				FILE *file_from = NULL;
				FILE *file_to = NULL;

				fopen_s(&file_from, from.c_str(), "rb");
				fopen_s(&file_to, to.c_str(), "wb");

				assert(file_from && file_to);

				char copying[1024];

				while (fread_s(copying, 1024, 1, 1024, file_from) > 0)
					fwrite(copying, 1, 1024, file_to);

				fclose(file_from);
				fclose(file_to);
			}
			std::string GetSystemFolderPaths(int csidl)
			{
				wchar_t Folder[1024];
				HRESULT hr = SHGetFolderPathW(0, csidl, 0, 0, Folder);
				if (SUCCEEDED(hr))
				{
					size_t num;
					char str[1024];
					//wcstombs(str, Folder, 1023);
					wcstombs_s(&num, str, Folder, 1024);
					return str;
				}
				else return "";
			}

			const commaPossib iCmdP(const std::string a)
			{
				// FUNCTIONENDD, CREATEFOLDER, DOWNLOADFILE, CALLASSYSTEM, DELETETHATFP, VERIFYDAFILE 
				if (a == "FUNCTIONENDD")
				{
					return FUNCTIONENDD;
				}
				if (a == "CREATEFOLDER")
				{
					return CREATEFOLDER;
				}
				if (a == "DOWNLOADFILE")
				{
					return DOWNLOADFILE;
				}
				if (a == "CALLASSYSTEM")
				{
					return CALLASSYSTEM;
				}
				if (a == "DELETETHATFP")
				{
					return DELETETHATFP;
				}
				if (a == "VERIFYDAFILE")
				{
					return VERIFYDAFILE;
				}
				return FAIL;
			}

			/// ************************************************************************* ///

			void downloader::push(const std::string s)
			{
				m.lock();
				q.push(s);
				m.unlock();
			}

			bool downloader::download(const char* singlefile, const char* to_where, float* const pp)
			{
				HINTERNET connect = InternetOpen((LPCSTR)connection_id.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
				if (!connect) return false;
				HINTERNET OpenAddress = InternetOpenUrlA(connect, singlefile, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
				if (!OpenAddress) return false;

				DWORD NumberOfBytesExpected = 0;
				DWORD NumberNow = 0;
				bool got_format = false;
				bool done_format = false;
				std::string working;

				char DataReceived[MAXDOWNLOADSTEPSIZE];
				DWORD NumberOfBytesRead = 0;
				FILE *fp;

				fopen_s(&fp, (mainfolder + to_where + '\0').c_str(), "wb");
				if (!fp) return false;

				while (InternetReadFile(OpenAddress, DataReceived, MAXDOWNLOADSTEPSIZE, &NumberOfBytesRead) && NumberOfBytesRead) {
					fwrite(DataReceived, 1, NumberOfBytesRead, fp);
					NumberNow += NumberOfBytesRead;

					if (!done_format)
					{
						if (!got_format) working = std::string(DataReceived);
						else working += std::string(DataReceived);

						if (working.find("%LSW_C") == 0)
						{
							got_format = true;
						}
						if ((NumberOfBytesExpected = working.find("%LSW_E")) != std::string::npos)
						{
							done_format = true;
						}
					}
					if (done_format) {
						std::stringstream ss(working);
						std::string tt;
						while (ss >> tt)
						{
							if (tt.length() == 0) continue;

							if (tt[0] >= '0' && tt[1] <= '9') {
								DWORD nuw = atoll(tt.c_str());
								NumberOfBytesExpected += nuw;
							}
						}
						working.clear();
					}

					if (NumberOfBytesExpected > 0 && pp) {
						*pp = 1.0 * NumberNow / NumberOfBytesExpected;
						if (*pp >= 1.0) {
							*pp = 0.999;
							NumberOfBytesExpected = 0;
						}
					}
				}

				*pp = 1.0;
				InternetCloseHandle(OpenAddress);
				InternetCloseHandle(connect);
				fclose(fp);
				return true;
			}
			bool downloader::downloadAsString(const char* singlefile, std::string& wheree, float* const pp)
			{
				HINTERNET connect = InternetOpen((LPCSTR)connection_id.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
				if (!connect) return false;
				HINTERNET OpenAddress = InternetOpenUrlA(connect, singlefile, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);
				if (!OpenAddress) return false;

				DWORD NumberOfBytesExpected = 0;
				DWORD NumberNow = 0;
				bool got_format = false;
				bool done_format = false;
				std::string working;

				char DataReceived[MAXDOWNLOADSTEPSIZE];
				DWORD NumberOfBytesRead = 0;

				wheree.clear();

				while (InternetReadFile(OpenAddress, DataReceived, MAXDOWNLOADSTEPSIZE, &NumberOfBytesRead) && NumberOfBytesRead) {
					for (DWORD a = 0; a < NumberOfBytesRead; a++)
					{
						wheree += DataReceived[a];
					}
					NumberNow += NumberOfBytesRead;

					if (!done_format)
					{
						if (!got_format) working = std::string(DataReceived);
						else working += std::string(DataReceived);

						if (working.find("%LSW_C") == 0)
						{
							got_format = true;
						}
						if ((NumberOfBytesExpected = working.find("%LSW_E")) != std::string::npos)
						{
							done_format = true;
						}
					}
					if (done_format) {
						std::stringstream ss(working);
						std::string tt;
						while (ss >> tt)
						{
							if (tt.length() == 0) continue;

							if (tt[0] >= '0' && tt[1] <= '9') {
								DWORD nuw = atoll(tt.c_str());
								NumberOfBytesExpected += nuw;
							}
						}
						working.clear();
					}

					if (NumberOfBytesExpected > 0 && pp) {
						*pp = 1.0 * NumberNow / NumberOfBytesExpected;
						if (*pp >= 1.0) {
							*pp = 0.999;
							NumberOfBytesExpected = 0;
						}
					}
					//fwrite(DataReceived, 1, NumberOfBytesRead, fp);
				}

				*pp = 1.0;
				InternetCloseHandle(OpenAddress);
				InternetCloseHandle(connect);
				return true;
			}

			const bool downloader::getQ(std::string& v)
			{
				if (q.size() < 1) return false;
				m.lock();
				v = q.front();
				q.pop();
				m.unlock();
				return true;
			}


			const float lineupD::percDone()
			{
				if (lastsize == 0) return 1.0f;
				else return ((1.0f *lastsize - commands.size()) * (1.0f / lastsize));
			}
			void lineupD::loadcommands(const std::string alllines)
			{
				while (commands.size() > 0)
					commands.pop();

				size_t po = 0;
				size_t max = alllines.length();
				bool keep = true;

				while (keep)
				{
					std::queue<std::string> quue;
					std::string nw = alllines.substr(po);
					char wheery[512];
					char urldwn[512];

					po += nw.find('\n') + 1;
					nw = nw.substr(0, nw.find('\n')); // point

					if (nw.find("FUNCTIONENDD") != std::string::npos)
					{
						quue.push("FUNCTIONENDD");
						commands.push(quue);

						keep = false;
						continue;
					}

					std::string comma = nw.substr(13);
					nw = nw.substr(0, 12);

					remover(removable, nw);
					remover(removable, comma);

					switch (iCmdP(nw))
					{
					case CREATEFOLDER:
						quue.push("CREATEFOLDER");
						quue.push(comma);
						commands.push(quue);
						break;
					case DOWNLOADFILE:
						sscanf_s(comma.c_str(), "%s %s", wheery, 512, urldwn, 512);
						quue.push("DOWNLOADFILE");
						quue.push(wheery);
						quue.push(urldwn);
						commands.push(quue);
						break;
					case VERIFYDAFILE:
						sscanf_s(comma.c_str(), "%s %s", wheery, 512, urldwn, 512); // what and size
						quue.push("VERIFYDAFILE");
						quue.push(wheery);
						quue.push(urldwn); // size
						commands.push(quue);
						break;
					case CALLASSYSTEM:
						quue.push("CALLASSYSTEM");
						quue.push(comma);
						commands.push(quue);
						break;
					case DELETETHATFP:
						quue.push("DELETETHATFP");
						quue.push(comma);
						commands.push(quue);
						break;
					}
				}
				lastsize = commands.size();
			}
			const bool lineupD::nextCommand()
			{
				if (commands.size() > 1)
				{
					commands.pop();
					return true;
				}
				else if (commands.size() == 1)
				{
					commands.pop();
					lastsize = 0;
				}
				return false;
			}
			const std::string lineupD::nextCommaArg()
			{
				std::string val;
				if (commands.size() > 0) {
					if (commands.front().size() > 0)
					{
						val = commands.front().front();
						commands.front().pop();
					}
				}
				return val;
			}

			void processList(std::string alllines, float* perc)
			{
				downloader down;
				lineupD thought;
				thought.loadcommands(alllines);
				bool downloadnext = true;
				string gamespath = Defaults::default_data_path.g();

				Tools::interpret_path(gamespath);
				if ((gamespath.back() == '\\') || (gamespath.back() == '/')) {
					gamespath.pop_back();
				}

				do {
					std::string wherr;
					long sizact;

					switch (iCmdP(thought.nextCommaArg()))
					{
					case FUNCTIONENDD:
						break;
					case CREATEFOLDER:
						CreateDirectoryA((gamespath + thought.nextCommaArg()).c_str(), NULL);
						break;
					case DOWNLOADFILE:
						if (downloadnext) // pra onde e url
						{
							wherr = (gamespath + thought.nextCommaArg());
							while (!down.download(thought.nextCommaArg().c_str(), wherr.c_str()));
						}
						downloadnext = true;
						break;
					case VERIFYDAFILE:
						sizact = getSize((gamespath + thought.nextCommaArg()).c_str());
						if (sizact == atol(thought.nextCommaArg().c_str()))
						{
							downloadnext = false;
						}
						break;
					case CALLASSYSTEM:
						system((gamespath + thought.nextCommaArg()).c_str());
						break;
					case DELETETHATFP:
						remove((gamespath + thought.nextCommaArg()).c_str());
						break;
					}
					*perc = thought.percDone();
				} while (thought.nextCommand());

				*perc = 1.0;
				return;
			}
			void easyDownload(const char *a, const char *b, float* const f)
			{
				Downloader::downloader down;
				down.download(a,b,f);
			}
			void easyDownloadAsString(const char *a, std::string & b, float* const f)
			{
				Downloader::downloader down;
				down.downloadAsString(a,b,f);
			}
		}
	}
}