#ifndef  COG_CORE_H
#define  COG_CORE_H

#include "cog_types.h"

#define COG_MAX_BUF 255
#define COG_MAX_FILE_BUF 4080

#define COG_STRUCT_MALLOC(type) (type*)cog_malloc(sizeof(type))

//#logging
void cog_errorf(const char* logMsg, ...);
void cog_debugf(const char* logMsg, ...);
void cog_infof(const char* logMsg, ...);

//#memory management
void* cog_malloc(cog_uint size);
void cog_free(void* ptr);
void* cog_memcpy(cog_dataptr destination, cog_dataptr source, cog_uint size);
cog_int cog_memcmp(cog_dataptr s1, cog_dataptr s2, cog_uint size);

#endif   // COG_CORE_H
