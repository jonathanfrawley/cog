#ifndef COG_WINDOW_SDL_H
#define COG_WINDOW_SDL_H

#include "cog_window.h"

void cog_window_sdl_init(cog_config config, cog_window* window);

void cog_window_sdl_update(cog_window* window);

void cog_window_sdl_quit(cog_window* window);

void cog_window_sdl_toggle_fullscreen(cog_window* window);

#endif
