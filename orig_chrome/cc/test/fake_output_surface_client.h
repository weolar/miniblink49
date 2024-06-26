// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_OUTPUT_SURFACE_CLIENT_H_
#define CC_TEST_FAKE_OUTPUT_SURFACE_CLIENT_H_

#include "cc/output/managed_memory_policy.h"
#include "cc/output/output_surface_client.h"

namespace cc {

class OutputSurface;

class FakeOutputSurfaceClient : public OutputSurfaceClient {
public:
    FakeOutputSurfaceClient()
        : swap_count_(0)
        , did_lose_output_surface_called_(false)
        , memory_policy_(0)
    {
    }

    void CommitVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval) override { }
    void SetNeedsRedrawRect(const gfx::Rect& damage_rect) override { }
    void DidSwapBuffers() override;
    void DidSwapBuffersComplete() override { }
    void ReclaimResources(const CompositorFrameAck* ack) override { }
    void DidLoseOutputSurface() override;
    void SetExternalDrawConstraints(
        const gfx::Transform& transform,
        const gfx::Rect& viewport,
        const gfx::Rect& clip,
        const gfx::Rect& viewport_rect_for_tile_priority,
        const gfx::Transform& transform_for_tile_priority,
        bool resourceless_software_draw) override { }
    void SetMemoryPolicy(const ManagedMemoryPolicy& policy) override;
    void SetTreeActivationCallback(const base::Closure&) override { }
    void OnDraw() override { }

    int swap_count() { return swap_count_; }

    bool did_lose_output_surface_called()
    {
        return did_lose_output_surface_called_;
    }

    const ManagedMemoryPolicy& memory_policy() const { return memory_policy_; }

private:
    int swap_count_;
    bool did_lose_output_surface_called_;
    ManagedMemoryPolicy memory_policy_;
};

} // namespace cc

#endif // CC_TEST_FAKE_OUTPUT_SURFACE_CLIENT_H_
