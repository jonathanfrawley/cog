#include "cog.h"


int main(void)
{
    cog_snd_id snd = cog_sound_load("../media/testsnd.wav");
    cog_sound_play(snd);
    while(! cog_sound_isfinished(snd))
    {
        cog_loopstep();
    }
}

