cmake_minimum_required(VERSION 3.10)
project(pqc_ta)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -Iinclude")

# Point to liboqs static library and headers
set(OQS_DIR ${CMAKE_SOURCE_DIR}/../liboqs)
set(OQS_LIB ${OQS_DIR}/liboqs.a)
include_directories(${OQS_DIR}/include)

add_library(pqc_ta SHARED pqc_ta.c)
target_link_libraries(pqc_ta ${OQS_LIB})