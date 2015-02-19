#include <cog.h>

void update_sprite(double timestep, cog_sprite* sprite) {
    static double timer = 0;
    timer += timestep;
    if(timer > 1.0) {
        if(sprite->pos.y > 9.0) {
            sprite->pos.y = 0.0; //Back on-screen
        } else {
            sprite->pos.y = 10.0; //offscreen
        }
        timer = 0.0;
    }
}

void update_anim(double timestep, cog_anim* anim) {
    if((anim->pos.x > 1.0) || (anim->pos.x < -1.0)) {
        anim->vel.x = -anim->vel.x;
    }
    if((anim->pos.y > 1.0) || (anim->pos.y < -1.0)) {
        anim->vel.y = -anim->vel.y;
    }
}

int main(int argc, char* argv[]) {
    cog_init();
    cog_sprite_id id = cog_sprite_add("assets/test0.png");
    cog_sprite_set(id, (cog_sprite) {
        .dim=(cog_dim2) {
            .w=1, .h=1
        },
        .rot=0.0,
         .update_func = update_sprite
    });
    cog_anim_id anim = cog_anim_add("assets/kitten_anim.png", 1, 3);
    cog_anim_set(anim, (cog_anim) {
        .dim = (cog_dim2) {.w=0.3, .h=0.3},
        .pos = (cog_pos2) {.x=0, .y=0},
        .vel = (cog_vec2) {.x=0.3, .y=0.9},
        .transition_time = 0.15,
         .looped = COG_TRUE,
          .ang_vel = COG_PI/1,
           .update_func = update_anim
    });
    cog_anim_set_frames(anim, 0, 1, 2);
    while(!cog_hasquit()) {
        cog_loopstep();
    }
    cog_quit();
    return 0;
}

