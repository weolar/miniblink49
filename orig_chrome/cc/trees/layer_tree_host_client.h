// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_LAYER_TREE_HOST_CLIENT_H_
#define CC_TREES_LAYER_TREE_HOST_CLIENT_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/debug/frame_timing_tracker.h"

namespace gfx {
class Vector2d;
class Vector2dF;
}

namespace cc {
class ContextProvider;
class InputHandlerClient;
class OutputSurface;
struct BeginFrameArgs;

class LayerTreeHostClient {
public:
    virtual void WillBeginMainFrame() = 0;
    // Marks finishing compositing-related tasks on the main thread. In threaded
    // mode, this corresponds to DidCommit().
    virtual void BeginMainFrame(const BeginFrameArgs& args) = 0;
    virtual void BeginMainFrameNotExpectedSoon() = 0;
    virtual void DidBeginMainFrame() = 0;
    virtual void Layout() = 0;
    virtual void ApplyViewportDeltas(
        const gfx::Vector2dF& inner_delta,
        const gfx::Vector2dF& outer_delta,
        const gfx::Vector2dF& elastic_overscroll_delta,
        float page_scale,
        float top_controls_delta)
        = 0;
    // Request an OutputSurface from the client. When the client has one it should
    // call LayerTreeHost::SetOutputSurface.  This will result in either
    // DidFailToInitializeOutputSurface or DidInitializeOutputSurface being
    // called.
    virtual void RequestNewOutputSurface() = 0;
    virtual void DidInitializeOutputSurface() = 0;
    virtual void DidFailToInitializeOutputSurface() = 0;
    virtual void WillCommit() = 0;
    virtual void DidCommit() = 0;
    virtual void DidCommitAndDrawFrame() = 0;
    virtual void DidCompleteSwapBuffers() = 0;
    virtual void RecordFrameTimingEvents(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
        = 0;

    // Called when page scale animation has completed.
    virtual void DidCompletePageScaleAnimation() = 0;

    // TODO(simonhong): Makes this to pure virtual function when client
    // implementation is ready.
    virtual void SendBeginFramesToChildren(const BeginFrameArgs& args) { }

protected:
    virtual ~LayerTreeHostClient() { }
};

} // namespace cc

#endif // CC_TREES_LAYER_TREE_HOST_CLIENT_H_
