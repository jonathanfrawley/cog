#ifndef  COG_TEXT_H
#define  COG_TEXT_H

#include "cog_core.h"
#include "cog_types.h"

typedef struct {
    cog_text_id id;
    //User attributes
    cog_pos2 pos;
    cog_dim2 dim;
    cog_dim2 scale;
    cog_color col;
    uint32_t layer;
    char str[COG_TEXT_MAX];
} cog_text;

cog_text_id cog_text_add();
cog_text* cog_text_get(cog_text_id id);
void cog_text_load_face(cog_string path, double pt_size);
void cog_text_remove(cog_text_id id);
void cog_text_removeall(void);
void cog_text_set(cog_text_id id, cog_text src);
void cog_text_set_str(cog_text_id id, char* str);
void cog_text_set_face(cog_text_id id, cog_string path, double pt_size);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_text_init(void);
void cog_text_draw_layer(uint32_t layer);

#endif // COG_TEXT_H
