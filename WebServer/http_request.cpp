#include "http_request.h"
using namespace std;

namespace webserver {
    void http_request::set_http_request_method(const string& client_requst_method) {
        method = client_requst_method;
    }

    void http_request::set_http_request_url(const string& client_requst_url) {
        url = client_requst_url;
    }

    void http_request::set_http_version(const string& client_http_version) {
        http_version = client_http_version;
    }

    void http_request::add_request_body_field(const string& name, const string& value) {
        request_body.emplace(name, value);
    }

    void http_request::add_http_request_param(const request_param& client_request_param) {
        request_params.push_back(client_request_param);
    }

    void http_request::add_http_request_header(http_header& client_request_header) {
        headers.push_back(client_request_header);
    }

    const vector<http_header>& http_request::get_headers() const {
        return headers;
    }

    const http_header http_request::get_header(const string& header_name) const {
        for (const auto& current_header : headers) {
            if (current_header.type == header_name) {
                return current_header;
            }
        }
        http_header error_header{"", ""};
        return error_header;
    }

    const vector<request_param> http_request::get_request_params() const {
        return request_params;
    }

    const map<string, string>& http_request::get_request_body() const {
        return request_body;
    }

    const string& http_request::get_request_url() const {
        return url;
    }

    const string& http_request::get_request_method() const {
        return method;
    }

    const string& http_request::get_request_http_version() const {
        return http_version;
    }
}