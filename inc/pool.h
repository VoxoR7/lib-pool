#include <stdlib.h>

void pool_init(void);

void *pool_malloc(size_t size);
void pool_free(void *ptr);
void *pool_calloc(size_t nmemb, size_t size);
void *pool_realloc(void *ptr, size_t size);
