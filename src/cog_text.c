#include "cog_text.h"

#include "cog_defs.h"
#include "cog_log.h"
#include "cog_list.h"
#include "cog_graphics.h"
#include "cog_map.h"
#include "cog_math.h"

#if COG_TEXT==COG_TEXT_SDL
#include "cog_text_freetype.h"
#elif COG_TEXT==COG_TEXT_SDL2
#include "cog_text_sdl2.h"
#else
#include "cog_text_freetype.h"
#endif

#define COG_TEXT_LAYER 3

typedef struct {
    void (*text_init)(void);
    void (*text_add)(cog_text_id id);
    void (*text_remove)(cog_text_id id);
    void (*text_set_face)(cog_text_id id, cog_string path, double pt_size);
} cog_text_renderer;

static cog_text_renderer renderer;
static cog_list activetexts;    //text drawn(active) at the moment
static cog_map texts;
static cog_text_id textcnt;

static cog_color default_color = {.r=1,.g=1,.b=1,.a=1};

cog_text_id cog_text_add() {
    cog_text_id id = textcnt++;
    renderer.text_add(id);
    cog_text* text = COG_STRUCT_MALLOC(cog_text);
    text->layer = COG_TEXT_LAYER;
    text->id = id;
    text->col = default_color;
    strcpy(text->str, "");
    cog_map_put(&texts, text->id, (cog_dataptr) text);
    cog_list_append(&activetexts, (cog_dataptr) & (text->id));
    return id;
}

void cog_text_set_face(cog_text_id id, cog_string path, double pt_size) {
    renderer.text_set_face(id, path, pt_size);
}

cog_text* cog_text_get(cog_text_id id) {
    return (cog_text*) cog_map_get(&texts, id);
}

void cog_text_set(cog_text_id id, cog_text src) {
    cog_text* text = cog_text_get(id);
    text->pos = src.pos;
    text->dim = src.dim;
    text->scale = src.scale;
    text->col = src.col;
}

void cog_text_set_str(cog_text_id id, char* str, ...) {
    cog_text* text = cog_text_get(id);
    va_list ap;
    va_start(ap, str);
    vsprintf(text->str, str, ap);
    //strcpy(text->str, str);
#ifdef USE_LEGACY_SDL
    //Flag update to sdl so it can reupload to GPU
//    cog_text_sdl_update(id, text->str);
#endif
}

void cog_text_remove(cog_text_id id) {
    COG_LIST_FOREACH(&activetexts) {
        if(*((cog_text_id*) curr->data) == id) {
            cog_list_remove(&activetexts, curr->data);
            break;
        }
    }
    cog_map_remove(&texts, id);
    renderer.text_remove(id);
}

void cog_text_removeall(void) {
    COG_LIST_FOREACH(&activetexts) {
        renderer.text_remove(*(cog_text_id*) curr->data);
    }
    cog_list_removeall(&activetexts);
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/

void cog_text_init(void) {
#if COG_TEXT==COG_TEXT_SDL
    renderer.text_init = cog_text_freetype_init;
    renderer.text_add = cog_text_freetype_add;
    renderer.text_remove = cog_text_freetype_remove;
    renderer.text_set_face = cog_text_freetype_set_face;
    /*
    renderer.text_init = cog_text_sdl_init;
    renderer.text_add = cog_text_sdl_add;
    renderer.text_remove = cog_text_sdl_remove;
    renderer.text_set_face = cog_text_sdl_set_face;
    */
#elif COG_TEXT==COG_TEXT_SDL2
    renderer.text_init = cog_text_sdl2_init;
    renderer.text_add = cog_text_sdl2_add;
    renderer.text_remove = cog_text_sdl2_remove;
    renderer.text_set_face = cog_text_sdl2_set_face;
#else
    renderer.text_init = cog_text_freetype_init;
    renderer.text_add = cog_text_freetype_add;
    renderer.text_remove = cog_text_freetype_remove;
    renderer.text_set_face = cog_text_freetype_set_face;
#endif
    //TODO
    //cog_text_set_face(default_path, default_pt_size);
    renderer.text_init();
    cog_map_init(&texts);
    cog_list_init(&activetexts, sizeof(cog_text_id));
}

void cog_text_draw_layer(uint32_t layer) {
    COG_LIST_FOREACH(&activetexts) {
        cog_text_id id = *(cog_text_id*) curr->data;
        cog_text* text = cog_text_get(id);
        if(text->layer == layer) {
            cog_graphics_draw_text(text);
        }
    }
}
