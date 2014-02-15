#include <cog.h>

/*-----------------------------------------------------------------------------
 *  Should see sprite in centre of screen
 *-----------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    cog_init();
    cog_sprite_id id = cog_sprite_add("media/test0.png");
    //cog_sprite_id id = cog_sprite_add("media/grid.png");
    cog_sprite_set(id, (cog_sprite) {
        .dim=(cog_dim2) {
            .w=1, .h=1
        },
        .rot=0.0
    });

    cog_sprite_id id2 = cog_sprite_add("media/test0.png");
    //cog_sprite_id id = cog_sprite_add("media/grid.png");
    cog_sprite_set(id2, (cog_sprite) {
        .pos=(cog_pos2) {
            .x=0.1, .y=0.0
        },
        .dim=(cog_dim2) {
            .w=1, .h=1
        },
        .rot=0.0
    });
    while(!cog_hasquit()) {
        cog_loopstep();
    }
    cog_quit();
    return 0;
}

