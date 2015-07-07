#include "cog.h"

int main(int argc, char* argv[]) {
    cog_init();
    cog_main_loop();
    cog_quit();
    return 0;
}
