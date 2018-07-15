#ifndef WEB_REQUEST_PARAM_H
#define WEB_REQUEST_PARAM_H

#include <string>

using namespace std;

namespace webserver {
    struct request_param {
        //Имя параметра
        string name;

        //Значение параметра
        string value;
    };
}

#endif //WEB_REQUEST_PARAM_H