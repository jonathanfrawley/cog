#include "cog.h"
#include "cog_math.h"
#include "cog_text.h"

int main(int argc, char* argv[]) {
    cog_init();
    cog_text_id id = cog_text_add();
    cog_text_set(id, (cog_text) {
        .scale = (cog_dim2) {.w=0.003, .h=0.003},
        .dim = (cog_dim2) {.w=1.0, .h=0.003},
        .pos = (cog_pos2) {.x=-1, .y=-1},
        .col=(cog_color) {
            .r=1,.g=1,.b=1,.a=1
        }
    });
    cog_text_set_str(id, "This is some single line text wrapped around.");
    cog_text_id mid = cog_text_add();
    cog_text_set(mid, (cog_text) {
        .scale = (cog_dim2) {.w=0.003, .h=0.003},
        .dim = (cog_dim2) {.w=2.0, .h=2.0}, //screen coords -1.0 -> 1.0
        .pos = (cog_pos2) {.x=-1, .y=0.5},
        .col=(cog_color) {
            .r=0.2,.g=1,.b=0.1,.a=1
        }
    });
    cog_text_set_str(mid, "This is some multiline text.\nThis is the second line.\nAnd the third.\n4");
    cog_text* text = cog_text_get(id);
    cog_text_set_face(id, "assets/fonts/FreeSans.ttf", 48);
    double xvel = 0.003;
    double yvel = 0.009;
    while(!cog_hasquit()) {
        text->pos.x += xvel;
        text->pos.y += yvel;
        if((text->pos.x > 1.0) || (text->pos.x < -1.0)) {
            xvel = -xvel;
        }
        if((text->pos.y > 1.0) || (text->pos.y < -1.0)) {
            yvel = -yvel;
        }
        cog_loopstep();
    }
    cog_quit();
    return 0;
}
