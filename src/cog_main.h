#ifndef  COG_MAIN_H
#define  COG_MAIN_H

#include "cog_types.h"

void cog_init();
cog_bool cog_updateready();
void cog_sleep(cog_uint millis);
void cog_sleepuntilupdate();
void cog_mainloop();
void cog_update();
void cog_loopstep();
void cog_quit();
cog_bool cog_hasquit();
cog_uint cog_time_delta_millis();

//default window functions
cog_uint cog_screenw();
cog_uint cog_screenh();
void cog_toggle_fullscreen();

//rng
cog_uint cog_nextrand();

// Remove all sprites, sounds and anims.
void cog_clear();

//Set background image.
void cog_set_background(char* img);

// Convert from screen space 0.0 -> 1.0 to cog coords.
cog_pos2 cog_screen_coords(cog_float x, cog_float y);

#endif // COG_MAIN_H
