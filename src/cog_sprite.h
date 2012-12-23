#ifndef  COG_SPRITE_H
#define  COG_SPRITE_H

#include <GL/glew.h>

#include "cog_types.h"

#define COG_SPRITE_LAYER 1

/*-----------------------------------------------------------------------------
 *  Represents a 2D image to be drawn to the screen
 *-----------------------------------------------------------------------------*/
typedef struct
{
    cog_sprite_id id;
    cog_uint layer;
    GLuint texid;
    //These coords and dimensions are for the whole sprite in the game world.
    cog_float x;
    cog_float y;
    cog_float w;
    cog_float h;
    cog_float rot;
    //These are the coords and dimensions of the sprite within the image. 
    //(Can have multiple sprites per image - cog_anims are implemented using this)
    cog_float texx;
    cog_float texy;
    cog_float texw;
    cog_float texh;
    //physics
    cog_float xvel;
    cog_float yvel;
} cog_sprite;

cog_sprite_id cog_sprite_add(char* img);
cog_bool cog_sprite_collides_sprite(cog_sprite_id id0, cog_sprite_id id1);
cog_sprite* cog_sprite_get(cog_sprite_id);
void cog_sprite_remove(cog_sprite_id id);
void cog_sprite_removeall(void);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_sprite_init(void);
cog_float cog_sprite_dist_anim(cog_sprite_id a, cog_anim_id b);
cog_float cog_sprite_dist_sprite(cog_sprite_id a, cog_sprite_id b);
void cog_sprite_draw_layer(cog_uint layer);
void cog_sprite_update(cog_float timedelta);

#endif   // COG_SPRITE_H
