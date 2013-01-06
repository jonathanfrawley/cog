#include "cog_window.h"

#include "cog_log.h"

void cog_window_init(cog_window* window)
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        cog_errorf(SDL_GetError());
    }
#if defined(HAVE_GLES)
    if(EGL_Open())
    {
        cog_errorf("Failed initialising EGL");
    }
#endif

    //TODO:Get from yaml conf.
#if !defined(HAVE_GLES)
    int width = 1024;
    int height = 768;
    //int width = 0;
    //int height = 0;
    int bpp = 32;
    int flags = SDL_OPENGL | SDL_DOUBLEBUF;
#else
    int width = 800;
    int height = 480;
    int bpp = 0;
    int flags = SDL_SWSURFACE | SDL_FULLSCREEN;
#endif
    if( (window->screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0 )
    {
        cog_errorf("cog_window_init failed when creating SDL window <%s> \n", SDL_GetError());
    }

#if defined(HAVE_GLES)
    EGL_Init();
#endif

    SDL_WM_SetCaption( "cog game", NULL );
}

void cog_window_toggle_fullscreen(cog_window* window)
{
    SDL_WM_ToggleFullScreen(window->screen);
}
