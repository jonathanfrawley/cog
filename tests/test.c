#include "cog.h"

int main(int argc, char* argv[]) {
    cog_init();
    cog_mainloop();
    cog_quit();
    return 0;
}
