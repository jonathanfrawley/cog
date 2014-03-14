#ifndef  COG_WINDOW_H
#define  COG_WINDOW_H

#include "cog_types.h"

typedef struct cog_window {
    double w;
    double h;
} cog_window;

/*-----------------------------------------------------------------------------
 *  Initialise a window.
 *-----------------------------------------------------------------------------*/
void cog_window_init(cog_config config, cog_window* window);

/*-----------------------------------------------------------------------------
 *  Main update.
 *-----------------------------------------------------------------------------*/
void cog_window_update(cog_window* window);

/*-----------------------------------------------------------------------------
 *  Shuts things down gracefully
 *-----------------------------------------------------------------------------*/
void cog_window_quit(cog_window* window);

/*-----------------------------------------------------------------------------
 *  Toggle fullscreen on the window.
 *-----------------------------------------------------------------------------*/
void cog_window_toggle_fullscreen(cog_window* window);

#endif // COG_WINDOW_H
