// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RASTER_ZERO_COPY_TILE_TASK_WORKER_POOL_H_
#define CC_RASTER_ZERO_COPY_TILE_TASK_WORKER_POOL_H_

#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "cc/raster/tile_task_runner.h"
#include "cc/raster/tile_task_worker_pool.h"

namespace base {
namespace trace_event {
    class ConvertableToTraceFormat;
}
}

namespace cc {
class ResourceProvider;

class CC_EXPORT ZeroCopyTileTaskWorkerPool : public TileTaskWorkerPool,
                                             public TileTaskRunner,
                                             public TileTaskClient {
public:
    ~ZeroCopyTileTaskWorkerPool() override;

    static scoped_ptr<TileTaskWorkerPool> Create(
        base::SequencedTaskRunner* task_runner,
        TaskGraphRunner* task_graph_runner,
        ResourceProvider* resource_provider,
        bool use_rgba_4444_texture_format);

    // Overridden from TileTaskWorkerPool:
    TileTaskRunner* AsTileTaskRunner() override;

    // Overridden from TileTaskRunner:
    void SetClient(TileTaskRunnerClient* client) override;
    void Shutdown() override;
    void ScheduleTasks(TileTaskQueue* queue) override;
    void CheckForCompletedTasks() override;
    ResourceFormat GetResourceFormat(bool must_support_alpha) const override;
    bool GetResourceRequiresSwizzle(bool must_support_alpha) const override;

    // Overridden from TileTaskClient:
    scoped_ptr<RasterBuffer> AcquireBufferForRaster(
        const Resource* resource,
        uint64_t resource_content_id,
        uint64_t previous_content_id) override;
    void ReleaseBufferForRaster(scoped_ptr<RasterBuffer> buffer) override;

protected:
    ZeroCopyTileTaskWorkerPool(base::SequencedTaskRunner* task_runner,
        TaskGraphRunner* task_graph_runner,
        ResourceProvider* resource_provider,
        bool use_rgba_4444_texture_format);

private:
    void OnTaskSetFinished(TaskSet task_set);
    //   scoped_refptr<base::trace_event::ConvertableToTraceFormat> StateAsValue()
    //       const;

    scoped_refptr<base::SequencedTaskRunner> task_runner_;
    TaskGraphRunner* task_graph_runner_;
    const NamespaceToken namespace_token_;
    TileTaskRunnerClient* client_;
    ResourceProvider* resource_provider_;

    bool use_rgba_4444_texture_format_;

    TaskSetCollection tasks_pending_;

    scoped_refptr<TileTask> task_set_finished_tasks_[kNumberOfTaskSets];

    // Task graph used when scheduling tasks and vector used to gather
    // completed tasks.
    TaskGraph graph_;
    Task::Vector completed_tasks_;

    base::WeakPtrFactory<ZeroCopyTileTaskWorkerPool>
        task_set_finished_weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(ZeroCopyTileTaskWorkerPool);
};

} // namespace cc

#endif // CC_RASTER_ZERO_COPY_TILE_TASK_WORKER_POOL_H_
