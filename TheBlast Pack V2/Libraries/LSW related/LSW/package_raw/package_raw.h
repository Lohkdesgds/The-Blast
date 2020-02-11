#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <functional>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

namespace LSW {
	namespace v4 {

		namespace Constants {

			// FOR THIS PROJECT
			enum class type_of_package { BLANK = 0, COMMAND, TEXT, BITMAP }; // BITMAP creates a PENDING_NAME named bitmap

			inline const int default_port = 42069;
			inline const size_t default_package_size = 128;
			inline const int connection_speed_delay = 1000 / 20;
			inline const int connection_timeout_speed = 10000; // 10 sec

			inline const std::string default_hello = "LSW_connection_verification";

			struct sockets_packages {
				char data[Constants::default_package_size];
				int data_len = 0;
				int data_type = 0;
				int combine_with_n_more = 0;
			};

			struct final_package {
				std::string variable_data;
				int data_type = 0;
			};

			const size_t max_buf = 20;
		}

		class connection_core {
			WSADATA wsaData = WSADATA();
			addrinfo* result = nullptr;
			bool failure = false, init = false;
		public:
			bool initialize(const char*, const int = Constants::default_port, const bool = false); // ip (can be null for server), port, ipv6?
			bool as_client(SOCKET&);
			bool as_host(SOCKET&);
		};

		class connection_each {
			connection_core core;

			SOCKET Connected = INVALID_SOCKET;

			std::thread* keep_alive_connection = nullptr;
			bool die = false;

			std::vector<Constants::sockets_packages> received, sending;
			std::mutex received_hold, sending_hold;

			std::function<void(const std::string)> prunt = std::function<void(const std::string)>();

			void __keep_connection_alive();
			// later: thread keeping alive, std::vector to buffer send/recv, bool to tell if still up

			bool _send_raw(void*, int);
			bool _recv_raw(void*, int);

			bool send_blank();

			bool send_package(Constants::sockets_packages&);
			bool recv_package(Constants::sockets_packages&);

			void start_internally_as_client();   // as client
			void start_internally_as_host(); // as host

		public:
			connection_each(SOCKET = INVALID_SOCKET); // custom non-verified call (probably from host)
			~connection_each();

			bool connect(const char* = "127.0.0.1", const int = Constants::default_port, const bool = false); // normally client call this one

			void kill_connection(); // only if you really want to die
			bool still_on();
			bool hasPackage();

			bool send_nolock(Constants::final_package);
			void send(Constants::final_package);
			bool recv(Constants::final_package&);

			void hookPrint(std::function<void(const std::string)>);
		};

		class connection_host {
			connection_core core;

			SOCKET Listening = INVALID_SOCKET;

			std::vector<connection_each*> connections;
			std::mutex connections_m;
			size_t max_connections_allowed = 1;

			std::thread* listener = nullptr;
			std::thread* autodisconnect = nullptr;
			bool still_running = false;

			std::function<void(const std::string)> prunt = std::function<void(const std::string)>();

			void auto_accept();
			void auto_cleanup();

			void _initialize(const char*, const int, const bool);
		public:
			connection_host(const int, const bool = false);
			connection_host(const bool = false);
			~connection_host();

			void lock();
			void unlock();
			std::vector<connection_each*>::iterator begin();
			std::vector<connection_each*>::iterator end();
			size_t size();

			void setMaxConnections(const size_t);

			void hookPrint(std::function<void(const std::string)>);
		};


		typedef connection_each con_client;
		typedef connection_host con_host;
	}
}