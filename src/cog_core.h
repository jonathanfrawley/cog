#ifndef  COG_CORE_H
#define  COG_CORE_H

#include "cog_types.h"

static int COG_MAX_BUF = 255;
static int COG_MAX_FILE_BUF = 4080;

#define COG_STRUCT_MALLOC(type) (type*)cog_malloc(sizeof(type))

//#logging
void cog_errorf(const char* logMsg, ...);
void cog_debugf(const char* logMsg, ...);

//#memory management
void* cog_malloc(cog_uint size);
void cog_free(void* ptr);
void* cog_memcpy(cog_dataptr destination, cog_dataptr source, cog_uint size);

#endif   // COG_CORE_H
