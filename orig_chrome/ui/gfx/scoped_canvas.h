// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_SCOPED_CANVAS_H_
#define UI_GFX_SCOPED_CANVAS_H_

#include "base/macros.h"
#include "ui/gfx/canvas.h"

namespace gfx {

// Saves the drawing state, and restores the state when going out of scope.
class ScopedCanvas {
public:
    explicit ScopedCanvas(gfx::Canvas* canvas)
        : canvas_(canvas)
    {
        if (canvas_)
            canvas_->Save();
    }
    ~ScopedCanvas()
    {
        if (canvas_)
            canvas_->Restore();
    }

private:
    gfx::Canvas* canvas_;

    DISALLOW_COPY_AND_ASSIGN(ScopedCanvas);
};

} // namespace gfx

#endif // UI_GFX_SCOPED_CANVAS_H_
