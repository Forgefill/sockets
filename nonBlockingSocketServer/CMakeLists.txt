cmake_minimum_required(VERSION 3.26)
project(nonBlockingSocketServer C)

set(CMAKE_C_STANDARD 23)

add_executable(nonBlockingSocketServer main.c)

target_link_libraries(nonBlockingSocketServer Ws2_32)