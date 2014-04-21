#include "cog_graphics.h"

#include "cog.h"
#include "cog_anim.h"
#include "cog_core.h"
#include "cog_defs.h"
#include "cog_list.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_sprite.h"

#if COG_RENDERER==COG_RENDERER_SDL
#include "cog_graphics_sdl.h"
#elif COG_RENDERER==COG_RENDERER_SDL2
#include "cog_graphics_sdl2.h"
#elif COG_RENDERER==COG_RENDERER_GLES
#include "cog_graphics_gles.h"
#else
#include "cog_graphics_opengl.h"
#endif

typedef struct {
    void (*init)(cog_window*);
    void (*clear)(void);
    void (*draw)(void);
    void (*draw_sprite)(cog_sprite* sprite, uint32_t idx);
    void (*draw_text)(cog_text* text);
    uint32_t (*gen_tex_id)(void);
    uint32_t (*load_texture)(const char* filename, int* width, int* height);
    void (*prepare)(uint32_t amount);
    void (*set_camera_pos)(cog_pos2* pos);
    void (*flush)(void);
} cog_renderer;

static cog_renderer r;
static cog_map sprite_cache;
static cog_list texture_list;
static cog_vec2 camera_vel;
static cog_pos2 camera_pos;

/*-----------------------------------------------------------------------------
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 *-----------------------------------------------------------------------------*/
void cog_graphics_draw_sprite(cog_sprite* sprite, uint32_t idx) {
#if GRAPHICS_DISABLED
#else
    r.draw_sprite(sprite, idx);
#endif
}

void cog_graphics_draw_text(cog_text* text) {
#if GRAPHICS_DISABLED
#else
    r.draw_text(text);
#endif
}

uint32_t cog_graphics_load_texture(const char* filename, int* width, int* height) {
#if GRAPHICS_DISABLED
    return 0;
#else
    //Cache textures so we don't load the same thing twice.
    void* item = cog_map_get_hash(&sprite_cache, filename);
    if(item != 0) {
        return *(uint32_t*)item;
    } else {
        uint32_t* tex_id = cog_malloc(sizeof(uint32_t));
        (*tex_id) = r.load_texture(filename, width, height);
        cog_map_put_hash(&sprite_cache, filename, (cog_dataptr)tex_id);
        cog_list_append(&texture_list, (cog_dataptr)tex_id);
        cog_debugf("Inserting tex_id %d into list for filename %s", *tex_id, filename);
        return (*tex_id);
    }
#endif
}


void cog_graphics_init(cog_window* win) {
#if GRAPHICS_DISABLED
#else
#if COG_RENDERER==COG_RENDERER_SDL2
    r.draw_sprite = cog_graphics_sdl2_draw_sprite;
    r.init = cog_graphics_sdl2_init;
    r.draw_text = cog_graphics_sdl2_draw_text;
    r.load_texture = cog_graphics_sdl2_load_texture;
    r.set_camera_pos = 0; //TODO: Implement
    r.clear = cog_graphics_sdl2_clear;
    r.flush = cog_graphics_sdl2_flush;
#elif COG_RENDERER==COG_RENDERER_SDL
    r.draw = cog_graphics_sdl_draw;
    r.draw_sprite = cog_graphics_sdl_draw_sprite;
    r.init = cog_graphics_sdl_init;
    r.draw_text = cog_graphics_sdl_draw_text;
    r.load_texture = cog_graphics_sdl_load_texture;
    r.prepare = cog_graphics_sdl_prepare;
    r.set_camera_pos = cog_graphics_sdl_set_camera_pos;
    r.clear = cog_graphics_sdl_clear;
    r.flush = cog_graphics_sdl_flush;
#elif COG_RENDERER==COG_RENDERER_GLES
    r.draw = cog_graphics_gles_draw;
    r.draw_sprite = cog_graphics_gles_draw_sprite;
    r.init = cog_graphics_gles_init;
    r.draw_text = cog_graphics_gles_draw_text;
    r.gen_tex_id = cog_graphics_gles_gen_tex_id;
    r.load_texture = cog_graphics_gles_load_texture;
    r.prepare = cog_graphics_gles_prepare;
    r.set_camera_pos = cog_graphics_gles_set_camera_pos;
    r.clear = cog_graphics_gles_clear;
    r.flush = cog_graphics_gles_flush;
#else
    r.draw = cog_graphics_opengl_draw;
    r.draw_sprite = cog_graphics_opengl_draw_sprite;
    r.init = cog_graphics_opengl_init;
    r.draw_text = cog_graphics_opengl_draw_text;
    r.gen_tex_id = cog_graphics_opengl_gen_tex_id;
    r.load_texture = cog_graphics_opengl_load_texture;
    r.prepare = cog_graphics_opengl_prepare;
    r.set_camera_pos = cog_graphics_opengl_set_camera_pos;
    r.clear = cog_graphics_opengl_clear;
    r.flush = cog_graphics_opengl_flush;
#endif
    r.init(win);
    cog_map_init(&sprite_cache);
    cog_list_init(&texture_list, sizeof(uint32_t));
    //camera_pos.x = 1.0;
#endif
}

void cog_graphics_update(double timestep) {
#if GRAPHICS_DISABLED
#else
    camera_pos.x += camera_vel.x * timestep;
    camera_pos.y += camera_vel.y * timestep;
#endif
}

void cog_graphics_render(cog_window* window) {
#if GRAPHICS_DISABLED
#else
    //Clear color buffer
    r.clear();
    r.set_camera_pos(&camera_pos);
    for(int i = 0; i < COG_LAYER_MAX; i++) {
        COG_LIST_FOREACH(&texture_list) {
            uint32_t tex_id = *((uint32_t*)curr->data);
            uint32_t cnt = cog_sprite_len(tex_id, i) + cog_anim_len(tex_id, i);
            if(cnt > 0) {
                r.prepare(cnt);
                uint32_t global_idx = 0;
                global_idx += cog_sprite_draw_layer(i, tex_id, global_idx);
                global_idx += cog_anim_draw_layer(i, tex_id, global_idx);
                r.draw();
            }
        }
        cog_text_draw_layer(i);
    }
    r.flush();
#endif
}

void cog_graphics_cam_set(cog_pos2* pos) {
#if GRAPHICS_DISABLED
#else
    camera_pos = (*pos);
#endif
}

void cog_graphics_cam_get(cog_pos2* pos) {
#if GRAPHICS_DISABLED
#else
    (*pos) = camera_pos;
#endif
}

void cog_graphics_cam_vel_set(cog_vec2* vel) {
#if GRAPHICS_DISABLED
#else
    camera_vel = (*vel);
#endif
}

void cog_graphics_cam_vel_get(cog_vec2* vel) {
#if GRAPHICS_DISABLED
#else
    (*vel) = camera_vel;
#endif
}

uint32_t cog_graphics_gen_tex_id() {
    return r.gen_tex_id();
}
