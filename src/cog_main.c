#include "cog_main.h"

#define EMSCRIPTEN_ENABLED //TODO :Figure out how to pass this on emcc path
#define DEBUG //TODO :Figure out how to pass this on emcc path
#define USE_LEGACY_SDL 1 //TODO :Figure out how to pass this on emcc path

#ifdef EMSCRIPTEN_ENABLED
#include <emscripten.h>
#endif

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#ifdef USE_LEGACY_SDL
#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#endif

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
#include "cog_tween.h"
#include "cog_window.h"

//constants
#define FRAMES_PER_SECOND 40
static uint32_t FRAME_TIME = 1000 / FRAMES_PER_SECOND;

//data structures
typedef struct {
    int finished;
} cog_game;
static cog_game game;

//#prototypes
//##main
double cog_math_radtodeg(double rad);

//#global vars
static cog_window window;
//##timing
static uint32_t delta_millis;
static uint32_t starttime;
static uint32_t now;
static uint32_t framedrawcounter;
static uint32_t lastframetime;
static uint32_t frametimecounter;
static uint32_t frameupdatecounter;
static uint32_t render_time;
static void (*main_loop_func)(void);

//implementations
void _cog_init(cog_config config) {
    //Init cog
    game.finished = 0;
    cog_snd_init();
    cog_sprite_init();
    cog_anim_init();
    cog_window_init(config, &window);
    cog_input_init(&window);
    cog_graphics_init(&window);
    cog_text_init();
    cog_tween_init();
    starttime = SDL_GetTicks();
    //init rng
    srand(2);
}

//This is the cog default loop, can be overrided by just using cog_loopstep instead.
void cog_main_loop() {
    while(!game.finished) {
        cog_loopstep();
    }
}

/* *
 * This will be return true when the user should call another
 * cog_update()
 * */
bool cog_updateready() {
    now = SDL_GetTicks();
    delta_millis = now - starttime;
    return (delta_millis > FRAME_TIME);
}

void cog_sleep(uint32_t millis) {
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
    double timestep = (double)delta_millis / 1000.0;
    cog_anim_update(timestep);
    cog_sprite_update(timestep);
    cog_graphics_update(timestep);
    cog_tween_update(timestep);
    if(main_loop_func) {
        (*main_loop_func)();
    }
    cog_input_check_keys();
    cog_input_check_mouse();
    //Useful logging every second.
    frametimecounter += lastframetime;
    if(frametimecounter >= 1000) {
#ifdef DEBUG
        cog_debugf("nupdates <%d>, ndraws <%d>, render_time <%d> cog_map_get_counter <%d> cog_map_get_timer <%lf>", frameupdatecounter,
                   framedrawcounter, render_time, cog_map_get_counter(), cog_map_get_timer());
#endif //DEBUG
        frametimecounter = 0;
        framedrawcounter = 0;
        frameupdatecounter = 0;
        cog_map_reset_counter();
        cog_map_reset_timer();
    }
}

//This is to allow the user to control the mainloop
void cog_loopstep() {
    cog_update();
    framedrawcounter++;
    uint32_t start_render = SDL_GetTicks();
    cog_graphics_render(&window);
    uint32_t end_render = SDL_GetTicks();
    render_time = end_render - start_render;
    cog_window_update(&window);
}

void cog_quit() {
    game.finished = 1;
    cog_input_quit();
    cog_snd_quit();
    cog_window_quit(&window);
}

bool cog_hasquit() {
    return game.finished;
}

uint32_t cog_time_delta_millis() {
    return delta_millis;
}

/*-----------------------------------------------------------------------------
 * Generates an integer between a and b.
 * Precondition: a <= N <= b
 *-----------------------------------------------------------------------------*/
int32_t cog_rand_int(int32_t a, int32_t b) {
    return (int32_t)cog_rand_uniform((double)a, (double)b);
}

double cog_rand() {
    return ((double)rand()/(double)RAND_MAX);
}

/*-----------------------------------------------------------------------------
 * Generates a real number between a and b.
 * Precondition: a <= N <= b
 *-----------------------------------------------------------------------------*/
double cog_rand_uniform(double a, double b) {
    double r = cog_rand();
    return a + (b - a) * r;
}

void cog_toggle_fullscreen() {
    cog_window_toggle_fullscreen(&window);
}

void cog_clear() {
    //TODO: init should probably do a clearall anyway?
    cog_sprite_removeall();
    cog_sprite_init();
    cog_anim_removeall();
    cog_anim_init();
    cog_text_removeall();
    cog_text_init();
    cog_snd_stopall();
    cog_snd_init();
}

void cog_set_background(char* img) {
    cog_sprite_id id = cog_sprite_add(img);
    cog_sprite* sprite = cog_sprite_get(id);
    sprite->pos.x = 0.0;
    sprite->pos.y = 0.0;
    sprite->dim.w = 1.0;
    sprite->dim.h = 1.0;
}

void cog_start_main_loop(void (*main_loop)(void)) {
    main_loop_func = main_loop;
#ifdef EMSCRIPTEN_ENABLED 
    emscripten_set_main_loop(cog_loopstep, 0, 0);
#else
    cog_main_loop();
#endif
}
