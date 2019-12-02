#include "sound.h"

namespace LSW {
	namespace v4 {

		_all_track_voiceNmixer Track::vnm;


		Track::Track()
		{
			if (!vnm.voice) {
				vnm.voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
				if (!vnm.voice) {
					throw "at Track::load [#" + std::to_string((size_t)this) + ";ID=" + id + "]: Failed to create voice.";
				}

				vnm.mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
				if (!vnm.mixer) {
					throw "at Track::load [#" + std::to_string((size_t)this) + ";ID=" + id + "]: Failed to create mixer.";
				}

				if (!al_attach_mixer_to_voice(vnm.mixer, vnm.voice)) {
					throw "at Track::load [#" + std::to_string((size_t)this) + ";ID=" + id + "]: Failed to set mixer in voice.";
				}

				/*float temp_vol_get;
				Config::Config conf;
				conf.get(Config::LAST_VOLUME, temp_vol_get, Defaults::Sound::global_volume);
				set(Sound::GLOBALVOLUME, temp_vol_get);*/

				set(track_f::GLOBALVOLUME, 0.2);
			}

			/*if (mse) al_destroy_sample(mse);

			mse = al_load_sample(path.c_str());
			if (!mse) {
				throw "at Track::load [#" + std::to_string((size_t)this) + ";ID=" + id + "]: Failed to load sample.";
				return false;
			}*/

			if (instance) unload();

			instance = al_create_sample_instance(nullptr);
			if (!instance) {
				throw "at Track::load [#" + std::to_string((size_t)this) + ";ID=" + id + "]: Failed to create instance.";
			}

			al_attach_sample_instance_to_mixer(instance, vnm.mixer);
			al_set_sample_instance_playing(instance, false);
		}

		void Track::unload()
		{
			if (instance) al_destroy_sample_instance(instance);
			instance = nullptr;
		}

		void Track::set(const track_p e, const bool v)
		{
			switch (e)
			{
			case track_p::PLAYING:
				if (!instance) {
					throw "at Track::set [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No instance loaded. Invalid operation (PLAYING).";
					return;
				}
				al_set_sample_instance_playing(instance, v);
				break;
			}
		}
		void Track::set(const track_f e, const float v)
		{
			switch (e)
			{
			case track_f::VOLUME:
				if (!instance) {
					throw "at Track::set [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No instance loaded. Invalid operation (VOLUME).";
					return;
				}
				al_set_sample_instance_gain(instance, v);
				break;
			case track_f::GLOBALVOLUME:
				if (!vnm.mixer) {
					throw "at Track::set [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No mixer loaded. Invalid operation (GLOBALVOLUME).";
					return;
				}
				al_set_mixer_gain(vnm.mixer, v);
				break;
			case track_f::SPEED:
				if (!instance) {
					throw "at Track::set [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No instance loaded. Invalid operation (SPEED).";
					return;
				}
				al_set_sample_instance_speed(instance, v);
				break;
			}
		}
		void Track::set(const track_i e, const track_i_0 v)
		{
			switch (e)
			{
			case track_i::PLAYMODE:
				if (!instance) {
					throw "at Track::set [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No instance loaded. Invalid operation (PLAYMODE).";
					return;
				}
				al_set_sample_instance_playmode(instance, (ALLEGRO_PLAYMODE)v);
				break;
			}
		}
		void Track::set(const track_s e, const std::string v)
		{
			switch (e)
			{
			case track_s::ID:
				id = v;
				break;
			case track_s::LOADID:
				__template_static_vector<ALLEGRO_SAMPLE> tracks;
				tracks.get(v, mse);
				al_set_sample(instance, mse);
				break;
			}
		}

		void Track::get(const track_p e, bool& v)
		{
			switch (e)
			{
			case track_p::PLAYING:
				if (!instance) {
					throw "at Track::get [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No instance loaded. Invalid operation (PLAYING).";
					return;
				}
				v = al_get_sample_instance_playing(instance);
				break;
			}
		}
		void Track::get(const track_f e, float& v)
		{
			switch (e)
			{
			case track_f::VOLUME:
				if (!instance) {
					throw "at Track::get [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No instance loaded. Invalid operation (VOLUME).";
					return;
				}
				v = al_get_sample_instance_gain(instance);
				break;
			case track_f::GLOBALVOLUME:
				if (!vnm.mixer) {
					throw "at Track::get [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No mixer loaded. Invalid operation (GLOBALVOLUME).";
					return;
				}
				v = al_get_mixer_gain(vnm.mixer);
				break;
			case track_f::SPEED:
				if (!instance) {
					throw "at Track::get [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No instance loaded. Invalid operation (SPEED).";
					return;
				}
				v = al_get_sample_instance_speed(instance);
				break;
			}
		}
		void Track::get(const track_i e, int& v)
		{
			switch (e)
			{
			case track_i::PLAYMODE:
				if (!instance) {
					throw "at Track::get [#" + std::to_string((size_t)this) + ";ID=" + id + "]: No instance loaded. Invalid operation (PLAYMODE).";
					return;
				}
				v = al_get_sample_instance_playmode(instance);
				break;
			}
		}
		void Track::get(const track_s e, std::string& v)
		{
			switch (e)
			{
			case track_s::ID:
				v = id;
				break;
			}
		}

	}
}