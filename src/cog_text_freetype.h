#ifndef  COG_TEXT_FREETYPE_H
#define  COG_TEXT_FREETYPE_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "cog_core.h"
#include "cog_types.h"

cog_text_id cog_text_freetype_add(cog_text_id id);
void cog_text_freetype_set_face(cog_text_id id, cog_string path, double pt_size);
void cog_text_freetype_remove(cog_text_id id);
void cog_text_freetype_init(void);

//Internal
typedef struct {
    cog_text_id id;
    uint32_t tex_id;
    FT_Face face;
} cog_text_freetype;

cog_text_freetype* cog_text_freetype_get(cog_text_id id);

#endif
