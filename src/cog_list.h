#ifndef  COG_LIST_H
#define  COG_LIST_H

#include "cog_types.h"

static cog_dataptr COG_LIST_ENDSENTINAL = 0;

#define COG_LIST_FOREACH(list) \
    for(cog_list* curr=(list)->next, *next=(list)->next->next; \
            curr->data!=COG_LIST_ENDSENTINAL; \
            curr=next,next=next->next)

//For double loops
#define COG_LIST_FOREACH2(list) \
    for(cog_list* curr2=(list)->next, *next2=(list)->next->next; \
            curr2->data!=COG_LIST_ENDSENTINAL; \
            curr2=next2,next2=next2->next)

typedef struct cog_list
{
    struct cog_list* next;
    cog_dataptr data;
} cog_list;

cog_list* cog_list_alloc();

void cog_list_init(cog_list*);

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
void cog_list_empty(cog_list* list);

//----------------------------------------------------------------------
//  Returns: Length of list.
//----------------------------------------------------------------------
cog_uint cog_list_length(cog_list* list);

//----------------------------------------------------------------------
//  Pops the front element of the list and returns it.
//----------------------------------------------------------------------
cog_dataptr cog_list_pop_first(cog_list* list);

#endif   // COG_LIST_H
