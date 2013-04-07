#include "cog_text_multiline.h"

#include <string.h>

#include "cog_log.h"
#include "cog_map.h"
#include "cog_text.h"

// TODO: Keep track of multiline strings.
static cog_text_multiline_id text_multiline_cnt;
static cog_float padding = 10.0f;

cog_text_multiline_id cog_text_multiline_add(cog_string str, 
        cog_float top_left_x, 
        cog_float top_left_y) {
    cog_text_multiline* text_multiline = COG_STRUCT_MALLOC(cog_text_multiline);
    text_multiline->id = text_multiline_cnt++;
    cog_string line = strtok(str, "\n");
    cog_list* text_list = cog_list_alloc(sizeof(cog_text_id));
    cog_int line_no = 0;
    while(line != COG_NULL) {
        cog_debugf("line is : %s", line);
        cog_text_id text_id = cog_text_add(line);
        cog_text* text = cog_text_get(text_id);
        // This is the important bit, move the line down on the y.
        // Add a bit of padding as it can look bad otherwise.
        text->pos.y = (((cog_float)line_no) * text->dim.h) + (((cog_float)line_no) * padding);
        cog_list_append(text_list, (cog_dataptr) &text_id);
        line = strtok(NULL, "\n");
        line_no++;
    }
    text_multiline->text_ids = text_list;
    return text_multiline->id;
}
