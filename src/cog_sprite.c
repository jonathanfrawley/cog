#include "cog_sprite.h"

#include "cog_anim.h"
#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_list.h"
#include "cog_map.h"
#include "cog_math.h"

static cog_list active_sprites; //sprites drawn(active) at the moment
static cog_map sprites;
static cog_sprite_id spritecnt;

cog_sprite_id cog_sprite_add(char* img) {
    cog_sprite_id id = cog_sprite_add_inactive(img);
    cog_sprite* sprite = cog_sprite_get(id);
    cog_list_append(&active_sprites, (cog_dataptr) & (sprite->id));
    return id;
}

cog_sprite_id cog_sprite_add_inactive(char* img) {
    cog_sprite* sprite = COG_STRUCT_MALLOC(cog_sprite);
    sprite->id = spritecnt++;
    sprite->tex_id = cog_graphics_load_texture(img);
    sprite->layer = COG_SPRITE_LAYER;
    sprite->pos.x = 0;
    sprite->pos.y = 0;
    sprite->dim.w = 0;
    sprite->dim.h = 0;
    sprite->rot = 0;
    sprite->tex_pos.x = 0.0f;
    sprite->tex_pos.y = 0.0f;
    sprite->tex_dim.w = 1.0f;
    sprite->tex_dim.h = 1.0f;
    //vel always 0 to start with
    sprite->vel.x = 0.0f;
    sprite->vel.y = 0.0f;
    cog_map_put(&sprites, sprite->id, (void*) sprite);
    return sprite->id;
}

cog_bool cog_sprite_collides_sprite(cog_sprite_id id0, cog_sprite_id id1) {
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

void cog_sprite_remove(cog_sprite_id id) {
    COG_LIST_FOREACH(&active_sprites) {
        if(*((cog_sprite_id*) curr->data) == id) {
            cog_list_remove(&active_sprites, curr->data);
            break;
        }
    }
    cog_map_remove(&sprites, id);
}

void cog_sprite_removeall(void) {
    cog_list_removeall(&active_sprites);
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_sprite_init(void) {
    cog_map_init(&sprites);
    cog_list_init(&active_sprites, sizeof(cog_sprite_id));
}

cog_float cog_sprite_dist_anim(cog_sprite_id a, cog_anim_id b) {
    cog_sprite* asprite = cog_sprite_get(a);
    cog_anim* banim = cog_anim_get(b);
    cog_sprite* bsprite = (cog_sprite*) banim->frames.next->data;
    return cog_math_sqrt((asprite->pos.x - bsprite->pos.x) *
                         (asprite->pos.x - bsprite->pos.x) + (asprite->pos.y -
                                 bsprite->pos.
                                 y) *
                         (asprite->pos.y - bsprite->pos.y));
}

cog_float cog_sprite_dist_sprite(cog_sprite_id a, cog_sprite_id b) {
    cog_sprite* asprite = cog_sprite_get(a);
    cog_sprite* bsprite = cog_sprite_get(b);
    return cog_math_sqrt((asprite->pos.x - bsprite->pos.x) *
                         (asprite->pos.x - bsprite->pos.x) + (asprite->pos.y -
                                 bsprite->pos.
                                 y) *
                         (asprite->pos.y - bsprite->pos.y));
}

void cog_sprite_draw_layer(cog_uint layer) {
    //Draw sprites
    COG_LIST_FOREACH(&active_sprites) {
        //draw current sprite if it is on the correct layer
        cog_sprite* curr_sprite = (cog_sprite*) cog_map_get(&sprites,
                                  *((cog_sprite_id
                                     *)
                                    curr->data));
        if(curr_sprite->layer == layer) {
            cog_graphics_draw_sprite(curr_sprite);
        }
    }
}

void cog_sprite_update(cog_float timedelta) {
    COG_LIST_FOREACH(&active_sprites) {
        cog_sprite* curr_sprite = (cog_sprite*) cog_map_get(&sprites,
                                  *((cog_sprite_id
                                     *)
                                    curr->data));
        //do physics update for current sprite
        curr_sprite->pos.x += timedelta * curr_sprite->vel.x;
        curr_sprite->pos.y += timedelta * curr_sprite->vel.y;
    }
}
