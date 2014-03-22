#include "cog_map.h"

#include "cog_core.h"

#include <sys/time.h>

void cog_map_put_recurse(cog_map_elem* elem, uint32_t key, void* data);
void cog_map_remove_recurse(cog_map_elem* elem, uint32_t key);
uint32_t cog_map_hash(const char* key);

static uint32_t get_cnt;
static uint32_t cog_map_counter;
static double total_get_time;

void cog_map_init(cog_map* map) {
    int i;
    //dynamically allocate pointers
    map->elems =
        (cog_map_elem**)(cog_malloc(sizeof(cog_map_elem*) * COG_MAP_SIZE));
    for(i = 0; i < COG_MAP_SIZE; i++) {
        map->elems[i] = COG_NULL;
    }
}

void* cog_map_get(cog_map* map, uint32_t key) {
    uint32_t idx = key % COG_MAP_SIZE;  //TODO:Improve.
    cog_map_elem* elem = map->elems[idx];
    while(elem != COG_NULL) {
        if(elem->key == key) {
            return elem->data;
        }
        elem = elem->next;
    }
    return COG_NULL;
}

static uint32_t collision_cnt;

void cog_map_put(cog_map* map, uint32_t key, void* data) {
    uint32_t idx = key % COG_MAP_SIZE;  //TODO:Improve.
    cog_map_elem* elem = map->elems[idx];
    if(elem == COG_NULL) {
        //First elem in linked list
        cog_map_elem* newelem =
            (cog_map_elem*) cog_malloc(sizeof(cog_map_elem));
        newelem->key = key;
        newelem->data = data;
        newelem->next = 0;
        map->elems[idx] = newelem;
    } else {
        collision_cnt = 0;
        //Find element in linked list bucket.
        cog_map_put_recurse(elem, key, data);
    }
}

void cog_map_remove(cog_map* map, uint32_t key) {
    uint32_t idx = key % COG_MAP_SIZE;  //TODO:Improve.
    cog_map_elem* elem = map->elems[idx];
    if(elem == COG_NULL) {
        //Work done
        return;
    } else {
        //Find element in linked list bucket.
        cog_map_remove_recurse(elem, key);
    }
}

void cog_map_put_hash(cog_map* map, const char* key, void* data) {
    return cog_map_put(map, cog_map_hash(key), data);
}

void* cog_map_get_hash(cog_map* map, const char* key) {
    return cog_map_get(map, cog_map_hash(key));
}
//Private Functions

void cog_map_remove_recurse(cog_map_elem* elem, uint32_t key) {
    if(elem->key == key) {
        elem->key = 0;
        elem = COG_NULL;
        //TODO:Maybe free here too?
        return;
    } else {
        if(elem->next == COG_NULL) {
            //done
            return;
        } else {
            cog_map_remove_recurse(elem->next, key);
        }
    }
}

void cog_map_put_recurse(cog_map_elem* elem, uint32_t key, void* data) {
    //XXX:Problem exists here.
    if(elem->key == key) {
        //Nothing to do.
        return;
    } else {
        if(elem->next == COG_NULL) {
            //elem->next = (cog_map_elem*)cog_malloc(sizeof(cog_map_elem));
            elem->next = COG_STRUCT_MALLOC(cog_map_elem);
            elem->next->key = key;
            elem->next->data = data;
            elem->next->next = COG_NULL;
            return;
        } else {
            collision_cnt++;
            cog_map_put_recurse(elem->next, key, data);
        }
    }
}

uint32_t cog_map_hash(const char* key) {
    uint32_t hash = 5381;
    int c;
    while(c = *key++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}


void cog_map_reset_counter() {
    cog_map_counter = 0;
}

uint32_t cog_map_get_counter() {
    return cog_map_counter;
}

void cog_map_reset_timer() {
    total_get_time = 0;
}

double cog_map_get_timer() {
    return total_get_time;
}
