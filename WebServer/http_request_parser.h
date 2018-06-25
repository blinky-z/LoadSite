#ifndef WEB_HTTP_REQUEST_PARSER_H
#define WEB_HTTP_REQUEST_PARSER_H
#include "http_request.h"
#include "http_parameterized_header_parser.h"
#include <string>
#include <sstream>

namespace webserver {
    class http_request_parser {
    private:
        void parse_request_line(http_request& request, const vector<string>& raw_request);

        string shrink_url_to_parameters(const string& url);

        void parse_request_parameters(const string& request_parameters, http_request& request);

        void parse_url_to_parameters(http_request& request);

        void extend_request_url_by_host(http_request& request);

        void parse_headers(http_request& request, const vector<string>& raw_http_request);

        bool check_if_request_parameters_exists(const string& url);

        bool check_if_request_url_is_absolute_path(const string& url);

        vector<string> get_splitted_raw_request_body(const vector<string>& raw_request);

        http_request_parameterized_header_parser parameterized_header_parser;

        struct content_type {
            string type;
            map<string, string> parameters;
        };

        struct content_disposition {
            string type;
            map<string, string> parameters;
        };

        content_disposition parse_content_disposition_header(const string& raw_content_disposition_header);

        content_type parse_content_type_header(const string& raw_content_type_header);

        void parse_urlencoded_body(http_request& post_request, const vector<string>& raw_request_body);

        bool check_if_current_request_body_line_is_boundary(const string& line, const string& boundary);

        bool check_if_current_request_body_line_is_end_boundary(const string& line, const string& boundary);

        void parse_formdata_body(http_request& post_request, const vector<string>& raw_request_body, const string& boundary);

        void parse_post_request(http_request& post_request, const vector<string>& raw_request);
    public:
        http_request parse_request(const vector<string> &raw_request);
    };
}

#endif //WEB_HTTP_REQUEST_PARSER_H
