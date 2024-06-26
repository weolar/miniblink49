// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_DISPLAY_SCHEDULER_H_
#define CC_SURFACES_DISPLAY_SCHEDULER_H_

#include "base/cancelable_callback.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/single_thread_task_runner.h"
#include "cc/scheduler/begin_frame_source.h"
#include "cc/surfaces/surface_id.h"
#include "cc/surfaces/surfaces_export.h"

namespace cc {

class OutputSurface;
class BeginFrameSource;

// TODO(brianderson): Reconcile with SurfacesScheduler crbug.com/476676
class CC_SURFACES_EXPORT DisplaySchedulerClient {
public:
    virtual ~DisplaySchedulerClient() { }

    virtual bool DrawAndSwap() = 0;
};

class CC_SURFACES_EXPORT DisplayScheduler : public BeginFrameObserverBase {
public:
    DisplayScheduler(DisplaySchedulerClient* client,
        BeginFrameSource* begin_frame_source,
        base::SingleThreadTaskRunner* task_runner,
        int max_pending_swaps);
    ~DisplayScheduler() override;

    void SetRootSurfaceResourcesLocked(bool locked);
    void ForceImmediateSwapIfPossible();
    virtual void DisplayResized();
    virtual void SetNewRootSurface(SurfaceId root_surface_id);
    virtual void SurfaceDamaged(SurfaceId surface_id);

    virtual void DidSwapBuffers();
    void DidSwapBuffersComplete();

    void OutputSurfaceLost();

    // BeginFrameObserverBase implementation
    bool OnBeginFrameDerivedImpl(const BeginFrameArgs& args) override;

protected:
    base::TimeTicks DesiredBeginFrameDeadlineTime();
    virtual void ScheduleBeginFrameDeadline();
    void AttemptDrawAndSwap();
    void OnBeginFrameDeadline();
    void DrawAndSwap();

    DisplaySchedulerClient* client_;
    BeginFrameSource* begin_frame_source_;
    base::SingleThreadTaskRunner* task_runner_;

    BeginFrameArgs current_begin_frame_args_;
    base::Closure begin_frame_deadline_closure_;
    base::CancelableClosure begin_frame_deadline_task_;
    base::TimeTicks begin_frame_deadline_task_time_;

    bool output_surface_lost_;
    bool root_surface_resources_locked_;

    bool inside_begin_frame_deadline_interval_;
    bool needs_draw_;
    bool expecting_root_surface_damage_because_of_resize_;
    bool all_active_child_surfaces_ready_to_draw_;

    int pending_swaps_;
    int max_pending_swaps_;

    SurfaceId root_surface_id_;
    bool root_surface_damaged_;
    bool expect_damage_from_root_surface_;

    std::set<SurfaceId> child_surface_ids_damaged_;
    std::set<SurfaceId> child_surface_ids_damaged_prev_;
    std::vector<SurfaceId> child_surface_ids_to_expect_damage_from_;

    base::WeakPtrFactory<DisplayScheduler> weak_ptr_factory_;

private:
    DISALLOW_COPY_AND_ASSIGN(DisplayScheduler);
};

} // namespace cc

#endif // CC_SURFACES_DISPLAY_SCHEDULER_H_
