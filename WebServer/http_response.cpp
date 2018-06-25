#include "http_response.h"

namespace webserver {
    void http_response::set_response_http_code(int code) {
        http_code = code;
    }

    void http_response::set_response_body(const string& response) {
        response_body = response;
    }

    void http_response::set_response_length(unsigned long response_length) {
        content_length = response_length;
    }

    void http_response::set_response_headers(const vector<http_header>& http_headers) {
        headers = http_headers;
    }

    int http_response::get_response_code() const {
        return http_code;
    }

    const string& http_response::get_response_body() const {
        return response_body;
    }

    unsigned long http_response::get_content_length() const {
        return content_length;
    }

    const vector<http_header>& http_response::get_response_headers() const {
        return headers;
    }
}