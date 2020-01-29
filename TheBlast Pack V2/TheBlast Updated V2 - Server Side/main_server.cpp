#include "..\LSW\package_raw\package_raw.h"

using namespace LSW::v4;

int main(int argc, char* argv[]) {

    int customport = Constants::default_port;
    bool isipv6 = false;

    {
        for (int u = 1; u < argc; u++) {
            if (argv[u][0] == '-') {
                std::string cmd = (argv[u] + 1);

                if (cmd == "port" && (u + 1) < argc) {
                    customport = atoi(argv[u + 1]);
                    if (customport <= 0 || customport >= (2 << 15)) customport = -1;
                    else {
                        printf_s("[LAUNCH] Defined port %d\n", customport);
                    }
                }
                if (cmd == "ipv6") {
                    isipv6 = true;
                    printf_s("[LAUNCH] Defined IPV6\n");
                }
            }
        }
    }

    con_host host(customport, isipv6);

    while (1) {

        host.lock();
        std::vector<Constants::final_package> copies;
        for (auto& i : host) {
            if (i->hasPackage()) {
                Constants::final_package pkg;
                i->recv(pkg);
                copies.push_back(pkg);

                printf_s("Got package id #%d with content: %s\n", pkg.data_type, pkg.variable_data.c_str());
            }
        }
        for (auto& i : host) for (auto& j : copies) i->send(j);
        host.unlock();

        Sleep(1000);
    }
}