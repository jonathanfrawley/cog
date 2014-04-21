#include "cog.h"

void main_loop(void) {
}

int main(int argc, char* argv[]) {
    cog_init();
    cog_sprite_id id = cog_sprite_add("assets/images/title.png");
    cog_sprite_set(id, (cog_sprite) {
        .dim=(cog_dim2) {
            .w=0.5, .h=0.5
        },
        .rot=COG_PI/2
    });
    cog_start_main_loop(main_loop);
    cog_quit();
    return 0;
}
