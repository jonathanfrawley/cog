#include "cog_window.h"

#include "cog_log.h"

void cog_window_init(cog_window* window)
{
    //TODO:Get from yaml conf.
    int width = 1024;
    int height = 768;
    //int width = 0;
    //int height = 0;
    int bpp = 32;
    int flags = SDL_OPENGL | SDL_DOUBLEBUF;
    if( (window->screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0 )
    {
        cog_errorf("cog_window_init failed when creating SDL window <%s> \n", SDL_GetError());
    }

    SDL_WM_SetCaption( "cog game", NULL );
}

void cog_window_toggle_fullscreen(cog_window* window)
{
    SDL_WM_ToggleFullScreen(window->screen);
}
