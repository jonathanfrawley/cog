#ifndef  COG_H
#define  COG_H

#include "cog_types.h"

#define COG_CONFIG_HWRENDER 0x00000001

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
cog_uint cog_gettimedelta();

//sound
cog_snd_id cog_snd_load(char* fname);
void cog_snd_play(cog_snd_id id);
void cog_snd_play_sfx(cog_snd_id snd);
void cog_snd_play_music(cog_snd_id snd);
void cog_snd_stop(cog_snd_id id);
void cog_snd_stopall();

//dimensions
cog_uint cog_screenw();
cog_uint cog_screenh();

//rng
cog_uint cog_nextrand();

//input
void cog_input_blank();
cog_bool cog_input_mouseleftjustpressed();
cog_bool cog_input_mouserightjustpressed();
cog_float cog_input_mousex();
cog_float cog_input_mousey();

#endif   // COG_H
