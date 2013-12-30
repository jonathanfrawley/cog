#include "cog_text.h"

#include "cog_log.h"
#include "cog_list.h"
#include "cog_graphics.h"
#include "cog_map.h"
#include "cog_math.h"

#ifdef USE_SDL
#include "cog_text_sdl2.h"
#else
#include "cog_text_freetype.h"
#endif

typedef struct {
    void (*text_init)(void);
    cog_text_id (*text_add)(cog_text_id id);
    cog_text* (*text_get)(cog_text_id id);
    void (*text_remove)(cog_text_id id);
    void (*text_set_face)(cog_text_id id, cog_string path, double pt_size);
} cog_text_renderer;

static cog_text_renderer renderer;
static cog_list activetexts;    //text drawn(active) at the moment
static cog_map texts;
static cog_text_id textcnt;

static const cog_string default_path = "media/font/04B_03__.ttf";
//static uint32_t default_pt_size = 48;

cog_text_id cog_text_add() {
    cog_text_id id = textcnt++;
    renderer.text_add(id);
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
    strcpy(text->str, src.str);
}

void cog_text_set_str(cog_text_id id, char* str) {
    cog_text* text = cog_text_get(id);
    strcpy(text->str, str);
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
#ifdef USE_SDL
    //TODO:Implement
    /*
    renderer->text_init = cog_text_sdl2_init;
    renderer->text_add = cog_text_sdl2_add;
    renderer->text_get = cog_text_sdl2_get;
    renderer->text_remove = cog_text_sdl2_remove;
    renderer->text_set_face = cog_text_sdl2_set_face;
    */
#else
    renderer.text_init = cog_text_freetype_init;
    renderer.text_add = cog_text_freetype_add;
    renderer.text_get = cog_text_freetype_get;
    renderer.text_remove = cog_text_freetype_remove;
    renderer.text_set_face = cog_text_freetype_set_face;
#endif
    //TODO
    //cog_text_set_face(default_path, default_pt_size);
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
