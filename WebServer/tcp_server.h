#ifndef WEB_TCP_SERVER_H
#define WEB_TCP_SERVER_H

#include <iostream>
#include "client.h"
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

using namespace std;

namespace webserver {
    class tcp_server {
    private:
        const unsigned int ALLOWED_CONNS_NUM = 512;

        const unsigned int MAX_EVENTS = 128;

        const unsigned int MAX_EPOLL_THREADS = 4;

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

        void listen_events(int EPoll, int thread_num);
    };
}

#endif //WEB_TCP_SERVER_V2_H
