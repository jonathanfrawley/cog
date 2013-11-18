#ifndef  COG_MATH_H
#define  COG_MATH_H

#include "cog_types.h"

#define COG_PI 3.14159265

double cog_math_cosf(double radians);
double cog_math_radians_to_degrees(double radians);
double cog_math_sinf(double radians);
double cog_math_slopef(double x1, double y1, double x2,
                       double y2);
double cog_math_sqrt(double x);

#endif // COG_MATH_H
