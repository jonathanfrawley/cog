#include "cog_text_freetype.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "cog_graphics.h"
#include "cog_text.h"

/*-----------------------------------------------------------------------------
 *  Represents some text_freetype
 *-----------------------------------------------------------------------------*/
typedef struct {
    cog_text_id id;
    uint32_t tex_id;
    FT_Face face;
} cog_text_freetype;

static FT_Library font_library;
static FT_Face default_face;

static cog_map text_freetypes;
static cog_color default_color = {.r=1,.g=1,.b=1,.a=1};

cog_text_freetype_id cog_text_freetype_add(cog_text_id id) {
    cog_text_freetype* text_freetype = COG_STRUCT_MALLOC(cog_text_freetype);
    text_freetype->tex_id = cog_graphics_gen_tex_id();
    text_freetype->face = default_face;
    text_freetype->layer = COG_TEXT_LAYER;
    text_freetype->col = default_color;
    strcpy(text_freetype->str, "");
    cog_map_put(&text_freetypes, text_freetype->id, (cog_dataptr) text_freetype);
    cog_list_append(&activetext_freetypes, (cog_dataptr) & (text_freetype->id));
    return text_freetype->id;
}

FT_Face cog_text_freetype_load_face(cog_string path, double pt_size) {
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

void cog_text_freetype_set_face(cog_text_freetype_id id, cog_string path, double pt_size) {
    cog_text_freetype* text_freetype = cog_map_get(&text_freetypes, id);
    text_freetype->face = cog_text_freetype_load_face(path, pt_size);
}

cog_text_freetype* cog_text_freetype_get(cog_text_freetype_id id) {
    return (cog_text_freetype*) cog_map_get(&text_freetypes, id);
}

void cog_text_freetype_remove(cog_text_freetype_id id) {
    cog_map_remove(&text_freetypes, id);
}

void cog_text_freetype_init(void) {
    if(FT_Init_FreeType(&font_library)) {
        cog_errorf("Could not init freetype library");
    }
    cog_text_freetype_load_face(default_path, default_pt_size);
    cog_map_init(&text_freetypes);
}
