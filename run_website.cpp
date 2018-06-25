#include "WebServer/web_server.h"
#include <vector>

const unsigned short int PORT = 8080;

int main() {
    vector<webserver::web_handler> handlers;

    webserver::web_server server(PORT, handlers);

    server.start();
}