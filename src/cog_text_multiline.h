#ifndef  COG_TEXT_MULTILINE_H
#define  COG_TEXT_MULTILINE_H

#include "cog_list.h"
#include "cog_types.h"

typedef struct {
    cog_text_multiline_id id;
    cog_list* text_ids;
} cog_text_multiline;

cog_text_multiline_id cog_text_multiline_add(cog_string str, 
        cog_float top_left_x, 
        cog_float top_left_y);

#endif   // COG_TEXT_MULTILINE_H
