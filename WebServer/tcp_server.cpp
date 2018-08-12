#include "tcp_server.h"

namespace webserver {
    tcp_server::tcp_server(unsigned short int PORT, const function<bool(string)>& is_full_message,
                           const function<string(string)>& convert_client_message) :
            PORT(PORT), is_full_message(is_full_message), convert_client_message(convert_client_message) {
        accept_allow = true;
    }

    void tcp_server::start() {
        listener_socket = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in server_address{};
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);

        int reuse_port = 1;
        setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(reuse_port));

        if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
            throw runtime_error("Binding failed");
        }

        if (listen(listener_socket, ALLOWED_CONNS_NUM) == -1) {
            throw runtime_error("Listening failed");
        }

        for (unsigned int thread_num = 1; thread_num <= MAX_EPOLL_THREADS; thread_num++) {
            int EPoll = epoll_create1(0);
            epoll_socket_list.emplace_back(EPoll);
            thread listen_events_thread(&tcp_server::listen_events, this, EPoll, thread_num);
            listen_events_thread.detach();
        }

        cout << "Configuring was done" << endl;
        cout << "Server was enabled" << endl;
        cout << "Waiting for connections on port " << PORT << "..." << endl;

        accept_connections();
    }

    void tcp_server::set_client(int client_sock) {
        uid++;

        client* cl = new client();

        cl->sock = client_sock;
        cl->set_id(uid);

        epoll_event client_event{};
        client_event.data.ptr = cl;
        client_event.events = EPOLLIN | EPOLLET;

        int epoll_sock = epoll_socket_list[epoll_socket_num];
        if (epoll_ctl(epoll_sock, EPOLL_CTL_ADD, cl->sock, &client_event) != -1) {
            cout << "New client [ID " << cl->get_id() << "] has been added to the Epoll instance" << endl;
        } else {
            cerr << "Error while adding new client [ID " << cl->get_id() << "] to the Epoll instance" << endl;
        }
        epoll_socket_num++;
        epoll_socket_num = epoll_socket_num % MAX_EPOLL_THREADS;
    }

    void tcp_server::accept_connections() {
        while (accept_allow) {
            int client_sock = accept4(listener_socket, nullptr, nullptr, SOCK_NONBLOCK);

            if (client_sock != -1 && accept_allow) {
                cout << "New connection has been accepted" << endl;
                set_client(client_sock);
            } else {
                if (!accept_allow && strcmp(strerror(errno), "SUCCESS") == 0) {
                    cerr << "Discard connection: Server is terminating" << endl;
                } else {
                    cerr << "Socket accept failure" << endl << strerror(errno) << endl;
                }
            }
        }
    }

    void tcp_server::listen_events(int EPoll, int thread_num) {
        struct epoll_event events[MAX_EVENTS];
        while (accept_allow) {
            int active_events_num = epoll_wait(EPoll, events, MAX_EVENTS, -1);
            if (active_events_num == -1) {
                cerr << "[Thread " << thread_num << "] " << "Error occurred while checking for an I/O events. "
                                                            "Error message: " << strerror(errno) << endl;
            }

            for (int current_event = 0; current_event < active_events_num; current_event++) {
                char read_buffer[1024];

                client* client_data = static_cast<client*>(events[current_event].data.ptr);

                int current_socket = client_data->sock;
                unsigned long current_socket_id = client_data->get_id();

                ssize_t recv_size = recv(current_socket, &read_buffer, sizeof(read_buffer) - 1, MSG_NOSIGNAL);

                if (recv_size == 0) {
                    cout << "Client [ID " << current_socket_id << "] has disconnected" << endl;

                    if (close(current_socket) != -1) {
                        cout << "Server side socket related to the client [ID " << current_socket_id
                             << "] has been closed" << endl;
                    } else {
                        cerr << "Unable to close server side socket related to the client [ID " << current_socket_id <<
                             "]. Error message: " << strerror(errno) << endl;
                    }

                    epoll_ctl(EPoll, EPOLL_CTL_DEL, current_socket, nullptr);
                    delete client_data;
                } else if (recv_size > 0) {
                    string received_message;
                    received_message.append(read_buffer, static_cast<unsigned long>(recv_size));

                    cout << "Client's message has been received:" << endl << read_buffer << endl;

                    while ((recv_size = recv(current_socket, &read_buffer, sizeof(read_buffer), MSG_NOSIGNAL)) >
                           0 && errno != EWOULDBLOCK) {
                        received_message.append(read_buffer, static_cast<unsigned long>(recv_size));

                        cout << "[Read Cycle Server] Client's message has been received:" << endl << read_buffer
                             << endl;
                    }

                    string response = convert_client_message(received_message);

                    cout << "[Thread " << thread_num << "]" << "Server's response:" << endl << response << endl;

                    if (send(current_socket, response.c_str(), response.size(), MSG_NOSIGNAL) != -1) {
                        cout << "Response has been sent to client [ID " << current_socket_id << "]" << endl;
                    } else {
                        cerr << "Response sending to client [ID " << current_socket_id << "] failed" << endl;
                    }
                } else {
                    cerr << "Error while receiving message from client [ID " << current_socket_id << "] " <<
                         "Error message: " << strerror(errno) << endl;
                }
            }
        }
    }

    void tcp_server::stop() {
        accept_allow = false;

        cout << "[Server Stop] Server terminating has started" << endl;

        if (close(listener_socket) != -1) {
            cout << "[Server Stop] Listener socket has been closed" << endl;
        } else {
            cerr << "[Server Stop] Unable to close listener socket"
                 << ". Error message: " << strerror(errno) << endl;
        }

        cout << "[Server Stop] Server has been terminated" << endl;
    }
}