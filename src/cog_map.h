#ifndef  COG_MAP_H
#define  COG_MAP_H

#include "cog_types.h"

#define COG_MAP_SIZE 255

typedef struct cog_map_elem
{
    int key;
    void* data;
    struct cog_map_elem* next;
} cog_map_elem;

typedef struct cog_map
{
    cog_map_elem* elems[COG_MAP_SIZE];
} cog_map;

void cog_map_init(cog_map*);
void* cog_map_get(cog_map*, cog_uint);
void cog_map_put(cog_map*, cog_uint, void*);

#endif   // COG_MAP_H
