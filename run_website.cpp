#include "WebServer/web_server.h"
#include "nlohmann/json.hpp"
#include <map>

const unsigned short int PORT = 8080;

int main() {
    vector<webserver::web_handler> handlers;

    function<webserver::http_response(webserver::http_request)> put_goods = [&](webserver::http_request request) {
        webserver::http_response response;
        string response_body;

        const unsigned int default_goods_number = 5;
        string user_name;

        if (request.get_request_method() == "GET") {
            const vector<webserver::request_param> request_parameters = request.get_request_params();
            for (auto current_request_parameter : request_parameters) {
                if (current_request_parameter.name == "name") {
                    user_name = current_request_parameter.value;
                    break;
                }
            }
        }
        else {
            const map<string, string>& request_body = request.get_request_body();
            for (auto current_param : request_body) {
                if (current_param.first == "name") {
                    user_name = current_param.second;
                    break;
                }
            }
        }

        nlohmann::json j;

        if (!user_name.empty()) {
            //fill with particular goods for current user
            j["nickname"] = user_name;

            int a = 0;
            for (unsigned int i = 0; i < user_name.length(); i++) {
                a += user_name[i];
            }

            for (unsigned int i = 0; i < user_name.length(); i++) {
                j["items"][i]["name"] = user_name + to_string(i);
                j["items"][i]["price"] = to_string((i + 1) * a);
            }
        }
        else {
            //fill with default goods
            for (int i = 0; i < 5; i++) {
                for (unsigned int i = 0; i < default_goods_number; i++) {
                    j["items"][i]["name"] = "default" + to_string(i);
                    j["items"][i]["price"] = to_string(i * 30);
                }
            }
        }

        response_body = j.dump();

        response.set_response_http_code(200);
        response.set_response_body(response_body);
        response.set_response_length(response_body.size());

        vector<webserver::http_header> headers;
        headers.push_back(webserver::http_header{"Content-Type", "application/json"});
        response.set_response_headers(headers);

        return response;
    };

    webserver::web_handler get_goods_get_request("/", "GET", put_goods);
    webserver::web_handler get_goods_post_request("/", "POST", put_goods);

    handlers.emplace_back(get_goods_get_request);
    handlers.emplace_back(get_goods_post_request);

    webserver::web_server server(PORT, handlers);

    server.start();
}