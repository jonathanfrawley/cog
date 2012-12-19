#ifndef  COG_H
#define  COG_H

#include "cog_types.h"

//main
void cog_init();
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

//default window functions
cog_uint cog_screenw();
cog_uint cog_screenh();
void cog_toggle_fullscreen();

//rng
cog_uint cog_nextrand();


#endif   // COG_H
