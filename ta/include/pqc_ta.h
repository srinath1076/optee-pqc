#ifndef PQC_TA_H
#define PQC_TA_H

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

/* UUID for this TA */
#define TA_PQC_UUID \
    { 0x12345678, 0x9abc, 0xdef0, \
      { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 } }

/* Command identifiers */
#define CMD_TEST_ML_KEM     0x0001
#define CMD_TEST_ML_DSA     0x0002
#define CMD_TEST_SPHINCS    0x0003
#define CMD_TEST_ALL        0x00FF
#endif /* PQC_TA_H */