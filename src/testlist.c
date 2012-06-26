#include "cog_list.h"
#include "cog_core.h"

int main(void)
{
    cog_list* list = cog_list_alloc();
    COG_LIST_FOREACH(list)
    {
        cog_errorf("ERROR:Should never get here");
    }

    //Add an element
    cog_uint* a = COG_STRUCT_MALLOC(cog_uint);
    (*a) = 4;
    cog_list_append(list, a);

    COG_LIST_FOREACH(list)
    {
        cog_debugf("This should be printed once");
        if((*(cog_uint*)curr->data) != 4)
        {
            cog_errorf("This shall not pass");
        }
    }

    //Add another element
    cog_uint* b = COG_STRUCT_MALLOC(cog_uint);
    (*b) = 4;
    cog_list_append(list, b);

    COG_LIST_FOREACH(list)
    {
        cog_debugf("This should be printed twice");
        if((*(cog_uint*)curr->data) != 4)
        {
            cog_errorf("This shall not pass");
        }
    }
    cog_list_empty(list);
    COG_LIST_FOREACH(list)
    {
        cog_errorf("ERROR:Should never get here");
    }
    return 0;
}
