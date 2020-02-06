#pragma once

#include "..\custom_abort\abort.h"
#include "..\big_templates\small_templates.h"
#include "..\logger\logger.h"

#include <Windows.h>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

#include "..\..\imported\source\discord.h"
#include "..\hidden\hidden.h"

namespace LSW {
	namespace v4 {

		class DiscordConnection {
			struct DiscordState {
				discord::User currentUser;
				std::unique_ptr<discord::Core> core;
			};

			DiscordState state{};
			discord::Core* core{};

			std::thread* runnin = nullptr;
			bool keep_runnin = false;
			bool has_discord_disconnected = true;

			std::mutex acticity_m;
			ULONGLONG last_activity_change = 0;
			discord::Activity activity = discord::Activity();
			ULONGLONG lasttime = GetTickCount64();

			void Update();
		public:
			~DiscordConnection();
			DiscordConnection();

			bool isConnected();

			void lock();
			discord::Activity& getActivity();
			void unlock();
		};

	}
}