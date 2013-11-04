#include "cog_text.h"

#include "cog_log.h"
#include "cog_graphics.h"
#include "cog_list.h"
#include "cog_map.h"
#include "cog_math.h"

static cog_list activetexts;    //text drawn(active) at the moment
static cog_map texts;
static cog_text_id textcnt;

static FT_Library font_library;
static FT_Face default_face;

static const cog_string default_path = "media/font/04B_03__.ttf";
static uint32_t default_pt_size = 48;
static cog_color default_color = {.r=1,.g=1,.b=1,.a=1};


FT_Face cog_text_load_face(cog_string path, double pt_size);

cog_text_id cog_text_add() {
    cog_text* text = COG_STRUCT_MALLOC(cog_text);
    text->id = textcnt++;
    text->tex_id = cog_graphics_gen_tex_id();
    text->face = default_face;
    text->layer = COG_TEXT_LAYER;
    text->col = default_color;
    strcpy(text->str, "");
    cog_map_put(&texts, text->id, (cog_dataptr) text);
    cog_list_append(&activetexts, (cog_dataptr) & (text->id));
    return text->id;
}

FT_Face cog_text_load_face(cog_string path, double pt_size) {
    FT_Face face;
    if(FT_New_Face(font_library, path, 0, &face)) {
        cog_errorf("Could not open font");
    }
    FT_Set_Pixel_Sizes(face, 0, pt_size);
    if(FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        cog_errorf("Could not load character 'X'");
    }
    return face;
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
}

void cog_text_removeall(void) {
    cog_list_removeall(&activetexts);
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/

void cog_text_init(void) {
    if(FT_Init_FreeType(&font_library)) {
        cog_errorf("Could not init freetype library");
    }
    default_face = cog_text_load_face(default_path, default_pt_size);
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
