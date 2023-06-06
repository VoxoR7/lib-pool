#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>

#include "pool.h"

#define STRESS_TEST_CASE 1000000

int main() {
    pool_init();

    printf("starting tests\n");
    printf("malloc attempt ...\n");

    void *ptr1 = pool_malloc(10);
    void *ptr2 = pool_malloc(100);
    void *ptr3 = pool_malloc(1000);
    void *ptr4 = pool_malloc(10000);
    void *ptr5 = pool_malloc(100000);

    printf("all malloc passed !\n");
    printf("free attempt ...\n");

    pool_free(ptr1);
    pool_free(ptr2);
    pool_free(ptr3);
    pool_free(ptr4);
    pool_free(ptr5);

    printf("all free passed !\n");
    printf("malloc attempt ...\n");

    ptr1 = pool_malloc(10);
    ptr2 = pool_malloc(100);
    ptr3 = pool_malloc(1000);
    ptr4 = pool_malloc(10000);
    ptr5 = pool_malloc(100000);

    printf("all malloc passed !\n");
    printf("free attempt ...\n");

    pool_free(ptr1);
    pool_free(ptr2);
    pool_free(ptr3);
    pool_free(ptr4);
    pool_free(ptr5);

    printf("all free passed !\n");

    printf("calloc attempt ...\n");
    ptr1 = pool_calloc(10, 10);
    ptr2 = pool_calloc(SIZE_MAX - 10, 10);
    if (ptr1 == NULL || ptr2 != NULL) {
        fprintf(stderr, "calloc failed\n");
        exit(EXIT_FAILURE);
    }
    printf("all malloc passed !\n");
    printf("free attempt ...\n");
    pool_free(ptr1);
    pool_free(ptr2);
    printf("all free passed !\n");

    printf("stress test\n");
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    
    void *ptr[STRESS_TEST_CASE];
    for (uint64_t i = 0; i < STRESS_TEST_CASE; i++) {
        ptr[i] = pool_malloc(1000);
        if (ptr[i] == NULL) {
            fprintf(stderr, "alloc %lu failed\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (uint64_t i = 0; i < STRESS_TEST_CASE; i++)
        pool_free(ptr[i]);

    gettimeofday(&stop, NULL);
    printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    printf("Exiting in success !\n");

    exit(EXIT_SUCCESS);
}
