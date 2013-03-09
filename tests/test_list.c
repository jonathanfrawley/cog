#include "cog_core.h"
#include "cog_list.h"
#include "cog_log.h"
#include "cog_test.h"

int main(void) {
    cog_list* list = cog_list_alloc(sizeof(cog_uint));
    //@test: COG_LIST_FOREACH
    cog_bool flag = COG_TRUE;
    COG_LIST_FOREACH(list) {
        cog_errorf("ERROR:Should never get here");
        flag = COG_FALSE;
    }
    cog_test("COG_LIST_FOREACH for list of length 0", flag);
    //@test: cog_list_append
    //Add an element
    cog_uint* a = COG_STRUCT_MALLOC(cog_uint);
    (*a) = 4;
    cog_list_append(list, a);
    flag = COG_TRUE;
    COG_LIST_FOREACH(list) {
        if((*(cog_uint*) curr->data) != 4) {
            flag = COG_FALSE;
        }
    }
    cog_test("cog_list_append for list of length 0", flag);
    //Add another element
    cog_uint* b = COG_STRUCT_MALLOC(cog_uint);
    (*b) = 4;
    cog_list_append(list, b);
    flag = COG_TRUE;
    COG_LIST_FOREACH(list) {
        if((*(cog_uint*) curr->data) != 4) {
            flag = COG_FALSE;
        }
    }
    cog_test("cog_list_append for list of length 1", flag);
    //@test: cog_list_removeall
    cog_list_removeall(list);
    flag = COG_TRUE;
    COG_LIST_FOREACH(list) {
        cog_errorf("ERROR:Should never get here");
        flag = COG_FALSE;
    }
    cog_test("cog_list_removeall for list of length 2", flag);
    //@test: cog_list_len
    cog_test("cog_list_len for lists of zero length",
             (cog_list_len(list) == 0));
    cog_uint* c = COG_STRUCT_MALLOC(cog_uint);
    (*c) = 4;
    cog_list_append(list, c);
    cog_test("cog_list_len for lists of length 1", (cog_list_len(list) == 1));
    //@test: cog_list_pop
    cog_dataptr data = cog_list_pop(list);
    cog_test("cog_list_pop for list of length 1",
             ((*(cog_uint*) data) == (*c)));
    cog_test("cog_list_pop list size decrease after pop",
             (cog_list_len(list) == 0));
    return 0;
}
