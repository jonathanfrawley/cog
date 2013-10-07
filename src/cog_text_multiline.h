#ifndef  COG_TEXT_MULTILINE_H
#define  COG_TEXT_MULTILINE_H

#include "cog_list.h"
#include "cog_types.h"

typedef struct {
    cog_text_multiline_id id;
    cog_list* text_ids;
    //User attributes
    cog_pos2 pos;
    cog_dim2 dim;
    cog_float rot;
    cog_color col;
    char str[COG_TEXT_MAX];
} cog_text_multiline;

cog_text_multiline_id cog_text_multiline_add();
cog_text_multiline* cog_text_multiline_get(cog_text_multiline_id id);
void cog_text_multiline_init(void);
void cog_text_multiline_set(cog_text_multiline_id id, cog_text_multiline src);

#endif   // COG_TEXT_MULTILINE_H
