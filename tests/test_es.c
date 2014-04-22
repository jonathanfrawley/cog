#include "cog.h"

void main_loop(void) {
}

int main(int argc, char* argv[]) {
    cog_init(.window_w = 800,
             .window_h = 600,
             .fullscreen = false);
    cog_sprite_id id = cog_sprite_add("assets/images/title.png");
    cog_sprite_set(id, (cog_sprite) {
        .dim=(cog_dim2) {
            .w=0.7, .h=0.5
        },
        .ang_vel=0.1
    });
    cog_anim_id anim = cog_anim_add("assets/images/tileset.png", 2, 4);
    cog_anim_set(anim, (cog_anim) {
        .dim = (cog_dim2) {.w=0.3, .h=0.3},
        .pos = (cog_pos2) {.x=0.3, .y=0.3},
         .transition_time = 0.1,
          .looped = COG_TRUE
    });
    cog_text_id text = cog_text_add();
    cog_text_set(text, (cog_text) {
        .scale = (cog_dim2) {.w=0.003, .h=0.003},
        .dim = (cog_dim2) {.w=2.0, .h=0.003},
        .pos = (cog_pos2) {.x=-1, .y=0.9},
        .col=(cog_color) {
            .r=0,.g=0,.b=0,.a=1
        }
    });
    cog_text_set_str(text, "This is really the ultimate ");
    /*
    cog_text_id text = cog_text_add();
    cog_text_set(text, (cog_text) {
        .scale = (cog_dim2) {.w=0.003, .h=0.003},
        .dim = (cog_dim2) {.w=2.0, .h=0.003},
        .pos = (cog_pos2) {.x=0, .y=0},
        .col=(cog_color) {
            .r=1,.g=1,.b=1,.a=1
        }
    });
    cog_text_set_str(text, "T");
    */

    cog_start_main_loop(main_loop);
    cog_quit();
    return 0;
}
