// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_TEST_CONTEXT_SUPPORT_H_
#define CC_TEST_TEST_CONTEXT_SUPPORT_H_

#include <vector>

#include "base/memory/weak_ptr.h"
#include "gpu/command_buffer/client/context_support.h"

namespace gfx {
class Rect;
class RectF;
}

namespace cc {

class TestContextSupport : public gpu::ContextSupport {
public:
    TestContextSupport();
    ~TestContextSupport() override;

    // gpu::ContextSupport implementation.
    void SignalSyncPoint(uint32 sync_point,
        const base::Closure& callback) override;
    void SignalQuery(uint32 query, const base::Closure& callback) override;
    void SetSurfaceVisible(bool visible) override;
    void SetAggressivelyFreeResources(bool aggressively_free_resources) override;
    void Swap() override;
    void PartialSwapBuffers(const gfx::Rect& sub_buffer) override;
    uint32 InsertFutureSyncPointCHROMIUM() override;
    void RetireSyncPointCHROMIUM(uint32 sync_point) override;
    void ScheduleOverlayPlane(int plane_z_order,
        gfx::OverlayTransform plane_transform,
        unsigned overlay_texture_id,
        const gfx::Rect& display_bounds,
        const gfx::RectF& uv_rect) override;
    uint64_t ShareGroupTracingGUID() const override;

    void CallAllSyncPointCallbacks();

    typedef base::Callback<void(bool visible)> SurfaceVisibleCallback;
    void SetSurfaceVisibleCallback(
        const SurfaceVisibleCallback& set_visible_callback);

    typedef base::Callback<void(int plane_z_order,
        gfx::OverlayTransform plane_transform,
        unsigned overlay_texture_id,
        const gfx::Rect& display_bounds,
        const gfx::RectF& crop_rect)>
        ScheduleOverlayPlaneCallback;
    void SetScheduleOverlayPlaneCallback(
        const ScheduleOverlayPlaneCallback& schedule_overlay_plane_callback);

private:
    std::vector<base::Closure> sync_point_callbacks_;
    SurfaceVisibleCallback set_visible_callback_;
    ScheduleOverlayPlaneCallback schedule_overlay_plane_callback_;

    base::WeakPtrFactory<TestContextSupport> weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(TestContextSupport);
};

} // namespace cc

#endif // CC_TEST_TEST_CONTEXT_SUPPORT_H_
