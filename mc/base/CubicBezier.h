// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_base_CubicBezier_h
#define mc_base_CubicBezier_h

namespace mc {

class CubicBezier {
public:
    CubicBezier(double x1, double y1, double x2, double y2);
    ~CubicBezier();

    // Returns an approximation of y at the given x.
    double solve(double x) const;

    // Returns an approximation of dy/dx at the given x.
    double slope(double x) const;

    // Sets |min| and |max| to the bezier's minimum and maximium y values in the
    // interval [0, 1].
    void range(double* min, double* max) const;

private:
    void initGradients();

    double m_x1;
    double m_y1;
    double m_x2;
    double m_y2;

    double m_startGradient;
    double m_endGradient;
};

}  // namespace mc

#endif  // mc_base_CubicBezier_h
