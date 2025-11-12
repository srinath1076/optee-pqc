#include <stdio.h>
#include <tee_client_api.h>
#include "pqc_host.h"

int main(void) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Result res;
    uint32_t origin;
    TEEC_UUID uuid = TA_PQC_UUID;

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("InitializeContext failed: 0x%x\n", res);
        return 1;
    }

    printf("[*] Opening session to PQC TA...\n");
    res = TEEC_OpenSession(&ctx, &sess, &uuid,
                           TEEC_LOGIN_PUBLIC,
                           NULL, NULL, &origin);
    if (res != TEEC_SUCCESS) {
        printf("Session open failed: res=0x%x origin=0x%x\n", res, origin);
        TEEC_FinalizeContext(&ctx);
        return 1;
    }

    printf("[+] Running PQC TA TEST ML KEM...\n");
    res = TEEC_InvokeCommand(&sess, CMD_TEST_ML_KEM, NULL, &origin);
    if (res == TEEC_SUCCESS)
        printf("[OK] PQC tests passed\n");
    else
        printf("[FAIL] PQC tests failed: res=0x%x origin=0x%x\n", res, origin);

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}