#include "cog_list.h"
#include "cog_core.h"

int main(void)
{
    cog_list* list = cog_list_alloc();
    cog_list* this;
    cog_list* next;
    for(this=list->next, next=list->next->next;
            this->data!=COG_LIST_ENDSENTINAL;
            this=next,next=next->next)
    {
        cog_errorf("ERROR:Should never get here");
    }

    //Add an element
    cog_uint* a = COG_STRUCT_MALLOC(cog_uint);
    (*a) = 4;
    cog_list_append(list, a);

    for(this=list->next, next=list->next->next;
            this->data!=COG_LIST_ENDSENTINAL;
            this=next,next=next->next)
    {
        cog_debugf("This should be printed once");
        if((*(cog_uint*)this->data) != 4)
        {
            cog_errorf("This shall not pass");
        }
    }

    //Add another element
    cog_uint* b = COG_STRUCT_MALLOC(cog_uint);
    (*b) = 4;
    cog_list_append(list, b);

    for(this=list->next, next=list->next->next;
            this->data!=COG_LIST_ENDSENTINAL;
            this=next,next=next->next)
    {
        cog_debugf("This should be printed twice");
        if((*(cog_uint*)this->data) != 4)
        {
            cog_errorf("This shall not pass");
        }
    }
    cog_list_empty(list);
    for(this=list->next, next=list->next->next;
            this->data!=COG_LIST_ENDSENTINAL;
            this=next,next=next->next)
    {
        cog_errorf("ERROR:Should never get here");
    }
    return 0;
}
