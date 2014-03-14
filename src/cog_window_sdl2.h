#ifndef COG_WINDOW_SDL2_H
#define COG_WINDOW_SDL2_H

#include <SDL2/SDL.h>

#include "cog_types.h"
#include "cog_window.h"

void cog_window_sdl2_init(cog_config config, cog_window* window);

void cog_window_sdl2_update(cog_window* window);

void cog_window_sdl2_quit(cog_window* window);

void cog_window_sdl2_toggle_fullscreen(cog_window* window);

//internal
SDL_Window* cog_window_sdl2_get_window(void);

#endif
