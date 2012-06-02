#ifndef  COG_H
#define  COG_H

#include "cog_core.h"

//main
void cog_init();
void cog_mainloop();
void cog_loopstep();
void cog_destroy();

//anim
cog_anim_id cog_add_anim(char* animimg,
        cog_uint transition_millis,
        cog_bool looped,
        cog_uint nimages, 
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h, ...);
void cog_play_anim(cog_anim_id id);
cog_anim_id cog_move_anim(char* animimg,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h, ...);

//sound
cog_snd_id cog_sound_load(char* fname);
void cog_sound_play(cog_snd_id snd);
int cog_sound_isfinished(cog_snd_id snd);

#endif   // COG_H
