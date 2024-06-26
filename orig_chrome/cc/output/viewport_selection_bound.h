// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_VIEWPORT_SELECTION_BOUND_H_
#define CC_OUTPUT_VIEWPORT_SELECTION_BOUND_H_

#include "cc/base/cc_export.h"
#include "cc/input/selection.h"
#include "cc/input/selection_bound_type.h"
#include "ui/gfx/geometry/point_f.h"

namespace cc {

// Marker for a selection end-point in (DIP) viewport coordinates.
// TODO(jdduke): Move this to ui/gfx and merge with ui::SelectionBound.
struct CC_EXPORT ViewportSelectionBound {
    ViewportSelectionBound();
    ~ViewportSelectionBound();

    SelectionBoundType type;
    gfx::PointF edge_top;
    gfx::PointF edge_bottom;
    bool visible;
};

CC_EXPORT bool operator==(const ViewportSelectionBound& lhs,
    const ViewportSelectionBound& rhs);
CC_EXPORT bool operator!=(const ViewportSelectionBound& lhs,
    const ViewportSelectionBound& rhs);

typedef Selection<ViewportSelectionBound> ViewportSelection;

} // namespace cc

#endif // CC_OUTPUT_VIEWPORT_SELECTION_BOUND_H_
