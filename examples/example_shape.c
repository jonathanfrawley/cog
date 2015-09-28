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
        }
    });
    while(!cog_hasquit()) {
        cog_loopstep();
    }
    cog_quit();
    return 0;
}

