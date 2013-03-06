#ifndef  COG_CORE_H
#define  COG_CORE_H

#include "cog_types.h"

#define COG_MAX_FILE_BUF 4080

#define COG_STRUCT_MALLOC(type) (type*)cog_malloc(sizeof(type))

//#memory management
void cog_free(void *ptr);
void *cog_malloc(cog_uint size);
cog_int cog_memcmp(cog_dataptr s1, cog_dataptr s2, cog_uint size);
void *cog_memcpy(cog_dataptr destination, cog_dataptr source, cog_uint size);

#endif // COG_CORE_H
