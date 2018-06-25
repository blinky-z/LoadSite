#ifndef WEB_HTTP_REQUEST_H
#define WEB_HTTP_REQUEST_H

#include "http_header.h"
#include "request_param.h"
#include <vector>
#include <map>

using namespace std;

namespace webserver {
    //Класс, который инкапсулирует в себе http запрос: HTTP заголовки,
    //URL запроса, request-параметры, тело запроса (в случае POST запроса), тип запроса - GET, POST, PUT
    class http_request {
    private:
        //заголовки HTTP запроса
        vector<http_header> headers;

        //Параметры запроса (например, если запрос был http://localhost:8083/comand?param=1&param2=2 ,
        //То нужно в этот вектор положить 2 пары: param и 1; param2 и 2
        vector<request_param> request_params;

        //тело запроса в кодировке, определенной в заголовке Content-Type. По умолчанию - utf-8
        //Тело может отсутствовать, например, если был GET запрос
        map<string, string> request_body;

        //URL запроса. Например, http://localhost:8083/comand?param=1&param2=2
        string url;

        //Метод запроса - POST, GET, PUT, PATCH, HEADER...
        string method;

        //Версия HTTP протокола - указывается в request line
        string http_version;
    public:
        void set_http_request_method(const string& client_requst_method);

        void set_http_request_url(const string& client_requst_url);

        void set_http_version(const string& client_http_version);

        void add_request_body_field(const string& name, const string& value);

        void add_http_request_param(const request_param& client_request_param);

        void add_http_request_header(http_header& client_request_header);

        const vector<http_header>& get_headers() const;

        const http_header get_header(const string& header_name) const;

        const vector<request_param> get_request_params() const;

        const map<string, string>& get_request_body() const;

        const string& get_request_url() const;

        const string& get_request_method() const;

        const string& get_request_http_version() const;
    };
}

#endif //WEB_HTTP_REQUEST_H
