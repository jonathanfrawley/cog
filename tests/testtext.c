#include "cog.h"
#include "cog_math.h"
#include "cog_text.h"

int main(void)
{
    cog_init(COG_CONFIG_HWRENDER);
    cog_text_id id = cog_text_add(" Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    cog_text* text = cog_text_get(id);
    while(! cog_hasquit())
    {
        text->x += 0.1;
        text->y += 0.1;
        text->rot += (COG_PI * 0.005);
        cog_loopstep();
    }
    cog_quit();
    return 0;
}

