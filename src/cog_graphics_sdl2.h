#ifndef  COG_GRAPHICS_SDL2_H
#define  COG_GRAPHICS_SDL2_H

#include "cog_sprite.h"
#include "cog_text.h"
#include "cog_window.h"

void cog_graphics_sdl2_draw_sprite(cog_sprite* sprite);
void cog_graphics_sdl2_init(cog_window*);
void cog_graphics_sdl2_draw_text(cog_text* text);
uint32_t cog_graphics_sdl2_load_texture(const char* filename, int* width, int* height);
void cog_graphics_sdl2_clear();
void cog_graphics_sdl2_flush(void);

#endif
