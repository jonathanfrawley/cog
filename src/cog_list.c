#include "cog_list.h"

#include "cog_core.h"

cog_list* cog_list_alloc()
{
    cog_list* obj = COG_STRUCT_MALLOC(cog_list);
    cog_list_init(obj);
    return obj;
}

void cog_list_init(cog_list* list)
{
    list->data = COG_NULL;
    list->next = COG_NULL;
}

cog_list* cog_list_append(cog_list* list, cog_dataptr data)
{
    cog_list* curr;
    cog_list* prev;
    for(prev=COG_NULL, curr=list ; curr != COG_NULL ; prev=curr, curr=curr->next)
    {
        ;
    }
    if(prev==COG_NULL)
    {
        //First element.
        //This seems a bit contrary, wish we could just use
        //list argument as a variable...
        /*
        cog_list* newfront;
        cog_list_init(newfront);
        //list->data = data;
        newfront->data = data;
        list = newfront;
        */
        list = cog_list_alloc();
        list->data = data;
        list->next = COG_NULL;
        return list;
    }
    else
    {
        prev->next = cog_list_alloc();
        prev->next->data = data;
        return list;
    }
}

void cog_list_remove(cog_list* list, cog_dataptr data)
{
    cog_list* prev;
    cog_list* curr;
    for(prev=COG_NULL, curr=list ; curr != COG_NULL ; prev=curr, curr=curr->next)
    {
        if(curr->data == data)
        {
            if(prev == COG_NULL)
            {
                list = curr->next;
                curr->next = COG_NULL;
                cog_free(curr);
            }
            else
            {
                prev->next = curr->next;
                curr->next = COG_NULL;
                cog_free(curr);
            }
            return;
        }
    }
}

void cog_list_empty(cog_list* list)
{
    if(list == COG_NULL)
    {
        return;
    }

    cog_list* curr;
    cog_list* next;
    for(curr=list, next=curr->next ;
            next != COG_NULL ;
            curr=next, next=next->next)
    {
        cog_free(curr->data);
        curr->next = COG_NULL;
        curr->data = COG_NULL;
        curr = COG_NULL;
    }
}

cog_uint cog_list_length(cog_list* list)
{
    cog_uint cnt = 0;
    cog_list* curr;
    for(curr=list ; curr != COG_NULL ; curr=curr->next)
    {
        if(curr->data != COG_NULL)
        {
            cnt++;
        }
    }
    return cnt;
}
