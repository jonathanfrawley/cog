#ifndef COG_WINDOW_GLES_H
#define COG_WINDOW_GLES_H

#include "cog_window.h"

void cog_window_gles_init(cog_config config, cog_window* window);
void cog_window_gles_update(cog_window* window);
void cog_window_gles_quit(cog_window* window);
void cog_window_gles_toggle_fullscreen(cog_window* window);

#endif // COG_WINDOW_GLES_H
