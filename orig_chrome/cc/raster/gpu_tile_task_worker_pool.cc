// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/gpu_tile_task_worker_pool.h"

#include <algorithm>

#include "base/bind.h"
#include "base/trace_event/trace_event.h"
#include "cc/playback/raster_source.h"
#include "cc/raster/gpu_rasterizer.h"
#include "cc/raster/raster_buffer.h"
#include "cc/raster/scoped_gpu_raster.h"
#include "cc/resources/resource.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "third_party/skia/include/core/SkMultiPictureDraw.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/gpu/GrContext.h"

namespace cc {
namespace {

    class RasterBufferImpl : public RasterBuffer {
    public:
        RasterBufferImpl(GpuRasterizer* rasterizer,
            const Resource* resource,
            uint64_t resource_content_id,
            uint64_t previous_content_id)
            : rasterizer_(rasterizer)
            , lock_(rasterizer->resource_provider(), resource->id())
            , resource_has_previous_content_(
                  resource_content_id && resource_content_id == previous_content_id)
        {
        }

        // Overridden from RasterBuffer:
        void Playback(const RasterSource* raster_source,
            const gfx::Rect& raster_full_rect,
            const gfx::Rect& raster_dirty_rect,
            uint64_t new_content_id,
            float scale,
            bool include_images) override
        {
            TRACE_EVENT0("cc", "RasterBufferImpl::Playback");
            // GPU raster doesn't do low res tiles, so should always include images.
            DCHECK(include_images);
            ContextProvider* context_provider = rasterizer_->resource_provider()
                                                    ->output_surface()
                                                    ->worker_context_provider();
            DCHECK(context_provider);

            ContextProvider::ScopedContextLock scoped_context(context_provider);

            gfx::Rect playback_rect = raster_full_rect;
            if (resource_has_previous_content_) {
                playback_rect.Intersect(raster_dirty_rect);
            }
            DCHECK(!playback_rect.IsEmpty())
                << "Why are we rastering a tile that's not dirty?";

            // TODO(danakj): Implement partial raster with raster_dirty_rect.
            // Rasterize source into resource.
            rasterizer_->RasterizeSource(&lock_, raster_source, raster_full_rect,
                playback_rect, scale);

            // Barrier to sync worker context output to cc context.
            scoped_context.ContextGL()->OrderingBarrierCHROMIUM();
        }

    private:
        GpuRasterizer* rasterizer_;
        ResourceProvider::ScopedWriteLockGr lock_;
        bool resource_has_previous_content_;

        DISALLOW_COPY_AND_ASSIGN(RasterBufferImpl);
    };

} // namespace

// static
scoped_ptr<TileTaskWorkerPool> GpuTileTaskWorkerPool::Create(
    base::SequencedTaskRunner* task_runner,
    TaskGraphRunner* task_graph_runner,
    ContextProvider* context_provider,
    ResourceProvider* resource_provider,
    bool use_distance_field_text,
    int gpu_rasterization_msaa_sample_count)
{
    return make_scoped_ptr<TileTaskWorkerPool>(new GpuTileTaskWorkerPool(
        task_runner, task_graph_runner, context_provider, resource_provider,
        use_distance_field_text, gpu_rasterization_msaa_sample_count));
}

GpuTileTaskWorkerPool::GpuTileTaskWorkerPool(
    base::SequencedTaskRunner* task_runner,
    TaskGraphRunner* task_graph_runner,
    ContextProvider* context_provider,
    ResourceProvider* resource_provider,
    bool use_distance_field_text,
    int gpu_rasterization_msaa_sample_count)
    : task_runner_(task_runner)
    , task_graph_runner_(task_graph_runner)
    , namespace_token_(task_graph_runner_->GetNamespaceToken())
    , rasterizer_(new GpuRasterizer(context_provider,
          resource_provider,
          use_distance_field_text,
          gpu_rasterization_msaa_sample_count))
    , task_set_finished_weak_ptr_factory_(this)
    , weak_ptr_factory_(this)
{
}

GpuTileTaskWorkerPool::~GpuTileTaskWorkerPool()
{
    DCHECK_EQ(0u, completed_tasks_.size());
}

TileTaskRunner* GpuTileTaskWorkerPool::AsTileTaskRunner()
{
    return this;
}

void GpuTileTaskWorkerPool::SetClient(TileTaskRunnerClient* client)
{
    client_ = client;
}

void GpuTileTaskWorkerPool::Shutdown()
{
    TRACE_EVENT0("cc", "GpuTileTaskWorkerPool::Shutdown");

    TaskGraph empty;
    task_graph_runner_->ScheduleTasks(namespace_token_, &empty);
    task_graph_runner_->WaitForTasksToFinishRunning(namespace_token_);
}

void GpuTileTaskWorkerPool::ScheduleTasks(TileTaskQueue* queue)
{
    TRACE_EVENT0("cc", "GpuTileTaskWorkerPool::ScheduleTasks");

    // Mark all task sets as pending.
    tasks_pending_.set();

    size_t priority = kTileTaskPriorityBase;

    graph_.Reset();

    // Cancel existing OnTaskSetFinished callbacks.
    task_set_finished_weak_ptr_factory_.InvalidateWeakPtrs();

    scoped_refptr<TileTask> new_task_set_finished_tasks[kNumberOfTaskSets];

    size_t task_count[kNumberOfTaskSets] = { 0 };

    for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
        new_task_set_finished_tasks[task_set] = CreateTaskSetFinishedTask(
            task_runner_.get(),
            base::Bind(&GpuTileTaskWorkerPool::OnTaskSetFinished,
                task_set_finished_weak_ptr_factory_.GetWeakPtr(), task_set));
    }

    for (TileTaskQueue::Item::Vector::const_iterator it = queue->items.begin();
         it != queue->items.end(); ++it) {
        const TileTaskQueue::Item& item = *it;
        RasterTask* task = item.task;
        DCHECK(!task->HasCompleted());

        for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
            if (!item.task_sets[task_set])
                continue;

            ++task_count[task_set];

            graph_.edges.push_back(
                TaskGraph::Edge(task, new_task_set_finished_tasks[task_set].get()));
        }

        InsertNodesForRasterTask(&graph_, task, task->dependencies(), priority++);
    }

    for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
        InsertNodeForTask(&graph_, new_task_set_finished_tasks[task_set].get(),
            kTaskSetFinishedTaskPriorityBase + task_set,
            task_count[task_set]);
    }

    ScheduleTasksOnOriginThread(this, &graph_);

    // Barrier to sync any new resources to the worker context.
    rasterizer_->resource_provider()
        ->output_surface()
        ->context_provider()
        ->ContextGL()
        ->OrderingBarrierCHROMIUM();

    task_graph_runner_->ScheduleTasks(namespace_token_, &graph_);

    std::copy(new_task_set_finished_tasks,
        new_task_set_finished_tasks + kNumberOfTaskSets,
        task_set_finished_tasks_);
}

void GpuTileTaskWorkerPool::CheckForCompletedTasks()
{
    TRACE_EVENT0("cc", "GpuTileTaskWorkerPool::CheckForCompletedTasks");

    task_graph_runner_->CollectCompletedTasks(namespace_token_,
        &completed_tasks_);
    CompleteTasks(completed_tasks_);
    completed_tasks_.clear();
}

ResourceFormat GpuTileTaskWorkerPool::GetResourceFormat(
    bool must_support_alpha) const
{
    return rasterizer_->resource_provider()->best_render_buffer_format();
}

bool GpuTileTaskWorkerPool::GetResourceRequiresSwizzle(
    bool must_support_alpha) const
{
    // This doesn't require a swizzle because we rasterize to the correct format.
    return false;
}

void GpuTileTaskWorkerPool::CompleteTasks(const Task::Vector& tasks)
{
    for (auto& task : tasks) {
        TileTask* tile_task = static_cast<TileTask*>(task.get());

        tile_task->WillComplete();
        tile_task->CompleteOnOriginThread(this);
        tile_task->DidComplete();
    }
    completed_tasks_.clear();
}

scoped_ptr<RasterBuffer> GpuTileTaskWorkerPool::AcquireBufferForRaster(
    const Resource* resource,
    uint64_t resource_content_id,
    uint64_t previous_content_id)
{
    return scoped_ptr<RasterBuffer>(new RasterBufferImpl(
        rasterizer_.get(), resource, resource_content_id, previous_content_id));
}

void GpuTileTaskWorkerPool::ReleaseBufferForRaster(
    scoped_ptr<RasterBuffer> buffer)
{
    // Nothing to do here. RasterBufferImpl destructor cleans up after itself.
}

void GpuTileTaskWorkerPool::OnTaskSetFinished(TaskSet task_set)
{
    TRACE_EVENT1("cc", "GpuTileTaskWorkerPool::OnTaskSetFinished", "task_set",
        task_set);

    DCHECK(tasks_pending_[task_set]);
    tasks_pending_[task_set] = false;
    client_->DidFinishRunningTileTasks(task_set);
}

} // namespace cc
