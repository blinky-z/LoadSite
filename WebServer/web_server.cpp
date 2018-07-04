#include "web_server.h"

namespace webserver {
    web_server::web_server(unsigned short int port, const vector<web_handler>& handlers) :
            handlers(handlers), server(port, check_is_full_message, convert_client_message) {}

    //Запуск web-server.
    //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
    void web_server::start() {
        server.start();
    }

    void web_server::stop() {
        server.stop();
    }
}