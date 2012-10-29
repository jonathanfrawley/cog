#ifndef  COG_H
#define  COG_H

#include "cog_types.h"

#define COG_CONFIG_HWRENDER 0x00000001

#define COG_PI 3.14159265

//main
void cog_init(cog_int config);
cog_bool cog_updateready();
void cog_sleep(cog_uint millis);
void cog_sleepuntilupdate();
void cog_mainloop();
void cog_update();
void cog_loopstep();
void cog_quit();
cog_bool cog_hasquit();
void cog_graphics_render();
cog_uint cog_gettimedelta();

//sprite
cog_sprite_id cog_sprite_simple_add(char* filename,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h,
        cog_float rot);
void cog_sprite_update_pos(cog_sprite_id,
        cog_float x,
        cog_float y);
cog_float cog_sprite_getx(cog_sprite_id);
cog_float cog_sprite_gety(cog_sprite_id);
cog_float cog_sprite_getw(cog_sprite_id);
cog_float cog_sprite_geth(cog_sprite_id);
cog_float cog_sprite_getrot(cog_sprite_id id);
void cog_sprite_update_pos(cog_anim_id,
        cog_float x,
        cog_float y);
void cog_sprite_update_rot(cog_sprite_id id, cog_float rot);
void cog_sprite_update_xvel(cog_sprite_id id, cog_float xvel);
void cog_sprite_update_yvel(cog_sprite_id id, cog_float yvel);
void cog_sprite_remove(cog_sprite_id id);
void cog_sprite_removeall(void);

//anim
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
cog_float cog_anim_getw(cog_anim_id);
cog_float cog_anim_geth(cog_anim_id);
cog_float cog_anim_getrot(cog_anim_id id);
void cog_anim_update_rot(cog_anim_id id, cog_float rot);
cog_bool cog_anim_isfinished(cog_anim_id id);
void cog_anim_remove(cog_anim_id id);
void cog_anim_removeall(void);

//sound
cog_snd_id cog_snd_load(char* fname);
void cog_snd_play(cog_snd_id id);
void cog_snd_play_sfx(cog_snd_id snd);
void cog_snd_play_music(cog_snd_id snd);
void cog_snd_stop(cog_snd_id id);
void cog_snd_stopall();

//dimensions
cog_uint cog_get_screenw();
cog_uint cog_get_screenh();

//rng
cog_uint cog_nextrand();

//collision detection
cog_float cog_sprite_dist(cog_sprite_id a, cog_sprite_id b);
cog_float cog_sprite_anim_dist(cog_sprite_id a, cog_anim_id b);

//physics
void cog_update_physics(cog_float timedelta);

//input
cog_bool cog_input_mouseleftjustpressed();
cog_bool cog_input_mouserightjustpressed();
cog_float cog_input_mousex();
cog_float cog_input_mousey();

//text loading
void cog_text_load_font(const char* fpath, cog_uint fsize);
cog_sprite_id cog_text_create_sprite(const char* text,
        cog_text_colour c,
        cog_float x,
        cog_float y,
        cog_float rot,
        cog_float texx,
        cog_float texy,
        cog_float texw,
        cog_float texh);
cog_sprite_id cog_text_create(const char* text,
        cog_text_colour c,
        cog_float x,
        cog_float y);
cog_sprite_id cog_text_create_xcentred(const char* text,
        cog_text_colour c,
        cog_float x,
        cog_float y);

#endif   // COG_H
