#include "nlohmann/json.hpp"
#include "WebServer/web_server.h"
#include <iostream>
using namespace std;

unsigned short int PORT = 8080;

void open_connection(int& sockfd, struct sockaddr_in& server_address) {
    server_address = {};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_aton("127.0.0.1", &server_address.sin_addr) == 0) {
        cout << "[Server] Invalid IP address" << endl;
    }

    if (connect(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        cout << "[Client] Connection failed" << endl;
    }
    cout << "[Client] All setting are done" << endl;
    cout << "[Client] Succefully connected to server" << endl << endl;
    cout << "----------------------------" << endl << endl;
}

int main() {
    int sockfd;
    struct sockaddr_in server_address;
    open_connection(sockfd, server_address);

    //formdata
    string request_body = "--sdf\r\n"
                          "Content-Disposition: form-data; name=\"name\"\r\n"
                          "maxim\r\n"
                          "--sdf--";

    string message = "POST / HTTP/1.1\r\n"
                     "Host: localhost:8080\r\n"
                     "Content-Type: multipart/form-data;boundary=\"sdf\"\r\n"
                     "Content-Length: " + to_string(request_body.size()) + "\r\n"
                     "\r\n";

    message += request_body;

    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cout << "[Client] Message sending failure" << endl;
    }
    else {
        cout << "[Client] Message was sent to server" << endl;
    }

    char buffer[128000];

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    string received_message = string(buffer);

    cout << "[Client] Server message:" << endl << buffer << endl << endl;

    string response_body = received_message.substr(received_message.find("\r\n\r\n") + 4);

    nlohmann::json j = nlohmann::json::parse(response_body);

    cout << "[Client] Response body:" << endl << j.dump(4) << endl;

    cout << endl << "---------------------" << endl << endl;

    //urlencoded
    request_body = "name=dima";

    message = "POST / HTTP/1.1\r\n"
                     "Host: localhost:8080\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "Content-Length: " + to_string(request_body.size()) + "\r\n"
                     "\r\n";

    message += request_body;

    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cout << "[Client] Message sending failure" << endl;
    }
    else {
        cout << "[Client] Message was sent to server" << endl;
    }

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    received_message = string(buffer);

    cout << "[Client] Server message:" << endl << buffer << endl << endl;

    response_body = received_message.substr(received_message.find("\r\n\r\n") + 4);

    j = nlohmann::json::parse(response_body);

    cout << "[Client] Response body:" << endl << j.dump(4) << endl;

    //pass name as a query param
    message = "GET /?name=petya HTTP/1.1\r\n"
              "Host: localhost:8080\r\n"
              "\r\n";

    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cout << "[Client] Message sending failure" << endl;
    }
    else {
        cout << "[Client] Message was sent to server" << endl;
    }

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    received_message = string(buffer);

    cout << "[Client] Server message:" << endl << buffer << endl << endl;

    response_body = received_message.substr(received_message.find("\r\n\r\n") + 4);

    j = nlohmann::json::parse(response_body);

    cout << "[Client] Response body:" << endl << j.dump(4) << endl;

    //get default goods
    message = "GET / HTTP/1.1\r\n"
              "Host: localhost:8080\r\n"
              "\r\n";

    if (write(sockfd, message.c_str(), message.size()) == -1) {
        cout << "[Client] Message sending failure" << endl;
    }
    else {
        cout << "[Client] Message was sent to server" << endl;
    }

    memset(&buffer, 0, 128000);
    read(sockfd, buffer, 128000);

    received_message = string(buffer);

    cout << "[Client] Server message:" << endl << buffer << endl << endl;

    response_body = received_message.substr(received_message.find("\r\n\r\n") + 4);

    j = nlohmann::json::parse(response_body);

    cout << "[Client] Response body:" << endl << j.dump(4) << endl;
}