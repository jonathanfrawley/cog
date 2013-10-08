#include "cog.h"
#include "cog_math.h"
#include "cog_text.h"
#include "cog_text_multiline.h"

int main(void) {
    cog_init();
    cog_text_id id = cog_text_add();
    cog_text_set(id, (cog_text) {
        .dim = (cog_dim2) {.w=0.003f, .h=0.003f},
        .pos = (cog_pos2) {.x=-1, .y=-1},
        .str="This is some single line text.",
        .col=(cog_color) {
            .r=1,.g=1,.b=1,.a=1
        }
    });
    cog_text_multiline_id mid = cog_text_multiline_add();
    cog_text_multiline_set(mid, (cog_text_multiline) {
        .dim = (cog_dim2) {.w=0.002f, .h=0.002f},
        .pos = (cog_pos2) {.x=-1, .y=0.5},
        .str="This is some multiline text.\nThis is the second line.\nAnd the third.\n4",
        .col=(cog_color) {
            .r=0.2f,.g=1,.b=0.1,.a=1
        }
    });
    cog_text* text = cog_text_get(id);
    text->face = cog_text_load_face("media/font/FreeSans.ttf", 48);
    float xvel = 0.003f;
    float yvel = 0.009f;
    while(!cog_hasquit()) {
        text->pos.x += xvel;
        text->pos.y += yvel;
        if((text->pos.x > 1.0f) || (text->pos.x < -1.0f)) {
            xvel = -xvel;
        }
        if((text->pos.y > 1.0f) || (text->pos.y < -1.0f)) {
            yvel = -yvel;
        }
        cog_loopstep();
    }
    cog_quit();
    return 0;
}
