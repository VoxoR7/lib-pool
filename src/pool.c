#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "pool.h"

#define POOL_SIZE_START_POWER 6UL
#define POOL_SIZE_START (1 << POOL_SIZE_START_POWER)
#define POOL_TYPE_NUMBER 5

#define POOL_NUMBER 100000

#if (POOL_SIZE_START == 0) || ((POOL_SIZE_START & (POOL_SIZE_START - 1)) != 0)
    #error "POOL_SIZE_START must be a power of 2"
#endif

#if (POOL_NUMBER >= UINT32_MAX)
    #error "POOL_NUMBER must be lower than UINT32_MAX"
#endif

#define NO_MORE_SPACE UINT32_MAX

struct pool_s {
    void *beggining;
    struct pool_s *next_pool;
    uint32_t link[POOL_NUMBER];
    uint32_t first_free;

    #ifdef POOL_STATS
        uint32_t used;
    #endif
};

struct pool_info_s {
    uint32_t elem_size;
    struct pool_s *first_pool;
};

struct pool_info_s *pools = NULL;

static void pool_exit(void) {
    printf("[INFO ] pool_exit ...\n");

    for (uint16_t i = 0; i < POOL_TYPE_NUMBER; i++) {
        if (pools[i].first_pool != NULL) {

            printf("[INFO ] freeing beggining %p\n", pools[i].first_pool->beggining);
            free(pools[i].first_pool->beggining);
            printf("[INFO ] freeing pool %p\n", pools[i].first_pool);
            free(pools[i].first_pool);
        }
    }

    free(pools);

    printf("[INFO ] OK\n");
}

void pool_init(void) {
    if (pools != NULL) {
        fprintf(stderr, "[FATAL] pool already initialized !\n");
        fprintf(stderr, "[FATAL] aborting\n");
        exit(EXIT_FAILURE);
    }

    printf("[INFO ] pool_init ...\n");

    atexit(pool_exit);

    pools = malloc(sizeof(struct pool_info_s) * POOL_TYPE_NUMBER);
    if (pools == NULL) {
        exit(EXIT_FAILURE);
    }

    for (uint16_t i = 0, pow = 1; i < POOL_TYPE_NUMBER; i++, pow *= 2) {
        pools[i].elem_size = POOL_SIZE_START * pow;
        pools[i].first_pool = malloc(sizeof(struct pool_s));
        if (pools[i].first_pool == NULL) {
            exit(EXIT_FAILURE);
        }
        pools[i].first_pool->beggining = malloc((POOL_SIZE_START * pow) * POOL_NUMBER);
        if (pools[i].first_pool->beggining == NULL) {
            exit(EXIT_FAILURE);
        }

        printf("[INFO ] pool %d, beggining : %p\n",(POOL_SIZE_START * pow) ,pools[i].first_pool->beggining);

        for (uint32_t j = 0; j < POOL_NUMBER; j++) {
            pools[i].first_pool->link[j] = j + 1;
        }

        pools[i].first_pool->link[POOL_NUMBER - 1] = NO_MORE_SPACE;
        pools[i].first_pool->first_free = 0;
        pools[i].first_pool->next_pool = NULL;
        #ifdef POOL_STATS
            pools[i].first_pool->used = 0;
        #endif
    }

    printf("[INFO ] OK\n");
}

void *pool_malloc(size_t size) {
    for (uint32_t i = 0, pow = 1; i < POOL_TYPE_NUMBER; i++, pow *= 2) {
        if (size < POOL_SIZE_START * pow) {
            #ifdef POOL_STATS
                pools[i].first_pool->used++;
            #endif
            uint32_t indice_return = pools[i].first_pool->first_free;
            pools[i].first_pool->first_free = pools[i].first_pool->link[indice_return];
            void *ptr = pools[i].first_pool->beggining + (indice_return * (pow * POOL_SIZE_START));
            printf("[INFO ] malloc pool %d, indice [%d] %p %lu\n", (pow * POOL_SIZE_START), indice_return, ptr, size);
            return ptr;
        }
    }

    void *ptr = malloc(size);
    printf("[INFO ] malloc %p %lu\n", ptr, size);
    return ptr;
}

void pool_free(void *ptr) {
    for (uint32_t i = 0, pow = 1; i < POOL_TYPE_NUMBER; i++, pow *= 2) {
        if (ptr >= pools[i].first_pool->beggining && 
                ptr < pools[i].first_pool->beggining + (POOL_NUMBER * (pow * POOL_SIZE_START))) {
            #ifdef POOL_STATS
                pools[i].first_pool->used--;
            #endif
            uint32_t indice_free = (ptr - pools[i].first_pool->beggining) / (pow * POOL_SIZE_START);
            pools[i].first_pool->link[indice_free] = pools[i].first_pool->first_free;
            printf("[INFO ] free  pool %d, indice [%d] %p\n", (pow * POOL_SIZE_START), indice_free, ptr);
            return;
        }
    }

    printf("[INFO ] free ptr %p\n", ptr);
    free(ptr);
}

void *pool_calloc(size_t nmemb, size_t size) {
    /* TODO : check for overflow */
    size_t malloc_size = nmemb * size;

    void *ptr = pool_malloc(malloc_size);
    if (ptr == NULL)
        return NULL;

    for (size_t i = 0; i < malloc_size; i++)
        ((uint8_t *)ptr)[i] = 0;

    return ptr;
}

void *pool_realloc(void *ptr, size_t size) {
    if (ptr == NULL) {
        return pool_malloc(size);
    }

    if (size == 0) {
        pool_free(ptr);
        return NULL;
    }

    uint32_t old, old_pool_size, new, pow;
    for (old = 0, pow = 1; old < POOL_TYPE_NUMBER; old++, pow *= 2) {
        if (ptr >= pools[old].first_pool->beggining && 
                ptr < pools[old].first_pool->beggining + (POOL_NUMBER * (pow * POOL_SIZE_START))) {
            old_pool_size = pow * POOL_SIZE_START;
            break;
        }
    }

    if (old == POOL_TYPE_NUMBER) {
        return realloc(ptr, size);
    }

    for (new = 0, pow = 1; new < POOL_TYPE_NUMBER; new++, pow *= 2) {
        if (size < POOL_SIZE_START * pow) {
            break;
        }
    }

    if (new == old) {
        return ptr;
    }

    if (new == POOL_TYPE_NUMBER) {
        void *new_ptr = malloc(size);
        memcpy(new_ptr, ptr, old_pool_size);
        free(ptr);
        return new_ptr;
    }

    if (new < old) {
        #ifdef REALLOC_MEMORY_GAIN
            return NULL;
        #endif
        return ptr;
    }

    return NULL;
}
