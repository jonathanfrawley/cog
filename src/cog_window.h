#ifndef  COG_WINDOW_H
#define  COG_WINDOW_H

#include <SDL/SDL.h>
#if defined(HAVE_GLES)
#include "eglport.h"
#endif

typedef struct cog_window
{
    SDL_Surface* screen;
} cog_window;

/*-----------------------------------------------------------------------------
 *  Initialise a window.
 *-----------------------------------------------------------------------------*/
void cog_window_init(cog_window* window);

/*-----------------------------------------------------------------------------
 *  Toggle fullscreen on the window.
 *-----------------------------------------------------------------------------*/
void cog_window_toggle_fullscreen(cog_window* window);

#endif   // COG_WINDOW_H
