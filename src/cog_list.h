#ifndef  COG_LIST_H
#define  COG_LIST_H

#include "cog_types.h"

#define COG_LIST_ENDSENTINAL 0

#define COG_LIST_FOREACH(list) \
    for(cog_list* curr=(list)->next, *next=(list)->next->next; \
            curr->data!=COG_LIST_ENDSENTINAL; \
            curr=next,next=next->next)

//nested for loop
#define COG_LIST_FOREACH2(list) \
    for(cog_list* curr2=(list)->next, *next2=(list)->next->next; \
            curr2->data!=COG_LIST_ENDSENTINAL; \
            curr2=next2,next2=next2->next)

typedef struct cog_list {
    struct cog_list* next;
    cog_dataptr data;
    uint32_t size;              //size of data member
} cog_list;

//List structure:
//  NULL -> a0 -> a1 -> a2 -> a3 ->|
//  / \                            |
//   |-----------------------------|
// The first element has COG_LIST_ENDSENTINAL as its data field.
// The last element is a pointer to the first element again, thereby acting as a sentinal to for loops.
//
//Memory:
// All data members are alloced and dealloced by the list data structure.
// Do not rely on these data values lying around for reference elsewhere.

cog_list* cog_list_alloc(uint32_t size);

void cog_list_init(cog_list* list, uint32_t size);

//----------------------------------------------------------------------
//  Adds data to this list. List will handle freeing data.
//----------------------------------------------------------------------
void cog_list_append(cog_list* list, cog_dataptr data);

//----------------------------------------------------------------------
//  Removes elem w/ data == data in this list.
//----------------------------------------------------------------------
void cog_list_remove(cog_list* list, cog_dataptr data);

//----------------------------------------------------------------------
//  Empties list and frees data.
//----------------------------------------------------------------------
void cog_list_removeall(cog_list* list);

//----------------------------------------------------------------------
//  Returns: Length of list.
//----------------------------------------------------------------------
uint32_t cog_list_len(cog_list* list);

//----------------------------------------------------------------------
//  Pops the front element of the list and returns it.
//----------------------------------------------------------------------
cog_dataptr cog_list_pop(cog_list* list);

#endif // COG_LIST_H
