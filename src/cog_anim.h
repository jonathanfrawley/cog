#ifndef  COG_ANIM_H
#define  COG_ANIM_H

#include "cog_list.h"
#include "cog_types.h"

/*-----------------------------------------------------------------------------
 * Anims are a collection of sprites with a specific duration between them.
 *-----------------------------------------------------------------------------*/
typedef struct
{
    cog_anim_id id;
    cog_uint transition_millis;
    cog_uint currentframe;
    cog_uint currentframe_millis;
    cog_bool looped;
    cog_list frames;
    cog_uint nframes;
    cog_bool paused;
    //For convenience
    cog_float x;
    cog_float y;
    cog_float w;
    cog_float h;
    cog_float rot;
    cog_float xvel;
    cog_float yvel;
    cog_bool finished;
} cog_anim;

cog_anim_id cog_anim_add(char* animimg, cog_int nframes);
cog_anim* cog_anim_get(cog_anim_id);
void cog_anim_remove(cog_anim_id id);
void cog_anim_removeall(void);
void cog_anim_set_frames(cog_anim_id id, cog_int frame0, ...);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_anim_draw(void);
void cog_anim_init();
void cog_anim_update(cog_uint deltamillis);

#endif   // COG_ANIM_H
