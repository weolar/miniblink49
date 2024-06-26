// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_DISPLAY_FINDER_H_
#define UI_GFX_DISPLAY_FINDER_H_

#include <vector>

#include "ui/gfx/gfx_export.h"

namespace gfx {

class Display;
class Point;
class Rect;

// Returns the display in |displays| closest to |point|.
GFX_EXPORT const Display* FindDisplayNearestPoint(
    const std::vector<Display>& displays,
    const Point& point);

// Returns the display in |displays| with the biggest intersection of |rect|.
// If none of the displays intersect |rect| null is returned.
GFX_EXPORT const Display* FindDisplayWithBiggestIntersection(
    const std::vector<Display>& displays,
    const Rect& rect);

} // namespace gfx

#endif // UI_GFX_DISPLAY_FINDER_H_
