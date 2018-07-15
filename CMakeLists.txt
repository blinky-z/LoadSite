cmake_minimum_required(VERSION 3.9)
project(LoadSite)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pthread")

add_executable(LoadSite
#        server source files
        WebServer/web_server.h WebServer/http_request.h WebServer/http_header.h WebServer/request_param.h
        WebServer/http_request_parser.h WebServer/http_request.cpp WebServer/http_request_parser.cpp WebServer/http_response.cpp
        WebServer/http_response.h WebServer/tcp_server.cpp WebServer/tcp_server.h WebServer/web_handler.cpp WebServer/web_handler.h
        WebServer/web_server.cpp WebServer/http_router.cpp WebServer/http_router.h WebServer/http_response_builder.cpp
        WebServer/http_response_builder.h WebServer/http_parameterized_header_parser.cpp WebServer/http_parameterized_header_parser.h
        WebServer/client.cpp WebServer/client.h
#        run website
        run_website.cpp
        )