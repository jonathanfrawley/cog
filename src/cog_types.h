#ifndef  COG_TYPES_H
#define  COG_TYPES_H

#include <stdint.h>
#include <stdbool.h>

//defines
#define COG_NULL 0
#define COG_FALSE 0
#define COG_TRUE 1

//constants
#define COG_TEXT_MAX 255

//types
typedef int32_t cog_anim_id;
typedef int32_t cog_snd_id;
typedef int32_t cog_sprite_id;
typedef int32_t cog_text_id;
typedef void* cog_dataptr;
typedef char* cog_string;

typedef struct cog_pos2 {
    double x;
    double y;
} cog_pos2;

typedef struct cog_vec2 {
    double x;
    double y;
} cog_vec2;

typedef struct cog_dim2 {
    double w;
    double h;
} cog_dim2;

typedef struct cog_color {
    double r;
    double g;
    double b;
    double a;
} cog_color;

typedef struct cog_config {
    uint32_t window_w;
    uint32_t window_h;
    bool fullscreen;
} cog_config;

#endif // COG_TYPES_H
