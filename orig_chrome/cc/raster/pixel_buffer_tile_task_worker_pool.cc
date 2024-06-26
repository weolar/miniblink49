// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/pixel_buffer_tile_task_worker_pool.h"

#include <algorithm>

#include "base/containers/stack_container.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/debug/traced_value.h"
#include "cc/raster/raster_buffer.h"
#include "cc/resources/platform_color.h"
#include "cc/resources/resource.h"
#include "gpu/command_buffer/client/gles2_interface.h"

namespace cc {
namespace {

    class RasterBufferImpl : public RasterBuffer {
    public:
        RasterBufferImpl(ResourceProvider* resource_provider,
            const Resource* resource)
            : resource_provider_(resource_provider)
            , resource_(resource)
            , memory_(NULL)
            , stride_(0)
        {
            resource_provider_->AcquirePixelBuffer(resource_->id());
            memory_ = resource_provider_->MapPixelBuffer(resource_->id(), &stride_);
        }

        ~RasterBufferImpl() override
        {
            resource_provider_->ReleasePixelBuffer(resource_->id());
        }

        // Overridden from RasterBuffer:
        void Playback(const RasterSource* raster_source,
            const gfx::Rect& raster_full_rect,
            const gfx::Rect& raster_dirty_rect,
            uint64_t new_content_id,
            float scale) override
        {
            if (!memory_)
                return;

            // TileTaskWorkerPool::PlaybackToMemory only supports unsigned strides.
            DCHECK_GE(stride_, 0);
            TileTaskWorkerPool::PlaybackToMemory(
                memory_, resource_->format(), resource_->size(),
                static_cast<size_t>(stride_), raster_source, raster_full_rect,
                raster_full_rect, scale);
        }

    private:
        ResourceProvider* resource_provider_;
        const Resource* resource_;
        uint8_t* memory_;
        int stride_;

        DISALLOW_COPY_AND_ASSIGN(RasterBufferImpl);
    };

    const int kCheckForCompletedRasterTasksDelayMs = 6;

    const size_t kMaxScheduledRasterTasks = 48;

    typedef base::StackVector<RasterTask*, kMaxScheduledRasterTasks>
        RasterTaskVector;

    TaskSetCollection NonEmptyTaskSetsFromTaskCounts(const size_t* task_counts)
    {
        TaskSetCollection task_sets;
        for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
            if (task_counts[task_set])
                task_sets[task_set] = true;
        }
        return task_sets;
    }

    void AddTaskSetsToTaskCounts(size_t* task_counts,
        const TaskSetCollection& task_sets)
    {
        for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
            if (task_sets[task_set])
                task_counts[task_set]++;
        }
    }

    void RemoveTaskSetsFromTaskCounts(size_t* task_counts,
        const TaskSetCollection& task_sets)
    {
        for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
            if (task_sets[task_set])
                task_counts[task_set]--;
        }
    }

} // namespace

PixelBufferTileTaskWorkerPool::RasterTaskState::RasterTaskState(
    RasterTask* task,
    const TaskSetCollection& task_sets)
    : type(UNSCHEDULED)
    , task(task)
    , task_sets(task_sets)
{
}

// static
scoped_ptr<TileTaskWorkerPool> PixelBufferTileTaskWorkerPool::Create(
    base::SequencedTaskRunner* task_runner,
    TaskGraphRunner* task_graph_runner,
    ContextProvider* context_provider,
    ResourceProvider* resource_provider,
    size_t max_transfer_buffer_usage_bytes)
{
    return make_scoped_ptr<TileTaskWorkerPool>(new PixelBufferTileTaskWorkerPool(
        task_runner, task_graph_runner, context_provider, resource_provider,
        max_transfer_buffer_usage_bytes));
}

PixelBufferTileTaskWorkerPool::PixelBufferTileTaskWorkerPool(
    base::SequencedTaskRunner* task_runner,
    TaskGraphRunner* task_graph_runner,
    ContextProvider* context_provider,
    ResourceProvider* resource_provider,
    size_t max_transfer_buffer_usage_bytes)
    : task_runner_(task_runner)
    , task_graph_runner_(task_graph_runner)
    , namespace_token_(task_graph_runner->GetNamespaceToken())
    , context_provider_(context_provider)
    , resource_provider_(resource_provider)
    , shutdown_(false)
    , scheduled_raster_task_count_(0u)
    , bytes_pending_upload_(0u)
    , max_bytes_pending_upload_(max_transfer_buffer_usage_bytes)
    , has_performed_uploads_since_last_flush_(false)
    , check_for_completed_raster_task_notifier_(
          task_runner,
          base::Bind(
              &PixelBufferTileTaskWorkerPool::CheckForCompletedRasterTasks,
              base::Unretained(this)),
          base::TimeDelta::FromMilliseconds(
              kCheckForCompletedRasterTasksDelayMs))
    , task_set_finished_weak_ptr_factory_(this)
{
    DCHECK(context_provider_);
    std::fill(task_counts_, task_counts_ + kNumberOfTaskSets, 0);
}

PixelBufferTileTaskWorkerPool::~PixelBufferTileTaskWorkerPool()
{
    DCHECK_EQ(0u, raster_task_states_.size());
    DCHECK_EQ(0u, raster_tasks_with_pending_upload_.size());
    DCHECK_EQ(0u, completed_raster_tasks_.size());
    DCHECK_EQ(0u, completed_image_decode_tasks_.size());
    DCHECK(NonEmptyTaskSetsFromTaskCounts(task_counts_).none());
}

TileTaskRunner* PixelBufferTileTaskWorkerPool::AsTileTaskRunner()
{
    return this;
}

void PixelBufferTileTaskWorkerPool::SetClient(TileTaskRunnerClient* client)
{
    client_ = client;
}

void PixelBufferTileTaskWorkerPool::Shutdown()
{
    TRACE_EVENT0("cc", "PixelBufferTileTaskWorkerPool::Shutdown");

    shutdown_ = true;

    TaskGraph empty;
    task_graph_runner_->ScheduleTasks(namespace_token_, &empty);
    task_graph_runner_->WaitForTasksToFinishRunning(namespace_token_);

    CheckForCompletedRasterizerTasks();
    CheckForCompletedUploads();

    check_for_completed_raster_task_notifier_.Shutdown();

    for (RasterTaskState::Vector::iterator it = raster_task_states_.begin();
         it != raster_task_states_.end(); ++it) {
        RasterTaskState& state = *it;

        // All unscheduled tasks need to be canceled.
        if (state.type == RasterTaskState::UNSCHEDULED) {
            completed_raster_tasks_.push_back(state.task);
            state.type = RasterTaskState::COMPLETED;
        }
    }
    DCHECK_EQ(completed_raster_tasks_.size(), raster_task_states_.size());
}

void PixelBufferTileTaskWorkerPool::ScheduleTasks(TileTaskQueue* queue)
{
    TRACE_EVENT0("cc", "PixelBufferTileTaskWorkerPool::ScheduleTasks");

    if (should_notify_client_if_no_tasks_are_pending_.none())
        TRACE_EVENT_ASYNC_BEGIN0("cc", "ScheduledTasks", this);

    should_notify_client_if_no_tasks_are_pending_.set();
    std::fill(task_counts_, task_counts_ + kNumberOfTaskSets, 0);

    // Update raster task state and remove items from old queue.
    for (TileTaskQueue::Item::Vector::const_iterator it = queue->items.begin();
         it != queue->items.end(); ++it) {
        const TileTaskQueue::Item& item = *it;
        RasterTask* task = item.task;

        // Remove any old items that are associated with this task. The result is
        // that the old queue is left with all items not present in this queue,
        // which we use below to determine what tasks need to be canceled.
        TileTaskQueue::Item::Vector::iterator old_it = std::find_if(raster_tasks_.items.begin(), raster_tasks_.items.end(),
            TileTaskQueue::Item::TaskComparator(task));
        if (old_it != raster_tasks_.items.end()) {
            std::swap(*old_it, raster_tasks_.items.back());
            raster_tasks_.items.pop_back();
        }

        RasterTaskState::Vector::iterator state_it = std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
            RasterTaskState::TaskComparator(task));
        if (state_it != raster_task_states_.end()) {
            RasterTaskState& state = *state_it;

            state.task_sets = item.task_sets;
            // |raster_tasks_required_for_activation_count| accounts for all tasks
            // that need to complete before we can send a "ready to activate" signal.
            // Tasks that have already completed should not be part of this count.
            if (state.type != RasterTaskState::COMPLETED)
                AddTaskSetsToTaskCounts(task_counts_, item.task_sets);

            continue;
        }

        DCHECK(!task->HasBeenScheduled());
        raster_task_states_.push_back(RasterTaskState(task, item.task_sets));
        AddTaskSetsToTaskCounts(task_counts_, item.task_sets);
    }

    // Determine what tasks in old queue need to be canceled.
    for (TileTaskQueue::Item::Vector::const_iterator it = raster_tasks_.items.begin();
         it != raster_tasks_.items.end(); ++it) {
        const TileTaskQueue::Item& item = *it;
        RasterTask* task = item.task;

        RasterTaskState::Vector::iterator state_it = std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
            RasterTaskState::TaskComparator(task));
        // We've already processed completion if we can't find a RasterTaskState for
        // this task.
        if (state_it == raster_task_states_.end())
            continue;

        RasterTaskState& state = *state_it;

        // Unscheduled task can be canceled.
        if (state.type == RasterTaskState::UNSCHEDULED) {
            DCHECK(!task->HasBeenScheduled());
            DCHECK(std::find(completed_raster_tasks_.begin(),
                       completed_raster_tasks_.end(),
                       task)
                == completed_raster_tasks_.end());
            completed_raster_tasks_.push_back(task);
            state.type = RasterTaskState::COMPLETED;
        }

        // No longer in any task set.
        state.task_sets.reset();
    }

    raster_tasks_.Swap(queue);

    // Check for completed tasks when ScheduleTasks() is called as
    // priorities might have changed and this maximizes the number
    // of top priority tasks that are scheduled.
    CheckForCompletedRasterizerTasks();
    CheckForCompletedUploads();
    FlushUploads();

    // Schedule new tasks.
    ScheduleMoreTasks();

    // Reschedule check for completed raster tasks.
    check_for_completed_raster_task_notifier_.Schedule();

    TRACE_EVENT_ASYNC_STEP_INTO1("cc", "ScheduledTasks", this, StateName(),
        "state", StateAsValue());
}

void PixelBufferTileTaskWorkerPool::CheckForCompletedTasks()
{
    TRACE_EVENT0("cc", "PixelBufferTileTaskWorkerPool::CheckForCompletedTasks");

    CheckForCompletedRasterizerTasks();
    CheckForCompletedUploads();
    FlushUploads();

    for (TileTask::Vector::const_iterator it = completed_image_decode_tasks_.begin();
         it != completed_image_decode_tasks_.end(); ++it) {
        TileTask* task = it->get();
        task->RunReplyOnOriginThread();
    }
    completed_image_decode_tasks_.clear();

    for (RasterTask::Vector::const_iterator it = completed_raster_tasks_.begin();
         it != completed_raster_tasks_.end(); ++it) {
        RasterTask* task = it->get();
        RasterTaskState::Vector::iterator state_it = std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
            RasterTaskState::TaskComparator(task));
        DCHECK(state_it != raster_task_states_.end());
        DCHECK_EQ(RasterTaskState::COMPLETED, state_it->type);

        std::swap(*state_it, raster_task_states_.back());
        raster_task_states_.pop_back();

        task->RunReplyOnOriginThread();
    }
    completed_raster_tasks_.clear();
}

ResourceFormat PixelBufferTileTaskWorkerPool::GetResourceFormat() const
{
    return resource_provider_->memory_efficient_texture_format();
}

bool PixelBufferTileTaskWorkerPool::GetResourceRequiresSwizzle() const
{
    return !PlatformColor::SameComponentOrder(GetResourceFormat());
}

scoped_ptr<RasterBuffer> PixelBufferTileTaskWorkerPool::AcquireBufferForRaster(
    const Resource* resource,
    uint64_t resource_content_id,
    uint64_t previous_content_id)
{
    return make_scoped_ptr<RasterBuffer>(
        new RasterBufferImpl(resource_provider_, resource));
}

void PixelBufferTileTaskWorkerPool::ReleaseBufferForRaster(
    scoped_ptr<RasterBuffer> buffer)
{
    // Nothing to do here. RasterBufferImpl destructor cleans up after itself.
}

void PixelBufferTileTaskWorkerPool::OnTaskSetFinished(TaskSet task_set)
{
    TRACE_EVENT2("cc", "PixelBufferTileTaskWorkerPool::OnTaskSetFinished",
        "task_set", task_set,
        "should_notify_client_if_no_tasks_are_pending",
        should_notify_client_if_no_tasks_are_pending_[task_set]);

    // There's no need to call CheckForCompletedRasterTasks() if the client has
    // already been notified.
    if (!should_notify_client_if_no_tasks_are_pending_[task_set])
        return;
    task_set_finished_tasks_pending_[task_set] = false;

    // This reduces latency between the time when all tasks required for
    // activation have finished running and the time when the client is
    // notified.
    CheckForCompletedRasterTasks();
}

void PixelBufferTileTaskWorkerPool::FlushUploads()
{
    if (!has_performed_uploads_since_last_flush_)
        return;

    context_provider_->ContextGL()->ShallowFlushCHROMIUM();
    has_performed_uploads_since_last_flush_ = false;
}

void PixelBufferTileTaskWorkerPool::CheckForCompletedUploads()
{
    RasterTask::Vector tasks_with_completed_uploads;

    // First check if any have completed.
    while (!raster_tasks_with_pending_upload_.empty()) {
        RasterTask* task = raster_tasks_with_pending_upload_.front().get();
        DCHECK(std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
                   RasterTaskState::TaskComparator(task))
            != raster_task_states_.end());
        DCHECK_EQ(
            RasterTaskState::UPLOADING,
            std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
                RasterTaskState::TaskComparator(task))
                ->type);

        // Uploads complete in the order they are issued.
        if (!resource_provider_->DidSetPixelsComplete(task->resource()->id()))
            break;

        tasks_with_completed_uploads.push_back(task);
        raster_tasks_with_pending_upload_.pop_front();
    }

    DCHECK(client_);
    TaskSetCollection tasks_that_should_be_forced_to_complete = client_->TasksThatShouldBeForcedToComplete();
    bool should_force_some_uploads_to_complete = shutdown_ || tasks_that_should_be_forced_to_complete.any();

    if (should_force_some_uploads_to_complete) {
        RasterTask::Vector tasks_with_uploads_to_force;
        RasterTaskDeque::iterator it = raster_tasks_with_pending_upload_.begin();
        while (it != raster_tasks_with_pending_upload_.end()) {
            RasterTask* task = it->get();
            RasterTaskState::Vector::const_iterator state_it = std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
                RasterTaskState::TaskComparator(task));
            DCHECK(state_it != raster_task_states_.end());
            const RasterTaskState& state = *state_it;

            // Force all uploads to complete for which the client requests to do so.
            // During shutdown, force all pending uploads to complete.
            if (shutdown_ || (state.task_sets & tasks_that_should_be_forced_to_complete).any()) {
                tasks_with_uploads_to_force.push_back(task);
                tasks_with_completed_uploads.push_back(task);
                it = raster_tasks_with_pending_upload_.erase(it);
                continue;
            }

            ++it;
        }

        // Force uploads in reverse order. Since forcing can cause a wait on
        // all previous uploads, we would rather wait only once downstream.
        for (RasterTask::Vector::reverse_iterator it = tasks_with_uploads_to_force.rbegin();
             it != tasks_with_uploads_to_force.rend(); ++it) {
            RasterTask* task = it->get();

            resource_provider_->ForceSetPixelsToComplete(task->resource()->id());
            has_performed_uploads_since_last_flush_ = true;
        }
    }

    // Release shared memory and move tasks with completed uploads
    // to |completed_raster_tasks_|.
    for (RasterTask::Vector::const_iterator it = tasks_with_completed_uploads.begin();
         it != tasks_with_completed_uploads.end(); ++it) {
        RasterTask* task = it->get();
        RasterTaskState::Vector::iterator state_it = std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
            RasterTaskState::TaskComparator(task));
        DCHECK(state_it != raster_task_states_.end());
        RasterTaskState& state = *state_it;

        // We can use UncheckedMemorySizeBytes here, since these tasks come from
        // tiles, the size of which is controlled by the compositor.
        bytes_pending_upload_ -= Resource::UncheckedMemorySizeBytes(
            task->resource()->size(), task->resource()->format());

        task->WillComplete();
        task->CompleteOnOriginThread(this);
        task->DidComplete();

        DCHECK(std::find(completed_raster_tasks_.begin(),
                   completed_raster_tasks_.end(),
                   task)
            == completed_raster_tasks_.end());
        completed_raster_tasks_.push_back(task);
        state.type = RasterTaskState::COMPLETED;
        // Triggers if the current task belongs to a set that should be empty.
        DCHECK((state.task_sets & ~NonEmptyTaskSetsFromTaskCounts(task_counts_))
                   .none());
        RemoveTaskSetsFromTaskCounts(task_counts_, state.task_sets);
    }
}

void PixelBufferTileTaskWorkerPool::CheckForCompletedRasterTasks()
{
    TRACE_EVENT0("cc",
        "PixelBufferTileTaskWorkerPool::CheckForCompletedRasterTasks");

    // Since this function can be called directly, cancel any pending checks.
    check_for_completed_raster_task_notifier_.Cancel();

    DCHECK(should_notify_client_if_no_tasks_are_pending_.any());

    CheckForCompletedRasterizerTasks();
    CheckForCompletedUploads();
    FlushUploads();

    // Determine what client notifications to generate.
    TaskSetCollection will_notify_client_that_no_tasks_are_pending = should_notify_client_if_no_tasks_are_pending_ & ~task_set_finished_tasks_pending_ & ~PendingTasks();

    // Adjust the need to generate notifications before scheduling more tasks.
    should_notify_client_if_no_tasks_are_pending_ &= ~will_notify_client_that_no_tasks_are_pending;

    scheduled_raster_task_count_ = 0;
    if (PendingRasterTaskCount())
        ScheduleMoreTasks();

    TRACE_EVENT_ASYNC_STEP_INTO1("cc", "ScheduledTasks", this, StateName(),
        "state", StateAsValue());

    // Schedule another check for completed raster tasks while there are
    // pending raster tasks or pending uploads.
    if (PendingTasks().any())
        check_for_completed_raster_task_notifier_.Schedule();

    if (should_notify_client_if_no_tasks_are_pending_.none())
        TRACE_EVENT_ASYNC_END0("cc", "ScheduledTasks", this);

    // Generate client notifications.
    for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
        if (will_notify_client_that_no_tasks_are_pending[task_set]) {
            DCHECK(!PendingTasks()[task_set]);
            client_->DidFinishRunningTileTasks(task_set);
        }
    }
}

void PixelBufferTileTaskWorkerPool::ScheduleMoreTasks()
{
    TRACE_EVENT0("cc", "PixelBufferTileTaskWorkerPool::ScheduleMoreTasks");

    RasterTaskVector tasks[kNumberOfTaskSets];

    size_t priority = kTileTaskPriorityBase;

    graph_.Reset();

    size_t bytes_pending_upload = bytes_pending_upload_;
    TaskSetCollection did_throttle_raster_tasks;
    size_t scheduled_raster_task_count = 0;

    for (TileTaskQueue::Item::Vector::const_iterator it = raster_tasks_.items.begin();
         it != raster_tasks_.items.end(); ++it) {
        const TileTaskQueue::Item& item = *it;
        RasterTask* task = item.task;
        DCHECK(item.task_sets.any());

        // |raster_task_states_| contains the state of all tasks that we have not
        // yet run reply callbacks for.
        RasterTaskState::Vector::iterator state_it = std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
            RasterTaskState::TaskComparator(task));
        if (state_it == raster_task_states_.end())
            continue;

        RasterTaskState& state = *state_it;

        // Skip task if completed.
        if (state.type == RasterTaskState::COMPLETED) {
            DCHECK(std::find(completed_raster_tasks_.begin(),
                       completed_raster_tasks_.end(),
                       task)
                != completed_raster_tasks_.end());
            continue;
        }

        // All raster tasks need to be throttled by bytes of pending uploads,
        // but if it's the only task allow it to complete no matter what its size,
        // to prevent starvation of the task queue.
        size_t new_bytes_pending_upload = bytes_pending_upload;
        // We can use UncheckedMemorySizeBytes here, since these tasks come from
        // tiles, the size of which is controlled by the compositor.
        new_bytes_pending_upload += Resource::UncheckedMemorySizeBytes(
            task->resource()->size(), task->resource()->format());
        if (new_bytes_pending_upload > max_bytes_pending_upload_ && bytes_pending_upload) {
            did_throttle_raster_tasks |= item.task_sets;
            continue;
        }

        // If raster has finished, just update |bytes_pending_upload|.
        if (state.type == RasterTaskState::UPLOADING) {
            DCHECK(!task->HasCompleted());
            bytes_pending_upload = new_bytes_pending_upload;
            continue;
        }

        // Throttle raster tasks based on kMaxScheduledRasterTasks.
        if (scheduled_raster_task_count >= kMaxScheduledRasterTasks) {
            did_throttle_raster_tasks |= item.task_sets;
            continue;
        }

        // Update |bytes_pending_upload| now that task has cleared all
        // throttling limits.
        bytes_pending_upload = new_bytes_pending_upload;

        DCHECK(state.type == RasterTaskState::UNSCHEDULED || state.type == RasterTaskState::SCHEDULED);
        state.type = RasterTaskState::SCHEDULED;

        InsertNodesForRasterTask(&graph_, task, task->dependencies(), priority++);

        ++scheduled_raster_task_count;
        for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
            if (item.task_sets[task_set])
                tasks[task_set].container().push_back(task);
        }
    }

    // Cancel existing OnTaskSetFinished callbacks.
    task_set_finished_weak_ptr_factory_.InvalidateWeakPtrs();

    scoped_refptr<TileTask> new_task_set_finished_tasks[kNumberOfTaskSets];
    size_t scheduled_task_counts[kNumberOfTaskSets] = { 0 };

    for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set) {
        scheduled_task_counts[task_set] = tasks[task_set].container().size();
        DCHECK_LE(scheduled_task_counts[task_set], task_counts_[task_set]);
        // Schedule OnTaskSetFinished call for task set only when notification is
        // pending and throttling is not preventing all pending tasks in the set
        // from being scheduled.
        if (!did_throttle_raster_tasks[task_set] && should_notify_client_if_no_tasks_are_pending_[task_set]) {
            new_task_set_finished_tasks[task_set] = CreateTaskSetFinishedTask(
                task_runner_.get(),
                base::Bind(&PixelBufferTileTaskWorkerPool::OnTaskSetFinished,
                    task_set_finished_weak_ptr_factory_.GetWeakPtr(),
                    task_set));
            task_set_finished_tasks_pending_[task_set] = true;
            InsertNodeForTask(&graph_, new_task_set_finished_tasks[task_set].get(),
                kTaskSetFinishedTaskPriorityBase + task_set,
                scheduled_task_counts[task_set]);
            for (RasterTaskVector::ContainerType::const_iterator it = tasks[task_set].container().begin();
                 it != tasks[task_set].container().end(); ++it) {
                graph_.edges.push_back(
                    TaskGraph::Edge(*it, new_task_set_finished_tasks[task_set].get()));
            }
        }
    }

    DCHECK_LE(scheduled_raster_task_count, PendingRasterTaskCount());

    ScheduleTasksOnOriginThread(this, &graph_);
    task_graph_runner_->ScheduleTasks(namespace_token_, &graph_);

    scheduled_raster_task_count_ = scheduled_raster_task_count;

    std::copy(new_task_set_finished_tasks,
        new_task_set_finished_tasks + kNumberOfTaskSets,
        task_set_finished_tasks_);
}

size_t PixelBufferTileTaskWorkerPool::PendingRasterTaskCount() const
{
    size_t num_completed_raster_tasks = raster_tasks_with_pending_upload_.size() + completed_raster_tasks_.size();
    DCHECK_GE(raster_task_states_.size(), num_completed_raster_tasks);
    return raster_task_states_.size() - num_completed_raster_tasks;
}

TaskSetCollection PixelBufferTileTaskWorkerPool::PendingTasks() const
{
    return NonEmptyTaskSetsFromTaskCounts(task_counts_);
}

const char* PixelBufferTileTaskWorkerPool::StateName() const
{
    if (scheduled_raster_task_count_)
        return "rasterizing";
    if (PendingRasterTaskCount())
        return "throttled";
    if (!raster_tasks_with_pending_upload_.empty())
        return "waiting_for_uploads";

    return "finishing";
}

void PixelBufferTileTaskWorkerPool::CheckForCompletedRasterizerTasks()
{
    TRACE_EVENT0(
        "cc", "PixelBufferTileTaskWorkerPool::CheckForCompletedRasterizerTasks");

    task_graph_runner_->CollectCompletedTasks(namespace_token_,
        &completed_tasks_);
    for (Task::Vector::const_iterator it = completed_tasks_.begin();
         it != completed_tasks_.end(); ++it) {
        TileTask* task = static_cast<TileTask*>(it->get());

        RasterTask* raster_task = task->AsRasterTask();
        if (!raster_task) {
            task->WillComplete();
            task->CompleteOnOriginThread(this);
            task->DidComplete();

            completed_image_decode_tasks_.push_back(task);
            continue;
        }

        RasterTaskState::Vector::iterator state_it = std::find_if(raster_task_states_.begin(), raster_task_states_.end(),
            RasterTaskState::TaskComparator(raster_task));
        DCHECK(state_it != raster_task_states_.end());

        RasterTaskState& state = *state_it;
        DCHECK_EQ(RasterTaskState::SCHEDULED, state.type);

        resource_provider_->UnmapPixelBuffer(raster_task->resource()->id());

        if (!raster_task->HasFinishedRunning()) {
            // When priorites change, a raster task can be canceled as a result of
            // no longer being of high enough priority to fit in our throttled
            // raster task budget. The task has not yet completed in this case.
            raster_task->WillComplete();
            raster_task->CompleteOnOriginThread(this);
            raster_task->DidComplete();

            TileTaskQueue::Item::Vector::const_iterator item_it = std::find_if(raster_tasks_.items.begin(), raster_tasks_.items.end(),
                TileTaskQueue::Item::TaskComparator(raster_task));
            if (item_it != raster_tasks_.items.end()) {
                state.type = RasterTaskState::UNSCHEDULED;
                continue;
            }

            DCHECK(std::find(completed_raster_tasks_.begin(),
                       completed_raster_tasks_.end(),
                       raster_task)
                == completed_raster_tasks_.end());
            completed_raster_tasks_.push_back(raster_task);
            state.type = RasterTaskState::COMPLETED;
            // Triggers if the current task belongs to a set that should be empty.
            DCHECK((state.task_sets & ~NonEmptyTaskSetsFromTaskCounts(task_counts_))
                       .none());
            RemoveTaskSetsFromTaskCounts(task_counts_, state.task_sets);
            continue;
        }

        resource_provider_->BeginSetPixels(raster_task->resource()->id());
        has_performed_uploads_since_last_flush_ = true;

        // We can use UncheckedMemorySizeBytes here, since these tasks come from
        // tiles, the size of which is controlled by the compositor.
        bytes_pending_upload_ += Resource::UncheckedMemorySizeBytes(
            raster_task->resource()->size(), raster_task->resource()->format());
        raster_tasks_with_pending_upload_.push_back(raster_task);
        state.type = RasterTaskState::UPLOADING;
    }
    completed_tasks_.clear();
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
PixelBufferTileTaskWorkerPool::StateAsValue() const
{
    scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();
    state->SetInteger("completed_count",
        static_cast<int>(completed_raster_tasks_.size()));
    state->BeginArray("pending_count");
    for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set)
        state->AppendInteger(static_cast<int>(task_counts_[task_set]));
    state->EndArray();
    state->SetInteger("pending_upload_count",
        static_cast<int>(raster_tasks_with_pending_upload_.size()));
    state->BeginDictionary("throttle_state");
    ThrottleStateAsValueInto(state.get());
    state->EndDictionary();
    return state;
}

void PixelBufferTileTaskWorkerPool::ThrottleStateAsValueInto(
    base::trace_event::TracedValue* throttle_state) const
{
    throttle_state->SetInteger(
        "bytes_available_for_upload",
        static_cast<int>(max_bytes_pending_upload_ - bytes_pending_upload_));
    throttle_state->SetInteger("bytes_pending_upload",
        static_cast<int>(bytes_pending_upload_));
    throttle_state->SetInteger("scheduled_raster_task_count",
        static_cast<int>(scheduled_raster_task_count_));
}

} // namespace cc
