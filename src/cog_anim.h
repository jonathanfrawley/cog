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
    uint32_t layer;
    uint32_t current_frame;
    uint32_t current_frame_millis;
    cog_list frames;
    uint32_t n_frames;
    //User attributes
    uint32_t transition_millis;
    bool looped;
    bool paused;
    cog_pos2 pos;
    cog_dim2 dim;
    double rot;
    cog_vec2 vel;
    double ang_vel;
    bool finished;
} cog_anim;

cog_anim_id cog_anim_add(const char* img, uint32_t rows, uint32_t n_frames);
bool cog_anim_collides_anim(cog_anim_id id0, cog_anim_id id1);
bool cog_anim_collides_sprite(cog_anim_id id0, cog_sprite_id id1);
cog_anim* cog_anim_get(cog_anim_id);
void cog_anim_remove(cog_anim_id id);
void cog_anim_removeall(void);
void cog_anim_set(cog_anim_id id, cog_anim src);
void cog_anim_set_frames(cog_anim_id id, int frame0, ...);
void cog_anim_set_frame(cog_anim_id id, uint32_t frame);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
double cog_anim_dist_anim(cog_anim_id id0, cog_anim_id id1);
double cog_anim_dist_sprite(cog_anim_id id0, cog_sprite_id id1);
void cog_anim_draw_layer(uint32_t layer);
void cog_anim_init();
void cog_anim_update(uint32_t deltamillis);

#endif // COG_ANIM_H
