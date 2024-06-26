// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_LAYER_TREE_HOST_CLIENT_H_
#define CC_TEST_FAKE_LAYER_TREE_HOST_CLIENT_H_

#include "base/memory/scoped_ptr.h"
#include "cc/input/input_handler.h"
#include "cc/test/test_context_provider.h"
#include "cc/trees/layer_tree_host_client.h"
#include "cc/trees/layer_tree_host_single_thread_client.h"

namespace cc {
class OutputSurface;

class FakeLayerTreeHostClient : public LayerTreeHostClient,
                                public LayerTreeHostSingleThreadClient {
public:
    enum RendererOptions {
        DIRECT_3D,
        DIRECT_SOFTWARE,
        DELEGATED_3D,
        DELEGATED_SOFTWARE
    };
    explicit FakeLayerTreeHostClient(RendererOptions options);
    ~FakeLayerTreeHostClient() override;

    // Caller responsible for unsetting this and maintaining the host's lifetime.
    void SetLayerTreeHost(LayerTreeHost* host) { host_ = host; }

    // LayerTreeHostClient implementation.
    void WillBeginMainFrame() override { }
    void DidBeginMainFrame() override { }
    void BeginMainFrame(const BeginFrameArgs& args) override { }
    void BeginMainFrameNotExpectedSoon() override { }
    void Layout() override { }
    void ApplyViewportDeltas(const gfx::Vector2dF& inner_delta,
        const gfx::Vector2dF& outer_delta,
        const gfx::Vector2dF& elastic_overscroll_delta,
        float page_scale,
        float top_controls_delta) override { }
    void RequestNewOutputSurface() override;
    void DidInitializeOutputSurface() override { }
    void DidFailToInitializeOutputSurface() override;
    void WillCommit() override { }
    void DidCommit() override { }
    void DidCommitAndDrawFrame() override { }
    void DidCompleteSwapBuffers() override { }
    void DidCompletePageScaleAnimation() override { }
    void RecordFrameTimingEvents(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
        override { }

    // LayerTreeHostSingleThreadClient implementation.
    void DidPostSwapBuffers() override { }
    void DidAbortSwapBuffers() override { }

private:
    bool use_software_rendering_;
    bool use_delegating_renderer_;

    LayerTreeHost* host_;
};

} // namespace cc

#endif // CC_TEST_FAKE_LAYER_TREE_HOST_CLIENT_H_
