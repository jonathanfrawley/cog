#ifndef  COG_MAIN_H
#define  COG_MAIN_H

#include "cog_types.h"

bool cog_hasquit();
void cog_configure(cog_config config);
#define cog_init(...) _cog_init((cog_config){__VA_ARGS__})
void _cog_init();
void cog_loopstep();
void cog_mainloop();
void cog_quit();
void cog_sleep(uint32_t millis);
void cog_sleepuntilupdate();
uint32_t cog_time_delta_millis();
void cog_toggle_fullscreen();
void cog_update();
bool cog_updateready();

//rng
int32_t cog_rand_int(int32_t a, int32_t b);
double cog_rand();
double cog_rand_uniform(double a, double b);

// Remove all sprites, sounds and anims.
void cog_clear();

//Set background image.
void cog_set_background(char* img);

#endif // COG_MAIN_H
