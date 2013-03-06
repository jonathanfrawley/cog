#include "cog_main.h"

#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "cog_anim.h"
#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_input.h"
#include "cog_list.h"
#include "cog_log.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_sprite.h"
#include "cog_snd.h"
#include "cog_text.h"
#include "cog_window.h"

//constants
#define FRAMES_PER_SECOND 40
static cog_uint FRAME_TIME = 1000 / FRAMES_PER_SECOND;

//data structures
typedef struct {
    int finished;
} cog_game;
static cog_game game;

//#prototypes
//##main
cog_float cog_math_radtodeg(cog_float rad);

//#global vars
static cog_window window;
//##timing
static cog_uint delta_millis;
static cog_uint starttime;
static cog_uint now;
static cog_uint framedrawcounter;
static cog_uint lastframetime;
static cog_uint frametimecounter;
static cog_uint frameupdatecounter;

//implementations
void cog_init(void) {
    //Init cog
    game.finished = 0;
    cog_snd_init();
    cog_sprite_init();
    cog_anim_init();
    cog_window_init(&window);
    cog_graphics_init();
    cog_text_init();
    starttime = SDL_GetTicks();
    //init rng
    srand(2);
}

//This is the cog default loop, can be overrided by just using cog_loopstep instead.
void cog_mainloop() {
    while(!game.finished) {
        cog_loopstep();
    }
}

/* *
 * This will be return true when the user should call another
 * cog_update()
 * */
cog_bool cog_updateready() {
    now = SDL_GetTicks();
    delta_millis = now - starttime;
    return (delta_millis > FRAME_TIME);
}

void cog_sleep(cog_uint millis) {
    SDL_Delay(millis);
}

void cog_sleepuntilupdate() {
    now = SDL_GetTicks();
    delta_millis = now - starttime;
    if(delta_millis < FRAME_TIME) {
        cog_sleep(FRAME_TIME - delta_millis);
    }
}

void cog_update() {
    now = SDL_GetTicks();
    delta_millis = now - starttime;

    //performance timing
    frameupdatecounter++;
    lastframetime = SDL_GetTicks() - starttime;
    starttime = SDL_GetTicks();
    //Useful logging every second.
    frametimecounter += lastframetime;
    if(frametimecounter >= 1000) {
#ifdef DEBUG
        cog_debugf("nupdates <%d>, ndraws <%d>", frameupdatecounter,
                   framedrawcounter);
#endif //DEBUG
        frametimecounter = 0;
        framedrawcounter = 0;
        frameupdatecounter = 0;
    }

    cog_input_check_keys();
    cog_input_check_mouse();
    cog_anim_update(delta_millis);
    cog_sprite_update(delta_millis);
}

//This is to allow the user to control the mainloop
void cog_loopstep() {
    cog_update();
    framedrawcounter++;
    cog_graphics_render();
}

void cog_quit() {
    game.finished = 1;
    //TODO:Add more cleanup here.
    alutExit();
}

cog_bool cog_hasquit() {
    return game.finished;
}

cog_uint cog_time_delta_millis() {
    return delta_millis;
}

cog_uint cog_screenw() {
    return window.screen->w;
}

cog_uint cog_screenh() {
    return window.screen->h;
}

cog_uint cog_nextrand() {
    return rand();
}

void cog_toggle_fullscreen() {
    cog_window_toggle_fullscreen(&window);
}
