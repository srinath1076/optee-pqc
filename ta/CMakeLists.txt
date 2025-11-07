cmake_minimum_required(VERSION 3.10)
project(pqc_ta C)

# --- OP-TEE TA dev kit path ---
set(TA_DEV_KIT_DIR $ENV{HOME}/optee/optee_os/out/export-ta_arm64)

# --- liboqs static lib path ---
set(OQS_DIR ${CMAKE_SOURCE_DIR}/../liboqs)
set(OQS_LIB ${OQS_DIR}/liboqs.a)

# --- Include paths ---
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${TA_DEV_KIT_DIR}/include
    ${TA_DEV_KIT_DIR}/../src
    ${OQS_DIR}/include
)

# --- TA build rules ---
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -I${TA_DEV_KIT_DIR}/include")

# --- OP-TEE TA specific build step ---
set(UUID b6c53aba-9669-4668-a7f2-205629d00f86)
set(TA_SRC pqc_ta.c)

add_custom_target(pqc_ta ALL
    COMMAND make -f ${TA_DEV_KIT_DIR}/mk/ta_dev_kit.mk O=${CMAKE_BINARY_DIR}
        TA_DEV_KIT_DIR=${TA_DEV_KIT_DIR}
        TA_SRC_DIR=${CMAKE_SOURCE_DIR}
        CROSS_COMPILE=aarch64-linux-gnu-
        BINARY=${UUID}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)