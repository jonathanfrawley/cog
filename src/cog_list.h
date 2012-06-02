#ifndef  COG_LIST_H
#define  COG_LIST_H

#include <cog_core.h>

typedef struct cog_list
{
    cog_list* next;
    cog_dataptr data;
} cog_list;

void cog_list_init(cog_list*);
void cog_list_get(cog_list*);

#endif   // COG_LIST_H
