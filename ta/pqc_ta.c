#include <string.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "pqc_ta.h"
#include "oqs/oqs.h"

/*
 * Hook liboqs RNG to OP-TEE's TEE_GenerateRandom()
 */
static void oqs_randombytes_tee(uint8_t *buf, size_t len) {
    TEE_GenerateRandom(buf, (uint32_t)len);
}

static TEE_Result init_oqs_rng(void) {
    IMSG("PQC TA: installing custom OQS RNG backed by TEE_GenerateRandom");
    OQS_randombytes_custom_algorithm(oqs_randombytes_tee);
    return TEE_SUCCESS;
}

/* ----------------- Tests ----------------- */

static TEE_Result test_ml_kem(void) {
    IMSG("PQC TA: test_ml_kem() start");

    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_768);
    if (!kem) {
        IMSG("PQC TA: OQS_KEM_new(ml_kem_768) failed");
        return TEE_ERROR_GENERIC;
    }

    uint8_t *pk = TEE_Malloc(OQS_KEM_ml_kem_768_length_public_key, 0);
    uint8_t *sk = TEE_Malloc(OQS_KEM_ml_kem_768_length_secret_key, 0);
    uint8_t *ct = TEE_Malloc(OQS_KEM_ml_kem_768_length_ciphertext, 0);
    uint8_t *ss_enc = TEE_Malloc(OQS_KEM_ml_kem_768_length_shared_secret, 0);
    uint8_t *ss_dec = TEE_Malloc(OQS_KEM_ml_kem_768_length_shared_secret, 0);

    if (!pk || !sk || !ct || !ss_enc || !ss_dec) {
        IMSG("PQC TA: malloc failed in ML-KEM test");
        goto cleanup;
    }

    if (OQS_KEM_keypair(kem, pk, sk) != OQS_SUCCESS) {
        IMSG("PQC TA: OQS_KEM_keypair failed");
        goto cleanup;
    }

    if (OQS_KEM_encaps(kem, ct, ss_enc, pk) != OQS_SUCCESS) {
        IMSG("PQC TA: OQS_KEM_encaps failed");
        goto cleanup;
    }

    if (OQS_KEM_decaps(kem, ss_dec, ct, sk) != OQS_SUCCESS) {
        IMSG("PQC TA: OQS_KEM_decaps failed");
        goto cleanup;
    }

    if (memcmp(ss_enc, ss_dec, kem->length_shared_secret) != 0) {
        IMSG("PQC TA: ML-KEM shared secrets mismatch");
        goto cleanup;
    }

    IMSG("PQC TA: ML-KEM-768 SUCCESS");
    TEE_Free(pk); TEE_Free(sk); TEE_Free(ct); TEE_Free(ss_enc); TEE_Free(ss_dec);
    OQS_KEM_free(kem);
    return TEE_SUCCESS;

cleanup:
    if (pk) TEE_Free(pk);
    if (sk) TEE_Free(sk);
    if (ct) TEE_Free(ct);
    if (ss_enc) TEE_Free(ss_enc);
    if (ss_dec) TEE_Free(ss_dec);
    if (kem) OQS_KEM_free(kem);
    return TEE_ERROR_GENERIC;
}

static TEE_Result test_ml_dsa(void) {
    IMSG("PQC TA: test_ml_dsa() start");

    OQS_SIG *sig = OQS_SIG_new(OQS_SIG_alg_ml_dsa_65);
    if (!sig) {
        IMSG("PQC TA: OQS_SIG_new(ml_dsa_65) failed");
        return TEE_ERROR_GENERIC;
    }

    uint8_t msg[] = "Test message for ML-DSA";
    uint8_t *pk = TEE_Malloc(OQS_SIG_ml_dsa_65_length_public_key, 0);
    uint8_t *sk = TEE_Malloc(OQS_SIG_ml_dsa_65_length_secret_key, 0);
    uint8_t *signature = TEE_Malloc(OQS_SIG_ml_dsa_65_length_signature, 0);
    size_t siglen = OQS_SIG_ml_dsa_65_length_signature;

    if (!pk || !sk || !signature) {
        IMSG("PQC TA: malloc failed in ML-DSA test");
        goto cleanup;
    }

    if (OQS_SIG_keypair(sig, pk, sk) != OQS_SUCCESS) goto cleanup;
    if (OQS_SIG_sign(sig, signature, &siglen, msg, sizeof(msg), sk) != OQS_SUCCESS) goto cleanup;

    OQS_STATUS ok = OQS_SIG_verify(sig, msg, sizeof(msg), signature, siglen, pk);

    TEE_Free(pk); TEE_Free(sk); TEE_Free(signature);
    OQS_SIG_free(sig);

    if (ok == OQS_SUCCESS) {
        IMSG("PQC TA: ML-DSA-65 SUCCESS");
        return TEE_SUCCESS;
    }

    IMSG("PQC TA: ML-DSA-65 verify failed");
    return TEE_ERROR_BAD_STATE;

cleanup:
    if (pk) TEE_Free(pk);
    if (sk) TEE_Free(sk);
    if (signature) TEE_Free(signature);
    if (sig) OQS_SIG_free(sig);
    return TEE_ERROR_GENERIC;
}

static TEE_Result test_sphincs(void) {
    IMSG("PQC TA: test_sphincs() start");

    OQS_SIG *sig = OQS_SIG_new(OQS_SIG_alg_sphincs_sha2_128f_simple);
    if (!sig) {
        IMSG("PQC TA: OQS_SIG_new(sphincs) failed (maybe disabled)");
        return TEE_ERROR_NOT_SUPPORTED;
    }

    uint8_t msg[] = "SPHINCS+ Stateless Hash Test";
    uint8_t *pk = TEE_Malloc(OQS_SIG_sphincs_sha2_128f_simple_length_public_key, 0);
    uint8_t *sk = TEE_Malloc(OQS_SIG_sphincs_sha2_128f_simple_length_secret_key, 0);
    uint8_t *signature = TEE_Malloc(OQS_SIG_sphincs_sha2_128f_simple_length_signature, 0);
    size_t siglen = OQS_SIG_sphincs_sha2_128f_simple_length_signature;

    if (!pk || !sk || !signature) {
        IMSG("PQC TA: malloc failed in SPHINCS test");
        goto cleanup;
    }

    if (OQS_SIG_keypair(sig, pk, sk) != OQS_SUCCESS) goto cleanup;
    if (OQS_SIG_sign(sig, signature, &siglen, msg, sizeof(msg), sk) != OQS_SUCCESS) goto cleanup;

    OQS_STATUS ok = OQS_SIG_verify(sig, msg, sizeof(msg), signature, siglen, pk);

    TEE_Free(pk); TEE_Free(sk); TEE_Free(signature);
    OQS_SIG_free(sig);

    if (ok == OQS_SUCCESS) {
        IMSG("PQC TA: SPHINCS+ SUCCESS");
        return TEE_SUCCESS;
    }

    IMSG("PQC TA: SPHINCS+ verify failed");
    return TEE_ERROR_BAD_STATE;

cleanup:
    if (pk) TEE_Free(pk);
    if (sk) TEE_Free(sk);
    if (signature) TEE_Free(signature);
    if (sig) OQS_SIG_free(sig);
    return TEE_ERROR_GENERIC;
}

/* --------------- TA lifecycle --------------- */

TEE_Result TA_CreateEntryPoint(void) {
    IMSG("PQC TA: TA_CreateEntryPoint");
    return init_oqs_rng();
}

void TA_DestroyEntryPoint(void) {
    IMSG("PQC TA: TA_DestroyEntryPoint");
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t ptype, TEE_Param params[4], void **ctx) {
    (void)ptype; (void)params; (void)ctx;
    IMSG("PQC TA: TA_OpenSessionEntryPoint");
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *ctx) {
    (void)ctx;
    IMSG("PQC TA: TA_CloseSessionEntryPoint");
}

/* --------------- Command dispatcher --------------- */

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id,
                                      uint32_t param_types, TEE_Param params[4]) {
    (void)sess_ctx;
    (void)param_types;
    (void)params;

    IMSG("PQC TA: TA_InvokeCommandEntryPoint(cmd_id=%u)", cmd_id);

    switch (cmd_id) {
    case CMD_TEST_ML_KEM:
        return test_ml_kem();
    case CMD_TEST_ML_DSA:
        return test_ml_dsa();
    case CMD_TEST_SPHINCS:
        return test_sphincs();
    case CMD_TEST_ALL:
        if (test_ml_kem() != TEE_SUCCESS)   return TEE_ERROR_GENERIC;
        if (test_ml_dsa() != TEE_SUCCESS)   return TEE_ERROR_GENERIC;
        if (test_sphincs() != TEE_SUCCESS)  return TEE_ERROR_GENERIC;
        return TEE_SUCCESS;
    default:
        IMSG("PQC TA: unknown cmd_id=%u", cmd_id);
        return TEE_ERROR_BAD_PARAMETERS;
    }
}