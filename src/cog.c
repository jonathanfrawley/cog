#include "cog.h"

#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <AL/alut.h>
#include <SDL/SDL_ttf.h>
#include <GL/glew.h>

#include "cog_anim.h"
#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_list.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_sprite.h"
#include "cog_text.h"

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

typedef struct
{
    SDL_Surface* screen;
} cog_window;
static cog_window window;

//#prototypes
//##main
void cog_platform_init(void);
void cog_window_init(void);
void cog_window_togglefullscreen(void);
void cog_audio_init(void);
cog_float cog_math_radtodeg(cog_float rad);
//##input
//###mouse
void cog_input_checkmouse(void);
//##keys
void cog_input_checkkeys(void);

//#global vars
static cog_list activesnds; //snds playing at the moment
static cog_snd_id cog_sndcnt;
static cog_map snds;
//##timing
static cog_uint timedelta;
static cog_uint starttime;
static cog_uint now;
static cog_uint framedrawcounter;
static cog_uint lastframetime;
static cog_uint frametimecounter;
static cog_uint frameupdatecounter;
//##input
static cog_bool mouseleftpressed;
static cog_bool mouserightpressed;
static cog_bool mouseleftjustpressed = 0;
static cog_bool mouserightjustpressed;
static cog_float mousex;
static cog_float mousey;

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
    cog_platform_init();
    cog_window_init();
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

    cog_input_checkkeys();
    cog_input_checkmouse();
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

//platform
void cog_platform_init(void)
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        cog_errorf(SDL_GetError());
    }
}

//window
void cog_window_init(void)
{
    //TODO:Get from yaml conf.
    int width = 0;
    int height = 0;
    int bpp = 32;
    int flags = SDL_OPENGL | SDL_DOUBLEBUF;
    if( (window.screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0 )
    {
        cog_errorf("cog_window_init failed when creating SDL window <%s> \n", SDL_GetError());
    }

    SDL_WM_SetCaption( "cog game", NULL );
}

void cog_window_togglefullscreen(void)
{
    SDL_WM_ToggleFullScreen(window.screen);
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

//#input
//##mouse
void cog_input_blank()
{
    mouseleftjustpressed = COG_FALSE;
    mouserightjustpressed = COG_FALSE;
}

cog_bool cog_input_mouseleftjustpressed()
{
    cog_bool just_pressed = mouseleftjustpressed;
    //Stop mouse from sticking
    mouseleftjustpressed = COG_FALSE;
    return just_pressed;
}

cog_bool cog_input_mouserightjustpressed()
{
    cog_bool just_pressed = mouserightjustpressed;
    //Stop mouse from sticking
    mouserightjustpressed = COG_FALSE;
    return just_pressed;
}

cog_float cog_input_mousex()
{
    return mousex;
}

cog_float cog_input_mousey()
{
    return mousey;
}

void cog_input_checkmouse(void)
{
    cog_int x,y;
    cog_uint state = SDL_GetMouseState(&x, &y);
    mousex = (cog_float)x;
    mousey = (cog_float)y;
    if(SDL_BUTTON_LEFT == SDL_BUTTON(state))
    {
        if(!mouseleftpressed)
        {
            mouseleftjustpressed = COG_TRUE;
        }
        else
        {
            mouseleftjustpressed = COG_FALSE;
        }
        mouseleftpressed = COG_TRUE;
    }
    else
    {
        mouseleftpressed = COG_FALSE;
    }
    if(SDL_BUTTON_RIGHT == SDL_BUTTON(state))
    {
        if(!mouserightpressed)
        {
            mouserightjustpressed = COG_TRUE;
        }
        else
        {
            mouserightjustpressed = COG_FALSE;
        }
        mouserightpressed = COG_TRUE;
    }
    else
    {
        mouserightpressed = COG_FALSE;
    }
}

//##keys
void cog_input_checkkeys(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                cog_quit();
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        cog_quit();
                        break;
                    case SDLK_f:
                        cog_window_togglefullscreen();
                        break;
                    default:
                        cog_debugf("WARNING: Unhandled keypress <%d>", (event.key.keysym.sym));
                        break;
                }
        }
    }
}
