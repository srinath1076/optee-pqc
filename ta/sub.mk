# OP-TEE PQC TA build description
# This file is included by ta_dev_kit.mk

# Debug print
$(info ===== OP-TEE PQC TA build started =====)
$(info TA_SRC_DIR=$(TA_SRC_DIR))
$(info TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR))
$(info O=$(O))
$(info BINARY=$(BINARY))
$(info =======================================)

# TA UUID (must match the one used in host app)
TA_INCLUDE_PATHS += $(TA_SRC_DIR)/include
global-incdirs += include $(TA_SRC_DIR)/include
srcs-y += pqc_ta.c
cflags-y += -I$(TA_DEV_KIT_DIR)/include -I$(TA_SRC_DIR)/include -I$(TA_SRC_DIR)/../liboqs/include
libdirs += $(TA_SRC_DIR)/../liboqs
libnames += oqs