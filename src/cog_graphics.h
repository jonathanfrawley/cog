#ifndef  COG_GRAPHICS_H
#define  COG_GRAPHICS_H

#include "cog_sprite.h"
#include "cog_text.h"
#include "cog_window.h"

#define COG_LAYER_MAX 16

typedef struct {
    void (*init)(cog_window*);
    void (*clear)(void);
    void (*draw_sprite)(cog_sprite* sprite);
    void (*draw_text)(cog_text* text);
    uint32_t (*load_texture)(const char* filename, int* width, int* height);
    uint32_t (*gen_tex_id)();
    void (*flush)(void);
} cog_renderer;

void cog_graphics_render(cog_window* window);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_graphics_init(cog_window*);
void cog_graphics_draw_sprite(cog_sprite* sprite);
void cog_graphics_draw_text(cog_text* text);
uint32_t cog_graphics_load_texture(const char* filename, int* width, int* height);
uint32_t cog_graphics_gen_tex_id();

#endif // COG_GRAPHICS_H
