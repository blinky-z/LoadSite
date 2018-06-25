#ifndef WEB_TCP_SERVER_H
#define WEB_TCP_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <thread>
#include <mutex>
#include <cerrno>
#include <stdexcept>
#include "client.h"
#include <queue>
using namespace std;

namespace webserver {
    class tcp_server {
    private:
        unsigned short int PORT;

        unsigned int allowed_connections_number;

        int listener_socket;

        volatile bool accept_connections;

        struct sockaddr_in server_address{};

        const function<bool(string)>& is_full_message;

        const function<string(string)>& convert_client_message;

        vector<client> clients;

        mutex mx;
    public:
        tcp_server(unsigned short int PORT, const function<bool(string)>& is_full_message,
                   const function<string(string)>& convert_client_message, unsigned int allowed_connections_number);

        void start();

        void stop();
    private:
        void connection_handler(const shared_ptr<client>& cl);

        void take_requests();

        int find_client_index(const shared_ptr<client>& cl);
    };
}

#endif //WEB_TCP_SERVER_H
