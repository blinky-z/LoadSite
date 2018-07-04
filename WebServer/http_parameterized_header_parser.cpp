#include "http_parameterized_header_parser.h"

namespace webserver {
    void http_request_parameterized_header_parser::set_parameterized_header_type(
            pair<string, map<string, string>>& parsed_parameterized_header, const string& type) {

        parsed_parameterized_header.first = type;
    }

    void http_request_parameterized_header_parser::add_parameterized_header_parameter(
            pair<string, map<string, string>>& parsed_parameterized_header, const string& key, const string& value) {

        parsed_parameterized_header.second.emplace(key, value);
    }

    pair<string, map<string, string>> http_request_parameterized_header_parser::parse_parameterized_header
            (const string& raw_parameterized_header) {

        pair<string, map<string, string>> parsed_parameterized_header;

        char parameters_delimiter = ';';

        size_t index_begin_of_new_parameter = raw_parameterized_header.find(parameters_delimiter);

        string type = (index_begin_of_new_parameter != string::npos) ?
                raw_parameterized_header.substr(0, index_begin_of_new_parameter) : raw_parameterized_header;

        set_parameterized_header_type(parsed_parameterized_header, type);

        //parse parameters
        if (index_begin_of_new_parameter != string::npos) {
            string parameter;
            string key;
            string value;

            char key_value_delimiter = '=';

            size_t begin_of_parameter = index_begin_of_new_parameter + 1;
            size_t end_of_parameter;

            while(raw_parameterized_header[begin_of_parameter] == ' ') {
                begin_of_parameter++;
            }

            while ((index_begin_of_new_parameter = raw_parameterized_header.find(';', index_begin_of_new_parameter + 1))
                    != string::npos) {

                end_of_parameter = index_begin_of_new_parameter;
                parameter = raw_parameterized_header.substr(begin_of_parameter, end_of_parameter - begin_of_parameter);

                key = parameter.substr(0, parameter.find(key_value_delimiter));
                value = parameter.substr(parameter.find(key_value_delimiter) + 1);

                //remove quotes
                if (value[0] == '"') {
                    value = value.substr(1, value.length() - 2);
                }

                add_parameterized_header_parameter(parsed_parameterized_header, key, value);

                begin_of_parameter = end_of_parameter + 1;
                while(raw_parameterized_header[begin_of_parameter] == ' ') {
                    begin_of_parameter++;
                }
            }

            parameter = raw_parameterized_header.substr(begin_of_parameter);
            key = parameter.substr(0, parameter.find(key_value_delimiter));
            value = parameter.substr(parameter.find(key_value_delimiter) + 1);

            if (value[0] == '"') {
                value = value.substr(1, value.length() - 2);
            }

            add_parameterized_header_parameter(parsed_parameterized_header, key, value);
        }

        return parsed_parameterized_header;
    }
}