// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/bitmap_tile_task_worker_pool.h"

#include <algorithm>

#include "base/bind.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/debug/traced_value.h"
#include "cc/playback/raster_source.h"
#include "cc/raster/raster_buffer.h"
#include "cc/resources/platform_color.h"
#include "cc/resources/resource.h"

namespace cc {
namespace {

    class RasterBufferImpl : public RasterBuffer {
    public:
        RasterBufferImpl(ResourceProvider* resource_provider,
            const Resource* resource,
            uint64_t resource_content_id,
            uint64_t previous_content_id)
            : lock_(resource_provider, resource->id())
            , resource_(resource)
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
            gfx::Rect playback_rect = raster_full_rect;
            if (resource_has_previous_content_) {
                playback_rect.Intersect(raster_dirty_rect);
            }
            DCHECK(!playback_rect.IsEmpty())
                << "Why are we rastering a tile that's not dirty?";

            size_t stride = 0u;
            TileTaskWorkerPool::PlaybackToMemory(
                lock_.sk_bitmap().getPixels(), resource_->format(), resource_->size(),
                stride, raster_source, raster_full_rect, playback_rect, scale,
                include_images);
        }

    private:
        ResourceProvider::ScopedWriteLockSoftware lock_;
        const Resource* resource_;
        bool resource_has_previous_content_;

        DISALLOW_COPY_AND_ASSIGN(RasterBufferImpl);
    };

} // namespace

// static
scoped_ptr<TileTaskWorkerPool> BitmapTileTaskWorkerPool::Create(
    base::SequencedTaskRunner* task_runner,
    TaskGraphRunner* task_graph_runner,
    ResourceProvider* resource_provider)
{
    return make_scoped_ptr<TileTaskWorkerPool>(new BitmapTileTaskWorkerPool(
        task_runner, task_graph_runner, resource_provider));
}

BitmapTileTaskWorkerPool::BitmapTileTaskWorkerPool(
    base::SequencedTaskRunner* task_runner,
    TaskGraphRunner* task_graph_runner,
    ResourceProvider* resource_provider)
    : task_runner_(task_runner)
    , task_graph_runner_(task_graph_runner)
    , namespace_token_(task_graph_runner->GetNamespaceToken())
    , resource_provider_(resource_provider)
    , task_set_finished_weak_ptr_factory_(this)
{
}

BitmapTileTaskWorkerPool::~BitmapTileTaskWorkerPool()
{
}

TileTaskRunner* BitmapTileTaskWorkerPool::AsTileTaskRunner()
{
    return this;
}

void BitmapTileTaskWorkerPool::SetClient(TileTaskRunnerClient* client)
{
    client_ = client;
}

void BitmapTileTaskWorkerPool::Shutdown()
{
    TRACE_EVENT0("cc", "BitmapTileTaskWorkerPool::Shutdown");

    TaskGraph empty;
    task_graph_runner_->ScheduleTasks(namespace_token_, &empty);
    task_graph_runner_->WaitForTasksToFinishRunning(namespace_token_);
}

void BitmapTileTaskWorkerPool::ScheduleTasks(TileTaskQueue* queue)
{
    TRACE_EVENT0("cc", "BitmapTileTaskWorkerPool::ScheduleTasks");

    //   if (tasks_pending_.none())
    //     TRACE_EVENT_ASYNC_BEGIN0("cc", "ScheduledTasks", this);

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
            base::Bind(&BitmapTileTaskWorkerPool::OnTaskSetFinished,
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
    task_graph_runner_->ScheduleTasks(namespace_token_, &graph_);

    std::copy(new_task_set_finished_tasks,
        new_task_set_finished_tasks + kNumberOfTaskSets,
        task_set_finished_tasks_);

    TRACE_EVENT_ASYNC_STEP_INTO1("cc", "ScheduledTasks", this, "running", "state",
        StateAsValue());
}

void BitmapTileTaskWorkerPool::CheckForCompletedTasks()
{
    TRACE_EVENT0("cc", "BitmapTileTaskWorkerPool::CheckForCompletedTasks");

    task_graph_runner_->CollectCompletedTasks(namespace_token_,
        &completed_tasks_);
    for (Task::Vector::const_iterator it = completed_tasks_.begin();
         it != completed_tasks_.end(); ++it) {
        TileTask* task = static_cast<TileTask*>(it->get());

        task->WillComplete();
        task->CompleteOnOriginThread(this);
        task->DidComplete();
    }
    completed_tasks_.clear();
}

ResourceFormat BitmapTileTaskWorkerPool::GetResourceFormat(
    bool must_support_alpha) const
{
    return resource_provider_->best_texture_format();
}

bool BitmapTileTaskWorkerPool::GetResourceRequiresSwizzle(
    bool must_support_alpha) const
{
    return !PlatformColor::SameComponentOrder(
        GetResourceFormat(must_support_alpha));
}

scoped_ptr<RasterBuffer> BitmapTileTaskWorkerPool::AcquireBufferForRaster(
    const Resource* resource,
    uint64_t resource_content_id,
    uint64_t previous_content_id)
{
    return scoped_ptr<RasterBuffer>(new RasterBufferImpl(
        resource_provider_, resource, resource_content_id, previous_content_id));
}

void BitmapTileTaskWorkerPool::ReleaseBufferForRaster(
    scoped_ptr<RasterBuffer> buffer)
{
    // Nothing to do here. RasterBufferImpl destructor cleans up after itself.
}

void BitmapTileTaskWorkerPool::OnTaskSetFinished(TaskSet task_set)
{
    TRACE_EVENT1("cc", "BitmapTileTaskWorkerPool::OnTaskSetFinished", "task_set",
        task_set);

    DCHECK(tasks_pending_[task_set]);
    tasks_pending_[task_set] = false;
    if (tasks_pending_.any()) {
        TRACE_EVENT_ASYNC_STEP_INTO1("cc", "ScheduledTasks", this, "running",
            "state", StateAsValue());
    } else {
        TRACE_EVENT_ASYNC_END0("cc", "ScheduledTasks", this);
    }
    client_->DidFinishRunningTileTasks(task_set);
}

// scoped_refptr<base::trace_event::ConvertableToTraceFormat>
// BitmapTileTaskWorkerPool::StateAsValue() const {
//   scoped_refptr<base::trace_event::TracedValue> state =
//       new base::trace_event::TracedValue();
//
//   state->BeginArray("tasks_pending");
//   for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set)
//     state->AppendBoolean(tasks_pending_[task_set]);
//   state->EndArray();
//   return state;
// }

} // namespace cc
