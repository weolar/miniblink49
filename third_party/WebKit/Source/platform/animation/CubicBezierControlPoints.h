// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CubicBezierControlPoints_h
#define CubicBezierControlPoints_h

namespace blink {

struct CubicBezierControlPoints {
    double x0, y0;
    double x1, y1;
    double x2, y2;
    double x3, y3;

    CubicBezierControlPoints()
        : x0(0)
        , y0(0)
        , x1(0)
        , y1(0)
        , x2(0)
        , y2(0)
        , x3(0)
        , y3(0)
    { }

    CubicBezierControlPoints(double x0, double y0, double x1, double y1,
        double x2, double y2, double x3, double y3)
        : x0(x0)
        , y0(y0)
        , x1(x1)
        , y1(y1)
        , x2(x2)
        , y2(y2)
        , x3(x3)
        , y3(y3)
    { }

    void divide(double t, CubicBezierControlPoints& left, CubicBezierControlPoints& right) const;
    size_t findTurningPoints(double& left, double& right) const;
    bool findIntersection(double intersectionY, double& intersectionX) const;

private:
    bool findInflexionPoint(double& solution) const;
};

} // namespace blink

#endif
