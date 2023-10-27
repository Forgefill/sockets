cmake_minimum_required(VERSION 3.26)
project(unixSocketServer C)

set(CMAKE_C_STANDARD 23)

add_executable(unixSocketServer main.c)

target_link_libraries(unixSocketServer Ws2_32)