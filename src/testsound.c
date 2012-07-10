#include "cog.h"


int main(void)
{
    cog_init(COG_CONFIG_HWRENDER);
    cog_snd_id snd = cog_snd_load("../media/testsnd.wav");
    cog_snd_play(snd,0);
    while(! cog_snd_isfinished(snd))
    {
        cog_loopstep();
    }
}

