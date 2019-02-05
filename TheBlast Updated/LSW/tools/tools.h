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


				template<class T, typename U> Safer::safe_vector<T*> manager<T, U>::data;
				template<class T, typename U> size_t(*manager<T, U>::f) (const Safer::safe_string, Safer::safe_vector<T*> &, bool &);
				template<class T, typename U> void(*manager<T, U>::drw) ();
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

		namespace Tools {

			const std::string paths_known[] = { "%appdata%", "%win_fonts%", "%win_photos_path%" }; //CSIDL_APPDATA, CSIDL_FONTS, CSIDL_MYPICTURES

			void throw_at_screen(const Safer::safe_string, const bool = true); // on tools.cpp

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

			template<typename Func, typename... Args>
			inline bool try_call(Func func, Args&&... args)
			{
				return try_call(nullptr, func, args);
			}

			template<typename Func, typename... Args>
			inline bool try_call(Safer::safe_string* err, Func func, Args&&... args)//	-> typename std::result_of<Func(Args...)>::type
			{
				try {
					func(std::forward<Args>(args)...);
					return true;
				}
				catch (const Safer::safe_string& s)
				{
					if (err) *err = s;
				}
				catch (const std::string& s)
				{
					if (err) *err = s;
				}
				catch (const char* s)
				{
					if (err) *err = s;
				}
				catch (const int i)
				{
					if (err) *err = std::to_string(i);
				}
				catch (...)
				{
					if (err) *err = "unknown";
				}
				return false;
			}
			
		}
	}
}