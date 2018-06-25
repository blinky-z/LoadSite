//
// Created by TwITe on 05/09/2018.
//

#include "http_response_builder.h"

namespace webserver {
    http_response_builder::http_response_builder() {
        reason_phrases[100] = "Continue";
        reason_phrases[101] = "Switching Protocols";
        reason_phrases[200] = "OK";
        reason_phrases[201] = "Created";
        reason_phrases[202] = "Accepted";
        reason_phrases[203] = "Non-Authoritative Information";
        reason_phrases[204] = "No Content";
        reason_phrases[205] = "Reset Content";
        reason_phrases[206] = "Partial Content";
        reason_phrases[300] = "Multiple Choices";
        reason_phrases[301] = "Moved Permanently";
        reason_phrases[302] = "Found";
        reason_phrases[303] = "See Other";
        reason_phrases[304] = "Not Modified";
        reason_phrases[305] = "Use Proxy";
        reason_phrases[307] = "Temporary Redirect";
        reason_phrases[400] = "Bad Request";
        reason_phrases[401] = "Unauthorized";
        reason_phrases[402] = "Payment Required";
        reason_phrases[403] = "Forbidden";
        reason_phrases[404] = "Not Found";
        reason_phrases[405] = "Method Not Allowed";
        reason_phrases[406] = "Not Acceptable";
        reason_phrases[407] = "Proxy Authentication Required";
        reason_phrases[408] = "Request Time-out";
        reason_phrases[409] = "Conflict";
        reason_phrases[410] = "Gone";
        reason_phrases[411] = "Length Required";
        reason_phrases[412] = "Precondition Failed";
        reason_phrases[413] = "Request Entity Too Large";
        reason_phrases[414] = "Request-URI Too Large";
        reason_phrases[415] = "Unsupported Media Type";
        reason_phrases[416] = "Requested range not satisfiable";
        reason_phrases[417] = "Expectation Failed";
        reason_phrases[500] = "Internal Server Error";
        reason_phrases[501] = "Not Implemented";
        reason_phrases[502] = "Bad Gateway";
        reason_phrases[503] = "Service Unavailable";
        reason_phrases[504] = "Gateway Time-out";
        reason_phrases[505] = "HTTP Version not supported";
    }

    string http_response_builder::build_response_status_line(const webserver::http_response& response) {
        string response_http_version = "HTTP/1.1";
        int response_status_code = response.get_response_code();
        const string& response_reason_phrase = reason_phrases[response_status_code];

        string response_status_line = response_http_version + " " + to_string(response_status_code) + " " + response_reason_phrase;

        return response_status_line;
    }

    void http_response_builder::add_response_status_line(const http_response& response, string& converted_to_string_response) {
        converted_to_string_response += build_response_status_line(response) + "\r\n";
    }

    void http_response_builder::add_response_header_fields(const http_response &response,
                                                           string &converted_to_string_response) {
        const vector<http_header>& response_headers = response.get_response_headers();

        for (const http_header& current_header : response_headers) {
            converted_to_string_response += current_header.type + ": " + current_header.value + "\r\n";
        }
    }

    void http_response_builder::add_content_length_header(const http_response& response, string& converted_to_string_response) {
        unsigned long response_body_length = response.get_content_length();

        http_header content_length_header;
        content_length_header.type = "Content-Length";
        content_length_header.value = to_string(response_body_length);

        converted_to_string_response += content_length_header.type + ": " + content_length_header.value + "\r\n";
    }

    void http_response_builder::add_response_body(const http_response& response, string& converted_to_string_response) {
        const string& response_body = response.get_response_body();

        converted_to_string_response += response_body;
    }

    string http_response_builder::build_response(const http_response& response) {
        string converted_to_string_response;

        add_response_status_line(response, converted_to_string_response);
        add_response_header_fields(response, converted_to_string_response);

        unsigned long response_body_length = response.get_content_length();

        bool is_response_message_body_exists = (response_body_length != 0);

        if (is_response_message_body_exists) {
            add_content_length_header(response, converted_to_string_response);
            converted_to_string_response += "\r\n";
            add_response_body(response, converted_to_string_response);
        }

        return converted_to_string_response;
    }
}