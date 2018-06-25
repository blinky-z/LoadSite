//
// Created by twite on 10.06.18.
//

#include "client.h"

namespace webserver {
    void client::set_id(int client_id) {
        id = client_id;
    }

    int client::get_id() {
        return id;
    }
}
