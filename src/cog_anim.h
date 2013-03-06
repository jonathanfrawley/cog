#ifndef  COG_ANIM_H
#define  COG_ANIM_H

#include "cog_list.h"
#include "cog_types.h"

#define COG_ANIM_LAYER 2

/*-----------------------------------------------------------------------------
 * Anims are a collection of sprites with a specific duration between them.
 *-----------------------------------------------------------------------------*/
typedef struct {
    cog_anim_id id;
    cog_uint layer;
    cog_uint transition_millis;
    cog_uint current_frame;
    cog_uint current_frame_millis;
    cog_bool looped;
    cog_list frames;
    cog_uint n_frames;
    cog_bool paused;
    //For convenience
    cog_pos2 pos;
    cog_dim2 dim;
    cog_float rot;
    cog_vec2 vel;
    cog_bool finished;
} cog_anim;

cog_anim_id cog_anim_add(char *anim_img, cog_int n_frames);
cog_bool cog_anim_collides_anim(cog_anim_id id0, cog_anim_id id1);
cog_bool cog_anim_collides_sprite(cog_anim_id id0, cog_sprite_id id1);
cog_anim *cog_anim_get(cog_anim_id);
void cog_anim_remove(cog_anim_id id);
void cog_anim_removeall(void);
void cog_anim_set_frames(cog_anim_id id, cog_int frame0, ...);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
cog_float cog_anim_dist_anim(cog_anim_id id0, cog_anim_id id1);
cog_float cog_anim_dist_sprite(cog_anim_id id0, cog_sprite_id id1);
void cog_anim_draw_layer(cog_uint layer);
void cog_anim_init();
void cog_anim_update(cog_uint deltamillis);

#endif // COG_ANIM_H
