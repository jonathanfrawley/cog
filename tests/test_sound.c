#include "cog.h"

int main(void) {
    cog_init();
    cog_snd_id snd = cog_snd_add("media/testsnd.wav");
    cog_snd_play(snd);
    while(!cog_hasquit()) {
        cog_loopstep();
    }
    cog_quit();
}
