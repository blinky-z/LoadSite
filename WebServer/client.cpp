//
// Created by twite on 10.06.18.
//

#include "client.h"

namespace webserver {
    void client::set_id(unsigned long client_id) {
        id = client_id;
    }

    unsigned long client::get_id() {
        return id;
    }
}
