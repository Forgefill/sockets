cmake_minimum_required(VERSION 3.26)
project(socketClient_c C)

set(CMAKE_C_STANDARD 23)

add_executable(socketClient_c main.c)

target_link_libraries(socketClient_c Ws2_32)