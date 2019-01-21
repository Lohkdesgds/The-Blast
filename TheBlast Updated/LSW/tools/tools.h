#pragma once

#include <string>
#include <ShlObj.h>
#include <sstream>
#include <list>

#include "..\safer\safer.h"

#define LIMIT_CHARL 4096

namespace LSW {
	namespace v2 {
		namespace experimental {
			namespace Tools {

				template<class T, typename U = int>
				class manager {
					static Safer::safe_vector<T*> data;
					static size_t(*f) (const Safer::safe_string, Safer::safe_vector<T*> &, bool &);
					static void(*drw) ();
					static U arg; // not being used nowadays
				public:
					manager(size_t(*fi) (const Safer::safe_string, Safer::safe_vector<T*> &, bool &) = nullptr, void(*df) () = nullptr);

					void add(T*);
					void create(T*&);
					void remove(const Safer::safe_string); // id
					void remove(const size_t);

					void clear();

					void draw();

					const bool get(T*&, const Safer::safe_string); // get from database by id
					const bool get(T*&, const size_t);
					U& getArg();

					Safer::safe_vector<T*>& work();
				};
							   

				template<class T, typename U> Safer::safe_vector<T*> manager<T,U>::data;
				template<class T, typename U> size_t (* manager<T,U>::f) (const Safer::safe_string, Safer::safe_vector<T*> &, bool &);
				template<class T, typename U> void (* manager<T,U>::drw) ();
				template<class T, typename U> U manager<T, U>::arg;


				template<class T, typename U>
				inline manager<T, U>::manager(size_t(*fi)(const Safer::safe_string, Safer::safe_vector<T*>&, bool &), void(*df) ())
				{
					if (!(f)) {
						/*if (!(fi)) {
							throw "FATAL ERROR AT FIRST DECLARATION OF A CLASS (constructor of manager)!";
							exit(-1);
						}*/
						if (fi) f = fi;
					}
					if (!(drw))
					{
						if (df) drw = df;
					}
				}

				template<class T, typename U>
				inline void manager<T, U>::add(T* u)
				{
					if (!u) return;
					data.push(u);
				}

				template <class T, typename U>
				inline void manager<T, U>::create(T*& u)
				{
					if (!u) u = new T();
					if (!u) {
						throw "CANNOT USE NEW AT manager::create";
						exit(-1);
					}
					data.push(u);
				}

				template<class T, typename U>
				inline void manager<T, U>::remove(const Safer::safe_string s)
				{
					if (!f) return;

					bool right = false;
					size_t pos = f(s, data, right);
					if (right) {
						remove(pos);
					}
				}
				

				template <class T, typename U>
				inline void manager<T, U>::remove(const size_t pos)
				{
					data.erase(pos);
				}

				template <class T, typename U>
				inline void manager<T, U>::clear()
				{
					data.clear();
				}

				template<class T, typename U>
				inline void manager<T, U>::draw()
				{
					if (drw) drw();
				}

				template <class T, typename U>
				inline const bool manager<T, U>::get(T*& r, const Safer::safe_string s) // get from database by id
				{
					if (!f) return false;

					bool right = false;
					size_t pos = f(s, data, right);
					data.lock();

					if (right)
					{
						r = data[pos];
						data.unlock();
						return true;
					}
					data.unlock();
					return false;
				}

				template<class T, typename U>
				inline const bool manager<T, U>::get(T *& r, const size_t pos)
				{
					data.lock();
					if (pos < data.getMax()) {
						r = data[pos];
						data.unlock();
						return true;
					}
					data.unlock();
					return false;
				}

				template<class T, typename U>
				inline U & manager<T, U>::getArg()
				{
					return arg;
				}

				template <class T, typename U>
				inline Safer::safe_vector<T*>& manager<T, U>::work()
				{
					return data;
				}

#define d_sprite_database experimental::Tools::manager<LSW::v2::Sprite::sprite/*,LSW::v2::Sprite::_sprites_database_data*/>
#define d_images_database experimental::Tools::manager<LSW::v2::Image::image_low/*,std::thread**/>
#define d_texts_database  experimental::Tools::manager<LSW::v2::Text::text>
#define d_musics_database experimental::Tools::manager<LSW::v2::Sound::track>
#define d_entity_database experimental::Tools::manager<LSW::v2::Entities::entity>

#define prepare_commom_templates \
{ \
d_sprite_database spr_data(LSW::v2::Sprite::_find,LSW::v2::Sprite::_draw); \
d_images_database img_data(LSW::v2::Image::_find, LSW::v2::Image::_draw); \
/*LSW::v2::Image::_start_thread();*/ \
d_texts_database  txt_data(LSW::v2::Text::_find,LSW::v2::Text::_draw); \
d_musics_database msk_data(LSW::v2::Sound::_find); \
d_entity_database ent_data(LSW::v2::Entities::_find, LSW::v2::Entities::_draw); \
}

			}
		}
		
		namespace Tools{

			const std::string paths_known[] = { "%appdata%", "%win_fonts%", "%win_photos_path%" }; //CSIDL_APPDATA, CSIDL_FONTS, CSIDL_MYPICTURES

			template <template<class, class, class...> class C, typename K, typename V, typename... Args>
			V GetWithDef(const C<K, V, Args...>& m, K const& key, const V & defval)
			{
				typename C<K, V, Args...>::const_iterator it = m.find(key);
				if (it == m.end())
					return defval;
				return it->second;
			}

			inline const bool GetFolderCSIDL(Safer::safe_string& s, const int& u)
			{
				wchar_t Folder[1024];
				HRESULT hr = SHGetFolderPathW(0, u, 0, 0, Folder);
				if (SUCCEEDED(hr))
				{
					char str[1024];
					size_t i;
					wcstombs_s(&i, str, Folder, 1023);
					s = str;
					return true;
				}
				return false;
			}

			inline void clearPath(Safer::safe_string& s, const bool inverted = false) // true = '/'
			{
				Safer::safe_string str;

				s.lock();
				for (auto& i : s.get()) {
					if (!inverted && (i == '/')) str += '\\';
					else if (inverted && (i == '\\')) str += '/';
					else str += i;
				}
				s.unlock();

				s = str;
				str.clear();
			}

			inline void interpret_path(Safer::safe_string& s)
			{
				std::string local_t = s.gC();

				for (size_t p = local_t.find('%'); (p != std::string::npos); p = local_t.find('%'))
				{
					if (p == 0 || (local_t[p - 1] != '\\')) {
						std::string elsee = local_t.substr(p);
						Safer::safe_string path_to_add;
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
								change = local_t.substr(0, p-1);
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
						change += path_to_add.g() + local_t.substr(p + paths_known[found].length());
						local_t = change;
					}
				}

				s = local_t;

				clearPath(s);
			}
			inline void interpret_path(std::string& s)
			{
				Safer::safe_string meh = s.c_str();
				interpret_path(meh);
				s = meh.get();
			}
			inline void createPath(Safer::safe_string s)
			{
				interpret_path(s);
				//clearPath(s);

				Safer::safe_string str;
				std::stringstream ss(s.g());
				std::string token;
				Safer::safe_vector<Safer::safe_string> paths;

				while (std::getline(ss, token, '\\'))
				{
					str += token;
					paths.push(str);
					str += '\\';
				}

				Safer::safe_string u = paths.pop();
				if (u.g().rfind(".") == std::string::npos) paths.push(u);

				paths.lock();
				size_t siz = paths.getMax();

				for (size_t p = 0; p < siz; p++)
				{
					CreateDirectoryA(paths.work()[p].g().c_str(), NULL);
				}

				paths.unlock();
			}
			inline __int64 getFileSize(const Safer::safe_string& s)
			{
				HANDLE hFile = CreateFile(s.g().c_str(), GENERIC_READ,
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
			
			/*inline const bool saveFromCloud(const Safer::safe_string url, const Safer::safe_string wher)
			{
				downloader down;
				Safer::safe_string wher_actual = Defaults::default_data_path.g() + wher.g();
				interpret_path(wher_actual);
				createPath(wher_actual);
				FILE* fp = nullptr;
				if (fopen_s(&fp, wher_actual.g().c_str(), "wb") == 0)
				{
					std::string buf;
					if (down.downloadAsString(url.g().c_str(), buf))
					{
						fwrite(buf.c_str(), sizeof(char), buf.length(), fp);
						fclose(fp);
						return true;
					}
					fclose(fp);
				}
				return false;
			}*/
		}
		/*namespace Tools_experimental {

			const int  amount_of_hashtg = 4;

			struct extraction {
				std::string radd;
				unsigned long sizz = 0;
			};

			inline void create_directory(const std::string guy)
			{
				std::string folders;
				unsigned working = 2;
				unsigned poss = guy.rfind("/");
				if (poss == std::string::npos) return;

				folders = guy.substr(0, poss);

				while (working < folders.length())
				{
					std::string next_folder;
					poss = folders.find("/", working + 1);
					next_folder = folders.substr(0, poss);
					working = poss;

					//std::cout << next_folder.c_str() << std::endl;
					CreateDirectory(next_folder.c_str(), NULL);
				}
			}
			inline int extractEPIC(const char* opath, const long epicc)
			{
				FILE *filee = nullptr;
				FILE *outfp = nullptr;

				if (fopen_s(&filee, opath, "rb")) {
					return false;
				}

				char *ch = new char;
				char reading[amount_of_hashtg];
				char temporary[LIMIT_CHARL];

				unsigned amount_of_files,
					actual_file = 1;

				std::list<extraction> megan;
				extraction onee;

				onee.sizz = 0;
				onee.radd = "";

				for (int a = 0; a < amount_of_hashtg; a++)
					reading[a] = '-';
				reading[amount_of_hashtg - 1] = '\0';
				*ch = '-';

				fseek(filee, epicc + 1, SEEK_SET);

				while (actual_file == 1 && *ch != EOF)
				{
					actual_file = fread(ch, 1, 1, filee);

					if (*ch == '#') {
						fseek(filee, -1, SEEK_CUR);

						if (fread(reading, 1, (amount_of_hashtg - 1), filee) == amount_of_hashtg - 1)
						{
							if (strcmp(reading, "#TT") == 0) // total
							{
								temporary[0] = '\0';
								for (int b = 0; b < LIMIT_CHARL && *ch != '\n'; b++) {
									if (fread(ch, 1, 1, filee) == 1) {
										temporary[b] = *ch;
										temporary[b + 1] = '\0';
									}
									else {
										return false;
									}
								}
								amount_of_files = atoi(temporary);
							}
							if (strcmp(reading, "#RD") == 0) // road
							{
								for (int b = 0; b < LIMIT_CHARL && *ch != '\n'; b++) {
									if (fread(ch, 1, 1, filee) == 1) {
										temporary[b] = *ch;
										temporary[b + 1] = '\0';
									}
									else {
										return false;
									}
								}

								for (unsigned c = 0; c < std::string(temporary).length(); c++)
								{
									if (temporary[c] == '\\') temporary[c] = '/';
									if (temporary[c] == '\n') temporary[c] = '\0';
								}
								onee.radd = temporary;
							}
							if (strcmp(reading, "#SZ") == 0) // size
							{
								for (int b = 0; b < LIMIT_CHARL && *ch != '\n'; b++) {
									if (fread(ch, 1, 1, filee) == 1) {
										temporary[b] = *ch;
										temporary[b + 1] = '\0';
									}
									else {
										return false;
									}
								}
								onee.sizz = atoi(temporary);
							}
							if (strcmp(reading, "#ND") == 0) // send
							{
								fseek(filee, 1, SEEK_CUR);

								megan.push_back(onee);

								onee.sizz = 0;
								onee.radd = "";
							}
						}
						else {
							return false;
						}
					}
					else {
						if (megan.size() != amount_of_files)
						{
							return false;
						}
						actual_file = 0;
					}
				}

				POINT p;
				if (!GetCursorPos(&p))
				{
					p.x = 0;
					p.y = 0;
				}

				fseek(filee, -1, SEEK_CUR);

				for (auto var : megan)
				{
					actual_file++;

					create_directory(var.radd);

					while (fopen_s(&outfp, var.radd.c_str(), "wb+"));

					char *logoeu = new char[var.sizz];
					if (logoeu) {
						long aa = fread(logoeu, 1, var.sizz, filee);
						fwrite(logoeu, 1, aa, outfp);

						if ((unsigned long)aa != var.sizz) {
							return false;
						}
					}
					delete[] logoeu;

					fclose(outfp);
				}

				fclose(filee);

				return 0;
			}
			inline int writee(const std::string from_, const int forced, FILE* file_to)
			{
				setlocale(LC_ALL, "");

				FILE *file_from;
				std::string innn;
				char *work = new char[1000];
				int test = 1000;

				file_from = NULL;


				if (fopen_s(&file_from, from_.c_str(), "rb") != 0 || file_to == NULL)
				{
					return -1;
				}

				//test = fread(work,1,1000,file_from);
				char content[512];
				char *poinn;
				std::string tuhts = "";

				if (forced == 1)
				{
					snprintf(content, 512, "%s%s%s%s\n", "\n####", "####", "####", from_.c_str());
					tuhts = content;
					for (unsigned u = 0; u < tuhts.length(); u++)
					{
						poinn = &tuhts[u];
						fwrite(poinn, 1, 1, file_to);
					}
				}

				while (!feof(file_from) && test == 1000) {
					//std::cout << *work;
					test = fread(work, 1, 1000, file_from);
					fwrite(work, 1, test, file_to);
				}

				if (forced == 1)
				{
					snprintf(content, 512, "%s%s%s", "\n****", "****", "****\n");
					tuhts = content;
					for (unsigned u = 0; u < tuhts.length(); u++)
					{
						poinn = &tuhts[u];
						fwrite(poinn, 1, 1, file_to);
					}
				}
				if (forced == 0) {
					snprintf(content, 512, "%s%s%s", "\nYOUWILLFIND", "FILESHERE", "FORSURE!\n");
					tuhts = content;
					for (unsigned u = 0; u < tuhts.length(); u++)
					{
						poinn = &tuhts[u];
						fwrite(poinn, 1, 1, file_to);
					}
				}

				delete[] work;
				fclose(file_from);

				return 0;
			}
			inline int writee(const std::string what_to_write, FILE* file_to)
			{
				setlocale(LC_ALL, "");

				if (file_to == NULL)
				{
					return -1;
				}

				if (fwrite(what_to_write.c_str(), 1, what_to_write.length(), file_to) != what_to_write.length())
					return -1;

				return 0;
			}
		}*/
	}
}