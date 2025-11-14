#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

#include "pqc_ta.h"

#define TA_UUID TA_PQC_UUID

#define TA_FLAGS                    (TA_FLAG_EXEC_DDR)
#define TA_STACK_SIZE               (128 * 1024)
#define TA_DATA_SIZE                (1024 * 1024)

#endif /* USER_TA_HEADER_DEFINES_H */