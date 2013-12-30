#ifndef  COG_TEXT_SDL2_H
#define  COG_TEXT_SDL2_H

cog_text_freetype_id cog_text_freetype_add(cog_text_id id);
void cog_text_freetype_set_face(cog_text_freetype_id id, cog_string path, double pt_size);
void cog_text_freetype_remove(cog_text_freetype_id id);
void cog_text_freetype_init(void);


#endif
