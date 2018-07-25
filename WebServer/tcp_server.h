#ifndef WEB_TCP_SERVER_H
#define WEB_TCP_SERVER_H

#include <iostream>
#include <cstring>
#include <unordered_set>
#include <unistd.h>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <cerrno>
#include <stdexcept>
#include <queue>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "client.h"

using namespace std;

namespace webserver {
    class tcp_server {
    private:
        const unsigned int ALLOWED_CONNS_NUM = 512;

        const unsigned int MAX_EVENTS = 1000;

        const unsigned int MAX_EPOLL_THREADS = 128;

        unsigned short int PORT;

        int listener_socket;

        volatile bool accept_allow;

        unsigned long uid = 0;

        unsigned int epoll_socket_num = 0;

        vector<int> epoll_socket_list;

        const function<bool(string)>& is_full_message;

        const function<string(string)>& convert_client_message;

    public:
        tcp_server(unsigned short int PORT, const function<bool(string)>& is_full_message,
                   const function<string(string)>& convert_client_message);

        void start();

        void stop();

    private:
        void set_client(int client_sock);

        void accept_connections();

        void listen_events(int EPoll);
    };
}

#endif //WEB_TCP_SERVER_V2_H
