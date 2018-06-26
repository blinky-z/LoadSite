#include "WebServer/web_server.h"
#include "nlohmann/json.hpp"
#include <map>

const unsigned short int PORT = 8080;

map<string, string> generate_default_goods() {
    map<string, string> goods;

    for (int i = 0; i < 5; i++) {
        goods["default" + to_string(i)] = to_string(i * 30);
    }

    return goods;
}

map<string, string> generate_user_goods(const string& name) {
    map<string, string> goods;

    int a = 0;

    for (unsigned int i = 0; i < name.length(); i++) {
        a += name[i];
    }

    for (unsigned int i = 0; i < name.length(); i++) {
        goods["name" + to_string(i)] = to_string((i + 1) * a);
    }

    return goods;
}

int main() {
    vector<webserver::web_handler> handlers;

    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"")
    function<webserver::http_response(webserver::http_request)> put_goods = [&](webserver::http_request request) {
        webserver::http_response response;
        string response_body;

        string user_name;

        const vector<webserver::request_param> request_parameters = request.get_request_params();
        for (auto current_request_parameter : request_parameters) {
            if (current_request_parameter.name == "name") {
                user_name = current_request_parameter.value;
                break;
            }
        }

        if (user_name.empty()) {
            const map<string, string>& request_body = request.get_request_body();
            for (auto current_param : request_body) {
                if (current_param.first == "name") {
                    user_name = current_param.second;
                    break;
                }
            }
        }

        map<string, string> goods;

        nlohmann::json j;

        if (!user_name.empty()) {
            goods = generate_user_goods(user_name);
            j["nickname"] = user_name;
        }
        else {
            goods = generate_default_goods();
        }

        nlohmann::json j_goods(goods);

        j["items"] = j_goods;

        response.set_response_body(response_body);
        response.set_response_http_code(200);
        response.set_response_length(response_body.size());

        vector<webserver::http_header> headers;
        headers.push_back(webserver::http_header{"Content-Type", "application/json"});
        response.set_response_headers(headers);

        return response;
    };
    _Pragma("GCC diagnostic pop")

    webserver::web_handler get_goods("/", "GET", put_goods);

    webserver::web_server server(PORT, handlers);

    server.start();
}