#ifndef WEB_HTTP_RESPONSE_H
#define WEB_HTTP_RESPONSE_H

#include "http_header.h"
#include <vector>
#include <string>
using namespace std;

namespace webserver {
    //Класс, который инкапсулирует в себе http ответ: HTTP заголовки,
    //размер данных, тело данных, content-type
    class http_response {
    private:
        //заголовки HTTP ответа
        vector<http_header> headers;

        //Код ответа: 200, 400...
        int http_code;

        //Тело ответа
        string response_body;

        //Размер response_body
        unsigned long content_length;
    public:
        void set_response_http_code(int code);

        void set_response_body(const string& response);

        void set_response_length(unsigned long response_length);

        void set_response_headers(const vector<http_header>& http_headers);

        int get_response_code() const;

        const string& get_response_body() const;

        unsigned long get_content_length() const;

        const vector<http_header>& get_response_headers() const;
    };
}

#endif //WEB_HTTP_RESPONSE_H
