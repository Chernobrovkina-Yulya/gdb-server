cmake_minimum_required(VERSION 3.0)

project(gdb_server)

add_executable(server src/tcp-connection.cpp src/main.cpp src/packet.cpp src/request.cpp src/utility.cpp)

add_library(uemu_v8 SHARED IMPORTED)

set_property(TARGET uemu_v8 PROPERTY IMPORTED_LOCATION ../libuemu_v8.so)

target_link_libraries(server  uemu_v8)