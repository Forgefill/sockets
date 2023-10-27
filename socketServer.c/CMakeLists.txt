cmake_minimum_required(VERSION 3.26)
project(socketServer_c C)

set(CMAKE_C_STANDARD 23)

add_executable(socketServer_c main.c)

target_link_libraries(socketServer_c Ws2_32)