#include "cog_list.h"

#include "cog_core.h"
#include "cog.h"

cog_list* cog_list_alloc(uint32_t size) {
    cog_list* obj = COG_STRUCT_MALLOC(cog_list);
    cog_list_init(obj, size);
    return obj;
}

void cog_list_init(cog_list* list, uint32_t size) {
    //Initial list has null data and a looped pointer to itself
    list->data = COG_LIST_ENDSENTINAL;
    list->next = list;
    list->size = size;
    list->len = 0;
}

void cog_list_append(cog_list* list, cog_dataptr data) {
    //To append, we need to loop through the whole list.
    cog_list* prev;
    cog_list* curr;
    for(prev = list, curr = list->next;
            curr->data != COG_LIST_ENDSENTINAL;
            curr = curr->next, prev = prev->next) {
        ;
    }
    //Add node between prev and curr
    cog_list* newnode = cog_list_alloc(list->size);
    newnode->next = curr;
    newnode->data = (cog_dataptr) cog_malloc(list->size);
    cog_memcpy(newnode->data, data, list->size);
    prev->next = newnode;
    list->len++;
}

void cog_list_remove(cog_list* list, cog_dataptr data) {
    cog_list* prev;
    cog_list* curr;
    for(prev = list, curr = list->next;
            curr->data != COG_LIST_ENDSENTINAL;
            curr = curr->next, prev = prev->next) {
        if(cog_memcmp(curr->data, data, list->size) == 0) {
            prev->next = curr->next;
            cog_free(curr->data);
            cog_free(curr);
            list->len--;
            break;
        }
    }
}

void cog_list_removeall(cog_list* list) {
    COG_LIST_FOREACH(list) {
        cog_free(curr->data);
        cog_free(curr);
    }
    list->len = 0;
}

uint32_t cog_list_len(cog_list* list) {
    return list->len;
}

cog_dataptr cog_list_pop(cog_list* list) {
    cog_list* curr = list->next;
    if(cog_list_len(list) == 0) {
        return COG_NULL;
    } else {
        //make copy of data
        cog_dataptr result = cog_malloc(list->size);
        cog_memcpy(result, curr->data, list->size);
        //then delete
        cog_list_remove(list, curr->data);
        //and return copy
        return result;
    }
}
