cmake_minimum_required(VERSION 3.26)
project(nonBlockingSocketClient C)

set(CMAKE_C_STANDARD 23)

add_executable(nonBlockingSocketClient main.c)


target_link_libraries(nonBlockingSocketClient Ws2_32)