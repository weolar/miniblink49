// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RASTER_PIXEL_BUFFER_TILE_TASK_WORKER_POOL_H_
#define CC_RASTER_PIXEL_BUFFER_TILE_TASK_WORKER_POOL_H_

#include <deque>
#include <vector>

#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "cc/base/delayed_unique_notifier.h"
#include "cc/output/context_provider.h"
#include "cc/raster/tile_task_runner.h"
#include "cc/raster/tile_task_worker_pool.h"

namespace base {
namespace trace_event {
    class ConvertableToTraceFormat;
    class TracedValue;
}
}

namespace cc {
class ResourceProvider;

class CC_EXPORT PixelBufferTileTaskWorkerPool : public TileTaskWorkerPool,
                                                public TileTaskRunner,
                                                public TileTaskClient {
public:
    ~PixelBufferTileTaskWorkerPool() override;

    static scoped_ptr<TileTaskWorkerPool> Create(
        base::SequencedTaskRunner* task_runner,
        TaskGraphRunner* task_graph_runner,
        ContextProvider* context_provider,
        ResourceProvider* resource_provider,
        size_t max_transfer_buffer_usage_bytes);

    // Overridden from TileTaskWorkerPool:
    TileTaskRunner* AsTileTaskRunner() override;

    // Overridden from TileTaskRunner:
    void SetClient(TileTaskRunnerClient* client) override;
    void Shutdown() override;
    void ScheduleTasks(TileTaskQueue* queue) override;
    void CheckForCompletedTasks() override;
    ResourceFormat GetResourceFormat() const override;
    bool GetResourceRequiresSwizzle() const override;

    // Overridden from TileTaskClient:
    scoped_ptr<RasterBuffer> AcquireBufferForRaster(
        const Resource* resource,
        uint64_t resource_content_id,
        uint64_t previous_content_id) override;
    void ReleaseBufferForRaster(scoped_ptr<RasterBuffer> buffer) override;

private:
    struct RasterTaskState {
        class TaskComparator {
        public:
            explicit TaskComparator(const RasterTask* task)
                : task_(task)
            {
            }

            bool operator()(const RasterTaskState& state) const
            {
                return state.task == task_;
            }

        private:
            const RasterTask* task_;
        };

        typedef std::vector<RasterTaskState> Vector;

        RasterTaskState(RasterTask* task, const TaskSetCollection& task_sets);

        enum { UNSCHEDULED,
            SCHEDULED,
            UPLOADING,
            COMPLETED } type;
        RasterTask* task;
        TaskSetCollection task_sets;
    };

    typedef std::deque<scoped_refptr<RasterTask>> RasterTaskDeque;

    PixelBufferTileTaskWorkerPool(base::SequencedTaskRunner* task_runner,
        TaskGraphRunner* task_graph_runner,
        ContextProvider* context_provider,
        ResourceProvider* resource_provider,
        size_t max_transfer_buffer_usage_bytes);

    void OnTaskSetFinished(TaskSet task_set);
    void FlushUploads();
    void CheckForCompletedUploads();
    void CheckForCompletedRasterTasks();
    void ScheduleMoreTasks();
    size_t PendingRasterTaskCount() const;
    TaskSetCollection PendingTasks() const;
    void CheckForCompletedRasterizerTasks();

    const char* StateName() const;
    scoped_refptr<base::trace_event::ConvertableToTraceFormat> StateAsValue()
        const;
    void ThrottleStateAsValueInto(
        base::trace_event::TracedValue* throttle_state) const;

    scoped_refptr<base::SequencedTaskRunner> task_runner_;
    TaskGraphRunner* task_graph_runner_;
    const NamespaceToken namespace_token_;
    TileTaskRunnerClient* client_;
    ContextProvider* context_provider_;
    ResourceProvider* resource_provider_;

    bool shutdown_;

    TileTaskQueue raster_tasks_;
    RasterTaskState::Vector raster_task_states_;
    RasterTaskDeque raster_tasks_with_pending_upload_;
    RasterTask::Vector completed_raster_tasks_;
    TileTask::Vector completed_image_decode_tasks_;

    size_t scheduled_raster_task_count_;
    size_t task_counts_[kNumberOfTaskSets];
    size_t bytes_pending_upload_;
    size_t max_bytes_pending_upload_;
    bool has_performed_uploads_since_last_flush_;

    TaskSetCollection should_notify_client_if_no_tasks_are_pending_;
    TaskSetCollection task_set_finished_tasks_pending_;

    DelayedUniqueNotifier check_for_completed_raster_task_notifier_;

    scoped_refptr<TileTask> task_set_finished_tasks_[kNumberOfTaskSets];

    // Task graph used when scheduling tasks and vector used to gather
    // completed tasks.
    TaskGraph graph_;
    Task::Vector completed_tasks_;

    base::WeakPtrFactory<PixelBufferTileTaskWorkerPool>
        task_set_finished_weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(PixelBufferTileTaskWorkerPool);
};

} // namespace cc

#endif // CC_RASTER_PIXEL_BUFFER_TILE_TASK_WORKER_POOL_H_
