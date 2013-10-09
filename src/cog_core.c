#include "cog_core.h"

#include <stdlib.h>
#include <string.h>

#include "cog_log.h"

void cog_free(void* ptr) {
    free(ptr);
}

void* cog_malloc(uint32_t size) {
    void* ptr = (void*) malloc(size);
    if(ptr == 0) {
        cog_errorf("Dynamic memory allocation failed for memory of size %d",
                   size);
        return 0;
    }
    memset(ptr, 0, size); //initialise to zero
    return ptr;
}

int cog_memcmp(cog_dataptr s1, cog_dataptr s2, uint32_t size) {
    return memcmp(s1, s2, size);
}

void* cog_memcpy(cog_dataptr destination, cog_dataptr source, uint32_t size) {
    return memcpy(destination, source, size);
}
