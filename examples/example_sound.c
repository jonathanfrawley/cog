#include "cog.h"

int main(int argc, char* argv[]) {
    cog_init();
    cog_snd_id snd = cog_snd_add("assets/testsnd.wav");
    cog_snd_play(snd);
    while(!cog_hasquit()) {
        cog_loopstep();
    }
    cog_quit();
}
