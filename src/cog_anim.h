#ifndef  COG_ANIM_H
#define  COG_ANIM_H

#include "cog_list.h"
#include "cog_types.h"

#define COG_ANIM_LAYER 2

struct cog_anim;

/*-----------------------------------------------------------------------------
 * Anims are a collection of sprites with a specific duration between them.
 *-----------------------------------------------------------------------------*/
typedef struct {
    cog_anim_id id;
    uint32_t tex_id;
    uint32_t layer;
    uint32_t current_frame;
    double current_frame_time;
    cog_list frames;
    uint32_t n_frames;
    //User attributes
    double transition_time;
    bool looped;
    bool paused;
    cog_pos2 pos;
    cog_dim2 dim;
    double rot;
    cog_vec2 vel;
    double ang_vel;
    bool finished;
    void (*update_func)(double timestep, struct cog_anim* anim);
} cog_anim;

cog_anim_id cog_anim_add(const char* img, uint32_t rows, uint32_t n_frames);
bool cog_anim_collides_anim(cog_anim_id id0, cog_anim_id id1);
bool cog_anim_collides_sprite(cog_anim_id id0, cog_sprite_id id1);
cog_anim* cog_anim_get(cog_anim_id);
uint32_t cog_anim_len(uint32_t tex_id, uint32_t layer);
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
uint32_t cog_anim_draw_layer(uint32_t layer, uint32_t tex_id, uint32_t idx_global);
void cog_anim_init();
void cog_anim_update(double timestep);

#endif // COG_ANIM_H
