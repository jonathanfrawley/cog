#include "cog_shape.h"

#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_list.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_types.h"

static cog_list active_rects;
static cog_map rects;
static cog_rect_id rectcnt;

cog_rect_id cog_rect_add_inactive(const char* img);

cog_rect_id cog_rect_add(const char* img) {
    cog_rect_id id = cog_rect_add_inactive(img);
    cog_rect* rect = cog_rect_get(id);
    cog_list_append(&active_rects, (cog_dataptr) & (rect->id));
    return id;
}

cog_rect_id cog_rect_add_inactive(const char* img) {
    cog_rect* rect = COG_STRUCT_MALLOC(cog_rect);
    rect->id = rectcnt++;
    rect->layer = COG_RECT_LAYER;
    cog_map_put(&rects, rect->id, (void*) rect);
    return rect->id;
}

double cog_rect_dist_rect(cog_rect_id a, cog_rect_id b) {
    cog_rect* arect = cog_rect_get(a);
    cog_rect* brect = cog_rect_get(b);
    return cog_math_sqrt((arect->pos.x - brect->pos.x) *
                         (arect->pos.x - brect->pos.x) + (arect->pos.y -
                                 brect->pos.
                                 y) *
                         (arect->pos.y - brect->pos.y));
}

bool cog_rect_collides_rect(cog_rect_id id0, cog_rect_id id1) {
    cog_rect* rect0 = cog_rect_get(id0);
    cog_rect* rect1 = cog_rect_get(id1);
    if(cog_rect_dist_rect(id0, id1) < (rect0->dim.w + rect1->dim.w)) {
        return COG_TRUE;
    } else {
        return COG_FALSE;
    }
}

cog_rect* cog_rect_get(cog_rect_id id) {
    return (cog_rect*) cog_map_get(&rects, id);
}

void cog_rect_remove(cog_rect_id id) {
    COG_LIST_FOREACH(&active_rects) {
        if(*((cog_rect_id*) curr->data) == id) {
            cog_list_remove(&active_rects, curr->data);
            break;
        }
    }
    cog_map_remove(&rects, id);
}

void cog_rect_removeall(void) {
    cog_list_removeall(&active_rects);
}

void cog_rect_set(cog_rect_id id, cog_rect src) {
    cog_rect* rect = cog_rect_get(id);
    rect->pos = src.pos;
    rect->dim = src.dim;
    rect->rot = src.rot;
    rect->vel = src.vel;
    rect->ang_vel = src.ang_vel;
    rect->col = src.col;
    rect->pixel_perfect = src.pixel_perfect;
    rect->update_func = src.update_func;
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_rect_init(void) {
    cog_map_init(&rects);
    cog_list_init(&active_rects, sizeof(cog_rect_id));
}

uint32_t cog_rect_draw_layer(uint32_t layer, uint32_t global_idx) {
    uint32_t idx = 0;
    //Draw rects
    COG_LIST_FOREACH(&active_rects) {
        //draw current rect if it is on the correct layer
        cog_rect* curr_rect = (cog_rect*) cog_map_get(&rects,
                                  *((cog_rect_id
                                     *)
                                    curr->data));
        if(curr_rect->layer == layer) {
            cog_graphics_draw_rect(curr_rect, global_idx + idx);
            idx++;
        }
    }
    return idx;
}

void cog_rect_update(double timestep) {
    COG_LIST_FOREACH(&active_rects) {
        cog_rect* curr_rect = (cog_rect*) cog_map_get(&rects,
                                  *((cog_rect_id
                                     *)
                                    curr->data));
        //do physics update for current rect
        curr_rect->pos.x += timestep * curr_rect->vel.x;
        curr_rect->pos.y += timestep * curr_rect->vel.y;
        curr_rect->rot += timestep * curr_rect->ang_vel;
        if(curr_rect->update_func != 0) {
            curr_rect->update_func(timestep, curr_rect);
        }
    }
}
