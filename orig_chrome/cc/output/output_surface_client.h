// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_OUTPUT_SURFACE_CLIENT_H_
#define CC_OUTPUT_OUTPUT_SURFACE_CLIENT_H_

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "cc/base/cc_export.h"
#include "cc/output/context_provider.h"
#include "ui/gfx/geometry/rect.h"

namespace gfx {
class Transform;
}

namespace cc {

class CompositorFrameAck;
struct ManagedMemoryPolicy;

class CC_EXPORT OutputSurfaceClient {
public:
    virtual void CommitVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval)
        = 0;
    virtual void SetNeedsRedrawRect(const gfx::Rect& damage_rect) = 0;
    virtual void DidSwapBuffers() = 0;
    virtual void DidSwapBuffersComplete() = 0;
    virtual void ReclaimResources(const CompositorFrameAck* ack) = 0;
    virtual void DidLoseOutputSurface() = 0;
    virtual void SetExternalDrawConstraints(
        const gfx::Transform& transform,
        const gfx::Rect& viewport,
        const gfx::Rect& clip,
        const gfx::Rect& viewport_rect_for_tile_priority,
        const gfx::Transform& transform_for_tile_priority,
        bool resourceless_software_draw)
        = 0;
    virtual void SetMemoryPolicy(const ManagedMemoryPolicy& policy) = 0;
    // If set, |callback| will be called subsequent to each new tree activation,
    // regardless of the compositor visibility or damage. |callback| must remain
    // valid for the lifetime of the OutputSurfaceClient or until unregisted --
    // use SetTreeActivationCallback(base::Closure()) to unregister it.
    virtual void SetTreeActivationCallback(const base::Closure& callback) = 0;
    // This allows the output surface to ask it's client for a draw.
    virtual void OnDraw() = 0;

protected:
    virtual ~OutputSurfaceClient() { }
};

} // namespace cc

#endif // CC_OUTPUT_OUTPUT_SURFACE_CLIENT_H_
