#include "cog.h"
#include "cog_math.h"
#include "cog_text.h"

int main(void)
{
    cog_init();
    cog_text_id id = cog_text_add(" Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    cog_text* text = cog_text_get(id);
    text->pos.x = cog_screenw()*0.25f;
    text->pos.y = cog_screenh()*0.5f;
    while(! cog_hasquit())
    {
        text->pos.x += 0.1;
        text->pos.y += 0.1;
        text->rot += (COG_PI * 0.005);
        cog_loopstep();
    }
    cog_quit();
    return 0;
}

