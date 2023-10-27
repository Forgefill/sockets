cmake_minimum_required(VERSION 3.26)
project(unixSocketClient_c C)

set(CMAKE_C_STANDARD 23)

add_executable(unixSocketClient_c main.c)

target_link_libraries(unixSocketClient_c Ws2_32)