#include "cog.h"

int main(void)
{
    cog_init();
    cog_mainloop();
    cog_destroy();
    return 0;
}
