#include "http_request_parser.h"

using namespace std;

namespace webserver {
    //split request body into vector of strings for ease of request body parsing
    vector<string> http_request_parser::get_splitted_raw_request_body(const vector<string>& raw_request) {
        vector<string> raw_request_body;
        string body;

        auto index_start_of_body = find(raw_request.begin(), raw_request.end(), "\r\n") + 1;

        for (auto current_line = index_start_of_body; current_line != raw_request.end(); current_line++) {
            body += *current_line;
        }

        //add "\r\n" to the end of raw body to correct body splitting
        body += "\r\n";

        regex rx("[^\r\n]+\r\n");
        sregex_iterator formated_body_list(body.begin(), body.end(), rx), rxend;

        while(formated_body_list != rxend) {
            const string& current_line = formated_body_list->str();
            raw_request_body.emplace_back(current_line.substr(0, current_line.length() - 2));
            ++formated_body_list;
        }

        return raw_request_body;
    }

    http_request_parser::content_type http_request_parser::parse_content_type_header
            (const string& raw_content_type_header) {

        pair<string, map<string, string>> parsed_content_type_header =
                parameterized_header_parser.parse_parameterized_header(raw_content_type_header);

        content_type converted_header{parsed_content_type_header.first, parsed_content_type_header.second};

        return converted_header;
    }

    http_request_parser::content_disposition http_request_parser::parse_content_disposition_header
            (const string& raw_content_disposition_header) {

        pair<string, map<string, string>> parsed_content_disposition_header =
                parameterized_header_parser.parse_parameterized_header(raw_content_disposition_header);

        content_disposition converted_header{parsed_content_disposition_header.first,
                                             parsed_content_disposition_header.second};

        return converted_header;
    }

    string http_request_parser::unescape_string(const string& escaped_string) {
        string unescaped_string;

        unsigned int hexadecimal_char_length = 3;

        for (unsigned long current_char_position = 0; current_char_position < escaped_string.size();) {
            char current_char = escaped_string[current_char_position];

            if (current_char == '+') {
                unescaped_string.push_back(' ');

                current_char_position++;
            } else if (current_char == '%' &&
                       escaped_string.size() - current_char_position >= hexadecimal_char_length) {
                char unescaped_char = static_cast<char>(stoi("0x" + escaped_string.substr(current_char_position + 1, 2),
                                                             nullptr, 16));
                unescaped_string.push_back(unescaped_char);

                current_char_position += hexadecimal_char_length;
            } else {
                unescaped_string.push_back(current_char);

                current_char_position++;
            }
        }

        return unescaped_string;
    }

    void http_request_parser::parse_urlencoded_body(http_request &post_request, const vector<string> &raw_request_body) {

        vector<string> unescaped_request_body;

        for (auto& current_line : raw_request_body) {
            string unescaped_body_part = unescape_string(current_line);
            unescaped_request_body.emplace_back(unescaped_body_part);
        }

        char parameters_delimiter = '&';
        char key_value_delimiter = '=';

        string key;
        string value;

        bool key_appeared = true;

        for (const auto& current_line : unescaped_request_body) {
            for (const auto& current_char : current_line) {
                if (current_char == parameters_delimiter) {
                    key_appeared = true;

                    post_request.add_request_body_field(key, value);

                    key.clear();
                    value.clear();

                    continue;
                }

                if (current_char == key_value_delimiter) {
                    key_appeared = false;
                    continue;
                }

                key_appeared ? key.push_back(current_char) :  value.push_back(current_char);
            }
        }

        post_request.add_request_body_field(key, value);
    }

    bool http_request_parser::check_if_current_request_body_line_is_end_boundary
            (const string& line, const string& boundary) {

        return line.find("--" + boundary + "--") == 0;
    }

    bool http_request_parser::check_if_current_request_body_line_is_boundary
            (const string& line, const string& boundary) {

        return line.find("--" + boundary) == 0;
    }

    void http_request_parser::parse_formdata_body
            (http_request &post_request, const vector<string>& raw_request_body, const string& boundary) {

        string key;
        string value;

        for (auto current_line = raw_request_body.begin(); current_line != raw_request_body.end(); current_line++) {
            if (check_if_current_request_body_line_is_end_boundary(*current_line, boundary)) {
                break;
            }

            if (check_if_current_request_body_line_is_boundary(*current_line, boundary)) {
                current_line++;
                content_disposition current_body_subpart_content_disposition = parse_content_disposition_header(
                        *current_line);

                key = current_body_subpart_content_disposition.parameters["name"];

                current_line++;

                while (*current_line == "\r\n") {
                    current_line++;
                }

                //add string to value until next boundary or body end
                while (true) {
                    value += *current_line;

                    if (current_line + 1 != raw_request_body.end() &&
                        !check_if_current_request_body_line_is_boundary(*(current_line + 1), boundary)) {
                        current_line++;
                    } else {
                        break;
                    }
                }

                post_request.add_request_body_field(key, value);

                key.clear();
                value.clear();
            }
        }
    }

    void http_request_parser::parse_request_line(http_request& request, const vector<string>& raw_request) {
        istringstream request_line(raw_request[0]);

        string method;
        string request_url;
        string protocol_version;

        request_line >> method >> request_url >> protocol_version;

        request.set_http_request_method(method);
        request.set_http_request_url(request_url);
        request.set_http_version(protocol_version);
    }

    bool http_request_parser::check_if_request_parameters_exists(const string& url) {
        for (auto current_char : url) {
            if (current_char == '?') {
                return true;
            }
        }
        return false;
    }

    string http_request_parser::shrink_url_to_parameters(const string& url) {
        string shrinked_url = url.substr(url.find('?') + 1);;
        return shrinked_url;
    }

    void http_request_parser::parse_request_parameters(const string& request_parameters, http_request& request) {
        bool parameter_name_appeared = true;

        string parameter_name;
        string parameter_value;

        char request_parameters_delimiter = '&';

        for (char current_char : request_parameters) {
            if (current_char == request_parameters_delimiter) {
                parameter_name_appeared = true;

                request_param current_request_parameter;

                current_request_parameter.name = parameter_name;
                current_request_parameter.value = parameter_value;

                request.add_http_request_param(current_request_parameter);

                parameter_name.clear();
                parameter_value.clear();

                continue;
            }

            if (current_char == '=') {
                parameter_name_appeared = false;
                continue;
            }

            if (parameter_name_appeared) {
                parameter_name.push_back(current_char);
            }
            else {
                parameter_value.push_back(current_char);
            }
        }

        request_param current_request_parameter;

        current_request_parameter.name = parameter_name;
        current_request_parameter.value = parameter_value;

        request.add_http_request_param(current_request_parameter);
    }

    void http_request_parser::parse_url_to_parameters(http_request& request) {
        string url = request.get_request_url();

        if (check_if_request_parameters_exists(url)) {
            url = shrink_url_to_parameters(url);
            parse_request_parameters(url, request);
        }
    }

    void http_request_parser::extend_request_url_by_host(http_request& request) {
        string full_url;
        string host;
        string resource_path = request.get_request_url();

        const vector<http_header> headers = request.get_headers();
        for (auto current_header: headers) {
            if (current_header.type == "Host") {
                host = current_header.value;
                break;
            }
        }

        full_url = host + resource_path;
        request.set_http_request_url(full_url);
    }

    void http_request_parser::parse_headers(http_request& request, const vector<string>& raw_http_request) {
        string headers_and_body_delimiter = "\r\n";
        char header_type_and_value_delimiter = ':';

        unsigned int index_start_of_headers = 1;

        for (auto current_message_line = raw_http_request.begin() + index_start_of_headers;
                *current_message_line != headers_and_body_delimiter; current_message_line++) {

            string current_header_type;
            string current_header_value;

            for (size_t current_char_postion = 0; current_char_postion < current_message_line->size();
                    current_char_postion++) {

                if ((*current_message_line)[current_char_postion] != header_type_and_value_delimiter) {
                    current_header_type.push_back((*current_message_line)[current_char_postion]);
                }
                else {
                    size_t value_start_position = current_char_postion + 1;
                    while ((*current_message_line)[value_start_position] == ' ') {
                        value_start_position++;
                    }

                    current_header_value = current_message_line->substr(value_start_position);

                    current_header_value.pop_back();
                    current_header_value.pop_back();

                    http_header current_header;
                    current_header.type = current_header_type;
                    current_header.value = current_header_value;

                    request.add_http_request_header(current_header);

                    break;
                }
            }
        }
    }

    bool http_request_parser::check_if_request_url_is_absolute_path(const string& request_url) {
        return request_url[0] == '/';
    }

    void http_request_parser::parse_post_request(http_request& post_request, const vector<string> &raw_request) {
        const vector<string>& raw_request_body = get_splitted_raw_request_body(raw_request);

        const http_header& content_type_header = post_request.get_header("Content-Type");

        if (content_type_header.value.empty()) {
            cerr << "No Content-Type header was given to parse request body";
            return;
        }

        string content_type_value = content_type_header.value;

        content_type obj = parse_content_type_header(content_type_value);

        if (obj.type == "application/x-www-form-urlencoded") {
            parse_urlencoded_body(post_request, raw_request_body);
        }
        else {
            parse_formdata_body(post_request, raw_request_body, obj.parameters["boundary"]);
        }
    }

    http_request http_request_parser::parse_request(const vector<string> &raw_request) {
        http_request request;

        parse_request_line(request, raw_request);
        parse_url_to_parameters(request);
        parse_headers(request, raw_request);

        const string& url = request.get_request_url();
        if (check_if_request_url_is_absolute_path(url)) {
            extend_request_url_by_host(request);
        }

        if (request.get_request_method() == "POST") {
            parse_post_request(request, raw_request);
        }

        return request;
    }
}
