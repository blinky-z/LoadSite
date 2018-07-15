#include "http_router.h"

namespace webserver {
    string http_router::get_request_pattern(const http_request& request) {
        string pattern;
        const string& url = request.get_request_url();

        size_t request_params_begin_position = url.find('?');
        string protocol_delimiter = "://";
        size_t http_prefix_end_position = url.find(protocol_delimiter) + protocol_delimiter.size();

        string shrinked_url;

        if (request_params_begin_position != string::npos) {
            shrinked_url = url.substr(http_prefix_end_position,
                                      request_params_begin_position - http_prefix_end_position);
        } else {
            shrinked_url = url.substr(http_prefix_end_position);
        }

        size_t host_end_position = (shrinked_url.find('/') != string::npos ? shrinked_url.find('/')
                                                                           : shrinked_url.size());

        shrinked_url = shrinked_url.substr(host_end_position);

        pattern = shrinked_url;

        if (pattern.empty()) {
            pattern = "/";
        }

        return pattern;
    }

    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")

    web_handler http_router::generate_404_error_handler() {
        function<webserver::http_response(webserver::http_request)> error_404_handler = [&](
                webserver::http_request request) {
            webserver::http_response response;

            string error_message = "Not Found";
            response.set_response_body(error_message);
            response.set_response_http_code(404);

            vector<http_header> response_headers;

            http_header content_type{"Content-Type", "text/plain"};

            response_headers.emplace_back(content_type);

            response.set_response_headers(response_headers);

            response.set_response_length(error_message.size());

            return response;
        };

        web_handler error_handler("", "", error_404_handler);

        return error_handler;
    }

    _Pragma("GCC diagnostic pop")


    web_handler
    http_router::get_suitable_request_handler(const vector<web_handler>& handlers, const http_request& request) {
        const string& client_request_method = request.get_request_method();
        const string& client_request_pattern = get_request_pattern(request);

        for (const auto& current_web_handler : handlers) {
            const string& current_web_handler_pattern = current_web_handler.get_web_handler_pattern();
            const string& current_web_handler_method = current_web_handler.get_web_handler_method();
            if (current_web_handler_method == client_request_method &&
                current_web_handler_pattern == client_request_pattern) {

                return current_web_handler;
            }
        }

        return generate_404_error_handler();
    }
}