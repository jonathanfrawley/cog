#include "cog_text_multiline.h"

#include <string.h>

#include "cog_log.h"
#include "cog_map.h"
#include "cog_text.h"

// TODO: Keep track of multiline strings.
static cog_text_multiline_id text_multiline_cnt;
static double padding = 0.1f;
static cog_map multiline_texts;

cog_text_multiline_id cog_text_multiline_add() {
    cog_text_multiline* text_multiline = COG_STRUCT_MALLOC(cog_text_multiline);
    text_multiline->id = text_multiline_cnt++;
    cog_map_put(&multiline_texts, text_multiline->id, (cog_dataptr) text_multiline);
    return text_multiline->id;
}

void cog_text_multiline_init(void) {
    cog_map_init(&multiline_texts);
}

cog_text_multiline* cog_text_multiline_get(cog_text_multiline_id id) {
    return (cog_text_multiline*) cog_map_get(&multiline_texts, id);
}

void cog_text_multiline_set(cog_text_multiline_id id, cog_text_multiline src) {
    cog_text_multiline* text = cog_text_multiline_get(id);
    text->pos = src.pos;
    text->dim = src.dim;
    text->col = src.col;
    strcpy(text->str, src.str);
    char* str = text->str;
    cog_string line = strtok(str, "\n");
    cog_list* text_list = cog_list_alloc(sizeof(cog_text_id));
    int line_no = 0;
    while(line != COG_NULL) {
        cog_text_id text_id = cog_text_add();
        // this is the important bit, move the line down on the y.
        // add a bit of padding as it can look bad otherwise.
        cog_text tmp = (cog_text) {
            .pos = (cog_pos2) {
                .x=text->pos.x, .y=text->pos.y - ((((double)line_no) * src.dim.h) +
                                                  (((double)line_no) * padding))
            },
            .dim = text->dim,
             .col = text->col,
        };
        strcpy(tmp.str, line);
        cog_text_set(text_id, tmp);
        cog_list_append(text_list, (cog_dataptr) &text_id);
        line = strtok(COG_NULL, "\n");
        line_no++;
    }
    text->text_ids = text_list;
}
