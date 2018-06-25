#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "request_param.h"
#include "http_request.h"
#include "http_header.h"
#include "http_request_parser.h"
#include "http_response.h"
#include "tcp_server.h"
#include "web_handler.h"
#include "http_router.h"
#include "http_response_builder.h"
#include <map>
#include <regex>

using namespace std;
//Реализация простого веб-сервера.
//
//Каждое соединенеи должно обрабатываться в отдельном thread (потоке).
//Соединения на входе принимает отдельный, выделенный поток (thread), и передаёт исполнение созданному специально для этого запроса потоку.
//
//Веб-сервер поддерживает описание Роутов (routes) с помощью лямбда-функций.
//Веб-сервер сам инициализирует http_request объект и передаёт исполнение в лямбду.
//
//Веб-сервер должен выдерживать 30 одновременных соединений.

namespace webserver {
    class web_server {
    private:
        unsigned int allowed_connections_number = 30;

        vector<web_handler> handlers;

        http_router request_handler_router;

        tcp_server server;

        http_request_parser parser;

        http_response_builder response_builder;

        function<bool(string)> check_is_full_message = [](const string& client_message) -> bool {
            map<string, string> headers;

            unsigned long index_start_of_headers = client_message.find("\r\n") + 2;
            unsigned long index_end_of_headers = client_message.find("\r\n\r\n");

            if (index_end_of_headers != string::npos) {
                index_end_of_headers += 4;

                regex rx("[^\r\n]+\r\n");
                sregex_iterator formated_headers_list(client_message.begin() + index_start_of_headers, client_message.begin() + index_end_of_headers, rx), rxend;

                while(formated_headers_list != rxend) {
                    string current_header = formated_headers_list->str();

                    unsigned long index_end_of_header_type = current_header.find(':');
                    unsigned long index_start_of_header_value = index_end_of_header_type + 1;

                    while (index_start_of_header_value == ' ') {
                        index_start_of_header_value++;
                    }

                    string header_type = current_header.substr(0, index_end_of_header_type);
                    string header_value = current_header.substr(index_start_of_header_value);

                    //remove "\r\n" from the end of header value
                    header_value.pop_back();
                    header_value.pop_back();

                    for (auto& current_char : header_type) {
                        current_char = tolower(current_char);
                    }

                    headers.emplace(header_type, header_value);
                    formated_headers_list++;
                }

                if (!headers["content-length"].empty()) {
                    unsigned long body_size = stol(headers["content-length"]);

                    if (body_size != 0) {
                        if (client_message.size() - index_end_of_headers != body_size) {
                            return false;
                        }
                    }
                }
                return true;
            }
            return false;
        };

        function<string(string)> convert_client_message = [&](string raw_client_message) -> string {
            vector<string> message_fields;
            string buffer;

            //add "\r\n" to the end for correct spliting of the client message
            raw_client_message += "\r\n";

            regex rx("[^\r\n]+\r\n|\r\n");
            sregex_iterator formated_body_list(raw_client_message.begin(), raw_client_message.end(), rx), rxend;

            while(formated_body_list != rxend) {
                const string& current_line = formated_body_list->str();
                message_fields.emplace_back(current_line.substr(0));
                ++formated_body_list;
            }

            http_request request = parser.parse_request(message_fields);

            http_response response;

            web_handler suitable_web_handler = request_handler_router.get_suitable_request_handler(handlers, request);
            const function<http_response(http_request)>& handler = suitable_web_handler.get_transform_to_response_function();

            response = handler(request);

            string server_response = response_builder.build_response(response);
            return server_response;
        };
    public:
        web_server(unsigned short int port, const vector<web_handler>& handlers);

        //Запуск web-server.
        //Функция должна блокировать поток, в котором она была запущена, чтобы веб-сервер не прекращал работу мгновенно.
        void start();

        void stop();
    };
}

#endif //WEB_SERVER_H