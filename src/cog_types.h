#ifndef  COG_TYPES_H
#define  COG_TYPES_H

//defines
#define COG_NULL 0
#define COG_FALSE 0
#define COG_TRUE 1

//types
typedef float cog_float;
typedef int cog_sprite_id;
typedef int cog_anim_id;
typedef int cog_snd_id;
typedef unsigned cog_uint;
typedef int cog_bool;
typedef void* cog_dataptr;
typedef unsigned cog_uint;
typedef unsigned char cog_uint8;
typedef int cog_int;

typedef struct cog_text_colour
{
    cog_uint8 r;
    cog_uint8 g;
    cog_uint8 b;
} cog_text_colour;

#endif   // COG_TYPES_H
