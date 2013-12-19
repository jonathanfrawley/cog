#include "cog_graphics.h"

#include "cog.h"
#include "cog_anim.h"
#include "cog_core.h"
#include "cog_list.h"
#include "cog_math.h"
#include "cog_sprite.h"
#ifdef OPENGL_RENDERING
#include "cog_graphics_opengl.h"
#endif

cog_renderer* r;

/*-----------------------------------------------------------------------------
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 *-----------------------------------------------------------------------------*/
void cog_graphics_draw_sprite(cog_sprite* sprite) {
    r->draw_sprite(sprite);
}

void cog_graphics_draw_text(cog_text* text) {
    r->draw_text(text);
}

uint32_t cog_graphics_gen_tex_id() {
    return r->gen_tex_id();
}

uint32_t cog_graphics_load_texture(const char* filename, int* width, int* height) {
    return r->load_texture(filename, width, height);
}


void cog_graphics_init(void) {
    r = COG_STRUCT_MALLOC(cog_renderer);
#ifdef OPENGL_RENDERING
    //TODO : Setup opengl renderer here.
    r->draw_sprite = cog_graphics_opengl_draw_sprite;
    r->init = cog_graphics_opengl_init;
    r->draw_text = cog_graphics_opengl_draw_text;
    r->gen_tex_id = cog_graphics_opengl_gen_tex_id;
    r->load_texture = cog_graphics_opengl_load_texture;
    r->clear = cog_graphics_opengl_clear;
#else
    //TODO : Setup SDL2 renderer here.
#endif
    r->init();
}

void cog_graphics_render(cog_window* window) {
    //Clear color buffer
    r->clear();
    //PASTE
    for(int i = 0; i < COG_LAYER_MAX; i++) {
        cog_sprite_draw_layer(i);
        cog_anim_draw_layer(i);
        cog_text_draw_layer(i);
    }
}
