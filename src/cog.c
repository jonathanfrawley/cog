#include "cog.h"

#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_image.h>
#include <AL/alut.h>
#include <SDL/SDL_ttf.h>
#include <GL/glew.h>

#include "cog_anim.h"
#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_input.h"
#include "cog_list.h"
#include "cog_log.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_sprite.h"
#include "cog_text.h"
#include "cog_window.h"

//constants
#define FRAMES_PER_SECOND 40
static cog_uint FRAME_TIME = 1000 / FRAMES_PER_SECOND;

//data structures
typedef struct
{
    int finished;
} cog_game;
static cog_game game;

/**
 * All sprites have a texture id and texture coords.
 **/
typedef struct
{
    cog_snd_id id;
    ALuint buffer;
    ALuint source;
} cog_snd;

//#prototypes
//##main
void cog_audio_init(void);
cog_float cog_math_radtodeg(cog_float rad);

//#global vars
static cog_list activesnds; //snds playing at the moment
static cog_snd_id cog_sndcnt;
static cog_map snds;
static cog_window window;
//##timing
static cog_uint timedelta;
static cog_uint starttime;
static cog_uint now;
static cog_uint framedrawcounter;
static cog_uint lastframetime;
static cog_uint frametimecounter;
static cog_uint frameupdatecounter;

//implementations
void cog_init(void)
{
    //Init globals
    cog_map_init(&snds);
    cog_list_init(&activesnds, sizeof(cog_sprite_id));

    //Init cog
    game.finished = 0;
    cog_sprite_init();
    cog_anim_init();
    cog_window_init(&window);
    cog_graphics_init();
    cog_audio_init();
    cog_text_init();
    starttime = SDL_GetTicks();
    //init rng
    srand(2);
}

//This is the cog default loop, can be overrided by just using cog_loopstep instead.
void cog_mainloop()
{
    while(!game.finished)
    {
        cog_loopstep();
    }
}

/* *
 * This will be return true when the user should call another
 * cog_update()
 * */
cog_bool cog_updateready()
{
    now = SDL_GetTicks();
    timedelta = now - starttime;
    return (timedelta > FRAME_TIME);
}

void cog_sleep(cog_uint millis)
{
    SDL_Delay(millis);
}

void cog_sleepuntilupdate()
{
    now = SDL_GetTicks();
    timedelta = now - starttime;
    if(timedelta<FRAME_TIME)
    {
        cog_sleep(FRAME_TIME-timedelta);
    }
}

void cog_update()
{
    now = SDL_GetTicks();
    timedelta = now - starttime;

    //performance timing
    frameupdatecounter++;
    lastframetime = SDL_GetTicks() - starttime;
    starttime = SDL_GetTicks();
    //Useful logging every second.
    frametimecounter += lastframetime;
    if(frametimecounter >= 1000)
    {
#ifdef DEBUG
        cog_debugf("nupdates <%d>, ndraws <%d>", frameupdatecounter, framedrawcounter);
#endif //DEBUG
        frametimecounter = 0;
        framedrawcounter = 0;
        frameupdatecounter = 0;
    }

    cog_input_check_keys();
    cog_input_check_mouse();
    cog_anim_update(timedelta);
    cog_sprite_update(timedelta);
}

//This is to allow the user to control the mainloop
void cog_loopstep()
{
    cog_update();
    framedrawcounter++;
    cog_graphics_render();
}

void cog_quit()
{
    game.finished = 1;
    //TODO:Add more cleanup here.
    alutExit();
}

cog_bool cog_hasquit()
{
    return game.finished;
}

void cog_audio_init(void)
{
    int argc = 1;
    char* argv = "cog";
    alutInit(&argc, &argv);
}

cog_uint cog_gettimedelta()
{
    return timedelta;
}

//sound
cog_snd_id cog_snd_load(char* fname)
{
    cog_snd* snd = COG_STRUCT_MALLOC(cog_snd);
    snd->id = cog_sndcnt++;
    snd->buffer = alutCreateBufferFromFile(fname);
    alGenSources(1, &snd->source);
    alSourcei(snd->source, AL_BUFFER, snd->buffer);
    cog_map_put(&snds, snd->id, (void*)snd);
    return snd->id;
}

//TODO:Remove channel arg.
void cog_snd_play(cog_snd_id id)
{
    cog_snd* snd = (cog_snd*)cog_map_get(&snds, id);
    alSourcePlay(snd->source);
    cog_list_append(&activesnds, (cog_dataptr)snd);
}

void cog_snd_play_sfx(cog_snd_id id)
{
    cog_snd_play(id);
}

void cog_snd_play_music(cog_snd_id id)
{
    cog_snd* snd = (cog_snd*)cog_map_get(&snds, id);
    alSourcei(snd->source,AL_LOOPING,AL_TRUE);
    cog_snd_play(id);
}

void cog_snd_stop(cog_snd_id id)
{
    cog_snd* snd = (cog_snd*)cog_map_get(&snds, id);
    alSourceStop(snd->source);
    cog_list_remove(&activesnds, (cog_dataptr)snd);
}

void cog_snd_stopall()
{
    COG_LIST_FOREACH(&activesnds)
    {
        cog_snd* snd = (cog_snd*)curr->data;
        cog_snd_stop(snd->id);
    }
}

cog_uint cog_screenw()
{
    return window.screen->w;
}

cog_uint cog_screenh()
{
    return window.screen->h;
}

cog_uint cog_nextrand()
{
    return rand();
}

void cog_toggle_fullscreen()
{
    cog_window_toggle_fullscreen(&window);
}
