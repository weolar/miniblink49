// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_PATH_H_
#define UI_GFX_PATH_H_

#include <stddef.h>

#include "base/macros.h"
#include "third_party/skia/include/core/SkPath.h"
#include "ui/gfx/gfx_export.h"
#include "ui/gfx/native_widget_types.h"

namespace gfx {

class GFX_EXPORT Path : public SkPath {
public:
    // Used by Path(Point,size_t) constructor.
    struct Point {
        int x;
        int y;
    };
    struct PointF {
        float x;
        float y;
    };

    Path();

    // Creates a path populated with the specified points.
    Path(const Point* points, size_t count);
    Path(const PointF* points, size_t count);

    ~Path();

private:
    DISALLOW_COPY_AND_ASSIGN(Path);
};

}

#endif // UI_GFX_PATH_H_
