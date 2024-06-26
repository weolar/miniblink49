// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/one_copy_tile_task_worker_pool.h"

#include <algorithm>
#include <limits>

#include "base/bind.h"
#include "base/strings/stringprintf.h"
#include "base/thread_task_runner_handle.h"
//#include "base/trace_event/memory_dump_manager.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/base/math_util.h"
#include "cc/debug/traced_value.h"
#include "cc/raster/raster_buffer.h"
#include "cc/resources/platform_color.h"
#include "cc/resources/resource_format.h"
#include "cc/resources/resource_util.h"
#include "cc/resources/scoped_resource.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "gpu/command_buffer/client/gpu_memory_buffer_manager.h"
#include "ui/gfx/buffer_format_util.h"

namespace cc {
namespace {

    class RasterBufferImpl : public RasterBuffer {
    public:
        RasterBufferImpl(OneCopyTileTaskWorkerPool* worker_pool,
            ResourceProvider* resource_provider,
            ResourceFormat resource_format,
            const Resource* resource,
            uint64_t previous_content_id)
            : worker_pool_(worker_pool)
            , resource_(resource)
            , lock_(resource_provider, resource->id())
            , previous_content_id_(previous_content_id)
        {
        }

        ~RasterBufferImpl() override { }

        // Overridden from RasterBuffer:
        void Playback(const RasterSource* raster_source,
            const gfx::Rect& raster_full_rect,
            const gfx::Rect& raster_dirty_rect,
            uint64_t new_content_id,
            float scale,
            bool include_images) override
        {
            worker_pool_->PlaybackAndCopyOnWorkerThread(
                resource_, &lock_, raster_source, raster_full_rect, raster_dirty_rect,
                scale, include_images, previous_content_id_, new_content_id);
        }

    private:
        OneCopyTileTaskWorkerPool* worker_pool_;
        const Resource* resource_;
        ResourceProvider::ScopedWriteLockGL lock_;
        uint64_t previous_content_id_;

        DISALLOW_COPY_AND_ASSIGN(RasterBufferImpl);
    };

    // Delay between checking for query result to be available.
    const int kCheckForQueryResultAvailableTickRateMs = 1;

    // Number of attempts to allow before we perform a check that will wait for
    // query to complete.
    const int kMaxCheckForQueryResultAvailableAttempts = 256;

    // 4MiB is the size of 4 512x512 tiles, which has proven to be a good
    // default batch size for copy operations.
    const int kMaxBytesPerCopyOperation = 1024 * 1024 * 4;

    // Delay before a staging buffer might be released.
    const int kStagingBufferExpirationDelayMs = 1000;

    bool CheckForQueryResult(gpu::gles2::GLES2Interface* gl, unsigned query_id)
    {
        unsigned complete = 1;
        gl->GetQueryObjectuivEXT(query_id, GL_QUERY_RESULT_AVAILABLE_EXT, &complete);
        return !!complete;
    }

    void WaitForQueryResult(gpu::gles2::GLES2Interface* gl, unsigned query_id)
    {
        TRACE_EVENT0("cc", "WaitForQueryResult");

        int attempts_left = kMaxCheckForQueryResultAvailableAttempts;
        while (attempts_left--) {
            if (CheckForQueryResult(gl, query_id))
                break;

            // We have to flush the context to be guaranteed that a query result will
            // be available in a finite amount of time.
            gl->ShallowFlushCHROMIUM();

            base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(
                kCheckForQueryResultAvailableTickRateMs));
        }

        unsigned result = 0;
        gl->GetQueryObjectuivEXT(query_id, GL_QUERY_RESULT_EXT, &result);
    }

} // namespace

OneCopyTileTaskWorkerPool::StagingBuffer::StagingBuffer(const gfx::Size& size,
    ResourceFormat format)
    : size(size)
    , format(format)
    , texture_id(0)
    , image_id(0)
    , query_id(0)
    , content_id(0)
{
}

OneCopyTileTaskWorkerPool::StagingBuffer::~StagingBuffer()
{
    DCHECK_EQ(texture_id, 0u);
    DCHECK_EQ(image_id, 0u);
    DCHECK_EQ(query_id, 0u);
}

void OneCopyTileTaskWorkerPool::StagingBuffer::DestroyGLResources(
    gpu::gles2::GLES2Interface* gl)
{
    if (query_id) {
        gl->DeleteQueriesEXT(1, &query_id);
        query_id = 0;
    }
    if (image_id) {
        gl->DestroyImageCHROMIUM(image_id);
        image_id = 0;
    }
    if (texture_id) {
        gl->DeleteTextures(1, &texture_id);
        texture_id = 0;
    }
}

void OneCopyTileTaskWorkerPool::StagingBuffer::OnMemoryDump(
    base::trace_event::ProcessMemoryDump* pmd,
    ResourceFormat format,
    bool in_free_list) const
{
    if (!gpu_memory_buffer)
        return;
    DebugBreak();
    //   gfx::GpuMemoryBufferId buffer_id = gpu_memory_buffer->GetId();
    //   std::string buffer_dump_name =
    //       base::StringPrintf("cc/one_copy/staging_memory/buffer_%d", buffer_id.id);
    //   base::trace_event::MemoryAllocatorDump* buffer_dump =
    //       pmd->CreateAllocatorDump(buffer_dump_name);
    //
    //   uint64_t buffer_size_in_bytes =
    //       ResourceUtil::UncheckedSizeInBytes<uint64_t>(size, format);
    //   buffer_dump->AddScalar(base::trace_event::MemoryAllocatorDump::kNameSize,
    //                          base::trace_event::MemoryAllocatorDump::kUnitsBytes,
    //                          buffer_size_in_bytes);
    //   buffer_dump->AddScalar("free_size",
    //                          base::trace_event::MemoryAllocatorDump::kUnitsBytes,
    //                          in_free_list ? buffer_size_in_bytes : 0);
    //
    //   // Emit an ownership edge towards a global allocator dump node.
    //   const uint64 tracing_process_id =
    //       base::trace_event::MemoryDumpManager::GetInstance()
    //           ->GetTracingProcessId();
    //   base::trace_event::MemoryAllocatorDumpGuid shared_buffer_guid =
    //       gfx::GetGpuMemoryBufferGUIDForTracing(tracing_process_id, buffer_id);
    //   pmd->CreateSharedGlobalAllocatorDump(shared_buffer_guid);
    //
    //   // By creating an edge with a higher |importance| (w.r.t. browser-side dumps)
    //   // the tracing UI will account the effective size of the buffer to the child.
    //   const int kImportance = 2;
    //   pmd->AddOwnershipEdge(buffer_dump->guid(), shared_buffer_guid, kImportance);
}

// static
scoped_ptr<TileTaskWorkerPool> OneCopyTileTaskWorkerPool::Create(
    base::SequencedTaskRunner* task_runner,
    TaskGraphRunner* task_graph_runner,
    ContextProvider* context_provider,
    ResourceProvider* resource_provider,
    int max_copy_texture_chromium_size,
    bool use_persistent_gpu_memory_buffers,
    int max_staging_buffer_usage_in_bytes,
    bool use_rgba_4444_texture_format)
{
    return make_scoped_ptr<TileTaskWorkerPool>(new OneCopyTileTaskWorkerPool(
        task_runner, task_graph_runner, resource_provider,
        max_copy_texture_chromium_size, use_persistent_gpu_memory_buffers,
        max_staging_buffer_usage_in_bytes, use_rgba_4444_texture_format));
}

OneCopyTileTaskWorkerPool::OneCopyTileTaskWorkerPool(
    base::SequencedTaskRunner* task_runner,
    TaskGraphRunner* task_graph_runner,
    ResourceProvider* resource_provider,
    int max_copy_texture_chromium_size,
    bool use_persistent_gpu_memory_buffers,
    int max_staging_buffer_usage_in_bytes,
    bool use_rgba_4444_texture_format)
    : task_runner_(task_runner)
    , task_graph_runner_(task_graph_runner)
    , namespace_token_(task_graph_runner->GetNamespaceToken())
    , resource_provider_(resource_provider)
    , max_bytes_per_copy_operation_(
          max_copy_texture_chromium_size
              ? std::min(kMaxBytesPerCopyOperation,
                  max_copy_texture_chromium_size)
              : kMaxBytesPerCopyOperation)
    , use_persistent_gpu_memory_buffers_(use_persistent_gpu_memory_buffers)
    , bytes_scheduled_since_last_flush_(0)
    , max_staging_buffer_usage_in_bytes_(max_staging_buffer_usage_in_bytes)
    , use_rgba_4444_texture_format_(use_rgba_4444_texture_format)
    , staging_buffer_usage_in_bytes_(0)
    , free_staging_buffer_usage_in_bytes_(0)
    , staging_buffer_expiration_delay_(
          base::TimeDelta::FromMilliseconds(kStagingBufferExpirationDelayMs))
    , reduce_memory_usage_pending_(false)
    , weak_ptr_factory_(this)
    , task_set_finished_weak_ptr_factory_(this)
{
    //   base::trace_event::MemoryDumpManager::GetInstance()->RegisterDumpProvider(
    //       this, "OneCopyTileTaskWorkerPool::OneCopyTileTaskWorkerPool", base::ThreadTaskRunnerHandle::Get());
    reduce_memory_usage_callback_ = base::Bind(&OneCopyTileTaskWorkerPool::ReduceMemoryUsage,
        weak_ptr_factory_.GetWeakPtr());
}

OneCopyTileTaskWorkerPool::~OneCopyTileTaskWorkerPool()
{
    //   base::trace_event::MemoryDumpManager::GetInstance()->UnregisterDumpProvider(
    //       this);
}

TileTaskRunner* OneCopyTileTaskWorkerPool::AsTileTaskRunner()
{
    return this;
}

void OneCopyTileTaskWorkerPool::SetClient(TileTaskRunnerClient* client)
{
    client_ = client;
}

void OneCopyTileTaskWorkerPool::Shutdown()
{
    TRACE_EVENT0("cc", "OneCopyTileTaskWorkerPool::Shutdown");

    TaskGraph empty;
    task_graph_runner_->ScheduleTasks(namespace_token_, &empty);
    task_graph_runner_->WaitForTasksToFinishRunning(namespace_token_);

    base::AutoLock lock(lock_);

    if (buffers_.empty())
        return;

    ReleaseBuffersNotUsedSince(base::TimeTicks() + base::TimeDelta::Max());
    DCHECK_EQ(staging_buffer_usage_in_bytes_, 0);
    DCHECK_EQ(free_staging_buffer_usage_in_bytes_, 0);
}

void OneCopyTileTaskWorkerPool::ScheduleTasks(TileTaskQueue* queue)
{
    TRACE_EVENT0("cc", "OneCopyTileTaskWorkerPool::ScheduleTasks");

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
            base::Bind(&OneCopyTileTaskWorkerPool::OnTaskSetFinished,
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
    resource_provider_->output_surface()
        ->context_provider()
        ->ContextGL()
        ->OrderingBarrierCHROMIUM();

    task_graph_runner_->ScheduleTasks(namespace_token_, &graph_);

    std::copy(new_task_set_finished_tasks,
        new_task_set_finished_tasks + kNumberOfTaskSets,
        task_set_finished_tasks_);

    TRACE_EVENT_ASYNC_STEP_INTO1("cc", "ScheduledTasks", this, "running", "state",
        StateAsValue());
}

void OneCopyTileTaskWorkerPool::CheckForCompletedTasks()
{
    TRACE_EVENT0("cc", "OneCopyTileTaskWorkerPool::CheckForCompletedTasks");

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

ResourceFormat OneCopyTileTaskWorkerPool::GetResourceFormat(
    bool must_support_alpha) const
{
    return use_rgba_4444_texture_format_
        ? RGBA_4444
        : resource_provider_->best_texture_format();
}

bool OneCopyTileTaskWorkerPool::GetResourceRequiresSwizzle(
    bool must_support_alpha) const
{
    return !PlatformColor::SameComponentOrder(
        GetResourceFormat(must_support_alpha));
}

scoped_ptr<RasterBuffer> OneCopyTileTaskWorkerPool::AcquireBufferForRaster(
    const Resource* resource,
    uint64_t resource_content_id,
    uint64_t previous_content_id)
{
    // TODO(danakj): If resource_content_id != 0, we only need to copy/upload
    // the dirty rect.
    return make_scoped_ptr<RasterBuffer>(
        new RasterBufferImpl(this, resource_provider_, resource->format(),
            resource, previous_content_id));
}

void OneCopyTileTaskWorkerPool::ReleaseBufferForRaster(
    scoped_ptr<RasterBuffer> buffer)
{
    // Nothing to do here. RasterBufferImpl destructor cleans up after itself.
}

void OneCopyTileTaskWorkerPool::PlaybackAndCopyOnWorkerThread(
    const Resource* resource,
    const ResourceProvider::ScopedWriteLockGL* resource_lock,
    const RasterSource* raster_source,
    const gfx::Rect& raster_full_rect,
    const gfx::Rect& raster_dirty_rect,
    float scale,
    bool include_images,
    uint64_t previous_content_id,
    uint64_t new_content_id)
{
    base::AutoLock lock(lock_);

    scoped_ptr<StagingBuffer> staging_buffer = AcquireStagingBuffer(resource, previous_content_id);
    DCHECK(staging_buffer);

    {
        base::AutoUnlock unlock(lock_);

        // Allocate GpuMemoryBuffer if necessary.
        if (!staging_buffer->gpu_memory_buffer) {
            staging_buffer->gpu_memory_buffer = resource_provider_->gpu_memory_buffer_manager()
                                                    ->AllocateGpuMemoryBuffer(staging_buffer->size,
                                                        BufferFormat(resource->format()),
                                                        use_persistent_gpu_memory_buffers_
                                                            ? gfx::BufferUsage::GPU_READ_CPU_READ_WRITE_PERSISTENT
                                                            : gfx::BufferUsage::GPU_READ_CPU_READ_WRITE);
            DCHECK_EQ(gfx::NumberOfPlanesForBufferFormat(
                          staging_buffer->gpu_memory_buffer->GetFormat()),
                1u);
        }

        gfx::Rect playback_rect = raster_full_rect;
        if (use_persistent_gpu_memory_buffers_ && previous_content_id) {
            // Reduce playback rect to dirty region if the content id of the staging
            // buffer matches the prevous content id.
            if (previous_content_id == staging_buffer->content_id)
                playback_rect.Intersect(raster_dirty_rect);
        }

        if (staging_buffer->gpu_memory_buffer) {
            gfx::GpuMemoryBuffer* buffer = staging_buffer->gpu_memory_buffer.get();
            DCHECK_EQ(1u, gfx::NumberOfPlanesForBufferFormat(buffer->GetFormat()));
            bool rv = buffer->Map();
            DCHECK(rv);
            DCHECK(buffer->memory(0));
            // TileTaskWorkerPool::PlaybackToMemory only supports unsigned strides.
            DCHECK_GE(buffer->stride(0), 0);

            DCHECK(!playback_rect.IsEmpty()) << "Why are we rastering a tile that's not dirty?";
            TileTaskWorkerPool::PlaybackToMemory(
                buffer->memory(0), resource->format(), staging_buffer->size,
                buffer->stride(0), raster_source, raster_full_rect, playback_rect,
                scale, include_images);
            buffer->Unmap();
            staging_buffer->content_id = new_content_id;
        }
    }

    ContextProvider* context_provider = resource_provider_->output_surface()->worker_context_provider();
    DCHECK(context_provider);

    {
        ContextProvider::ScopedContextLock scoped_context(context_provider);

        gpu::gles2::GLES2Interface* gl = scoped_context.ContextGL();
        DCHECK(gl);

        unsigned image_target = resource_provider_->GetImageTextureTarget(resource->format());

        // Create and bind staging texture.
        if (!staging_buffer->texture_id) {
            gl->GenTextures(1, &staging_buffer->texture_id);
            gl->BindTexture(image_target, staging_buffer->texture_id);
            gl->TexParameteri(image_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            gl->TexParameteri(image_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            gl->TexParameteri(image_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            gl->TexParameteri(image_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        } else {
            gl->BindTexture(image_target, staging_buffer->texture_id);
        }

        // Create and bind image.
        if (!staging_buffer->image_id) {
            if (staging_buffer->gpu_memory_buffer) {
                staging_buffer->image_id = gl->CreateImageCHROMIUM(
                    staging_buffer->gpu_memory_buffer->AsClientBuffer(),
                    staging_buffer->size.width(), staging_buffer->size.height(),
                    GLInternalFormat(resource->format()));
                gl->BindTexImage2DCHROMIUM(image_target, staging_buffer->image_id);
            }
        } else {
            gl->ReleaseTexImage2DCHROMIUM(image_target, staging_buffer->image_id);
            gl->BindTexImage2DCHROMIUM(image_target, staging_buffer->image_id);
        }

        // Unbind staging texture.
        gl->BindTexture(image_target, 0);

        if (resource_provider_->use_sync_query()) {
            if (!staging_buffer->query_id)
                gl->GenQueriesEXT(1, &staging_buffer->query_id);

#if defined(OS_CHROMEOS)
            // TODO(reveman): This avoids a performance problem on some ChromeOS
            // devices. This needs to be removed to support native GpuMemoryBuffer
            // implementations. crbug.com/436314
            gl->BeginQueryEXT(GL_COMMANDS_ISSUED_CHROMIUM, staging_buffer->query_id);
#else
            gl->BeginQueryEXT(GL_COMMANDS_COMPLETED_CHROMIUM,
                staging_buffer->query_id);
#endif
        }

        int bytes_per_row = (BitsPerPixel(resource->format()) * resource->size().width()) / 8;
        int chunk_size_in_rows = std::max(1, max_bytes_per_copy_operation_ / bytes_per_row);
        // Align chunk size to 4. Required to support compressed texture formats.
        chunk_size_in_rows = MathUtil::UncheckedRoundUp(chunk_size_in_rows, 4);
        int y = 0;
        int height = resource->size().height();
        while (y < height) {
            // Copy at most |chunk_size_in_rows|.
            int rows_to_copy = std::min(chunk_size_in_rows, height - y);
            DCHECK_GT(rows_to_copy, 0);

            gl->CopySubTextureCHROMIUM(GL_TEXTURE_2D, staging_buffer->texture_id,
                resource_lock->texture_id(), 0, y, 0, y,
                resource->size().width(), rows_to_copy, false,
                false, false);
            y += rows_to_copy;

            // Increment |bytes_scheduled_since_last_flush_| by the amount of memory
            // used for this copy operation.
            bytes_scheduled_since_last_flush_ += rows_to_copy * bytes_per_row;

            if (bytes_scheduled_since_last_flush_ >= max_bytes_per_copy_operation_) {
                gl->ShallowFlushCHROMIUM();
                bytes_scheduled_since_last_flush_ = 0;
            }
        }

        if (resource_provider_->use_sync_query()) {
#if defined(OS_CHROMEOS)
            gl->EndQueryEXT(GL_COMMANDS_ISSUED_CHROMIUM);
#else
            gl->EndQueryEXT(GL_COMMANDS_COMPLETED_CHROMIUM);
#endif
        }

        // Barrier to sync worker context output to cc context.
        gl->OrderingBarrierCHROMIUM();
    }

    staging_buffer->last_usage = base::TimeTicks::Now();
    busy_buffers_.push_back(staging_buffer.Pass());

    ScheduleReduceMemoryUsage();
}

bool OneCopyTileTaskWorkerPool::OnMemoryDump(
    const base::trace_event::MemoryDumpArgs& args,
    base::trace_event::ProcessMemoryDump* pmd)
{
    base::AutoLock lock(lock_);

    for (const auto& buffer : buffers_) {
        buffer->OnMemoryDump(pmd, buffer->format,
            std::find(free_buffers_.begin(), free_buffers_.end(),
                buffer)
                != free_buffers_.end());
    }

    return true;
}

void OneCopyTileTaskWorkerPool::AddStagingBuffer(
    const StagingBuffer* staging_buffer,
    ResourceFormat format)
{
    lock_.AssertAcquired();

    DCHECK(buffers_.find(staging_buffer) == buffers_.end());
    buffers_.insert(staging_buffer);
    int buffer_usage_in_bytes = ResourceUtil::UncheckedSizeInBytes<int>(staging_buffer->size, format);
    staging_buffer_usage_in_bytes_ += buffer_usage_in_bytes;
}

void OneCopyTileTaskWorkerPool::RemoveStagingBuffer(
    const StagingBuffer* staging_buffer)
{
    lock_.AssertAcquired();

    DCHECK(buffers_.find(staging_buffer) != buffers_.end());
    buffers_.erase(staging_buffer);
    int buffer_usage_in_bytes = ResourceUtil::UncheckedSizeInBytes<int>(
        staging_buffer->size, staging_buffer->format);
    DCHECK_GE(staging_buffer_usage_in_bytes_, buffer_usage_in_bytes);
    staging_buffer_usage_in_bytes_ -= buffer_usage_in_bytes;
}

void OneCopyTileTaskWorkerPool::MarkStagingBufferAsFree(
    const StagingBuffer* staging_buffer)
{
    lock_.AssertAcquired();

    int buffer_usage_in_bytes = ResourceUtil::UncheckedSizeInBytes<int>(
        staging_buffer->size, staging_buffer->format);
    free_staging_buffer_usage_in_bytes_ += buffer_usage_in_bytes;
}

void OneCopyTileTaskWorkerPool::MarkStagingBufferAsBusy(
    const StagingBuffer* staging_buffer)
{
    lock_.AssertAcquired();

    int buffer_usage_in_bytes = ResourceUtil::UncheckedSizeInBytes<int>(
        staging_buffer->size, staging_buffer->format);
    DCHECK_GE(free_staging_buffer_usage_in_bytes_, buffer_usage_in_bytes);
    free_staging_buffer_usage_in_bytes_ -= buffer_usage_in_bytes;
}

scoped_ptr<OneCopyTileTaskWorkerPool::StagingBuffer>
OneCopyTileTaskWorkerPool::AcquireStagingBuffer(const Resource* resource,
    uint64_t previous_content_id)
{
    lock_.AssertAcquired();

    scoped_ptr<StagingBuffer> staging_buffer;

    ContextProvider* context_provider = resource_provider_->output_surface()->worker_context_provider();
    DCHECK(context_provider);

    ContextProvider::ScopedContextLock scoped_context(context_provider);

    gpu::gles2::GLES2Interface* gl = scoped_context.ContextGL();
    DCHECK(gl);

    // Check if any busy buffers have become available.
    if (resource_provider_->use_sync_query()) {
        while (!busy_buffers_.empty()) {
            if (!CheckForQueryResult(gl, busy_buffers_.front()->query_id))
                break;

            MarkStagingBufferAsFree(busy_buffers_.front());
            free_buffers_.push_back(busy_buffers_.take_front());
        }
    }

    // Wait for memory usage of non-free buffers to become less than the limit.
    while (
        (staging_buffer_usage_in_bytes_ - free_staging_buffer_usage_in_bytes_) >= max_staging_buffer_usage_in_bytes_) {
        // Stop when there are no more busy buffers to wait for.
        if (busy_buffers_.empty())
            break;

        if (resource_provider_->use_sync_query()) {
            WaitForQueryResult(gl, busy_buffers_.front()->query_id);
            MarkStagingBufferAsFree(busy_buffers_.front());
            free_buffers_.push_back(busy_buffers_.take_front());
        } else {
            // Fall-back to glFinish if CHROMIUM_sync_query is not available.
            gl->Finish();
            while (!busy_buffers_.empty()) {
                MarkStagingBufferAsFree(busy_buffers_.front());
                free_buffers_.push_back(busy_buffers_.take_front());
            }
        }
    }

    // Find a staging buffer that allows us to perform partial raster when
    // using persistent GpuMemoryBuffers.
    if (use_persistent_gpu_memory_buffers_ && previous_content_id) {
        StagingBufferDeque::iterator it = std::find_if(free_buffers_.begin(), free_buffers_.end(),
            [previous_content_id](const StagingBuffer* buffer) {
                return buffer->content_id == previous_content_id;
            });
        if (it != free_buffers_.end()) {
            staging_buffer = free_buffers_.take(it);
            MarkStagingBufferAsBusy(staging_buffer.get());
        }
    }

    // Find staging buffer of correct size and format.
    if (!staging_buffer) {
        StagingBufferDeque::iterator it = std::find_if(free_buffers_.begin(), free_buffers_.end(),
            [resource](const StagingBuffer* buffer) {
                return buffer->size == resource->size() && buffer->format == resource->format();
            });
        if (it != free_buffers_.end()) {
            staging_buffer = free_buffers_.take(it);
            MarkStagingBufferAsBusy(staging_buffer.get());
        }
    }

    // Create new staging buffer if necessary.
    if (!staging_buffer) {
        staging_buffer = make_scoped_ptr(
            new StagingBuffer(resource->size(), resource->format()));
        AddStagingBuffer(staging_buffer.get(), resource->format());
    }

    // Release enough free buffers to stay within the limit.
    while (staging_buffer_usage_in_bytes_ > max_staging_buffer_usage_in_bytes_) {
        if (free_buffers_.empty())
            break;

        free_buffers_.front()->DestroyGLResources(gl);
        MarkStagingBufferAsBusy(free_buffers_.front());
        RemoveStagingBuffer(free_buffers_.front());
        free_buffers_.take_front();
    }

    return staging_buffer.Pass();
}

base::TimeTicks OneCopyTileTaskWorkerPool::GetUsageTimeForLRUBuffer()
{
    lock_.AssertAcquired();

    if (!free_buffers_.empty())
        return free_buffers_.front()->last_usage;

    if (!busy_buffers_.empty())
        return busy_buffers_.front()->last_usage;

    return base::TimeTicks();
}

void OneCopyTileTaskWorkerPool::ScheduleReduceMemoryUsage()
{
    lock_.AssertAcquired();

    if (reduce_memory_usage_pending_)
        return;

    reduce_memory_usage_pending_ = true;

    // Schedule a call to ReduceMemoryUsage at the time when the LRU buffer
    // should be released.
    base::TimeTicks reduce_memory_usage_time = GetUsageTimeForLRUBuffer() + staging_buffer_expiration_delay_;
    task_runner_->PostDelayedTask(
        FROM_HERE, reduce_memory_usage_callback_,
        reduce_memory_usage_time - base::TimeTicks::Now());
}

void OneCopyTileTaskWorkerPool::ReduceMemoryUsage()
{
    base::AutoLock lock(lock_);

    reduce_memory_usage_pending_ = false;

    if (free_buffers_.empty() && busy_buffers_.empty())
        return;

    base::TimeTicks current_time = base::TimeTicks::Now();
    ReleaseBuffersNotUsedSince(current_time - staging_buffer_expiration_delay_);

    if (free_buffers_.empty() && busy_buffers_.empty())
        return;

    reduce_memory_usage_pending_ = true;

    // Schedule another call to ReduceMemoryUsage at the time when the next
    // buffer should be released.
    base::TimeTicks reduce_memory_usage_time = GetUsageTimeForLRUBuffer() + staging_buffer_expiration_delay_;
    task_runner_->PostDelayedTask(FROM_HERE, reduce_memory_usage_callback_,
        reduce_memory_usage_time - current_time);
}

void OneCopyTileTaskWorkerPool::ReleaseBuffersNotUsedSince(
    base::TimeTicks time)
{
    lock_.AssertAcquired();

    ContextProvider* context_provider = resource_provider_->output_surface()->worker_context_provider();
    DCHECK(context_provider);

    {
        ContextProvider::ScopedContextLock scoped_context(context_provider);

        gpu::gles2::GLES2Interface* gl = scoped_context.ContextGL();
        DCHECK(gl);

        // Note: Front buffer is guaranteed to be LRU so we can stop releasing
        // buffers as soon as we find a buffer that has been used since |time|.
        while (!free_buffers_.empty()) {
            if (free_buffers_.front()->last_usage > time)
                return;

            free_buffers_.front()->DestroyGLResources(gl);
            MarkStagingBufferAsBusy(free_buffers_.front());
            RemoveStagingBuffer(free_buffers_.front());
            free_buffers_.take_front();
        }

        while (!busy_buffers_.empty()) {
            if (busy_buffers_.front()->last_usage > time)
                return;

            busy_buffers_.front()->DestroyGLResources(gl);
            RemoveStagingBuffer(busy_buffers_.front());
            busy_buffers_.take_front();
        }
    }
}

void OneCopyTileTaskWorkerPool::OnTaskSetFinished(TaskSet task_set)
{
    TRACE_EVENT1("cc", "OneCopyTileTaskWorkerPool::OnTaskSetFinished", "task_set",
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

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
OneCopyTileTaskWorkerPool::StateAsValue() const
{
    scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();

    state->BeginArray("tasks_pending");
    for (TaskSet task_set = 0; task_set < kNumberOfTaskSets; ++task_set)
        state->AppendBoolean(tasks_pending_[task_set]);
    state->EndArray();
    state->BeginDictionary("staging_state");
    StagingStateAsValueInto(state.get());
    state->EndDictionary();

    return state;
}

void OneCopyTileTaskWorkerPool::StagingStateAsValueInto(
    base::trace_event::TracedValue* staging_state) const
{
    base::AutoLock lock(lock_);

    staging_state->SetInteger("staging_buffer_count",
        static_cast<int>(buffers_.size()));
    staging_state->SetInteger("busy_count",
        static_cast<int>(busy_buffers_.size()));
    staging_state->SetInteger("free_count",
        static_cast<int>(free_buffers_.size()));
}

} // namespace cc
