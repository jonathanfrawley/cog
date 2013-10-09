#include "cog_map.h"

#include "cog_core.h"

void* cog_map_get_recurse(cog_map_elem* elem, uint32_t key);
void cog_map_put_recurse(cog_map_elem* elem, uint32_t key, void* data);
void cog_map_remove_recurse(cog_map_elem* elem, uint32_t key);

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
    return cog_map_get_recurse(elem, key);
}

void* cog_map_get_recurse(cog_map_elem* elem, uint32_t key) {
    if(elem == COG_NULL) {
        return COG_NULL;
    } else {
        if(elem->key == key) {
            return elem->data;
        } else {
            if(elem->next == COG_NULL) {
                return 0;
            }
            return cog_map_get_recurse(elem->next, key);
        }
    }
}

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
        //Find element in linked list bucket.
        cog_map_put_recurse(elem, key, data);
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
            cog_map_put_recurse(elem->next, key, data);
        }
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
