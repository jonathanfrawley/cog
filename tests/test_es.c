#include "cog.h"

void main_loop(void) {
    cog_debugf("HI");
}

int main(int argc, char* argv[]) {
    cog_init();
    cog_start_main_loop(main_loop);
    cog_quit();
    return 0;
}
