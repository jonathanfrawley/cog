#include "cog_math.h"

#include <math.h>

double cog_math_cosf(double radians) {
    return cosf(radians);
}

double cog_math_radians_to_degrees(double radians) {
    return (radians * (180.0f / COG_PI));
}

double cog_math_sinf(double radians) {
    return sinf(radians);
}

double cog_math_slopef(double x1, double y1, double x2,
                       double y2) {
    return atan2((y2 - y1), (x2 - x1));
}

double cog_math_sqrt(double x) {
    return sqrt(x);
}
