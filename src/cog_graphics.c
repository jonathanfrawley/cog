#include "cog_graphics.h"

#include "cog.h"
#include "cog_anim.h"
#include "cog_core.h"
#include "cog_list.h"
#include "cog_math.h"
#include "cog_sprite.h"
#ifdef OPENGL_RENDERING
#include "cog_graphics_opengl.c"
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
    r->draw_text();
}

uint32_t cog_graphics_gen_tex_id() {
    r->gen_tex_id();
}

void cog_graphics_init(void) {
    r = COG_STRUCT_NEW(cog_renderer);
#ifdef OPENGL_RENDERING
    //TODO : Setup opengl renderer here.
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
