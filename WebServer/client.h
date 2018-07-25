#ifndef WEB_CLIENT_H
#define WEB_CLIENT_H

#include <string>

namespace webserver {
    class client {
    private:
        unsigned long id;
    public:
        int sock;

        void set_id(unsigned long client_id);

        unsigned long get_id();
    };
}


#endif //WEB_CLIENT_H
