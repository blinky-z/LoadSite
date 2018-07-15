#ifndef WEB_HTTP_HEADER_H
#define WEB_HTTP_HEADER_H

#include <string>

using namespace std;

namespace webserver {
    struct http_header {
        //Тип заголовка (например, Content-type)
        string type;
        //Значение заголовка (например, text/html; charset=utf-8)
        string value;
    };
}

#endif //WEB_HTTP_HEADER_H
