#include "discord_thread.h"

namespace LSW {
	namespace v4 {
		void DiscordConnection::Update()
		{
			has_discord_disconnected = false;

			while (keep_runnin) {
				Sleep(16);

				auto result = core->RunCallbacks();

				if (result == discord::Result::NotRunning) {
					keep_runnin = false;
					has_discord_disconnected = true;
					continue;
				}

				if (last_activity_change == 0 || GetTickCount64() - last_activity_change > 5000) { // discord api only allow 5 updates per 20 sec, so let's do 4 / 20
					last_activity_change = GetTickCount64();
					acticity_m.lock();
					state.core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
						if (result != discord::Result::Ok) {
							gfile logg;
							logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Discord UpdateActivity task had failure with result: " << +result << L::ELF;
						}
						});
					acticity_m.unlock();
				}
			}
		}

		DiscordConnection::~DiscordConnection()
		{
			if (runnin) {
				keep_runnin = false;
				runnin->join();
				delete runnin;
				runnin = nullptr;
			}
		}

		DiscordConnection::DiscordConnection()
		{
			if (!keep_runnin) {
				auto result = discord::Core::Create(Hidden::clientMe, DiscordCreateFlags_Default, &core);
				if (result != discord::Result::Ok) {
					throw Abort::warn(__FUNCSIG__, "Discord wasn't found or something went wrong!", +result);
				}
				state.core.reset(core);
				if (!state.core) throw Abort::abort(__FUNCSIG__, std::string("Failed to instantiate discord core! (err ") + std::to_string(static_cast<int>(result)) + ")");

				core->SetLogHook(discord::LogLevel::Debug, [](discord::LogLevel level, const char* message) {
					gfile logg;
					logg << L::SLF << fsr(__FUNCSIG__, E::ERRR) << "Discord had an error level " << +level << ": " << message << L::ELF;
					});

				getActivity().SetName("The Blast V2");
				getActivity().SetState("");
				getActivity().SetType(discord::ActivityType::Playing);
				getActivity().SetDetails("");

				keep_runnin = true;
				has_discord_disconnected = false;
				runnin = new std::thread([&]() {Update();});

				while (!isConnected()) Sleep(20);
			}
		}
		bool DiscordConnection::isConnected()
		{
			return !has_discord_disconnected;
		}
		void DiscordConnection::lock()
		{
			acticity_m.lock();
		}
		discord::Activity& DiscordConnection::getActivity()
        {
            return this->activity;
        }
		void DiscordConnection::unlock()
		{
			acticity_m.unlock();
		}
	}
}