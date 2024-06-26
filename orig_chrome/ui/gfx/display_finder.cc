// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/display_finder.h"

#include <limits>

#include "base/logging.h"
#include "ui/gfx/display.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"

namespace gfx {

const Display* FindDisplayNearestPoint(const std::vector<Display>& displays,
    const Point& point)
{
    DCHECK(!displays.empty());
    int min_distance = std::numeric_limits<int>::max();
    const Display* nearest_display = nullptr;
    for (const auto& display : displays) {
        const int distance = display.bounds().ManhattanDistanceToPoint(point);
        if (distance < min_distance) {
            min_distance = distance;
            nearest_display = &display;
        }
    }
    // There should always be at least one display that is less than INT_MAX away.
    DCHECK(nearest_display);
    return nearest_display;
}

const Display* FindDisplayWithBiggestIntersection(
    const std::vector<Display>& displays,
    const Rect& rect)
{
    DCHECK(!displays.empty());
    int max_area = 0;
    const Display* matching = nullptr;
    for (const auto& display : displays) {
        const Rect intersect = IntersectRects(display.bounds(), rect);
        const int area = intersect.width() * intersect.height();
        if (area > max_area) {
            max_area = area;
            matching = &display;
        }
    }
    return matching;
}

} // namespace gfx
