//
// Created by TwITe on 05/09/2018.
//

#ifndef WEB_HTTP_RESPONSE_BUILDER_H
#define WEB_HTTP_RESPONSE_BUILDER_H

#include "http_response.h"
#include "web_handler.h"
#include <map>

namespace webserver {
    class http_response_builder {
    private:
        map<int, string> reason_phrases;

        string build_response_status_line(const http_response& response);

        void add_response_status_line(const http_response& response, string& converted_to_string_response);

        void add_content_length_header(const http_response& response, string& converted_to_string_response);

        void add_response_header_fields(const http_response& response, string& converted_to_string_response);

        void add_response_body(const http_response& response, string& converted_to_string_response);

    public:
        http_response_builder();

        string build_response(const http_response& response);
    };
}


#endif //WEB_HTTP_RESPONSE_BUILDER_H
