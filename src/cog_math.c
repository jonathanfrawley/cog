#include "cog_math.h"

#include <math.h>

cog_float cog_math_cosf(cog_float radians) {
    return cosf(radians);
}

cog_float cog_math_radians_to_degrees(cog_float radians) {
    return (radians * (180.0f / COG_PI));
}

cog_float cog_math_sinf(cog_float radians) {
    return sinf(radians);
}

cog_float cog_math_slopef(cog_float x1, cog_float y1, cog_float x2,
                          cog_float y2) {
    return atan2((y2 - y1), (x2 - x1));
}

cog_float cog_math_sqrt(cog_float x) {
    return sqrt(x);
}
