#include "cog_text_freetype.h"

#include "cog_graphics.h"
#include "cog_log.h"
#include "cog_map.h"
#include "cog_text.h"

static FT_Library font_library;
static FT_Face default_face;

static cog_map text_freetypes;

static const cog_string default_path = "../assets/fonts/04B_03__.ttf";
static uint32_t default_pt_size = 48;

void cog_text_freetype_add(cog_text_id id) {
    cog_text_freetype* text_freetype = COG_STRUCT_MALLOC(cog_text_freetype);
    text_freetype->id = id;
    text_freetype->tex_id = cog_graphics_gen_tex_id();
    text_freetype->face = default_face;
    cog_map_put(&text_freetypes, id, (cog_dataptr) text_freetype);
}

FT_Face cog_text_freetype_load_face(cog_string path, double pt_size) {
    FT_Face face;
    if(FT_New_Face(font_library, path, 0, &face)) {
        cog_errorf("Could not open font %s", path);
    }
    FT_Set_Pixel_Sizes(face, 0, pt_size);
    if(FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        cog_errorf("Could not load character 'X'");
    }
    return face;
}

void cog_text_freetype_set_face(cog_text_id id, cog_string path, double pt_size) {
    cog_text_freetype* text_freetype = cog_map_get(&text_freetypes, id);
    text_freetype->face = cog_text_freetype_load_face(path, pt_size);
}

cog_text_freetype* cog_text_freetype_get(cog_text_id id) {
    return (cog_text_freetype*) cog_map_get(&text_freetypes, id);
}

void cog_text_freetype_remove(cog_text_id id) {
    cog_map_remove(&text_freetypes, id);
}

void cog_text_freetype_init(void) {
    if(FT_Init_FreeType(&font_library)) {
        cog_errorf("Could not init freetype library");
    }
    default_face = cog_text_freetype_load_face(default_path, default_pt_size);
    cog_map_init(&text_freetypes);
}
