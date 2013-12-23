#include <cog.h>

/*-----------------------------------------------------------------------------
 *  Should see sprite in centre of screen
 *-----------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    cog_init();
    double x_size=2.0;
    double y_size=2.0;
    for(int i=0; i<x_size; i++) {
        for(int j=0; j<y_size; j++) {
            cog_sprite_id id = cog_sprite_add("media/grid.png");
            cog_sprite_set(id, (cog_sprite) {
                .dim=(cog_dim2) {
                    .w=2.0/x_size, .h=2.0/y_size
                },
                .pos=(cog_pos2) {
                    .x=((2.0/x_size) * i) - 1.0,
                     .y=((2.0/y_size) * j) - 1.0
                }
            });
        }
    }
    while(!cog_hasquit()) {
        cog_loopstep();
    }
    cog_quit();
    return 0;
}

