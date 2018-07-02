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

        //set user name if it exists
        if (request.get_request_method() == "GET") {
            const vector<webserver::request_param> request_parameters = request.get_request_params();
            for (auto current_request_parameter : request_parameters) {
                if (current_request_parameter.name == "name") {
                    user_name = current_request_parameter.value;
                    break;
                }
            }
        }
        if (request.get_request_method() == "POST") {
            const map<string, string>& request_body = request.get_request_body();
            for (auto current_param : request_body) {
                if (current_param.first == "name") {
                    user_name = current_param.second;
                    break;
                }
            }
        }

        nlohmann::json json_response_body;

        if (!user_name.empty()) {
            //fill with particular goods for current user
            json_response_body["nickname"] = user_name;

            unsigned int multiplier = 0;
            for (char current_char_number : user_name) {
                multiplier += current_char_number;
            }

            for (unsigned int current_good_number = 0; current_good_number < user_name.length(); current_good_number++) {
                json_response_body["items"][current_good_number]["name"] = user_name + to_string(current_good_number);
                json_response_body["items"][current_good_number]["price"] = to_string((current_good_number + 1) * multiplier);
            }
        }
        else {
            //fill with default goods
            unsigned int multiplier = 30;

            for (unsigned int current_good_number = 0; current_good_number < default_goods_number; current_good_number++) {
                json_response_body["items"][current_good_number]["name"] = "default" + to_string(current_good_number);
                json_response_body["items"][current_good_number]["price"] = to_string(current_good_number * multiplier);
            }
        }

        //get string representation of the json body
        response_body = json_response_body.dump();

        response.set_response_http_code(200);
        response.set_response_body(response_body);
        response.set_response_length(response_body.size());

        vector<webserver::http_header> headers;
        headers.push_back(webserver::http_header{"Content-Type", "application/json"});
        response.set_response_headers(headers);

        return response;
    };

    function<webserver::http_response(webserver::http_request)> buy_goods_check = [&](webserver::http_request request) {
        webserver::http_response response;

        nlohmann::json json_response_body;
        string success_purchase_message = "success";
        string failure_purchase_message = "failure";

        const map<string, string>& request_body = request.get_request_body();

        string raw_json_body = request_body.begin()->second;

        nlohmann::json json_request_body = nlohmann::json::parse(raw_json_body);

        string good_name = json_request_body["name"];

        if (good_name.length() % 2 == 0) {
            json_response_body["result"] = success_purchase_message;
        }
        else {
            json_response_body["result"] = failure_purchase_message;
        }

        //get string representation of the json body
        string response_body = json_response_body.dump();

        response.set_response_http_code(200);
        response.set_response_body(response_body);
        response.set_response_length(response_body.size());

        return response;
    };

    webserver::web_handler get_goods_get_request("/", "GET", put_goods);
    webserver::web_handler get_goods_post_request("/", "POST", put_goods);
    webserver::web_handler buy_goods("/buy", "POST", buy_goods_check);

    handlers.emplace_back(get_goods_get_request);
    handlers.emplace_back(get_goods_post_request);
    handlers.emplace_back(buy_goods);

    webserver::web_server server(PORT, handlers);

    server.start();
}