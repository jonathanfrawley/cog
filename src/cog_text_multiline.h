#ifndef  COG_TEXT_MULTILINE_H
#define  COG_TEXT_MULTILINE_H

#include "cog_list.h"
#include "cog_types.h"

typedef struct {
    cog_text_multiline_id id;
    cog_list* text_ids;
} cog_text_multiline;

cog_text_multiline_id cog_text_multiline_add(cog_string str,
        cog_pos2 pos);

#endif   // COG_TEXT_MULTILINE_H
