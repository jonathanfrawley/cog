#include "cog.h"

int main(void)
{
    cog_init(COG_CONFIG_HWRENDER);

    //XXX:Test font
    cog_text_colour c;
    c.r = 0x44;
    c.g = 0x04;
    c.b = 0x04;
    cog_text_create_sprite("Yo there",
            c,
            cog_get_screenw() * 0.5, //x
            cog_get_screenh() * 0.5, //y
            0, //rot
            0, // texx,
            0, // texy,
            1, // texw,
            1); // texh

    cog_mainloop();
    cog_quit();
    return 0;
}

