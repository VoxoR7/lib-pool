#include <stdio.h>

#include "pool.h"

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

    exit(EXIT_SUCCESS);
}
