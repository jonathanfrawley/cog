#ifndef  COG_MAP_H
#define  COG_MAP_H

#include "cog_types.h"

#define COG_MAP_SIZE 8192

typedef struct cog_map_elem {
    int key;
    void* data;
    struct cog_map_elem* next;
} cog_map_elem;

typedef struct cog_map {
    cog_map_elem** elems;
} cog_map;

void cog_map_init(cog_map*);
void* cog_map_get(cog_map*, uint32_t);
void* cog_map_get_hash(cog_map* map, const char* key);
void cog_map_put(cog_map*, uint32_t, void*);
void cog_map_put_hash(cog_map* map, const char* key, void* data);
void cog_map_remove(cog_map*, uint32_t);


//Internal
void cog_map_reset_counter();
uint32_t cog_map_get_counter();
void cog_map_reset_timer();
double cog_map_get_timer();

#endif // COG_MAP_H
