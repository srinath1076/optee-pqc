cmake_minimum_required(VERSION 3.10)
project(pqc_ta C)

# --- Directories ---
set(TA_DEV_KIT_DIR $ENV{HOME}/optee/optee_os/out/arm/export-ta_arm64)
set(OQS_DIR ${CMAKE_SOURCE_DIR}/../liboqs)
set(OQS_LIB ${OQS_DIR}/liboqs.a)

# --- Include paths ---
include_directories(
    include
    ${TA_DEV_KIT_DIR}/include
    ${TA_DEV_KIT_DIR}/../src
    ${OQS_DIR}/include
)

# --- Compiler and linker flags ---
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -I${TA_DEV_KIT_DIR}/include")

# --- TA Build ---
add_library(pqc_ta SHARED pqc_ta.c)
set_target_properties(pqc_ta PROPERTIES OUTPUT_NAME "pqc_ta")

# --- Output ---
target_link_libraries(pqc_ta ${OQS_LIB})
