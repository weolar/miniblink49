// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_SOFTWARE_OUTPUT_DEVICE_H_
#define CC_OUTPUT_SOFTWARE_OUTPUT_DEVICE_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/vector2d.h"

class SkBitmap;
class SkCanvas;

namespace gfx {
class VSyncProvider;
}

namespace cc {

// This is a "tear-off" class providing software drawing support to
// OutputSurface, such as to a platform-provided window framebuffer.
class CC_EXPORT SoftwareOutputDevice {
public:
    SoftwareOutputDevice();
    virtual ~SoftwareOutputDevice();

    // Discards any pre-existing backing buffers and allocates memory for a
    // software device of |size|. This must be called before the
    // |SoftwareOutputDevice| can be used in other ways.
    virtual void Resize(const gfx::Size& pixel_size, float scale_factor);

    // Called on BeginDrawingFrame. The compositor will draw into the returned
    // SkCanvas. The |SoftwareOutputDevice| implementation needs to provide a
    // valid SkCanvas of at least size |damage_rect|. This class retains ownership
    // of the SkCanvas.
    virtual SkCanvas* BeginPaint(const gfx::Rect& damage_rect);

    // Called on FinishDrawingFrame. The compositor will no longer mutate the the
    // SkCanvas instance returned by |BeginPaint| and should discard any reference
    // that it holds to it.
    virtual void EndPaint();

    // Discard the backing buffer in the surface provided by this instance.
    virtual void DiscardBackbuffer() { }

    // Ensures that there is a backing buffer available on this instance.
    virtual void EnsureBackbuffer() { }

    // VSyncProvider used to update the timer used to schedule draws with the
    // hardware vsync. Return NULL if a provider doesn't exist.
    virtual gfx::VSyncProvider* GetVSyncProvider();

protected:
    gfx::Size viewport_pixel_size_;
    float scale_factor_;
    gfx::Rect damage_rect_;
    skia::RefPtr<SkSurface> surface_;
    scoped_ptr<gfx::VSyncProvider> vsync_provider_;

private:
    DISALLOW_COPY_AND_ASSIGN(SoftwareOutputDevice);
};

} // namespace cc

#endif // CC_OUTPUT_SOFTWARE_OUTPUT_DEVICE_H_
