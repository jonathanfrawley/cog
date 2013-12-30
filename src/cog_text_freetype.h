#ifndef  COG_TEXT_FREETYPE_H
#define  COG_TEXT_FREETYPE_H

#include "cog_core.h"
#include "cog_types.h"

cog_text_freetype_id cog_text_freetype_add(cog_text_id id);
void cog_text_freetype_set_face(cog_text_freetype_id id, cog_string path, double pt_size);
cog_text_freetype* cog_text_freetype_get(cog_text_freetype_id id);
void cog_text_freetype_remove(cog_text_freetype_id id);
void cog_text_freetype_init(void);

#endif
