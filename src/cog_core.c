#include "cog_core.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>

void cog_errorf(const char* logMsg, ...)
{
    va_list ap;
    va_start(ap, logMsg);
    char buf[COG_MAX_BUF];
    vsprintf(buf, logMsg, ap);
    printf("CRITICAL: %s \n", buf);
    assert(0);
}

void cog_debugf(const char* logMsg, ...)
{
    va_list ap;
    va_start(ap, logMsg);
    char buf[COG_MAX_BUF];
    vsprintf(buf, logMsg, ap);
    printf("DEBUG: %s \n", buf);
}

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
