
#ifndef WEB_CLIENT_H
#define WEB_CLIENT_H

namespace webserver {
    class client {
    private:
        int id;
    public:
        int sock;

        void set_id(int client_id);

        int get_id();
    };
}


#endif //WEB_CLIENT_H
