#include "cog_graphics.h"

#include "cog.h"
#include "cog_anim.h"
#include "cog_core.h"
#include "cog_list.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_sprite.h"
#ifdef USE_SDL
#include "cog_graphics_sdl2.h"
#else
#include "cog_graphics_opengl.h"
#endif

typedef struct {
    void (*init)(cog_window*);
    void (*clear)(void);
    void (*draw_sprite)(cog_sprite* sprite);
    void (*draw_text)(cog_text* text);
    uint32_t (*load_texture)(const char* filename, int* width, int* height);
    void (*flush)(void);
} cog_renderer;

static cog_renderer r;
static cog_map sprite_cache;

/*-----------------------------------------------------------------------------
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 *-----------------------------------------------------------------------------*/
void cog_graphics_draw_sprite(cog_sprite* sprite) {
    r.draw_sprite(sprite);
}

void cog_graphics_draw_text(cog_text* text) {
    r.draw_text(text);
}

uint32_t cog_graphics_load_texture(const char* filename, int* width, int* height) {
    //Cache textures so we don't load the same thing twice.
    void* item = cog_map_get_hash(&sprite_cache, filename);
    if(item != 0) {
        return *(uint32_t*)item;
    } else {
        uint32_t* tex_id = cog_malloc(sizeof(uint32_t));
        (*tex_id) = r.load_texture(filename, width, height);
        cog_map_put_hash(&sprite_cache, filename, (cog_dataptr)tex_id);
        return (*tex_id);
    }
}


void cog_graphics_init(cog_window* win) {
#ifdef USE_SDL
    r.draw_sprite = cog_graphics_sdl2_draw_sprite;
    r.init = cog_graphics_sdl2_init;
    r.draw_text = cog_graphics_sdl2_draw_text;
    r.load_texture = cog_graphics_sdl2_load_texture;
    r.clear = cog_graphics_sdl2_clear;
    r.flush = cog_graphics_sdl2_flush;
#else
    r.draw_sprite = cog_graphics_opengl_draw_sprite;
    r.init = cog_graphics_opengl_init;
    r.draw_text = cog_graphics_opengl_draw_text;
    r.load_texture = cog_graphics_opengl_load_texture;
    r.clear = cog_graphics_opengl_clear;
    r.flush = cog_graphics_opengl_flush;
#endif
    r.init(win);
    cog_map_init(&sprite_cache);
}

void cog_graphics_render(cog_window* window) {
    //Clear color buffer
    r.clear();
    //PASTE
    for(int i = 0; i < COG_LAYER_MAX; i++) {
        cog_sprite_draw_layer(i);
        cog_anim_draw_layer(i);
        cog_text_draw_layer(i);
    }
    r.flush();
}
