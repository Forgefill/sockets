cmake_minimum_required(VERSION 3.26)
project(asyncSocketServer C)

set(CMAKE_C_STANDARD 23)

add_executable(asyncSocketServer main.c)

target_link_libraries(asyncSocketServer Ws2_32)