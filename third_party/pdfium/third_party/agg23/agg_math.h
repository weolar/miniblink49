
//----------------------------------------------------------------------------
// Anti-Grain Geometry - Version 2.3
// Copyright (C) 2002-2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
// Bessel function (besj) was adapted for use in AGG library by Andy Wilk
// Contact: castor.vulgaris@gmail.com
//----------------------------------------------------------------------------
#ifndef AGG_MATH_INCLUDED
#define AGG_MATH_INCLUDED
#include "agg_basics.h"
namespace agg
{
const float intersection_epsilon = 1.0e-30f;
AGG_INLINE float calc_point_location(float x1, float y1,
                                        float x2, float y2,
                                        float x,  float y)
{
  return ((x - x2) * (y2 - y1)) - ((y - y2) * (x2 - x1));
}
AGG_INLINE float calc_distance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return FXSYS_sqrt2(dx, dy);
}
AGG_INLINE float calc_line_point_distance(float x1, float y1,
        float x2, float y2,
        float x,  float y)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float d = FXSYS_sqrt2(dx, dy);
    if(d < intersection_epsilon) {
        return calc_distance(x1, y1, x, y);
    }
    return ((x - x2) * dy / d) - ((y - y2) * dx / d);
}
AGG_INLINE bool calc_intersection(float ax, float ay, float bx, float by,
                                  float cx, float cy, float dx, float dy,
                                  float* x, float* y)
{
  float num = ((ay - cy) * (dx - cx)) - ((ax - cx) * (dy - cy));
  float den = ((bx - ax) * (dy - cy)) - ((by - ay) * (dx - cx));
  if (fabs(den) < intersection_epsilon) {
    return false;
    }
    *x = ax + ((bx - ax) * num / den);
    *y = ay + ((by - ay) * num / den);
    return true;
}
}
#endif
