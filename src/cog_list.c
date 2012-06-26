#include "cog_list.h"

#include "cog_core.h"
#include "cog.h"

cog_list* cog_list_alloc()
{
    cog_list* obj = COG_STRUCT_MALLOC(cog_list);
    cog_list_init(obj);
    return obj;
}

void cog_list_init(cog_list* list)
{
    //Initial list is a pointer to itself
    list->data = COG_LIST_ENDSENTINAL;
    list->next = list;
}

void cog_list_append(cog_list* list, cog_dataptr data)
{
    //cog_list* list = *listptr;

    cog_list* curr;
    cog_list* prev;
    for(prev=list, curr=list->next ;
            curr->data != COG_LIST_ENDSENTINAL ;
            prev=curr, curr=curr->next)
    {
        ;
    }
    //Add node between prev and curr
    cog_list* newnode = cog_list_alloc();
    newnode->data = data;
    newnode->next = curr;
    prev->next = newnode;
}

void cog_list_remove(cog_list* list, cog_dataptr data)
{
    //cog_list* list = *listptr;

    cog_list* curr;
    cog_list* prev;
    for(prev=list, curr=list->next ;
            curr->data != COG_LIST_ENDSENTINAL ;
            prev=curr, curr=curr->next)
    {
        if(curr->data == data)
        {
            prev->next = curr->next;
            curr->next = COG_LIST_ENDSENTINAL;
            break;
        }
    }
}

void cog_list_empty(cog_list* list)
{
    //cog_list* list = *listptr;
    cog_list* curr;
    cog_list* next;
    for(curr=list->next, next=curr->next->next ;
            curr->data != COG_LIST_ENDSENTINAL ;
            curr=next, next=next->next)
    {
        cog_free(curr->data);
        cog_free(curr);
    }
    list->next = list;
    //(*listptr) = COG_NULL;
    //(*listptr) = cog_list_alloc();
}

cog_uint cog_list_length(cog_list* list)
{
    //cog_list* list = *listptr;
    cog_uint cnt = 0;
    cog_list* curr;
    for(curr=list ; curr->data != COG_LIST_ENDSENTINAL ; curr=curr->next)
    {
        cnt++;
    }
    return cnt;
}
