#include "cog_list.h"

int main(void)
{
    cog_list* list = cog_list_alloc();
    for(cog_list* this=list, cog_list* next=list->next;
            this->data!=COG_LIST_ENDSENTINAL;
            this=next,next=next->next)
    {
        cog_errorf("ERROR:Should never get here");
    }
    cog_uint* a = COG_STRUCT_MALLOC(cog_uint);
    cog_list_append(&list,
}
