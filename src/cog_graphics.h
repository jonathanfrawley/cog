#ifndef  COG_GRAPHICS_H
#define  COG_GRAPHICS_H

#include "cog_sprite.h"
#include "cog_text.h"
#include "cog_window.h"

#define COG_LAYER_MAX 16

void cog_graphics_render(cog_window* window);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_graphics_init(cog_window*);
void cog_graphics_draw_sprite(cog_sprite* sprite, uint32_t idx);
void cog_graphics_draw_text(cog_text* text);
uint32_t cog_graphics_load_texture(const char* filename, int* width, int* height);
uint32_t cog_graphics_gen_tex_id();

#endif // COG_GRAPHICS_H
