#ifndef  COG_TYPES_H
#define  COG_TYPES_H

//defines
#define COG_NULL 0
#define COG_FALSE 0
#define COG_TRUE 1

//types
typedef float cog_float;
typedef int cog_anim_id;
typedef int cog_snd_id;
typedef int cog_sprite_id;
typedef int cog_text_id;
typedef int cog_bool;
typedef void* cog_dataptr;
typedef unsigned cog_uint;
typedef unsigned char cog_uint8;
typedef int cog_int;
typedef const char* cog_string;

typedef struct cog_pos2
{
    cog_float x;
    cog_float y;
} cog_pos2;

typedef struct cog_vec2
{
    cog_float x;
    cog_float y;
} cog_vec2;

typedef struct cog_dim2
{
    cog_float w;
    cog_float h;
} cog_dim2;

#endif   // COG_TYPES_H
