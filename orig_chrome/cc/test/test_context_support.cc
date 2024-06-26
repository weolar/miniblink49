// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/test_context_support.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"

namespace cc {

TestContextSupport::TestContextSupport()
    : weak_ptr_factory_(this)
{
}

TestContextSupport::~TestContextSupport() { }

void TestContextSupport::SignalSyncPoint(uint32 sync_point,
    const base::Closure& callback)
{
    sync_point_callbacks_.push_back(callback);
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::Bind(&TestContextSupport::CallAllSyncPointCallbacks, weak_ptr_factory_.GetWeakPtr()));
}

void TestContextSupport::SignalQuery(uint32 query,
    const base::Closure& callback)
{
    sync_point_callbacks_.push_back(callback);
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::Bind(&TestContextSupport::CallAllSyncPointCallbacks, weak_ptr_factory_.GetWeakPtr()));
}

void TestContextSupport::SetSurfaceVisible(bool visible)
{
    if (!set_visible_callback_.is_null()) {
        set_visible_callback_.Run(visible);
    }
}

void TestContextSupport::SetAggressivelyFreeResources(
    bool aggressively_free_resources)
{
}

void TestContextSupport::CallAllSyncPointCallbacks()
{
    for (size_t i = 0; i < sync_point_callbacks_.size(); ++i) {
        base::ThreadTaskRunnerHandle::Get()->PostTask(FROM_HERE,
            sync_point_callbacks_[i]);
    }
    sync_point_callbacks_.clear();
}

void TestContextSupport::SetSurfaceVisibleCallback(
    const SurfaceVisibleCallback& set_visible_callback)
{
    set_visible_callback_ = set_visible_callback;
}

void TestContextSupport::SetScheduleOverlayPlaneCallback(
    const ScheduleOverlayPlaneCallback& schedule_overlay_plane_callback)
{
    schedule_overlay_plane_callback_ = schedule_overlay_plane_callback;
}

void TestContextSupport::Swap()
{
}

uint32 TestContextSupport::InsertFutureSyncPointCHROMIUM()
{
    NOTIMPLEMENTED();
    return 0;
}

void TestContextSupport::RetireSyncPointCHROMIUM(uint32 sync_point)
{
    NOTIMPLEMENTED();
}

void TestContextSupport::PartialSwapBuffers(const gfx::Rect& sub_buffer)
{
}

void TestContextSupport::ScheduleOverlayPlane(
    int plane_z_order,
    gfx::OverlayTransform plane_transform,
    unsigned overlay_texture_id,
    const gfx::Rect& display_bounds,
    const gfx::RectF& uv_rect)
{
    if (!schedule_overlay_plane_callback_.is_null()) {
        schedule_overlay_plane_callback_.Run(plane_z_order,
            plane_transform,
            overlay_texture_id,
            display_bounds,
            uv_rect);
    }
}

uint64_t TestContextSupport::ShareGroupTracingGUID() const
{
    NOTIMPLEMENTED();
    return 0;
}

} // namespace cc
