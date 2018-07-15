#include "web_handler.h"

namespace webserver {
    web_handler::web_handler(const string& pattern, const string& method, function<http_response(http_request)> handler)
            : pattern(pattern), method(method), handler(move(handler)) {};

    const string& web_handler::get_web_handler_method() const {
        return method;
    }

    const string& web_handler::get_web_handler_pattern() const {
        return pattern;
    }

    const function<http_response(http_request)>& web_handler::get_transform_to_response_function() const {
        return handler;
    }
}