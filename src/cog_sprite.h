#ifndef  COG_SPRITE_H
#define  COG_SPRITE_H

#include <GL/glew.h>

#include "cog_types.h"

/*-----------------------------------------------------------------------------
 *  Represents a 2D image to be drawn to the screen
 *-----------------------------------------------------------------------------*/
typedef struct
{
    cog_sprite_id id;
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
cog_float cog_sprite_anim_dist(cog_sprite_id a, cog_anim_id b);
cog_float cog_sprite_dist(cog_sprite_id a, cog_sprite_id b);
cog_sprite* cog_sprite_get(cog_sprite_id);
void cog_sprite_remove(cog_sprite_id id);
void cog_sprite_removeall(void);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_sprite_init(void);
void cog_sprite_draw(void);
void cog_sprite_update(cog_float timedelta);

#endif   // COG_SPRITE_H
