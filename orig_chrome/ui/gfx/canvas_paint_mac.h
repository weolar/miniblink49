
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_CANVAS_PAINT_MAC_H_
#define UI_GFX_CANVAS_PAINT_MAC_H_

#include "skia/ext/platform_canvas.h"
#include "ui/gfx/canvas.h"

#import <Cocoa/Cocoa.h>

namespace gfx {

// A class designed to translate skia painting into a region to the current
// graphics context.  On construction, it will set up a context for painting
// into, and on destruction, it will commit it to the current context.
// Note: The created context is always inialized to (0, 0, 0, 0).
class GFX_EXPORT CanvasSkiaPaint : public Canvas {
public:
    // This constructor assumes the result is opaque.
    explicit CanvasSkiaPaint(NSRect dirtyRect);
    CanvasSkiaPaint(NSRect dirtyRect, bool opaque);
    ~CanvasSkiaPaint() override;

    // If true, the data painted into the CanvasSkiaPaint is blended onto the
    // current context, else it is copied.
    void set_composite_alpha(bool composite_alpha)
    {
        composite_alpha_ = composite_alpha;
    }

    // Returns true if the invalid region is empty. The caller should call this
    // function to determine if anything needs painting.
    bool is_empty() const
    {
        return NSIsEmptyRect(rectangle_);
    }

    const NSRect& rectangle() const
    {
        return rectangle_;
    }

private:
    void Init(bool opaque);

    CGContext* context_;
    NSRect rectangle_;
    // See description above setter.
    bool composite_alpha_;

    // Disallow copy and assign.
    CanvasSkiaPaint(const CanvasSkiaPaint&);
    CanvasSkiaPaint& operator=(const CanvasSkiaPaint&);
};

} // namespace gfx

#endif // UI_GFX_CANVAS_PAINT_MAC_H_
