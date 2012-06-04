#ifndef  COG_LIST_H
#define  COG_LIST_H

#include "cog_types.h"

typedef struct cog_list
{
    struct cog_list* next;
    cog_dataptr data;
} cog_list;

void cog_list_init(cog_list*);

//----------------------------------------------------------------------
//  Adds data to this list. List will handle freeing data.
//  Sets list pointer to point to new beginning of list.
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

#endif   // COG_LIST_H
