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

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wall -I${TA_DEV_KIT_DIR}/include")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -fdata-sections -ffunction-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

# --- UUID and sources ---
set(UUID 12345678-9abc-def0-1234-56789abcdef0)
set(TA_SRC pqc_ta.c oqs_align.c)

# --- TA artifact paths ---
set(TA_ELF ${CMAKE_BINARY_DIR}/${UUID}.elf)
set(SIGNED_TA ${CMAKE_BINARY_DIR}/${UUID}.ta)
set(SIGN_DIGEST ${CMAKE_BINARY_DIR}/${UUID}.sig)
set(SIGN_SCRIPT ${TA_DEV_KIT_DIR}/scripts/sign_encrypt.py)
set(SIGN_KEY ${TA_DEV_KIT_DIR}/keys/default_ta.pem)

# --- Build the TA ELF using the dev kit ---
add_custom_target(pqc_ta_elf ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Building TA ELF..."
    COMMAND make -f ${TA_DEV_KIT_DIR}/mk/ta_dev_kit.mk O=${CMAKE_BINARY_DIR}
        TA_DEV_KIT_DIR=${TA_DEV_KIT_DIR}
        TA_SRC_DIR=${CMAKE_SOURCE_DIR}
        CROSS_COMPILE=aarch64-linux-gnu-
        BINARY=${UUID}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# --- Sign the TA ELF ---
add_custom_target(sign_ta ALL
    DEPENDS pqc_ta_elf
    COMMAND ${CMAKE_COMMAND} -E echo "Signing TA: generating digest..."
    COMMAND python3 ${SIGN_SCRIPT} sign-enc
        --uuid ${UUID}
        --key ${SIGN_KEY}
        --in ${TA_ELF}
        --out ${SIGNED_TA}
    COMMENT "Signing and generating final TA"
)

add_dependencies(sign_ta pqc_ta_elf)
add_custom_target(pqc_ta ALL DEPENDS sign_ta)