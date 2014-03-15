#include "cog.h"

#include "emscripten.h" //TODO: Remove

static int loop_cnt;

void main_loop(void) {
    cog_loopstep();
    loop_cnt++;
    if(cog_input_key_pressed()) {
        cog_debugf("Key pressed : %d", cog_input_key_code_pressed());
        cog_snd_id snd = cog_snd_add("audio.wav"); //Remember to preload this!
        cog_snd_play(snd);
    }
    if(cog_input_key_depressed()) {
        cog_debugf("Key depressed : %d", cog_input_key_code_pressed());
    }
}

int main(int argc, char* argv[]) {
    cog_init();
    cog_sprite_id id = cog_sprite_add("test0.png");

    cog_sprite_set(id, (cog_sprite) {
        .dim=(cog_dim2) {
            .w=1.0, .h=1.0
        },
        .rot=COG_PI/2
    });

    cog_anim_id tileset = cog_anim_add("tileset.png", 2, 2);
    cog_anim_set(tileset, (cog_anim) {
        .dim = (cog_dim2) {.w=0.3, .h=0.3},
        .pos = (cog_pos2) {.x=-0.3, .y=-0.3},
        .transition_time = 0.5,
        .looped = COG_TRUE
    });
    cog_anim_set_frames(tileset, 0, 1, 2, 3);
    cog_set_main_loop(main_loop);
    cog_main_loop();
    //main_loop();
    cog_quit();
    return 0;
}
