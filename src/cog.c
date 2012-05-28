#include "cog.h"
#include <stdarg.h>
#include <assert.h>
#include <SDL/SDL.h>

//constants
static COG_MAX_BUF = 255;

//data structures
typedef struct
{
    int finished;
} cog_game;
static cog_game game;

typedef struct
{
    SDL_Surface* screen;
} cog_window;
static cog_window window;

//prototypes
void cog_errorf(const char* logMsg, ...);
void cog_platform_init(void);
void cog_window_init(void);
void cog_window_togglefullscreen(void);
void cog_checkkeys(void);

//implementations
void cog_init()
{
    game.finished = 0;
    cog_platform_init();
    cog_window_init();
}

//This is the cog default loop, can be overrided by just using cog_loopstep instead.
void cog_mainloop()
{
    while(!game.finished)
    {
        cog_loopstep();
    }
}

//This is to allow the user to control the mainloop
void cog_loopstep()
{
    cog_checkkeys();
}

void cog_destroy()
{
    game.finished = 1;
}

//local
void cog_errorf(const char* logMsg, ...)
{
    va_list ap;
    va_start(ap, logMsg);
    char buf[COG_MAX_BUF];
    vsprintf(buf, logMsg, ap);
    printf("CRITICAL: %s \n", buf);
    cog_destroy();
    assert(0);
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
    int width = 800;
    int height = 600;
    int bpp = 32;
    int flags = SDL_OPENGL | SDL_DOUBLEBUF;
    if( (window.screen = SDL_SetVideoMode( width, height, bpp, flags )) == 0 )
    {
        cog_errorf("cog_window_init failed when creating SDL window <%s> \n", SDL_GetError());
    }

    SDL_WM_SetCaption( "cog game", NULL );
}

void cog_window_togglefullscreen(void)
{
    SDL_WM_ToggleFullScreen(window.screen);
}

//keys
void cog_checkkeys(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                cog_destroy();
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        cog_destroy();
                        break;
                    case SDLK_f:
                        cog_window_togglefullscreen();
                        break;
                }
        }
    }
}
