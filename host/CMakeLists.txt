cmake_minimum_required(VERSION 3.10)
project(pqc_host)

# Absolute paths
set(HOST_DIR ${CMAKE_SOURCE_DIR})
set(OPTEE_CLIENT_EXPORT $ENV{HOME}/optee/optee_client/out/export)
set(TA_DEV_KIT_DIR $ENV{HOME}/optee/optee_os/out/export-ta_arm64)

include_directories(
    ${HOST_DIR}
    ${HOST_DIR}/include
    ${HOST_DIR}/../ta/include
    ${HOST_DIR}/../liboqs/include
    ${OPTEE_CLIENT_EXPORT}/usr/include
    ${TA_DEV_KIT_DIR}/include
)
link_directories(${OPTEE_CLIENT_EXPORT}/usr/lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_executable(pqc_host ${HOST_DIR}/pqc_host.c)
target_link_libraries(pqc_host teec)