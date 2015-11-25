#ifndef COG_GRAPHICS_OPENGL
#define COG_GRAPHICS_OPENGL

#include "cog_rect.h"
#include "cog_sprite.h"
#include "cog_text.h"
#include "cog_types.h"
#include "cog_window.h"

void cog_graphics_opengl_clear();
void cog_graphics_opengl_draw();
void cog_graphics_opengl_draw_rect(cog_rect* sprite);
void cog_graphics_opengl_draw_sprite(cog_sprite* sprite, uint32_t idx);
void cog_graphics_opengl_draw_text(cog_text* text);
void cog_graphics_opengl_flush();
void cog_graphics_opengl_init(cog_window* win);
uint32_t cog_graphics_opengl_gen_tex_id();
uint32_t cog_graphics_opengl_load_texture(const char* filename, int* width, int* height);
void cog_graphics_opengl_prepare(uint32_t amount);
void cog_graphics_opengl_set_camera_pos(cog_pos2* pos);

//internal
uint32_t cog_graphics_opengl_gen_tex_id();

#endif // COG_GRAPHICS_OPENGL
