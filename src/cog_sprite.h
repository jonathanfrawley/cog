#ifndef  COG_SPRITE_H
#define  COG_SPRITE_H

#if !defined(HAVE_GLES)
#include <GL/glew.h>
#else
#include <GLES/gl.h>
#endif

#include "cog_types.h"

#define COG_SPRITE_LAYER 1

/*-----------------------------------------------------------------------------
 *  Represents a 2D image to be drawn to the screen
 *-----------------------------------------------------------------------------*/
typedef struct {
    cog_sprite_id id;
    uint32_t layer;
    GLuint tex_id;
    //These are the coords and dimensions of the sprite within the image.
    //(Can have multiple sprites per image - cog_anims are implemented using this)
    cog_pos2 tex_pos;
    cog_dim2 tex_dim;
    //User attributes
    cog_pos2 pos;
    cog_dim2 dim;
    double rot;
    cog_vec2 vel;
    double ang_vel;
} cog_sprite;

cog_sprite_id cog_sprite_add(const char* img);
cog_sprite_id cog_sprite_add_inactive(const char* img);
bool cog_sprite_collides_sprite(cog_sprite_id id0, cog_sprite_id id1);
cog_sprite* cog_sprite_get(cog_sprite_id);
void cog_sprite_remove(cog_sprite_id id);
void cog_sprite_removeall(void);
void cog_sprite_set(cog_sprite_id id, cog_sprite src);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_sprite_init(void);
double cog_sprite_dist_anim(cog_sprite_id a, cog_anim_id b);
double cog_sprite_dist_sprite(cog_sprite_id a, cog_sprite_id b);
void cog_sprite_draw_layer(uint32_t layer);
void cog_sprite_update(double timedelta);

#endif // COG_SPRITE_H
