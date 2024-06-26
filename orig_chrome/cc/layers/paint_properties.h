// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_PAINT_PROPERTIES_H_
#define CC_LAYERS_PAINT_PROPERTIES_H_

#include "ui/gfx/geometry/size.h"

namespace cc {

// Container for properties that layers need to save before they can be paint.
struct CC_EXPORT PaintProperties {
    PaintProperties()
        : source_frame_number(-1)
    {
    }

    gfx::Size bounds;

    int source_frame_number;
};

} // namespace cc

#endif // CC_LAYERS_PAINT_PROPERTIES_H_
