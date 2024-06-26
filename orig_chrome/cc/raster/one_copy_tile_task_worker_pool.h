// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RASTER_ONE_COPY_TILE_TASK_WORKER_POOL_H_
#define CC_RASTER_ONE_COPY_TILE_TASK_WORKER_POOL_H_

#include <set>

#include "base/memory/weak_ptr.h"
#include "base/synchronization/lock.h"
#include "base/time/time.h"
#include "base/trace_event/memory_dump_provider.h"
#include "base/values.h"
#include "cc/base/scoped_ptr_deque.h"
#include "cc/output/context_provider.h"
#include "cc/raster/tile_task_runner.h"
#include "cc/raster/tile_task_worker_pool.h"
#include "cc/resources/resource_provider.h"

namespace base {
namespace trace_event {
    class ConvertableToTraceFormat;
    class TracedValue;
}
}

namespace gpu {
namespace gles2 {
    class GLES2Interface;
}
}

namespace cc {
class ResourcePool;

class CC_EXPORT OneCopyTileTaskWorkerPool
    : public TileTaskWorkerPool,
      public TileTaskRunner,
      public TileTaskClient,
      public base::trace_event::MemoryDumpProvider {
public:
    ~OneCopyTileTaskWorkerPool() override;

    static scoped_ptr<TileTaskWorkerPool> Create(
        base::SequencedTaskRunner* task_runner,
        TaskGraphRunner* task_graph_runner,
        ContextProvider* context_provider,
        ResourceProvider* resource_provider,
        int max_copy_texture_chromium_size,
        bool use_persistent_gpu_memory_buffers,
        int max_staging_buffer_usage_in_bytes,
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

    // Overridden from base::trace_event::MemoryDumpProvider:
    bool OnMemoryDump(const base::trace_event::MemoryDumpArgs& args,
        base::trace_event::ProcessMemoryDump* pmd) override;

    // Playback raster source and copy result into |resource|.
    void PlaybackAndCopyOnWorkerThread(
        const Resource* resource,
        const ResourceProvider::ScopedWriteLockGL* resource_lock,
        const RasterSource* raster_source,
        const gfx::Rect& raster_full_rect,
        const gfx::Rect& raster_dirty_rect,
        float scale,
        bool include_images,
        uint64_t resource_content_id,
        uint64_t previous_content_id);

protected:
    OneCopyTileTaskWorkerPool(base::SequencedTaskRunner* task_runner,
        TaskGraphRunner* task_graph_runner,
        ResourceProvider* resource_provider,
        int max_copy_texture_chromium_size,
        bool use_persistent_gpu_memory_buffers,
        int max_staging_buffer_usage_in_bytes,
        bool use_rgba_4444_texture_format);

private:
    struct StagingBuffer {
        StagingBuffer(const gfx::Size& size, ResourceFormat format);
        ~StagingBuffer();

        void DestroyGLResources(gpu::gles2::GLES2Interface* gl);
        void OnMemoryDump(base::trace_event::ProcessMemoryDump* pmd,
            ResourceFormat format,
            bool is_free) const;

        const gfx::Size size;
        const ResourceFormat format;
        scoped_ptr<gfx::GpuMemoryBuffer> gpu_memory_buffer;
        base::TimeTicks last_usage;
        unsigned texture_id;
        unsigned image_id;
        unsigned query_id;
        uint64_t content_id;
    };

    void AddStagingBuffer(const StagingBuffer* staging_buffer,
        ResourceFormat format);
    void RemoveStagingBuffer(const StagingBuffer* staging_buffer);
    void MarkStagingBufferAsFree(const StagingBuffer* staging_buffer);
    void MarkStagingBufferAsBusy(const StagingBuffer* staging_buffer);
    scoped_ptr<StagingBuffer> AcquireStagingBuffer(const Resource* resource,
        uint64_t previous_content_id);
    base::TimeTicks GetUsageTimeForLRUBuffer();
    void ScheduleReduceMemoryUsage();
    void ReduceMemoryUsage();
    void ReleaseBuffersNotUsedSince(base::TimeTicks time);

    void OnTaskSetFinished(TaskSet task_set);
    scoped_refptr<base::trace_event::ConvertableToTraceFormat> StateAsValue()
        const;
    void StagingStateAsValueInto(
        base::trace_event::TracedValue* staging_state) const;

    scoped_refptr<base::SequencedTaskRunner> task_runner_;
    TaskGraphRunner* task_graph_runner_;
    const NamespaceToken namespace_token_;
    TileTaskRunnerClient* client_;
    ResourceProvider* const resource_provider_;
    const int max_bytes_per_copy_operation_;
    const bool use_persistent_gpu_memory_buffers_;
    TaskSetCollection tasks_pending_;
    scoped_refptr<TileTask> task_set_finished_tasks_[kNumberOfTaskSets];

    // Task graph used when scheduling tasks and vector used to gather
    // completed tasks.
    TaskGraph graph_;
    Task::Vector completed_tasks_;

    mutable base::Lock lock_;
    // |lock_| must be acquired when accessing the following members.
    using StagingBufferSet = std::set<const StagingBuffer*>;
    StagingBufferSet buffers_;
    using StagingBufferDeque = ScopedPtrDeque<StagingBuffer>;
    StagingBufferDeque free_buffers_;
    StagingBufferDeque busy_buffers_;
    int bytes_scheduled_since_last_flush_;
    const int max_staging_buffer_usage_in_bytes_;
    bool use_rgba_4444_texture_format_;
    int staging_buffer_usage_in_bytes_;
    int free_staging_buffer_usage_in_bytes_;
    const base::TimeDelta staging_buffer_expiration_delay_;
    bool reduce_memory_usage_pending_;
    base::Closure reduce_memory_usage_callback_;

    base::WeakPtrFactory<OneCopyTileTaskWorkerPool> weak_ptr_factory_;
    // "raster finished" tasks need their own factory as they need to be
    // canceled when ScheduleTasks() is called.
    base::WeakPtrFactory<OneCopyTileTaskWorkerPool>
        task_set_finished_weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(OneCopyTileTaskWorkerPool);
};

} // namespace cc

#endif // CC_RASTER_ONE_COPY_TILE_TASK_WORKER_POOL_H_
