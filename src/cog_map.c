#include "cog_map.h"

#include "cog_core.h"

void* cog_map_get_recurse(cog_map_elem* elem, cog_uint key);
void cog_map_put_recurse(cog_map_elem* elem, cog_uint key, void* data);

void cog_map_init(cog_map* map)
{
    int i;
    for(i=0;i<COG_MAP_SIZE;i++)
    {
        map->elems[i] = 0;
    }
}

void* cog_map_get(cog_map* map, cog_uint key)
{
    cog_uint idx = key % COG_MAP_SIZE; //TODO:Improve.
    cog_map_elem* elem = map->elems[idx];
    return cog_map_get_recurse(elem, key);
}

void* cog_map_get_recurse(cog_map_elem* elem, cog_uint key)
{
    if(elem==0)
    {
        return 0;
    }
    else
    {
        if(elem->key == key)
        {
            return elem->data;
        }
        else
        {
            if(elem->next == 0)
            {
                return 0;
            }
            return cog_map_get_recurse(elem->next, key);
        }
    }
}

void cog_map_put(cog_map* map, cog_uint key, void* data)
{
    cog_uint idx = key % COG_MAP_SIZE; //TODO:Improve.
    cog_map_elem* elem = map->elems[idx];
    if(elem==0)
    {
        //First elem in linked list
        cog_map_elem* newelem = (cog_map_elem*)cog_malloc(sizeof(cog_map_elem));
        newelem->key = key;
        newelem->data = data;
        newelem->next = 0;
        map->elems[idx] = newelem;
    }
    else
    {
        cog_map_put_recurse(elem,key,data);
    }
}

void cog_map_put_recurse(cog_map_elem* elem, cog_uint key, void* data)
{
    if(elem->key == key)
    {
        //Nothing to do.
        return;
    }
    else
    {
        if(elem->next==0)
        {
            elem->next = (cog_map_elem*)cog_malloc(sizeof(cog_map_elem));
            elem->next->key = key;
            elem->next->data = data;
            elem->next->next = 0;
            return;
        }
        else
        {
            cog_map_put_recurse(elem->next, key, data);
        }
    }
}
