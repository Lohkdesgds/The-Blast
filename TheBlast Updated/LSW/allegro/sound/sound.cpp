#include "..\..\all\enable_all.h"

namespace LSW {
	namespace v2 {
		namespace Sound {

			const bool track::load(const Safer::safe_string e_o)
			{
				if (mse) al_destroy_sample(mse);

				Safer::safe_string e = Defaults::default_data_path.g() + e_o.g();
				Tools::interpret_path(e);

				mse = al_load_sample(e.g().c_str());
				if (!mse) return false;

				instance = al_create_sample_instance(mse);
				if (!instance) return false;

				al_attach_sample_instance_to_mixer(instance, al_get_default_mixer());
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
				if (!instance) return;

				switch (e)
				{
				case PLAYING:
					al_set_sample_instance_playing(instance, v);
					break;
				}
			}
			void track::set(const track_f e, const float v)
			{
				if (!instance) return;

				switch (e)
				{
				case VOLUME:
					al_set_sample_instance_gain(instance, v);
					break;
				case SPEED:
					al_set_sample_instance_speed(instance, v);
					break;
				}
			}
			void track::set(const track_i e, const track_i_0 v)
			{
				if (!instance) return;

				switch (e)
				{
				case PLAYMODE:
					al_set_sample_instance_playmode(instance, (ALLEGRO_PLAYMODE)v);
					break;
				}
			}
			void track::set(const track_s e, const Safer::safe_string v)
			{
				//if (!instance) return;

				switch (e)
				{
				case ID:
					my_nick = v;
					break;
				}
			}

			void track::get(const track_p e, bool& v)
			{
				if (!instance) return;

				switch (e)
				{
				case PLAYING:
					v = al_get_sample_instance_playing(instance);
					break;
				}
			}
			void track::get(const track_f e, float& v)
			{
				if (!instance) return;

				switch (e)
				{
				case VOLUME:
					v = al_get_sample_instance_gain(instance);
					break;
				case SPEED:
					v = al_get_sample_instance_speed(instance);
					break;
				}
			}
			void track::get(const track_i e, int& v)
			{
				if (!instance) return;

				switch (e)
				{
				case PLAYMODE:
					v = al_get_sample_instance_playmode(instance);
					break;
				}
			}
			void track::get(const track_s e, Safer::safe_string& v)
			{
				//if (!instance) return;

				switch (e)
				{
				case ID:
					v = my_nick;
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
					throw "EXCEPTION AT TRACK GETORCREATE: NOT FOUND AND NOT SUPPOSED TO CREATE!";
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