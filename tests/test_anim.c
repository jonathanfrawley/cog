#include "cog.h"
#include "cog_anim.h"
#include "cog_math.h"

static const int N_ENTITIES = 1;

typedef struct entity {
    cog_anim_id anim;
} entity;

void entity_init(entity* obj) {
    for(int i=0; i<10;i++) {
        obj->anim = cog_anim_add("media/kitten_anim.png", 1, 3);
        cog_anim_set(obj->anim, (cog_anim) {
                .dim = (cog_dim2) {.w=0.3, .h=0.3},
                .pos = (cog_pos2) {.x=0, .y=0},
                .vel = (cog_vec2) {.x=0.3, .y=0.9},
                .transition_time = 0.15,
                .looped = COG_TRUE,
                .ang_vel = COG_PI/1,
        });
        cog_anim_set_frames(obj->anim, 0, 1, 2);
    }
}

void entity_update(entity* obj) {
    cog_anim* anim = cog_anim_get(obj->anim);
    if((anim->pos.x > 1.0) || (anim->pos.x < -1.0)) {
        anim->vel.x = -anim->vel.x;
    }
    if((anim->pos.y > 1.0) || (anim->pos.y < -1.0)) {
        anim->vel.y = -anim->vel.y;
    }
}

int main(int argc, char* argv[]) {
    cog_init();
    entity myentity[N_ENTITIES];

    cog_anim_id anim = cog_anim_add("media/2d_anim_test.png", 2, 4);
    cog_anim_set(anim, (cog_anim) {
        .dim = (cog_dim2) {.w=0.3, .h=0.3},
        .pos = (cog_pos2) {.x=0.3, .y=0.3},
        .transition_time = 1.0,
        .looped = COG_TRUE
    });
    cog_anim_set_frames(anim, 0, 1, 2, 3, 4, 5, 6, 7);

    cog_anim_id tileset = cog_anim_add("media/tileset.png", 2, 2);
    cog_anim_set(tileset, (cog_anim) {
        .dim = (cog_dim2) {.w=0.3, .h=0.3},
        .pos = (cog_pos2) {.x=-0.3, .y=-0.3},
        .paused = COG_TRUE
    });
    cog_anim_set_frames(tileset, 0, 1, 2, 3);
    uint32_t frame_no = 0;
    for(int i = 0; i < N_ENTITIES; i++) {
        entity_init(&myentity[i]);
    }
    while(!cog_hasquit()) {
        for(int i = 0; i < N_ENTITIES; i++) {
            entity_update(&myentity[i]);
        }
        cog_loopstep();

        if(cog_input_key_pressed()) {
            uint32_t key = cog_input_key_code_pressed();
            cog_debugf("key is %d", key);
            if(key == 'a') {
                frame_no = (frame_no - 1) % 4;
            }
            if(key == 'd') {
                frame_no = (frame_no + 1) % 4;
            }
            cog_debugf("Setting frame to %d", frame_no);
            cog_anim_set_frame(tileset, frame_no);
        }
    }
}
