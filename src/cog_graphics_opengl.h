#ifndef COG_GRAPHICS_OPENGL
#define COG_GRAPHICS_OPENGL

#include "cog_sprite.h"
#include "cog_text.h"

void cog_graphics_opengl_draw_sprite(cog_sprite* sprite);
void cog_graphics_opengl_init(void);
void cog_graphics_opengl_draw_text(cog_text* text);
uint32_t cog_graphics_opengl_gen_tex_id();
uint32_t cog_graphics_opengl_load_texture(const char* filename, int* width, int* height);
void cog_graphics_opengl_clear();

#endif // COG_GRAPHICS_OPENGL
