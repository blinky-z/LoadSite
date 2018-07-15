#ifndef WEB_HTTP_ROUTER_H
#define WEB_HTTP_ROUTER_H

#include "web_handler.h"
#include "http_request.h"

namespace webserver {
    //searches the proper web handler for the particular request
    class http_router {
    private:
        string get_request_pattern(const http_request& request);

        web_handler generate_404_error_handler();

    public:
        web_handler get_suitable_request_handler(const vector<web_handler>& handlers, const http_request& request);
    };
}

#endif //WEB_HTTP_ROUTER_H
