#ifndef WEB_HTTP_REQUEST_PARAMETERIZED_HEADER_PARSER_H
#define WEB_HTTP_REQUEST_PARAMETERIZED_HEADER_PARSER_H

#include <string>
#include <map>
#include <utility>
using namespace std;

namespace webserver {
    class http_request_parameterized_header_parser {
    private:
        void set_parameterized_header_type(pair<string, map<string, string>>& parsed_parameterized_header,
                                           const string& type);

        void add_parameterized_header_parameter(pair<string, map<string, string>>& parsed_parameterized_header,
                                                const string& key, const string& value);
    public:
        pair<string, map<string, string>> parse_parameterized_header(const string& raw_parameterized_header);
    };
}


#endif //WEB_HTTP_REQUEST_PARAMETERIZED_HEADER_PARSER_H
