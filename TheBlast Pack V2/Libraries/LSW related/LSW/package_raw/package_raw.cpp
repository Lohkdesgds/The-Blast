#include "package_raw.h"


namespace LSW {
    namespace v4 {

        bool connection_core::initialize(const char* ip_str, const int port, const bool isthis_ipv6)
        {
            if (init) return true;

            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return !(failure = true);

            struct addrinfo hints;

            char port_str[16];
            sprintf_s(port_str, "%d", port);

            SecureZeroMemory(&hints, sizeof(hints));
            hints.ai_family = isthis_ipv6 ? AF_INET6 : AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_flags = AI_PASSIVE;

            // Resolve the server address and port
            if (getaddrinfo(ip_str, port_str, &hints, &result) != 0) return !(failure = true);

            init = true;
            return true;
        }

        bool connection_core::as_client(SOCKET& ConnectSocket)
        {
            if (failure || !init) return false;

            struct addrinfo* ptr = NULL;

            // Attempt to connect to an address until one succeeds
            for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

                // Create a SOCKET for connecting to server
                ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                    ptr->ai_protocol);
                if (ConnectSocket == INVALID_SOCKET) return false;

                // Connect to server.
                if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
                    closesocket(ConnectSocket);
                    ConnectSocket = INVALID_SOCKET;
                    continue;
                }
                break;
            }

            freeaddrinfo(result);

            if (ConnectSocket == INVALID_SOCKET) return false;
            // connected
            return true;
        }

        bool connection_core::as_host(SOCKET& ListenSocket)
        {
            if (failure || !init) return false;

            ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (ListenSocket == INVALID_SOCKET) return false;

            // Setup the TCP listening socket
            if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
                freeaddrinfo(result);
                closesocket(ListenSocket);
                return false;
            }

            freeaddrinfo(result);
            // ready to listen
            return true;
        }

        void connection_each::__keep_connection_alive() // starts receiving
        {
            bool locked_recv = false, locked_send = false;
            bool safedie = false;
            while (!die && !safedie) {
                // -------- RECV -------- //
                Constants::sockets_packages pkg;
                if (!recv_package(pkg)) {
                    die = true;
                    continue;
                }
                if (pkg.data_type != 0) {
                    if (!locked_recv) received_hold.lock();
                    locked_recv = true;

                    received.push_back(pkg);
                }
                else {
                    if (locked_recv) received_hold.unlock();
                    locked_recv = false;
                }

                // -------- SEND -------- //
                Constants::sockets_packages empty;

                if (sending.size() > 0) {
                    if (!locked_send) sending_hold.lock();
                    locked_send = true;

                    empty = sending[0];
                    sending.erase(sending.begin());
                }
                else {
                    if (locked_send) sending_hold.unlock();
                    locked_send = false;
                }
                if (!send_package(empty)) {
                    safedie = true;
                    continue;
                }

                //printf_s("\n- updated -");
                Sleep(Constants::connection_speed_delay);
            }
            if (safedie || die) {
                closesocket(this->Connected);
                die = true;
                Connected = INVALID_SOCKET;
            }

            if (locked_recv) received_hold.unlock();
            if (locked_send) sending_hold.unlock();
        }

        bool connection_each::_send_raw(void* v, int s)
        {
            return ::send(Connected, (char*)v, s, 0) > 0;
        }

        bool connection_each::_recv_raw(void* v, int s)
        {
            return ::recv(Connected, (char*)v, s, 0) > 0;
        }

        bool connection_each::send_blank()
        {
            Constants::sockets_packages blunk;
            return send_package(blunk);
        }

        bool connection_each::send_package(Constants::sockets_packages& pack)
        {
            if (pack.data_len) {
                //if (prunt) prunt("Sending package of size " + std::to_string(pack.data_len));
                //printf_s("Sending package of size %d.\n", pack.data_len);
            }

            void* end = &pack;
            int siz = sizeof(Constants::sockets_packages);

            return _send_raw(end, siz);
        }

        bool connection_each::recv_package(Constants::sockets_packages& pack)
        {
            void* end = &pack;
            int siz = sizeof(Constants::sockets_packages);

            bool a = _recv_raw(end, siz);

            if (pack.data_len) {
                //if (prunt) prunt("Received package of size " + std::to_string(pack.data_len));
                //printf_s("Received package of size %d.\n", pack.data_len);
            }

            return a;
        }

        void connection_each::start_internally_as_client()
        {
            if (!keep_alive_connection) keep_alive_connection = new std::thread([&]() {this->send_blank();  __keep_connection_alive(); });
        }

        void connection_each::start_internally_as_host()
        {
            if (!keep_alive_connection) keep_alive_connection = new std::thread([&]() { __keep_connection_alive(); });
        }

        connection_each::connection_each(SOCKET already_connected) : core()
        {
            if (already_connected != INVALID_SOCKET) {
                Connected = already_connected;
                start_internally_as_host();
            }
        }

        connection_each::~connection_each()
        {
            kill_connection();
            received.clear();
            sending.clear();
        }

        bool connection_each::connect(const char* a, const int b, const bool c)
        {
            if (!core.initialize(a, b, c)) return false;
            if (!core.as_client(Connected)) return false;
            start_internally_as_client();
            return true;
        }

        void connection_each::kill_connection()
        {
            die = true;
            if (keep_alive_connection) {
                keep_alive_connection->join();
                delete keep_alive_connection;
                keep_alive_connection = nullptr;
            }
        }

        bool connection_each::still_on()
        {
            return !die;
        }

        bool connection_each::hasPackage()
        {
            return received.size() > 0;
        }

        bool connection_each::send_nolock(Constants::final_package pack)
        {
            if (sending.size() >= Constants::max_buf) return false;

            if (sending_hold.try_lock()) {
                while (pack.variable_data.length() > 0)
                {
                    Constants::sockets_packages small_p;
                    size_t max_siz = pack.variable_data.length();
                    for (small_p.data_len = 0; small_p.data_len < max_siz && small_p.data_len < Constants::default_package_size; small_p.data_len++)
                    {
                        small_p.data[small_p.data_len] = pack.variable_data[0];
                        pack.variable_data.erase(0, 1);
                    }
                    small_p.combine_with_n_more = (static_cast<int>(max_siz) - 1) / Constants::default_package_size; // if eq, 0
                    small_p.data_type = pack.data_type;

                    sending.push_back(small_p);
                }
                sending_hold.unlock();
                return true;
            }
            return false;
        }

        void connection_each::send(Constants::final_package pack)
        {
            sending_hold.lock();
            while (pack.variable_data.length() > 0)
            {
                Constants::sockets_packages small_p;
                size_t max_siz = pack.variable_data.length();
                for (small_p.data_len = 0; small_p.data_len < max_siz && small_p.data_len < Constants::default_package_size; small_p.data_len++)
                {
                    small_p.data[small_p.data_len] = pack.variable_data[0];
                    pack.variable_data.erase(0, 1);
                }
                small_p.combine_with_n_more = (static_cast<int>(max_siz) - 1) / Constants::default_package_size; // if eq, 0
                small_p.data_type = pack.data_type;

                sending.push_back(small_p);
            }
            sending_hold.unlock();
        }

        bool connection_each::recv(Constants::final_package& pack)
        {
            pack.variable_data.clear();
            received_hold.lock();

            for (bool still_has = true; still_has && received.size() > 0;)
            {
                auto& i = received[0];
                still_has = i.combine_with_n_more != 0;
                for (int u = 0; u < i.data_len; u++) pack.variable_data += i.data[u];
                pack.data_type = i.data_type;
                received.erase(received.begin());
            }
            received_hold.unlock();
            return pack.variable_data.size() > 0;
        }

        void connection_each::hookPrint(std::function<void(const std::string)> f)
        {
            prunt = f;
        }

        void connection_host::auto_accept()
        {
            while (still_running) {
                if (listen(Listening, SOMAXCONN) == SOCKET_ERROR) continue;

                // Accept a client socket
                SOCKET ClientSocket = accept(Listening, NULL, NULL);
                if (ClientSocket == INVALID_SOCKET) continue;

                connection_each* dis = new connection_each(ClientSocket);
                if (connections.size() >= max_connections_allowed) {
                    dis->kill_connection();
                    delete dis;
                    continue;
                }

                //dis->start_internally_as_host(); // cause sended last time, so should receive so there's no error

                connections_m.lock();
                connections.push_back(dis);
                connections_m.unlock();
                if (prunt) prunt("Someone has connected!");
                //printf_s("\nSomeone has connected!");
            }
        }

        void connection_host::auto_cleanup()
        {
            while (still_running) {
                Sleep(Constants::connection_timeout_speed);
                connections_m.lock();
                for (size_t p = 0; p < connections.size(); p++)
                {
                    auto& i = connections[p];
                    if (!i->still_on()) {
                        delete i;
                        connections.erase(connections.begin() + p--);
                        if (prunt) prunt("Someone has disconnected.");
                        //printf_s("\nSomeone has disconnected!");
                    }
                }
                connections_m.unlock();
            }
        }

        void connection_host::_initialize(const char* a, const int b, const bool c)
        {
            core.initialize(a, b, c);
            core.as_host(Listening);
            still_running = true;
            listener = new std::thread([&]() {auto_accept(); });
            autodisconnect = new std::thread([&]() {auto_cleanup(); });
        }

        connection_host::connection_host(const int port, const bool ipv6)
        {
            _initialize(nullptr, port, ipv6);
        }

        connection_host::connection_host(const bool ipv6)
        {
            _initialize(nullptr, Constants::default_port, ipv6);
        }

        connection_host::~connection_host()
        {
            still_running = false;
            connection_each oopsie; // unlock auto_accept (trigger a loop)
            oopsie.connect();
            listener->join();
            autodisconnect->join();
            delete listener;
            delete autodisconnect;
            listener = nullptr;
            autodisconnect = nullptr;
            connections.clear();
        }

        void connection_host::lock()
        {
            connections_m.lock();
        }

        void connection_host::unlock()
        {
            connections_m.unlock();
        }

        std::vector<connection_each*>::iterator connection_host::begin()
        {
            return connections.begin();
        }

        std::vector<connection_each*>::iterator connection_host::end()
        {
            return connections.end();
        }

        size_t connection_host::size()
        {
            return connections.size();
        }
        void connection_host::setMaxConnections(const size_t v)
        {
            max_connections_allowed = v;
        }
        void connection_host::hookPrint(std::function<void(const std::string)> f)
        {
            prunt = f;
        }
    }
}