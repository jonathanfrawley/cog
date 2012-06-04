#include "cog.h"

int main(void)
{
    cog_init(COG_CONFIG_HWRENDER);
    cog_mainloop();
    cog_quit();
    return 0;
}
