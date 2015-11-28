#include <cog.h>

/*-----------------------------------------------------------------------------
 *  Should see shapes on screen 
 *-----------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    cog_init();
    cog_rect_id id = cog_rect_add();
    cog_rect_set(id, (cog_rect) {
        .dim=(cog_dim2) {
            .w=0.3, .h=0.1
        },
        .rot=COG_PI/2,
        .col=(cog_color) {
            .r=1,.g=1,.b=1,.a=1
        },
        .vel=0.01,
        .ang_vel=COG_PI/4
    });

    cog_rect_id id2 = cog_rect_add();
    cog_rect_set(id2, (cog_rect) {
        .dim=(cog_dim2) {
            .w=0.1, .h=0.3
        },
        .pos=(cog_pos2) {
            .x=0.5, .y=0.6
        },
        .rot=COG_PI/2,
        .col=(cog_color) {
            .r=0,.g=1,.b=0,.a=1
        },
        .vel=0.01,
        .ang_vel=COG_PI/4
    });
    cog_rect_id id3 = cog_rect_add();
    cog_rect_set(id3, (cog_rect) {
        .dim=(cog_dim2) {
            .w=0.05, .h=0.2
        },
        .pos=(cog_pos2) {
            .x=-0.5, .y=-0.6
        },
        .rot=COG_PI/4,
        .col=(cog_color) {
            .r=1,.g=0,.b=1,.a=0.5
        },
        .vel=0.01,
        .ang_vel=3*COG_PI/4
    });
    while(!cog_hasquit()) {
        cog_loopstep();
    }
    cog_quit();
    return 0;
}

