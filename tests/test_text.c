#include "cog.h"
#include "cog_math.h"
#include "cog_text.h"
#include "cog_text_multiline.h"

int main(void) {
    cog_init();
    cog_text_id id = cog_text_add();
    cog_text_set(id, (cog_text){
            .dim = (cog_dim2) {.w=0.002f, .h=0.002f},
            .pos = (cog_pos2) {.x=-1, .y=-1},
            .str="This is some single line text."
            });

    cog_text_multiline_id mid = cog_text_multiline_add();
    cog_text_multiline_set(mid, (cog_text_multiline){
            .dim = (cog_dim2) {.w=0.002f, .h=0.002f},
            .pos = (cog_pos2) {.x=-1, .y=0.5},
            .str="This is some multiline text.\nThis is the second line.\nAnd the third.\n4"
            });
    //cog_text* text = cog_text_get(id);
    while(!cog_hasquit()) {
        //text->pos.x += 0.1;
        //text->pos.y += 0.1;
        //text->rot += (COG_PI * 0.005);
        cog_loopstep();
    }
    cog_quit();
    return 0;
}
