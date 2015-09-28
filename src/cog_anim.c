#include "cog_anim.h"

#include <stdio.h>
#include <assert.h>

#include "cog_core.h"
#include "cog_log.h"
#include "cog_graphics.h"
#include "cog_map.h"
#include "cog_math.h"
#include "cog_sprite.h"

#define MAX_KEY 255

static cog_anim_id animcnt;
static cog_map anims;
static cog_list active_anims;   //anims drawn(active) at the moment
static cog_map tex_anim_lens;

uint32_t cog_anim_get_len_key(uint32_t tex_id, uint32_t layer);
void cog_anim_decrease_tex_anim_len(cog_anim* anim);
void cog_anim_increase_tex_anim_len(cog_anim* anim);

typedef struct cog_len_map_entry {
    uint32_t tex_id;
    uint32_t layer;
    uint32_t cnt;
} cog_len_map_entry;

/*-----------------------------------------------------------------------------
 * Assumes animation is a single 1D animation frame.
 *-----------------------------------------------------------------------------*/
cog_anim_id cog_anim_add(const char* img, uint32_t rows, uint32_t cols) {
    cog_anim* anim = COG_STRUCT_MALLOC(cog_anim);
    anim->id = animcnt++;
    anim->layer = COG_ANIM_LAYER;
    cog_list_init(&anim->frames, sizeof(cog_sprite));
    anim->n_frames = rows * cols;
    double h_frame = (1.0 / (double)rows);
    double w_frame = (1.0 / (double)cols);
    //Load nimages sprites in, with offset dependant on frame number.
    for(int i = 0; i < anim->n_frames; i++) {
        cog_sprite_id sid = cog_sprite_add_inactive(img);
        cog_sprite* sprite = cog_sprite_get(sid);
        anim->tex_id = sprite->tex_id;
        sprite->tex_pos.x = (i % cols) * w_frame;
        sprite->tex_pos.y = ((cols - 1) - (i / cols)) * h_frame;
        sprite->tex_dim.w = w_frame;
        sprite->tex_dim.h = h_frame;
        cog_list_append(&(anim->frames), sprite);
    }
    cog_map_put(&anims, anim->id, (void*) anim);
    cog_anim_increase_tex_anim_len(anim);
    cog_list_append(&active_anims, (cog_dataptr) & (anim->id));
    return anim->id;
}

bool cog_anim_collides_anim(cog_anim_id id0, cog_anim_id id1) {
    cog_anim* anim0 = cog_anim_get(id0);
    cog_anim* anim1 = cog_anim_get(id1);
    if(cog_anim_dist_anim(id0, id1) < (anim0->dim.w + anim1->dim.w)) {
        return COG_TRUE;
    } else {
        return COG_FALSE;
    }
}

bool cog_anim_collides_sprite(cog_anim_id id0, cog_sprite_id id1) {
    cog_anim* anim0 = cog_anim_get(id0);
    cog_sprite* sprite1 = cog_sprite_get(id1);
    if(cog_anim_dist_sprite(id0, id1) < (anim0->dim.w + sprite1->dim.w)) {
        return COG_TRUE;
    } else {
        return COG_FALSE;
    }
}

cog_anim* cog_anim_get(cog_anim_id id) {
    return (cog_anim*) cog_map_get(&anims, id);
}

uint32_t cog_anim_get_len_key(uint32_t tex_id, uint32_t layer) {
    return cog_map_int32_hash(tex_id, layer);
}

uint32_t cog_anim_len(uint32_t tex_id, uint32_t layer) {
    uint32_t key = cog_anim_get_len_key(tex_id, layer);
    cog_len_map_entry* result = cog_map_get(&tex_anim_lens, key);
    if(result == 0) {
        return 0;
    } else {
        //Collisions TODO: Handle them
        assert(result->tex_id == tex_id);
        assert(result->layer == layer);
        return result->cnt;
    }
}

void cog_anim_remove(cog_anim_id id) {
    COG_LIST_FOREACH(&active_anims) {
        if(*((cog_anim_id*) curr->data) == id) {
            cog_list_remove(&active_anims, curr->data);
            //BUG: Update len here with removal
            cog_anim* anim = cog_anim_get(id);
            cog_anim_decrease_tex_anim_len(anim);
            break;
        }
    }
    cog_map_remove(&anims, id);
}

void cog_anim_removeall(void) {
    cog_list_removeall(&active_anims);
}

void cog_anim_set(cog_anim_id id, cog_anim src) {
    cog_anim* anim = cog_anim_get(id);
    anim->transition_time = src.transition_time;
    anim->looped = src.looped;
    anim->paused = src.paused;
    anim->pos = src.pos;
    anim->dim = src.dim;
    anim->rot = src.rot;
    anim->vel = src.vel;
    anim->ang_vel = src.ang_vel;
    anim->finished = src.finished;
    anim->pixel_perfect = src.pixel_perfect;
    anim->update_func = src.update_func;
}

void cog_anim_set_frames_array(cog_anim_id id, int frames[]) {
    cog_anim* anim = cog_anim_get(id);
    for(int i=0; frames[i] != COG_NULL; i++) {
        cog_list_append(&(anim->frames), (cog_dataptr) & frames[i]);
    }
}

void cog_anim_set_frame(cog_anim_id id, uint32_t frame) {
    cog_anim* anim = cog_anim_get(id);
    anim->current_frame = frame;
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
double cog_anim_dist_anim(cog_anim_id id0, cog_anim_id id1) {
    cog_anim* anim0 = cog_anim_get(id0);
    cog_anim* anim1 = cog_anim_get(id1);
    return cog_math_sqrt(((anim0->pos.x - anim1->pos.x) *
                          (anim0->pos.x - anim1->pos.x)) +
                         ((anim0->pos.y - anim1->pos.y) *
                          (anim0->pos.y - anim1->pos.y)));
}

double cog_anim_dist_sprite(cog_anim_id id0, cog_sprite_id id1) {
    cog_anim* anim0 = cog_anim_get(id0);
    cog_sprite* sprite1 = cog_sprite_get(id1);
    return cog_math_sqrt(((anim0->pos.x - sprite1->pos.x) *
                          (anim0->pos.x - sprite1->pos.x)) +
                         ((anim0->pos.y - sprite1->pos.y) *
                          (anim0->pos.y - sprite1->pos.y)));
}

uint32_t cog_anim_draw_layer(uint32_t layer, uint32_t tex_id, uint32_t idx_global) {
    uint32_t idx = 0;
    //Draw anims
    COG_LIST_FOREACH(&active_anims) {
        //draw current sprite
        cog_anim* anim = cog_anim_get(*(cog_anim_id*) curr->data);
        if(anim->layer != layer || anim->tex_id != tex_id || anim->finished) {
            continue;
        }
        //find active frame to render
        cog_list* frame = (anim->frames.next);
        uint32_t i = 0;
        while(i < anim->current_frame) {
            frame = frame->next;
            i++;
        }
        cog_sprite* sprite = frame->data;
        sprite->pos.x = anim->pos.x;
        sprite->pos.y = anim->pos.y;
        sprite->dim.w = anim->dim.w;
        sprite->dim.h = anim->dim.h;
        sprite->rot = anim->rot;
        sprite->pixel_perfect = anim->pixel_perfect;
        cog_graphics_draw_sprite(sprite, idx_global + idx);
        idx++;
    }
    return idx;
}

void cog_anim_decrease_tex_anim_len(cog_anim* anim) {
    uint32_t key = cog_anim_get_len_key(anim->tex_id, anim->layer);
    cog_len_map_entry* tex_anim_len = cog_map_get(&tex_anim_lens, key);
    if(tex_anim_len == 0) {
        tex_anim_len = (cog_len_map_entry*)cog_malloc(sizeof(cog_len_map_entry));
        tex_anim_len->cnt = 0;
        tex_anim_len->tex_id = anim->tex_id;
        tex_anim_len->layer = anim->layer;
    }
    tex_anim_len->cnt--;
    cog_map_put(&tex_anim_lens, key, tex_anim_len);
}

void cog_anim_increase_tex_anim_len(cog_anim* anim) {
    uint32_t key = cog_anim_get_len_key(anim->tex_id, anim->layer);
    cog_len_map_entry* tex_anim_len = cog_map_get(&tex_anim_lens, key);
    if(tex_anim_len == 0) {
        tex_anim_len = (cog_len_map_entry*)cog_malloc(sizeof(cog_len_map_entry));
        tex_anim_len->cnt = 0;
        tex_anim_len->tex_id = anim->tex_id;
        tex_anim_len->layer = anim->layer;
    }
    tex_anim_len->cnt++;
    cog_map_put(&tex_anim_lens, key, tex_anim_len);
}

void cog_anim_init() {
    cog_map_init(&anims);
    cog_map_init(&tex_anim_lens);
    cog_list_init(&active_anims, sizeof(cog_sprite_id));
}

void cog_anim_update(double timestep) {
    COG_LIST_FOREACH(&active_anims) {
        //Draw current sprite
        cog_anim_id id = *((cog_anim_id*) curr->data);
        cog_anim* this_anim = (cog_anim*) cog_map_get(&anims, id);
        if(this_anim->paused) {
            continue;
        }
        this_anim->current_frame_time += timestep;
        if(this_anim->current_frame_time >= this_anim->transition_time) {
            this_anim->current_frame++;
            this_anim->current_frame_time = 0.0;
            if(this_anim->current_frame >= this_anim->n_frames) {
                if(this_anim->looped) {
                    this_anim->current_frame = 0;
                } else {
                    this_anim->finished = COG_TRUE;
                }
            }
        }
    }
    //Do physics
    COG_LIST_FOREACH(&active_anims) {
        cog_anim* this_anim = cog_anim_get(*(cog_anim_id*) curr->data);
        this_anim->pos.x += timestep * this_anim->vel.x;
        this_anim->pos.y += timestep * this_anim->vel.y;
        this_anim->rot += timestep * this_anim->ang_vel;
        if(this_anim->update_func != 0) {
            this_anim->update_func(timestep, this_anim);
        }
    }
}
