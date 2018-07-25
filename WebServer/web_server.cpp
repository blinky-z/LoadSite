#include "web_server.h"

namespace webserver {
    web_server::web_server(unsigned short int port, const vector<web_handler>& handlers) :
            handlers(handlers), server(port, check_is_full_message, convert_client_message) {}

    void web_server::start() {
        std::ofstream inf("./logs/Info.log", fstream::out | fstream::trunc);
        std::cout.rdbuf(inf.rdbuf());

        std::ofstream err("./logs/Error.log", fstream::out | fstream::trunc);
        std::cerr.rdbuf(err.rdbuf());

        server.start();
    }

    void web_server::stop() {
        server.stop();
    }
}