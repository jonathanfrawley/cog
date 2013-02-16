#ifndef  COG_MATH_H
#define  COG_MATH_H

#include "cog_types.h"

#define COG_PI 3.14159265

cog_float cog_math_cosf(cog_float radians);
cog_float cog_math_degrees_to_radians(cog_float degrees);
cog_float cog_math_radians_to_degrees(cog_float radians);
cog_float cog_math_sinf(cog_float radians);
cog_float cog_math_slopef(cog_float x1, cog_float y1, cog_float x2, cog_float y2);
cog_float cog_math_sqrt(cog_float x);

#endif   // COG_MATH_H
