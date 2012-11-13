#include "cog_math.h"

#include <math.h>

cog_float cog_math_radtodeg(cog_float rad)
{
    return (rad * (180.0f/COG_PI));
}

cog_float cog_math_sqrt(cog_float x)
{
    return sqrt(x);
}

