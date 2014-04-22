#include "cog_sprite.h"

#include "cog_anim.h"
#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_list.h"
#include "cog_log.h"
#include "cog_map.h"
#include "cog_math.h"

#include "stdio.h"
#include "assert.h"

#define MAX_KEY 255

static cog_list active_sprites; //sprites drawn(active) at the moment
static cog_map sprites;
static cog_sprite_id spritecnt;
static cog_map tex_sprite_lens;

uint32_t cog_sprite_get_len_key(uint32_t tex_id, uint32_t layer);
void cog_sprite_decrease_tex_sprite_len(cog_sprite* sprite);
void cog_sprite_increase_tex_sprite_len(cog_sprite* sprite);

typedef struct cog_len_map_entry {
    uint32_t tex_id;
    uint32_t layer;
    uint32_t cnt;
} cog_len_map_entry;

cog_sprite_id cog_sprite_add(const char* img) {
    cog_sprite_id id = cog_sprite_add_inactive(img);
    cog_sprite* sprite = cog_sprite_get(id);
    cog_list_append(&active_sprites, (cog_dataptr) & (sprite->id));
    cog_sprite_increase_tex_sprite_len(sprite);
    return id;
}

cog_sprite_id cog_sprite_add_inactive(const char* img) {
    cog_sprite* sprite = COG_STRUCT_MALLOC(cog_sprite);
    sprite->id = spritecnt++;
    int w, h;
    sprite->tex_id = cog_graphics_load_texture(img, &w, &h);
    sprite->layer = COG_SPRITE_LAYER;
    sprite->dim.w = (double)w;
    sprite->dim.h = (double)h;
    sprite->tex_dim.w = 1.0f;
    sprite->tex_dim.h = 1.0f;
    sprite->pixel_perfect = true;
    cog_map_put(&sprites, sprite->id, (void*) sprite);
    return sprite->id;
}

//TODO: Remove duplication
cog_sprite_id cog_sprite_add_explicit(uint32_t tex_id, cog_pos2 pos, cog_dim2 dim) {
    cog_sprite* sprite = COG_STRUCT_MALLOC(cog_sprite);
    sprite->id = spritecnt++;
    cog_list_append(&active_sprites, (cog_dataptr) & (sprite->id));
    sprite->tex_id = tex_id;
    sprite->layer = COG_SPRITE_LAYER;
    sprite->pos = pos;
    sprite->dim = dim;
    sprite->tex_dim.w = 1.0f;
    sprite->tex_dim.h = 1.0f;
    sprite->pixel_perfect = false;
    cog_map_put(&sprites, sprite->id, (void*) sprite);
    cog_sprite_increase_tex_sprite_len(sprite);
    return sprite->id;
}

bool cog_sprite_collides_sprite(cog_sprite_id id0, cog_sprite_id id1) {
    cog_sprite* sprite0 = cog_sprite_get(id0);
    cog_sprite* sprite1 = cog_sprite_get(id1);
    if(cog_sprite_dist_sprite(id0, id1) < (sprite0->dim.w + sprite1->dim.w)) {
        return COG_TRUE;
    } else {
        return COG_FALSE;
    }
}

cog_sprite* cog_sprite_get(cog_sprite_id id) {
    return (cog_sprite*) cog_map_get(&sprites, id);
}

uint32_t cog_sprite_len(uint32_t tex_id, uint32_t layer) {
    uint32_t key = cog_sprite_get_len_key(tex_id, layer);
    cog_len_map_entry* result = cog_map_get(&tex_sprite_lens, key);
    if(result == 0) {
        return 0;
    } else {
        //cog_debugf("result : tex_id %d layer %d ", result->tex_id, result->layer);
        //Collisions TODO: Handle them
        assert(result->tex_id == tex_id);
        assert(result->layer == layer);
        return result->cnt;
    }
    /*
    char key[MAX_KEY];
    cog_sprite_get_len_key(key, tex_id, layer);
    uint32_t* result = cog_map_get_hash(&tex_sprite_lens, key);
    if(result == 0) {
        return 0;
    } else {
        return *result;
    }
    */
}

void cog_sprite_remove(cog_sprite_id id) {
    COG_LIST_FOREACH(&active_sprites) {
        if(*((cog_sprite_id*) curr->data) == id) {
            cog_list_remove(&active_sprites, curr->data);
            cog_sprite* sprite = cog_sprite_get(id);
            cog_sprite_decrease_tex_sprite_len(sprite);
            break;
        }
    }
    cog_map_remove(&sprites, id);
}

void cog_sprite_removeall(void) {
    cog_list_removeall(&active_sprites);
}

void cog_sprite_set(cog_sprite_id id, cog_sprite src) {
    cog_sprite* sprite = cog_sprite_get(id);
    sprite->pos = src.pos;
    sprite->dim = src.dim;
    sprite->rot = src.rot;
    sprite->vel = src.vel;
    sprite->ang_vel = src.ang_vel;
    sprite->update_func = src.update_func;
    sprite->pixel_perfect = src.pixel_perfect;
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_sprite_init(void) {
    cog_map_init(&sprites);
    cog_map_init(&tex_sprite_lens);
    cog_list_init(&active_sprites, sizeof(cog_sprite_id));
}

double cog_sprite_dist_anim(cog_sprite_id a, cog_anim_id b) {
    cog_sprite* asprite = cog_sprite_get(a);
    cog_anim* banim = cog_anim_get(b);
    cog_sprite* bsprite = (cog_sprite*) banim->frames.next->data;
    return cog_math_sqrt((asprite->pos.x - bsprite->pos.x) *
                         (asprite->pos.x - bsprite->pos.x) + (asprite->pos.y -
                                 bsprite->pos.
                                 y) *
                         (asprite->pos.y - bsprite->pos.y));
}

double cog_sprite_dist_sprite(cog_sprite_id a, cog_sprite_id b) {
    cog_sprite* asprite = cog_sprite_get(a);
    cog_sprite* bsprite = cog_sprite_get(b);
    return cog_math_sqrt((asprite->pos.x - bsprite->pos.x) *
                         (asprite->pos.x - bsprite->pos.x) + (asprite->pos.y -
                                 bsprite->pos.
                                 y) *
                         (asprite->pos.y - bsprite->pos.y));
}

uint32_t cog_sprite_draw_layer(uint32_t layer, uint32_t tex_id, uint32_t global_idx) {
    uint32_t idx = 0;
    //Draw sprites
    COG_LIST_FOREACH(&active_sprites) {
        //draw current sprite if it is on the correct layer
        cog_sprite* curr_sprite = (cog_sprite*) cog_map_get(&sprites,
                                  *((cog_sprite_id
                                     *)
                                    curr->data));
        if(curr_sprite->layer == layer && curr_sprite->tex_id == tex_id) {
            cog_graphics_draw_sprite(curr_sprite, global_idx + idx);
            idx++;
        }
    }
    return idx;
}

/*
void cog_sprite_get_len_key(char* buf, uint32_t tex_id, uint32_t layer) {
    sprintf(buf, "%d:%d", tex_id, layer);
}
*/
uint32_t cog_sprite_get_len_key(uint32_t tex_id, uint32_t layer) {
    return cog_map_int32_hash(tex_id, layer);
}

void cog_sprite_decrease_tex_sprite_len(cog_sprite* sprite) {
    uint32_t key = cog_sprite_get_len_key(sprite->tex_id, sprite->layer);
    cog_len_map_entry* tex_sprite_len = cog_map_get(&tex_sprite_lens, key);
    if(tex_sprite_len == 0) {
        tex_sprite_len = (cog_len_map_entry*)cog_malloc(sizeof(cog_len_map_entry));
        tex_sprite_len->cnt = 0;
        tex_sprite_len->tex_id = sprite->tex_id;
        tex_sprite_len->layer = sprite->layer;
    }
    tex_sprite_len->cnt--;
    cog_map_put(&tex_sprite_lens, key, tex_sprite_len);
}

void cog_sprite_increase_tex_sprite_len(cog_sprite* sprite) {
    uint32_t key = cog_sprite_get_len_key(sprite->tex_id, sprite->layer);
    cog_len_map_entry* tex_sprite_len = cog_map_get(&tex_sprite_lens, key);
    if(tex_sprite_len == 0) {
        tex_sprite_len = (cog_len_map_entry*)cog_malloc(sizeof(cog_len_map_entry));
        cog_debugf("inserting sprite with tex_id %d ", sprite->tex_id);
        tex_sprite_len->cnt = 0;
        tex_sprite_len->tex_id = sprite->tex_id;
        tex_sprite_len->layer = sprite->layer;
    }
    tex_sprite_len->cnt++;
    cog_map_put(&tex_sprite_lens, key, tex_sprite_len);
    /*
    char key[MAX_KEY];
    cog_sprite_get_len_key(key, sprite->tex_id, sprite->layer);
    uint32_t* tex_sprite_len = cog_map_get_hash(&tex_sprite_lens, key);
    if(tex_sprite_len == 0) {
        tex_sprite_len = (uint32_t*)cog_malloc(sizeof(uint32_t));
    }
    (*tex_sprite_len)++;
    cog_map_put_hash(&tex_sprite_lens, key, tex_sprite_len);
    */
}

void cog_sprite_update(double timestep) {
    COG_LIST_FOREACH(&active_sprites) {
        cog_sprite* curr_sprite = (cog_sprite*) cog_map_get(&sprites,
                                  *((cog_sprite_id
                                     *)
                                    curr->data));
        //do physics update for current sprite
        curr_sprite->pos.x += timestep * curr_sprite->vel.x;
        curr_sprite->pos.y += timestep * curr_sprite->vel.y;
        curr_sprite->rot += timestep * curr_sprite->ang_vel;
        if(curr_sprite->update_func != 0) {
            curr_sprite->update_func(timestep, curr_sprite);
        }
    }
}
