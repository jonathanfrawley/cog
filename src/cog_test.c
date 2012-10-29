#include "cog_test.h"

#include "cog_core.h"

void cog_test(cog_string testname, cog_bool condition)
{
    if(condition) 
    {
        cog_infof("[PASS] : %s", testname);
    }
    else
    {
        cog_debugf("[FAIL] : %s", testname);
    }
}
