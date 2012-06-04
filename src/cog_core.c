#include "cog_core.h"

#include <stdlib.h>
#include <assert.h>

void* cog_malloc(cog_uint size)
{
    void* ptr = (void*)malloc(size);
    if(ptr == 0)
    {
        assert(0 && "dynamic memory allocation failed");
        return 0;
    }
    return ptr;
}

void cog_free(void* ptr)
{
    free(ptr);
}
