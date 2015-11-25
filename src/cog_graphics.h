#ifndef  COG_GRAPHICS_H
#define  COG_GRAPHICS_H

#include "cog_rect.h"
#include "cog_sprite.h"
#include "cog_text.h"
#include "cog_window.h"

#define COG_LAYER_MAX 16

void cog_graphics_render(cog_window* window);
void cog_graphics_cam_set(cog_pos2* pos);
void cog_graphics_cam_get(cog_pos2* pos);
void cog_graphics_cam_vel_set(cog_vec2* vel);
void cog_graphics_cam_vel_get(cog_vec2* vel);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_graphics_init(cog_window*);
void cog_graphics_draw_sprite(cog_sprite* sprite, uint32_t idx);
void cog_graphics_draw_rect(cog_rect* rect);
void cog_graphics_draw_text(cog_text* text);
uint32_t cog_graphics_load_texture(const char* filename, int* width, int* height);
uint32_t cog_graphics_gen_tex_id();
void cog_graphics_update(double timestep);

#endif // COG_GRAPHICS_H
