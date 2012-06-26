#include "cog_list.h"

int main(void)
{
    cog_list* list = cog_list_alloc();
    cog_list* this;
    cog_list* next;
    for(this=list, next=list->next;
            this->data!=COG_LIST_ENDSENTINAL;
            this=next,next=next->next)
    {
        cog_errorf("ERROR:Should never get here");
    }
    cog_uint* a = COG_STRUCT_MALLOC(cog_uint);
    cog_list_append(&list, a);

    for(cog_list* this=list, cog_list* next=list->next;
            this->data!=COG_LIST_ENDSENTINAL;
            this=next,next=next->next)
    {
        cog_debugf("This should be printed exactly once");
    }
    cog_empty(&list);
    for(cog_list* this=list, cog_list* next=list->next;
            this->data!=COG_LIST_ENDSENTINAL;
            this=next,next=next->next)
    {
        cog_errorf("ERROR:Should never get here");
    }
    return 0;
}
