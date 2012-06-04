#ifndef  COG_CORE_H
#define  COG_CORE_H

#include "cog_types.h"

#define COG_STRUCT_MALLOC(type) (type*)cog_malloc(sizeof(type))

//memory management
void* cog_malloc(cog_uint size);
void cog_free(void* ptr);

#endif   // COG_CORE_H
