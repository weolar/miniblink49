
//----------------------------------------------------------------------------
// XYQ: 2006-01-22 Copied from AGG project.
// TODO: This file uses intensive floating point operations, so it's NOT suitable
// for platforms like Symbian OS. We need to change to FIX format.
//----------------------------------------------------------------------------
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

#include "agg_curves.h"
#include "agg_math.h"

namespace agg
{
const float curve_collinearity_epsilon              = 1e-30f;
enum curve_recursion_limit_e { curve_recursion_limit = 16 };
void curve4_div::init(float x1, float y1,
                      float x2, float y2,
                      float x3, float y3,
                      float x4, float y4)
{
    m_points.remove_all();
    m_distance_tolerance_square = 1.0f / 4;
    m_distance_tolerance_manhattan = 1.0f * 4;
    bezier(x1, y1, x2, y2, x3, y3, x4, y4);
    m_count = 0;
}
void curve4_div::recursive_bezier(float x1, float y1,
                                  float x2, float y2,
                                  float x3, float y3,
                                  float x4, float y4,
                                  unsigned level)
{
    if(level > curve_recursion_limit) {
        return;
    }
    float x12   = (x1 + x2) / 2;
    float y12   = (y1 + y2) / 2;
    float x23   = (x2 + x3) / 2;
    float y23   = (y2 + y3) / 2;
    float x34   = (x3 + x4) / 2;
    float y34   = (y3 + y4) / 2;
    float x123  = (x12 + x23) / 2;
    float y123  = (y12 + y23) / 2;
    float x234  = (x23 + x34) / 2;
    float y234  = (y23 + y34) / 2;
    float x1234 = (x123 + x234) / 2;
    float y1234 = (y123 + y234) / 2;
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = fabs(((x2 - x4) * dy) - ((y2 - y4) * dx));
    float d3 = fabs(((x3 - x4) * dy) - ((y3 - y4) * dx));
    switch((int(d2 > curve_collinearity_epsilon) << 1) +
            int(d3 > curve_collinearity_epsilon)) {
        case 0:
          if (fabs(x1 + x3 - x2 - x2) + fabs(y1 + y3 - y2 - y2) +
                  fabs(x2 + x4 - x3 - x3) + fabs(y2 + y4 - y3 - y3) <=
              m_distance_tolerance_manhattan) {
            m_points.add(point_type(x1234, y1234, path_flags_jr));
            return;
            }
            break;
        case 1:
          if ((d3 * d3) <=
              (m_distance_tolerance_square * ((dx * dx) + (dy * dy)))) {
                m_points.add(point_type(x23, y23, path_flags_jr));
                return;
            }
            break;
        case 2:
          if ((d2 * d2) <=
              (m_distance_tolerance_square * ((dx * dx) + (dy * dy)))) {
                m_points.add(point_type(x23, y23, path_flags_jr));
                return;
            }
            break;
        case 3:
          if (((d2 + d3) * (d2 + d3)) <=
              (m_distance_tolerance_square * ((dx * dx) + (dy * dy)))) {
                m_points.add(point_type(x23, y23, path_flags_jr));
                return;
            }
            break;
    }
    recursive_bezier(x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
    recursive_bezier(x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);
}
void curve4_div::bezier(float x1, float y1,
                        float x2, float y2,
                        float x3, float y3,
                        float x4, float y4)
{
    m_points.add(point_type(x1, y1));
    recursive_bezier(x1, y1, x2, y2, x3, y3, x4, y4, 0);
    m_points.add(point_type(x4, y4));
}
}
