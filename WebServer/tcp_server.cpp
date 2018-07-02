#include "tcp_server.h"

namespace webserver {
    tcp_server::tcp_server(unsigned short int PORT, const function<bool(string)>& is_full_message,
                           const function<string(string)>& convert_client_message) :
            PORT(PORT), is_full_message(is_full_message),
            convert_client_message(convert_client_message) {
        accept_connections = true;
    }

    void tcp_server::start() {
        memset(&server_address, 0, sizeof(server_address));

        listener_socket = socket(AF_INET, SOCK_STREAM, 0);

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORT);
        server_address.sin_addr.s_addr = INADDR_ANY;

        //For setsock opt (REUSEADDR)
        int reuse_port = 1;

        //Avoid bind error if the socket was not closed last time;
        setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(int));

        if (bind(listener_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1) {
            throw runtime_error("[Server] Binding failed");
        }

        cout << "[Server] Configuring was done" << endl;
        cout << "[Server] Server was enabled" << endl;

        if (listen(listener_socket, allowed_connections_number) == -1) {
            throw runtime_error("[Server] Socket listening failed");
        }

        cout << "[Server] Waiting for connections on port " << PORT << "..." << endl;

        take_requests();
    }

    int tcp_server::find_client_index(const shared_ptr<client>& cl) {
        for (unsigned int index = 0; index < clients.size(); index++) {
            if (clients[index].get_id() == cl->get_id()) {
                return index;
            }
        }
        cerr << "Client with id " << cl->get_id() << " not found" << endl;
        return -1;
    }

    void tcp_server::connection_handler(const shared_ptr<client>& cl) {
        // add client to the clients <vector>
        mx.lock();

        cl->set_id(static_cast<int>(clients.size()));
        clients.push_back(*cl);
        cout << "New client with id " << cl->get_id() << " has been added to the clients list" << endl << endl;

        mx.unlock();

        char read_buffer[1024];
        ssize_t message_size;
        string received_message;

        while (accept_connections) {
            memset(&read_buffer, 0, sizeof(read_buffer));

            if ((message_size = recv(cl->sock, read_buffer, sizeof(read_buffer) - 1, 0)) > 0) {
                cout << "[Server] Client's message has been received" << endl;
                cout << "[Server] Client's message: " << endl;
                cout << "----------------------------" << endl;
                cout << read_buffer << endl;
                cout << "----------------------------" << endl;

                received_message.append(read_buffer, static_cast<unsigned long>(message_size));

                if (is_full_message(received_message)) {
                    string response_message = convert_client_message(received_message);

                    cout << "[Server] Server's response: " << endl;
                    cout << "----------------------------" << endl;
                    cout << response_message << endl;
                    cout << "----------------------------" << endl << endl;

                    if (send(cl->sock, response_message.c_str(), response_message.size(), 0) == -1) {
                        cout << "[Server] Message sending to client with id " << cl->get_id() << " failed" << endl;
                        cout << "============================" << endl << endl;
                    }
                    else {
                        cout << "[Server] Message has been sent to client with id " << cl->get_id() << endl;
                        cout << "============================" << endl << endl;
                    }

                    received_message.clear();
                }
            }
            if (message_size == 0) {
                // client disconnect
                cout << "Client with id " << cl->get_id() << " has been disconnected" << endl;
                close(cl->sock);

                //remove client from the clients <vector>
                mx.lock();

                int client_index = find_client_index(cl);
                if (client_index != -1) {
                    clients.erase(clients.begin() + client_index);
                    cout << "Client with id " << cl->get_id() << " has been removed from the clients vector" << endl;
                }
                else {
                    cout << "Client was not found in the clients vector" << endl;
                }

                mx.unlock();

                break;
            }
            if (message_size == -1) {
                if (!accept_connections) {
                    cerr << "Unable to receive message from client with id " << cl->get_id() << ". Server has been shut down. Stop receiving messages from this client" << endl;
                }
                else {
                    cerr << "Error while receiving message from client with id " << cl->get_id() << endl;
                }
                cerr << strerror(errno) << endl;
            }
        }
    }

    void tcp_server::take_requests() {
        shared_ptr<client> current_client;

        socklen_t cli_size = sizeof(sockaddr_in);

        while (accept_connections) {
            current_client = make_shared<client>();

            current_client->sock = accept(listener_socket, (struct sockaddr *) &server_address, &cli_size);

            if (current_client->sock != -1 && accept_connections) {
                cout << "----------------------------" << endl << endl;
                cout << "[Server] New connection has been accepted" << endl << endl;
                cout << "----------------------------" << endl << endl;


                thread handling_thread(&tcp_server::connection_handler, this, current_client);
                handling_thread.detach();
            }
            else {
                cout << "----------------------------" << endl << endl;
                cout << "[Server] Socket accept failed" << endl << endl;
                cout << "----------------------------" << endl << endl;
            }
        }
    }

    void tcp_server::stop() {
        accept_connections = false;

        close(listener_socket);
        cout << "[Server Stop] Main server's listener socket has been closed" << endl;
        cout << "[Server Stop] Server has been terminated" << endl;

        mx.lock();
        for (auto& cl : clients) {
            // client disconnect
            cout << "[Server Stop] Connection with client with id " << cl.get_id() << " has been closed" << endl;
            close(cl.sock);
        }
        mx.unlock();
    }
}