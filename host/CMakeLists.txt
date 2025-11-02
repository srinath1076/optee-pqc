cmake_minimum_required(VERSION 3.10)
project(pqc_host)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Iinclude -I../ta/include -I../liboqs/include")
add_executable(pqc_host pqc_host.c)
target_link_libraries(pqc_host teec)