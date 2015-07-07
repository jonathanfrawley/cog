#include "cog.h"
#include "cog_anim.h"
#include "cog_log.h"
#include "cog_math.h"

static const int N_ENTITIES = 1;

typedef struct entity {
    cog_anim_id anim;
} entity;
cog_sprite_id joystick_sprite;

void entity_init(entity* obj) {
    obj->anim = cog_anim_add("assets/kitten_anim.png", 1, 3);
    cog_anim_set(obj->anim, (cog_anim) {
        .dim = (cog_dim2) {.w=0.3, .h=0.3},
        .pos = (cog_pos2) {.x=0, .y=0},
        .vel = (cog_vec2) {.x=0.0001, .y=0.0003},
        .transition_time = 0.15,
         .looped = COG_TRUE,
          .ang_vel = COG_PI
    });
    cog_anim_set_frames(obj->anim, 0, 1, 2);
}

void entity_update(entity* obj) {
    cog_anim* anim = cog_anim_get(obj->anim);
    anim->pos.x = cog_input_mouse_x();
    anim->pos.y = cog_input_mouse_y();
}

int main(int argc, char* argv[]) {
    cog_init();
    entity myentity[N_ENTITIES];
    for(int i = 0; i < N_ENTITIES; i++) {
        entity_init(&myentity[i]);
    }
    joystick_sprite = cog_sprite_add("assets/test0.png");
    cog_sprite* s = cog_sprite_get(joystick_sprite);
    s->dim.w = 0.3;
    s->dim.h = 0.3;
    while(!cog_hasquit()) {
        for(int i = 0; i < N_ENTITIES; i++) {
            entity_update(&myentity[i]);
        }
        //Update joystick
        cog_sprite* s = cog_sprite_get(joystick_sprite);
        cog_loopstep();
        //cog_debugf("x is %lf, y is %lf", cog_input_mouse_x(), cog_input_mouse_y());
        double vel = 0.001;
        if(cog_input_joystick_pressed()) {
            if(cog_input_joystick_x_pressed()) {
                s->vel.x = vel * cog_input_joystick_x_value();
            }
            if(cog_input_joystick_y_pressed()) {
                s->vel.y = vel * cog_input_joystick_y_value();
            }
        }
        if(cog_input_joystick_depressed()) {
            if(cog_input_joystick_x_depressed()) {
                s->vel.x = 0;
            }
            if(cog_input_joystick_y_depressed()) {
                s->vel.y = 0;
            }
        }
    }
}
