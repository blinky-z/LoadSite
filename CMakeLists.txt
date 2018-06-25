cmake_minimum_required(VERSION 3.9)
project(LoadSite)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -lpthread")

add_executable(LoadSite main.cpp)
target_link_libraries(LoadSite /home/twite/CLionProjects/LoadSite/libhttpserver.a)