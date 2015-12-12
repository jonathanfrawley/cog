#ifndef  COG_SHAPE_H
#define  COG_SHAPE_H

#include "cog_types.h"

#define COG_RECT_LAYER 2

typedef struct cog_rect {
    cog_rect_id id;
    uint32_t layer;
    //User attributes
    cog_pos2 pos;
    cog_dim2 dim;
    double rot;
    cog_vec2 vel;
    double ang_vel;
    bool pixel_perfect;
    cog_color col;
    void (*update_func)(double timestep, struct cog_rect* anim);
} cog_rect;

cog_rect_id cog_rect_add();
bool cog_rect_collides_rect(cog_rect_id id0, cog_rect_id id1);
uint32_t cog_rect_draw_layer(uint32_t layer);
cog_rect* cog_rect_get(cog_rect_id);
uint32_t cog_rect_len(uint32_t layer);
void cog_rect_remove(cog_rect_id id);
void cog_rect_removeall(void);
void cog_rect_update(double timestep);
void cog_rect_set(cog_rect_id id, cog_rect src);

void cog_rect_init(void);

#endif // COG_SHAPE_H
