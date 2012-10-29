#include "cog_list.h"
#include "cog_core.h"

int main(void)
{
    cog_list* list = cog_list_alloc(sizeof(cog_uint));
    COG_LIST_FOREACH(list)
    {
        cog_errorf("ERROR:Should never get here");
    }

    //@test: cog_list_append
    //Add an element
    cog_uint* a = COG_STRUCT_MALLOC(cog_uint);
    (*a) = 4;
    cog_list_append(list, a);

    COG_LIST_FOREACH(list)
    {
        cog_debugf("This should be printed once");
        if((*(cog_uint*)curr->data) != 4)
        {
            cog_debugf("data <%d>", (*(cog_uint*)curr->data));
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
    
    //@test: cog_list_removeall
    cog_list_removeall(list);
    COG_LIST_FOREACH(list)
    {
        cog_errorf("ERROR:Should never get here");
    }

    //@test: cog_list_len
    if(cog_list_len(list) != 0) 
    {
        cog_errorf("cog_list_length broken for lists of zero length");
    }
    cog_uint* c = COG_STRUCT_MALLOC(cog_uint);
    (*c) = 4;
    cog_list_append(list, c);
    if(cog_list_len(list) != 1) 
    {
        cog_errorf("cog_list_length not working for lists of length 1.");
    }

    //@test: cog_list_pop_first
    cog_dataptr data = cog_list_pop(list);
    if( (*(cog_uint*)data) != (*c) )
    {
        cog_errorf("cog_list_pop broken");
    }

    return 0;
}
