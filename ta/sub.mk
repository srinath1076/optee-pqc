# OP-TEE PQC TA build description
# This file is included by ta_dev_kit.mk

# --- Debug print for context ------------------------------------
$(info ===== OP-TEE PQC TA build started =====)
$(info TA_SRC_DIR=$(TA_SRC_DIR))
$(info TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR))
$(info O=$(O))
$(info BINARY=$(BINARY))
$(info =======================================)

# --- TA UUID ----------------------------------------------------
TA_INCLUDE_PATHS += $(TA_SRC_DIR)/include
global-incdirs += include $(TA_SRC_DIR)/include
srcs-y += pqc_ta.c \
           oqs_align.c

# --- Include + Library Paths -----------------------------------
cflags-y += -I$(TA_DEV_KIT_DIR)/include \
            -I$(TA_SRC_DIR)/include \
            -I$(TA_SRC_DIR)/../liboqs/include
libdirs += $(TA_SRC_DIR)/../liboqs
libnames += oqs

# --- Enable verbose TA logging ---------------------------------
# TRACE_LEVEL_DEBUG = 4 → prints IMSG/DMSG/EMSG
#cflags-y += -DCFG_TEE_TA_LOG_LEVEL=4
# Optional: enable heap guards for catching bad frees/overruns
cflags-y += -DCFG_TA_MALLOC_DEBUG=1

# --- Optional: extra safety during bring-up --------------------
# Avoid symbol stripping during debugging
cflags-y += -g