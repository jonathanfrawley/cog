#ifndef  COG_H
#define  COG_H

#include "cog_types.h"

#define COG_CONFIG_HWRENDER 0x00000001

static cog_float COG_PI = 3.14159265;

//main
void cog_init(cog_int config);
void cog_mainloop();
void cog_loopstep();
void cog_quit();
cog_bool cog_hasquit();

//anim
cog_sprite_id cog_sprite_simple_add(char* filename,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h,
        cog_float rot);
cog_anim_id cog_anim_add(
        char* animimg,
        cog_uint transition_millis,
        cog_bool looped,
        cog_uint nimages,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h,
        cog_float rot, ...);
void cog_anim_play(cog_anim_id id);
void cog_anim_update_pos(cog_anim_id,
        cog_float x,
        cog_float y);
cog_float cog_anim_getx(cog_anim_id);
cog_float cog_anim_gety(cog_anim_id);
cog_float cog_anim_getrot(cog_anim_id id);
cog_float cog_anim_update_rot(cog_anim_id id, cog_float rot);

//sound
cog_snd_id cog_sound_load(char* fname);
void cog_sound_play(cog_snd_id snd);
int cog_sound_isfinished(cog_snd_id snd);

//dimensions
cog_float cog_get_screenw();
cog_float cog_get_screenh();

#endif   // COG_H
