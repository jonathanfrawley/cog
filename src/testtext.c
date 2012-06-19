#include "cog.h"

int main(void)
{
    cog_init(COG_CONFIG_HWRENDER);

    //XXX:Test font
    cog_sprite_id id = cog_text_createsprite("YO THERE",
            COG_TEXT_COL_RED,
            cog_get_screenw() * 0.2, //x
            cog_get_screenh() * 0.3, //y
            cog_get_screenw() * 1.0, //w
            cog_get_screenh() * 0.1, //h
            0, //rot
            0, // texx,
            0, // texy,
            1, // texw,
            1); // texh

    cog_mainloop();
    cog_quit();
    return 0;
}

