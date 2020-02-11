#include "sound.h"

namespace LSW {
	namespace v4 {

		Mixer::controller Mixer::control;


		Mixer::Mixer()
		{
			if (!control.device) {
				__systematic sys;
				sys.initAllegro();

				control.device = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
				if (!control.device) {
					throw Abort::abort(__FUNCSIG__, "Failed to create voice.");
				}

				control.mixing = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
				if (!control.mixing) {
					throw Abort::abort(__FUNCSIG__, "Failed to create mixer.");
				}

				if (!al_attach_mixer_to_voice(control.mixing, control.device)) {
					throw Abort::abort(__FUNCSIG__, "Failed to set mixer in voice.");
				}

				double temp_vol_get = Constants::start_default_global_volume;
				Database conf;
				conf.get(Constants::io__conf_double::LAST_VOLUME, temp_vol_get);
				conf.set(Constants::io__conf_double::LAST_VOLUME, temp_vol_get);

				volume(temp_vol_get);
			}

		}

		void Mixer::volume(const double v)
		{
			Database conf;
			al_set_mixer_gain(control.mixing, v);
			conf.set(Constants::io__conf_double::LAST_VOLUME, v);
		}

		double Mixer::getVolume()
		{
			return al_get_mixer_gain(control.mixing);
		}

		void Mixer::attachInstance(ALLEGRO_SAMPLE_INSTANCE* si)
		{
			al_attach_sample_instance_to_mixer(si, control.mixing);
		}

		Track::Track()
		{
			Mixer mxr;

			if (instance) unload();

			instance = al_create_sample_instance(nullptr);
			if (!instance) {
				throw Abort::abort(__FUNCSIG__, "Failed to create instance.");
			}

			mxr.attachInstance(instance);
			al_set_sample_instance_playing(instance, false);
		}

		void Track::unload()
		{
			if (instance) {
				al_detach_sample_instance(instance);
				al_destroy_sample_instance(instance);
			}
			instance = nullptr;
		}

		void Track::set(const Constants::io__track_boolean e, const bool v)
		{
			switch (e)
			{
			case Constants::io__track_boolean::PLAYING:
				if (!instance) {
					throw Abort::abort(__FUNCSIG__, "Failed to set PLAYING because there was no instance loaded.", 1);
					return;
				}
				al_set_sample_instance_playing(instance, v);
				break;
			}
		}
		void Track::set(const Constants::io__track_double e, const double v)
		{
			switch (e)
			{
			case Constants::io__track_double::VOLUME:
				if (!instance) {
					throw Abort::abort(__FUNCSIG__, "Failed to set VOLUME because there was no instance loaded.", 1);
					return;
				}
				al_set_sample_instance_gain(instance, +v);
				break;
			case Constants::io__track_double::SPEED:
				if (!instance) {
					throw Abort::abort(__FUNCSIG__, "Failed to set SPEED because there was no instance loaded.", 1);
					return;
				}
				al_set_sample_instance_speed(instance, +v);
				break;
			}
		}
		void Track::set(const Constants::io__track_integer e, const Constants::io__track_integer_modes v)
		{
			switch (e)
			{
			case Constants::io__track_integer::PLAYMODE:
				if (!instance) {
					throw Abort::abort(__FUNCSIG__, "Failed to set PLAYMODE because there was no instance loaded.", 1);
					return;
				}
				al_set_sample_instance_playmode(instance, (ALLEGRO_PLAYMODE)v);
				break;
			}
		}
		void Track::set(const Constants::io__track_string e, const std::string v)
		{
			switch (e)
			{
			case Constants::io__track_string::ID:
				id = v;
				break;
			case Constants::io__track_string::LOADID:
				__template_static_vector<ALLEGRO_SAMPLE> tracks;
				tracks.get(v, mse);
				al_set_sample(instance, mse);
				break;
			}
		}

		void Track::get(const Constants::io__track_boolean e, bool& v)
		{
			switch (e)
			{
			case Constants::io__track_boolean::PLAYING:
				if (!instance) {
					throw Abort::abort(__FUNCSIG__, "Failed to get PLAYING because there was no instance loaded.", 1);
					return;
				}
				v = al_get_sample_instance_playing(instance);
				break;
			}
		}
		void Track::get(const Constants::io__track_double e, double& v)
		{
			switch (e)
			{
			case Constants::io__track_double::VOLUME:
				if (!instance) {
					throw Abort::abort(__FUNCSIG__, "Failed to get VOLUME because there was no instance loaded.", 1);
					return;
				}
				v = +al_get_sample_instance_gain(instance);
				break;
			case Constants::io__track_double::SPEED:
				if (!instance) {
					throw Abort::abort(__FUNCSIG__, "Failed to get SPEED because there was no instance loaded.", 1);
					return;
				}
				v = +al_get_sample_instance_speed(instance);
				break;
			}
		}
		void Track::get(const Constants::io__track_integer e, int& v)
		{
			switch (e)
			{
			case Constants::io__track_integer::PLAYMODE:
				if (!instance) {
					throw Abort::abort(__FUNCSIG__, "Failed to get PLAYMODE because there was no instance loaded.", 1);
					return;
				}
				v = (int)al_get_sample_instance_playmode(instance);
				break;
			}
		}
		void Track::get(const Constants::io__track_string e, std::string& v)
		{
			switch (e)
			{
			case Constants::io__track_string::ID:
				v = id;
				break;
			}
		}

}
}