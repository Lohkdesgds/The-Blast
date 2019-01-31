#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Sound {

			_all_track_voiceNmixer track::vnm;


			track::track()
			{
				logg << Log::START << "[IMGL:CONST][INFO] Registered spawn of image_low ID#" + std::to_string((size_t)this) << Log::ENDL;
			}
			track::~track()
			{
				unload();
				logg << Log::START << "[IMGL:CONST][INFO] Registered despawn of image_low ID#" + std::to_string((size_t)this) << ";ID=" << id << Log::ENDL;
			}

			const bool track::load()
			{
				if (!vnm.voice) {
					vnm.voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
					if (!vnm.voice) {
						throw "TRACK::LOAD - FAILED TO CREATE VOICE FOR TRACKS";
						return false;
					}
					
					vnm.mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
					if (!vnm.mixer) {
						throw "TRACK::LOAD - FAILED TO CREATE MIXER FOR TRACKS";
						return false;
					}

					if (!al_attach_mixer_to_voice(vnm.mixer, vnm.voice)) {
						throw "TRACK::LOAD - FAILED TO SET MIXER INTO VOICE";
						return false;
					}

					set(Sound::GLOBALVOLUME, Defaults::Sound::global_volume);
				}

				if (mse) al_destroy_sample(mse);

				mse = al_load_sample(path.g().c_str());
				if (!mse) return false;

				instance = al_create_sample_instance(nullptr);
				if (!instance) return false;

				al_set_sample(instance, mse);

				al_attach_sample_instance_to_mixer(instance, vnm.mixer);
				al_set_sample_instance_playing(instance, false);

				return true;
			}

			void track::unload()
			{
				if (instance) al_destroy_sample_instance(instance);
				if (mse) al_destroy_sample(mse);
				instance = nullptr;
				mse = nullptr;
			}
			
			void track::set(const track_p e, const bool v)
			{
				switch (e)
				{
				case PLAYING:
					if (!instance) {
						throw "at track::set [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No instance loaded. Invalid operation (PLAYING).";
						return;
					}
					al_set_sample_instance_playing(instance, v);
					break;
				}
			}
			void track::set(const track_f e, const float v)
			{
				switch (e)
				{
				case VOLUME:
					if (!instance) {
						throw "at track::set [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No instance loaded. Invalid operation (VOLUME).";
						return;
					}
					al_set_sample_instance_gain(instance, v);
					break;
				case GLOBALVOLUME:
					if (!vnm.mixer) {
						throw "at track::set [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No mixer loaded. Invalid operation (GLOBALVOLUME).";
						return;
					}
					al_set_mixer_gain(vnm.mixer, v);
					break;
				case SPEED:
					if (!instance) {
						throw "at track::set [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No instance loaded. Invalid operation (SPEED).";
						return;
					}
					al_set_sample_instance_speed(instance, v);
					break;
				}
			}
			void track::set(const track_i e, const track_i_0 v)
			{
				switch (e)
				{
				case PLAYMODE:
					if (!instance) {
						throw "at track::set [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No instance loaded. Invalid operation (PLAYMODE).";
						return;
					}
					al_set_sample_instance_playmode(instance, (ALLEGRO_PLAYMODE)v);
					break;
				}
			}
			void track::set(const track_s e, const Safer::safe_string v)
			{
				switch (e)
				{
				case ID:
					id = v;
					break;
				case RAW_PATH:
					path_raw = v;
					path = path_raw;
					break;
				case PATH:
					path = Defaults::default_data_path.g() + v.g();
					Tools::interpret_path(path);
					break;
				}
			}

			void track::get(const track_p e, bool& v)
			{
				switch (e)
				{
				case PLAYING:
					if (!instance) {
						throw "at track::get [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No instance loaded. Invalid operation (PLAYING).";
						return;
					}
					v = al_get_sample_instance_playing(instance);
					break;
				}
			}
			void track::get(const track_f e, float& v)
			{
				switch (e)
				{
				case VOLUME:
					if (!instance) {
						throw "at track::get [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No instance loaded. Invalid operation (VOLUME).";
						return;
					}
					v = al_get_sample_instance_gain(instance);
					break;
				case GLOBALVOLUME:
					if (!vnm.mixer) {
						throw "at track::get [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No mixer loaded. Invalid operation (GLOBALVOLUME).";
						return;
					}
					v = al_get_mixer_gain(vnm.mixer);
					break;
				case SPEED:
					if (!instance) {
						throw "at track::get [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No instance loaded. Invalid operation (SPEED).";
						return;
					}
					v = al_get_sample_instance_speed(instance);
					break;
				}
			}
			void track::get(const track_i e, int& v)
			{
				switch (e)
				{
				case PLAYMODE:
					if (!instance) {
						throw "at track::get [#" + std::to_string((size_t)this) + ";ID=" + id.g() + "]: No instance loaded. Invalid operation (PLAYMODE).";
						return;
					}
					v = al_get_sample_instance_playmode(instance);
					break;
				}
			}
			void track::get(const track_s e, Safer::safe_string& v)
			{
				switch (e)
				{
				case ID:
					v = id;
					break;
				case RAW_PATH:
					v = path_raw;
					break;
				case PATH:
					v = path;
					break;
				}
			}




			size_t _find(const Safer::safe_string s, Safer::safe_vector<track*>& v, bool& u)
			{
				u = false;
				for (size_t p = 0; p < v.getMax(); p++)
				{
					Safer::safe_string str;
					v[p]->get(ID, str);
					if (str == s) {
						u = true;
						return p;
					}
				}
				return 0;
			}


			track* getOrCreate(const Safer::safe_string s, const bool create)
			{
				d_musics_database msk_data;
				track* ref = nullptr;
				if (msk_data.get(ref, s)) return ref;
				if (create) {
					msk_data.create(ref);
					ref->set(Sound::ID, s);
					return ref;
				}
				else {
					throw "at Sound::getOrCreate(): Could not find \"" + s.g() + "\".";
					return nullptr;
				}
			}
			void easyRemove(const Safer::safe_string s)
			{
				d_musics_database msk_data;
				track* ref = nullptr;
				if (msk_data.get(ref, s)) {
					msk_data.remove(s);
					ref->unload();
					delete ref;
				}
			}
		}
	}
}