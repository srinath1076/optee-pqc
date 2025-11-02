#include <stdio.h>
#include <tee_client_api.h>
#include "pqc_host.h"

int main(void) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Result res;
    TEEC_UUID uuid = TA_PQC_UUID;

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &res);
    if (res != TEEC_SUCCESS) {
        printf("Session open failed: 0x%x\n", res);
        return 1;
    }

    printf("[+] Running PQC TA self-test...\n");
    res = TEEC_InvokeCommand(&sess, CMD_TEST_ALL, NULL, NULL);
    printf(res == TEEC_SUCCESS ? "[OK] PQC tests passed\n" : "[FAIL] PQC tests failed\n");

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}