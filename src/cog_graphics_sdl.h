#ifndef COG_GRAPHICS_SDL_H
#define COG_GRAPHICS_SDL_H

#include "cog_sprite.h"
#include "cog_text.h"
#include "cog_types.h"
#include "cog_window.h"

void cog_graphics_sdl_clear();
void cog_graphics_sdl_draw();
void cog_graphics_sdl_draw_sprite(cog_sprite* sprite, uint32_t idx);
void cog_graphics_sdl_draw_text(cog_text* text);
void cog_graphics_sdl_flush();
void cog_graphics_sdl_init(cog_window* win);
uint32_t cog_graphics_sdl_gen_tex_id();
uint32_t cog_graphics_sdl_load_texture(const char* filename, int* width, int* height);
void cog_graphics_sdl_prepare(uint32_t amount);
void cog_graphics_sdl_set_camera_pos(cog_pos2* pos);


#endif // COG_GRAPHICS_SDL_H
