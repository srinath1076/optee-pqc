#include <tee_internal_api.h>
#include <stddef.h>
#include <stdint.h>

/*
 * oqs_align.c
 * Safe alignment helpers for liboqs inside OP-TEE TA.
 * We avoid redefining OP-TEE's malloc/free/aligned_alloc
 * and only provide minimal glue for posix_memalign-style allocation.
 */

/* Align up helper */
static void *align_up(void *p, size_t a) {
    uintptr_t u = (uintptr_t)p;
    uintptr_t aligned = (u + (a - 1)) & ~(uintptr_t)(a - 1);
    return (void *)aligned;
}

/* Provide prototypes explicitly */
int posix_memalign(void **memptr, size_t alignment, size_t size);
void *oqs_aligned_alloc(size_t alignment, size_t size);
void oqs_aligned_free(void *ptr);

/*
 * Minimal posix_memalign implementation using OP-TEE heap
 */
int posix_memalign(void **memptr, size_t alignment, size_t size) {
    if (!memptr || alignment < sizeof(void*) || (alignment & (alignment - 1)))
        return TEE_ERROR_BAD_PARAMETERS;

    size_t total = size + alignment + sizeof(void*);
    void *base = TEE_Malloc(total, 0);
    if (!base)
        return TEE_ERROR_OUT_OF_MEMORY;

    void *aligned = align_up((uint8_t*)base + sizeof(void*), alignment);
    ((void**)aligned)[-1] = base;
    *memptr = aligned;
    return 0;
}

/*
 * Prefixed aligned_alloc wrapper for liboqs internals
 */
void *oqs_aligned_alloc(size_t alignment, size_t size) {
    void *p = NULL;
    if (posix_memalign(&p, alignment, size))
        return NULL;
    return p;
}

/*
 * Free function matching our custom alignment logic
 */
void oqs_aligned_free(void *ptr) {
    if (!ptr)
        return;
    void *base = ((void**)ptr)[-1];
    TEE_Free(base);
}

/*
 * Optional weak aliases to satisfy liboqs references to aligned_alloc/free.
 * They override only if not provided elsewhere.
 */
void *aligned_alloc(size_t alignment, size_t size)
    __attribute__((weak, alias("oqs_aligned_alloc")));

void free(void *ptr)
    __attribute__((weak, alias("oqs_aligned_free")));