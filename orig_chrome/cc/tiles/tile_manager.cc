// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/tiles/tile_manager.h"

#include <algorithm>
#include <limits>
#include <string>

#include "base/bind.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/metrics/histogram.h"
#include "base/numerics/safe_conversions.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/base/histograms.h"
#include "cc/debug/devtools_instrumentation.h"
#include "cc/debug/frame_viewer_instrumentation.h"
#include "cc/debug/traced_value.h"
#include "cc/layers/picture_layer_impl.h"
#include "cc/raster/raster_buffer.h"
#include "cc/raster/tile_task_runner.h"
#include "cc/tiles/tile.h"
#include "ui/gfx/geometry/rect_conversions.h"

namespace cc {
namespace {

    // Flag to indicate whether we should try and detect that
    // a tile is of solid color.
    const bool kUseColorEstimator = true;

    DEFINE_SCOPED_UMA_HISTOGRAM_AREA_TIMER(
        ScopedRasterTaskTimer,
        "Compositing.%s.RasterTask.RasterUs",
        "Compositing.%s.RasterTask.RasterPixelsPerMs");

    class RasterTaskImpl : public RasterTask {
    public:
        RasterTaskImpl(
            const Resource* resource,
            RasterSource* raster_source,
            const gfx::Rect& content_rect,
            const gfx::Rect& invalid_content_rect,
            float contents_scale,
            TileResolution tile_resolution,
            int layer_id,
            uint64_t source_prepare_tiles_id,
            const void* tile,
            uint64_t new_content_id,
            uint64_t previous_content_id,
            uint64_t resource_content_id,
            int source_frame_number,
            bool analyze_picture,
            const base::Callback<void(const RasterSource::SolidColorAnalysis&, bool)>&
                reply,
            ImageDecodeTask::Vector* dependencies)
            : RasterTask(dependencies)
            , resource_(resource)
            , raster_source_(raster_source)
            , content_rect_(content_rect)
            , invalid_content_rect_(invalid_content_rect)
            , contents_scale_(contents_scale)
            , tile_resolution_(tile_resolution)
            , layer_id_(layer_id)
            , source_prepare_tiles_id_(source_prepare_tiles_id)
            , tile_(tile)
            , new_content_id_(new_content_id)
            , previous_content_id_(previous_content_id)
            , resource_content_id_(resource_content_id)
            , source_frame_number_(source_frame_number)
            , analyze_picture_(analyze_picture)
            , reply_(reply)
        {
        }

        // Overridden from Task:
        void RunOnWorkerThread() override
        {
            TRACE_EVENT1("cc", "RasterizerTaskImpl::RunOnWorkerThread",
                "source_prepare_tiles_id", source_prepare_tiles_id_);

            DCHECK(raster_source_.get());
            DCHECK(raster_buffer_);

            if (analyze_picture_) {
                Analyze(raster_source_.get());
                if (analysis_.is_solid_color)
                    return;
            }

            Raster(raster_source_.get());
        }

        // Overridden from TileTask:
        void ScheduleOnOriginThread(TileTaskClient* client) override
        {
            DCHECK(!raster_buffer_);
            raster_buffer_ = client->AcquireBufferForRaster(
                resource_, resource_content_id_, previous_content_id_);
        }
        void CompleteOnOriginThread(TileTaskClient* client) override
        {
            client->ReleaseBufferForRaster(raster_buffer_.Pass());
            reply_.Run(analysis_, !HasFinishedRunning());
        }

    protected:
        ~RasterTaskImpl() override { DCHECK(!raster_buffer_); }

    private:
        void Analyze(const RasterSource* raster_source)
        {
            frame_viewer_instrumentation::ScopedAnalyzeTask analyze_task(
                tile_, tile_resolution_, source_frame_number_, layer_id_);

            DCHECK(raster_source);

            raster_source->PerformSolidColorAnalysis(content_rect_, contents_scale_,
                &analysis_);
            // Clear the flag if we're not using the estimator.
            analysis_.is_solid_color &= kUseColorEstimator;
        }

        void Raster(const RasterSource* raster_source)
        {
            frame_viewer_instrumentation::ScopedRasterTask raster_task(
                tile_, tile_resolution_, source_frame_number_, layer_id_);
            //     ScopedRasterTaskTimer timer;
            //     timer.SetArea(content_rect_.size().GetArea());
            DCHECK(raster_source);

            bool include_images = tile_resolution_ != LOW_RESOLUTION;
            raster_buffer_->Playback(raster_source_.get(), content_rect_,
                invalid_content_rect_, new_content_id_,
                contents_scale_, include_images);
        }
        const Resource* resource_;
        RasterSource::SolidColorAnalysis analysis_;
        scoped_refptr<RasterSource> raster_source_;
        gfx::Rect content_rect_;
        gfx::Rect invalid_content_rect_;
        float contents_scale_;
        TileResolution tile_resolution_;
        int layer_id_;
        uint64_t source_prepare_tiles_id_;
        const void* tile_;
        uint64_t new_content_id_;
        uint64_t previous_content_id_;
        uint64_t resource_content_id_;
        int source_frame_number_;
        bool analyze_picture_;
        const base::Callback<void(const RasterSource::SolidColorAnalysis&, bool)>
            reply_;
        scoped_ptr<RasterBuffer> raster_buffer_;

        DISALLOW_COPY_AND_ASSIGN(RasterTaskImpl);
    };

    const char* TaskSetName(TaskSet task_set)
    {
        switch (task_set) {
        case TileManager::ALL:
            return "ALL";
        case TileManager::REQUIRED_FOR_ACTIVATION:
            return "REQUIRED_FOR_ACTIVATION";
        case TileManager::REQUIRED_FOR_DRAW:
            return "REQUIRED_FOR_DRAW";
        }

        NOTREACHED();
        return "Invalid TaskSet";
    }

} // namespace

RasterTaskCompletionStats::RasterTaskCompletionStats()
    : completed_count(0u)
    , canceled_count(0u)
{
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
RasterTaskCompletionStatsAsValue(const RasterTaskCompletionStats& stats)
{
    scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();
    state->SetInteger("completed_count",
        base::saturated_cast<int>(stats.completed_count));
    state->SetInteger("canceled_count",
        base::saturated_cast<int>(stats.canceled_count));
    return state;
}

// static
scoped_ptr<TileManager> TileManager::Create(
    TileManagerClient* client,
    base::SequencedTaskRunner* task_runner,
    size_t scheduled_raster_task_limit)
{
    return make_scoped_ptr(
        new TileManager(client, task_runner, scheduled_raster_task_limit));
}

TileManager::TileManager(
    TileManagerClient* client,
    const scoped_refptr<base::SequencedTaskRunner>& task_runner,
    size_t scheduled_raster_task_limit)
    : client_(client)
    , task_runner_(task_runner)
    , resource_pool_(nullptr)
    , tile_task_runner_(nullptr)
    , scheduled_raster_task_limit_(scheduled_raster_task_limit)
    , all_tiles_that_need_to_be_rasterized_are_scheduled_(true)
    , did_check_for_completed_tasks_since_last_schedule_tasks_(true)
    , did_oom_on_last_assign_(false)
    , more_tiles_need_prepare_check_notifier_(
          task_runner_.get(),
          base::Bind(&TileManager::CheckIfMoreTilesNeedToBePrepared,
              base::Unretained(this)))
    , signals_check_notifier_(task_runner_.get(),
          base::Bind(&TileManager::CheckAndIssueSignals,
              base::Unretained(this)))
    , has_scheduled_tile_tasks_(false)
    , prepare_tiles_count_(0u)
    , next_tile_id_(0u)
{
}

TileManager::~TileManager()
{
    FinishTasksAndCleanUp();
}

void TileManager::FinishTasksAndCleanUp()
{
    if (!tile_task_runner_)
        return;

    global_state_ = GlobalStateThatImpactsTilePriority();

    TileTaskQueue empty;
    tile_task_runner_->ScheduleTasks(&empty);
    orphan_raster_tasks_.clear();

    // This should finish all pending tasks and release any uninitialized
    // resources.
    tile_task_runner_->Shutdown();
    tile_task_runner_->CheckForCompletedTasks();

    FreeResourcesForReleasedTiles();
    CleanUpReleasedTiles();

    tile_task_runner_ = nullptr;
    resource_pool_ = nullptr;
    more_tiles_need_prepare_check_notifier_.Cancel();
    signals_check_notifier_.Cancel();
}

void TileManager::SetResources(ResourcePool* resource_pool,
    TileTaskRunner* tile_task_runner,
    size_t scheduled_raster_task_limit)
{
    DCHECK(!tile_task_runner_);
    DCHECK(tile_task_runner);

    scheduled_raster_task_limit_ = scheduled_raster_task_limit;
    resource_pool_ = resource_pool;
    tile_task_runner_ = tile_task_runner;
    tile_task_runner_->SetClient(this);
}

void TileManager::Release(Tile* tile)
{
    released_tiles_.push_back(tile);
}

void TileManager::FreeResourcesForReleasedTiles()
{
    for (auto* tile : released_tiles_)
        FreeResourcesForTile(tile);
}

void TileManager::CleanUpReleasedTiles()
{
    std::vector<Tile*> tiles_to_retain;
    for (auto* tile : released_tiles_) {
        if (tile->HasRasterTask()) {
            tiles_to_retain.push_back(tile);
            continue;
        }

        DCHECK(!tile->draw_info().has_resource());
        DCHECK(tiles_.find(tile->id()) != tiles_.end());
        tiles_.erase(tile->id());

        image_decode_controller_.SubtractLayerUsedCount(tile->layer_id());
        delete tile;
    }
    released_tiles_.swap(tiles_to_retain);
}

void TileManager::DidFinishRunningTileTasks(TaskSet task_set)
{
    TRACE_EVENT1("cc", "TileManager::DidFinishRunningTileTasks", "task_set",
        TaskSetName(task_set));
    DCHECK(resource_pool_);
    DCHECK(tile_task_runner_);

    switch (task_set) {
    case ALL: {
        has_scheduled_tile_tasks_ = false;

        bool memory_usage_above_limit = resource_pool_->memory_usage_bytes() > global_state_.soft_memory_limit_in_bytes;

        if (all_tiles_that_need_to_be_rasterized_are_scheduled_ && !memory_usage_above_limit) {
            // TODO(ericrk): We should find a better way to safely handle re-entrant
            // notifications than always having to schedule a new task.
            // http://crbug.com/498439
            signals_.all_tile_tasks_completed = true;
            signals_check_notifier_.Schedule();
            return;
        }

        more_tiles_need_prepare_check_notifier_.Schedule();
        return;
    }
    case REQUIRED_FOR_ACTIVATION:
        signals_.ready_to_activate = true;
        signals_check_notifier_.Schedule();
        return;

    case REQUIRED_FOR_DRAW:
        signals_.ready_to_draw = true;
        signals_check_notifier_.Schedule();
        return;
    }

    NOTREACHED();
}

bool TileManager::PrepareTiles(
    const GlobalStateThatImpactsTilePriority& state)
{
    ++prepare_tiles_count_;

    TRACE_EVENT1("cc", "TileManager::PrepareTiles", "prepare_tiles_id",
        prepare_tiles_count_);

    if (!tile_task_runner_) {
        TRACE_EVENT_INSTANT0("cc", "PrepareTiles aborted",
            TRACE_EVENT_SCOPE_THREAD);
        return false;
    }

    signals_.reset();
    global_state_ = state;

    // We need to call CheckForCompletedTasks() once in-between each call
    // to ScheduleTasks() to prevent canceled tasks from being scheduled.
    if (!did_check_for_completed_tasks_since_last_schedule_tasks_) {
        tile_task_runner_->CheckForCompletedTasks();
        did_check_for_completed_tasks_since_last_schedule_tasks_ = true;
    }

    FreeResourcesForReleasedTiles();
    CleanUpReleasedTiles();

    PrioritizedTileVector tiles_that_need_to_be_rasterized;
    scoped_ptr<RasterTilePriorityQueue> raster_priority_queue(
        client_->BuildRasterQueue(global_state_.tree_priority,
            RasterTilePriorityQueue::Type::ALL));
    AssignGpuMemoryToTiles(raster_priority_queue.get(),
        scheduled_raster_task_limit_,
        &tiles_that_need_to_be_rasterized);

    // Inform the client that will likely require a draw if the highest priority
    // tile that will be rasterized is required for draw.
    client_->SetIsLikelyToRequireADraw(
        !tiles_that_need_to_be_rasterized.empty() && tiles_that_need_to_be_rasterized.front().tile()->required_for_draw());

    // Schedule tile tasks.
    ScheduleTasks(tiles_that_need_to_be_rasterized);

    TRACE_EVENT_INSTANT1("cc", "DidPrepareTiles", TRACE_EVENT_SCOPE_THREAD,
        "state", BasicStateAsValue());
    return true;
}

void TileManager::Flush()
{
    TRACE_EVENT0("cc", "TileManager::Flush");

    if (!tile_task_runner_) {
        TRACE_EVENT_INSTANT0("cc", "Flush aborted", TRACE_EVENT_SCOPE_THREAD);
        return;
    }

    tile_task_runner_->CheckForCompletedTasks();

    did_check_for_completed_tasks_since_last_schedule_tasks_ = true;

    TRACE_EVENT_INSTANT1("cc", "DidFlush", TRACE_EVENT_SCOPE_THREAD, "stats",
        RasterTaskCompletionStatsAsValue(flush_stats_));
    flush_stats_ = RasterTaskCompletionStats();
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
TileManager::BasicStateAsValue() const
{
    scoped_refptr<base::trace_event::TracedValue> value = new base::trace_event::TracedValue();
    BasicStateAsValueInto(value.get());
    return value;
}

void TileManager::BasicStateAsValueInto(
    base::trace_event::TracedValue* state) const
{
    state->SetInteger("tile_count", base::saturated_cast<int>(tiles_.size()));
    state->SetBoolean("did_oom_on_last_assign", did_oom_on_last_assign_);
    state->BeginDictionary("global_state");
    global_state_.AsValueInto(state);
    state->EndDictionary();
}

scoped_ptr<EvictionTilePriorityQueue>
TileManager::FreeTileResourcesUntilUsageIsWithinLimit(
    scoped_ptr<EvictionTilePriorityQueue> eviction_priority_queue,
    const MemoryUsage& limit,
    MemoryUsage* usage)
{
    while (usage->Exceeds(limit)) {
        if (!eviction_priority_queue) {
            eviction_priority_queue = client_->BuildEvictionQueue(global_state_.tree_priority);
        }
        if (eviction_priority_queue->IsEmpty())
            break;

        Tile* tile = eviction_priority_queue->Top().tile();
        *usage -= MemoryUsage::FromTile(tile);
        FreeResourcesForTileAndNotifyClientIfTileWasReadyToDraw(tile);
        eviction_priority_queue->Pop();
    }
    return eviction_priority_queue;
}

scoped_ptr<EvictionTilePriorityQueue>
TileManager::FreeTileResourcesWithLowerPriorityUntilUsageIsWithinLimit(
    scoped_ptr<EvictionTilePriorityQueue> eviction_priority_queue,
    const MemoryUsage& limit,
    const TilePriority& other_priority,
    MemoryUsage* usage)
{
    while (usage->Exceeds(limit)) {
        if (!eviction_priority_queue) {
            eviction_priority_queue = client_->BuildEvictionQueue(global_state_.tree_priority);
        }
        if (eviction_priority_queue->IsEmpty())
            break;

        const PrioritizedTile& prioritized_tile = eviction_priority_queue->Top();
        if (!other_priority.IsHigherPriorityThan(prioritized_tile.priority()))
            break;

        Tile* tile = prioritized_tile.tile();
        *usage -= MemoryUsage::FromTile(tile);
        FreeResourcesForTileAndNotifyClientIfTileWasReadyToDraw(tile);
        eviction_priority_queue->Pop();
    }
    return eviction_priority_queue;
}

bool TileManager::TilePriorityViolatesMemoryPolicy(
    const TilePriority& priority)
{
    switch (global_state_.memory_limit_policy) {
    case ALLOW_NOTHING:
        return true;
    case ALLOW_ABSOLUTE_MINIMUM:
        return priority.priority_bin > TilePriority::NOW;
    case ALLOW_PREPAINT_ONLY:
        return priority.priority_bin > TilePriority::SOON;
    case ALLOW_ANYTHING:
        return priority.distance_to_visible == std::numeric_limits<float>::infinity();
    }
    NOTREACHED();
    return true;
}

void TileManager::AssignGpuMemoryToTiles(
    RasterTilePriorityQueue* raster_priority_queue,
    size_t scheduled_raster_task_limit,
    PrioritizedTileVector* tiles_that_need_to_be_rasterized)
{
    TRACE_EVENT_BEGIN0("cc", "TileManager::AssignGpuMemoryToTiles");

    DCHECK(resource_pool_);
    DCHECK(tile_task_runner_);

    // Maintain the list of released resources that can potentially be re-used
    // or deleted. If this operation becomes expensive too, only do this after
    // some resource(s) was returned. Note that in that case, one also need to
    // invalidate when releasing some resource from the pool.
    resource_pool_->CheckBusyResources();

    // Now give memory out to the tiles until we're out, and build
    // the needs-to-be-rasterized queue.
    unsigned schedule_priority = 1u;
    all_tiles_that_need_to_be_rasterized_are_scheduled_ = true;
    bool had_enough_memory_to_schedule_tiles_needed_now = true;

    MemoryUsage hard_memory_limit(global_state_.hard_memory_limit_in_bytes,
        global_state_.num_resources_limit);
    MemoryUsage soft_memory_limit(global_state_.soft_memory_limit_in_bytes,
        global_state_.num_resources_limit);
    MemoryUsage memory_usage(resource_pool_->memory_usage_bytes(),
        resource_pool_->resource_count());

    scoped_ptr<EvictionTilePriorityQueue> eviction_priority_queue;
    for (; !raster_priority_queue->IsEmpty(); raster_priority_queue->Pop()) {
        const PrioritizedTile& prioritized_tile = raster_priority_queue->Top();
        Tile* tile = prioritized_tile.tile();
        TilePriority priority = prioritized_tile.priority();

        if (TilePriorityViolatesMemoryPolicy(priority)) {
            TRACE_EVENT_INSTANT0(
                "cc", "TileManager::AssignGpuMemory tile violates memory policy",
                TRACE_EVENT_SCOPE_THREAD);
            break;
        }

        // We won't be able to schedule this tile, so break out early.
        if (tiles_that_need_to_be_rasterized->size() >= scheduled_raster_task_limit) {
            all_tiles_that_need_to_be_rasterized_are_scheduled_ = false;
            break;
        }

        tile->scheduled_priority_ = schedule_priority++;

        DCHECK_IMPLIES(tile->draw_info().mode() != TileDrawInfo::OOM_MODE,
            !tile->draw_info().IsReadyToDraw());

        // If the tile already has a raster_task, then the memory used by it is
        // already accounted for in memory_usage. Otherwise, we'll have to acquire
        // more memory to create a raster task.
        MemoryUsage memory_required_by_tile_to_be_scheduled;
        if (!tile->raster_task_.get()) {
            memory_required_by_tile_to_be_scheduled = MemoryUsage::FromConfig(
                tile->desired_texture_size(), DetermineResourceFormat(tile));
        }

        bool tile_is_needed_now = priority.priority_bin == TilePriority::NOW;

        // This is the memory limit that will be used by this tile. Depending on
        // the tile priority, it will be one of hard_memory_limit or
        // soft_memory_limit.
        MemoryUsage& tile_memory_limit = tile_is_needed_now ? hard_memory_limit : soft_memory_limit;

        const MemoryUsage& scheduled_tile_memory_limit = tile_memory_limit - memory_required_by_tile_to_be_scheduled;
        eviction_priority_queue = FreeTileResourcesWithLowerPriorityUntilUsageIsWithinLimit(
            eviction_priority_queue.Pass(), scheduled_tile_memory_limit,
            priority, &memory_usage);
        bool memory_usage_is_within_limit = !memory_usage.Exceeds(scheduled_tile_memory_limit);

        // If we couldn't fit the tile into our current memory limit, then we're
        // done.
        if (!memory_usage_is_within_limit) {
            if (tile_is_needed_now)
                had_enough_memory_to_schedule_tiles_needed_now = false;
            all_tiles_that_need_to_be_rasterized_are_scheduled_ = false;
            break;
        }

        memory_usage += memory_required_by_tile_to_be_scheduled;
        tiles_that_need_to_be_rasterized->push_back(prioritized_tile);
    }

    // Note that we should try and further reduce memory in case the above loop
    // didn't reduce memory. This ensures that we always release as many resources
    // as possible to stay within the memory limit.
    eviction_priority_queue = FreeTileResourcesUntilUsageIsWithinLimit(
        eviction_priority_queue.Pass(), hard_memory_limit, &memory_usage);

    UMA_HISTOGRAM_BOOLEAN("TileManager.ExceededMemoryBudget",
        !had_enough_memory_to_schedule_tiles_needed_now);
    did_oom_on_last_assign_ = !had_enough_memory_to_schedule_tiles_needed_now;

    memory_stats_from_last_assign_.total_budget_in_bytes = global_state_.hard_memory_limit_in_bytes;
    memory_stats_from_last_assign_.total_bytes_used = memory_usage.memory_bytes();
    DCHECK_GE(memory_stats_from_last_assign_.total_bytes_used, 0);
    memory_stats_from_last_assign_.had_enough_memory = had_enough_memory_to_schedule_tiles_needed_now;

    TRACE_EVENT_END2("cc", "TileManager::AssignGpuMemoryToTiles",
        "all_tiles_that_need_to_be_rasterized_are_scheduled",
        all_tiles_that_need_to_be_rasterized_are_scheduled_,
        "had_enough_memory_to_schedule_tiles_needed_now",
        had_enough_memory_to_schedule_tiles_needed_now);
}

void TileManager::FreeResourcesForTile(Tile* tile)
{
    TileDrawInfo& draw_info = tile->draw_info();
    if (draw_info.resource_) {
        resource_pool_->ReleaseResource(draw_info.resource_, tile->id());
        draw_info.resource_ = nullptr;
    }
}

void TileManager::FreeResourcesForTileAndNotifyClientIfTileWasReadyToDraw(
    Tile* tile)
{
    bool was_ready_to_draw = tile->draw_info().IsReadyToDraw();
    FreeResourcesForTile(tile);
    if (was_ready_to_draw)
        client_->NotifyTileStateChanged(tile);
}

void TileManager::ScheduleTasks(
    const PrioritizedTileVector& tiles_that_need_to_be_rasterized)
{
    TRACE_EVENT1("cc",
        "TileManager::ScheduleTasks",
        "count",
        tiles_that_need_to_be_rasterized.size());

    DCHECK(did_check_for_completed_tasks_since_last_schedule_tasks_);

    raster_queue_.Reset();

    // Even when scheduling an empty set of tiles, the TTWP does some work, and
    // will always trigger a DidFinishRunningTileTasks notification. Because of
    // this we unconditionally set |has_scheduled_tile_tasks_| to true.
    has_scheduled_tile_tasks_ = true;

    //   if (0 != tiles_that_need_to_be_rasterized.size()) {
    //       char* outString = (char*)malloc(1000);
    //       sprintf(outString, "TileManager::ScheduleTasks:%d\n", tiles_that_need_to_be_rasterized.size());
    //       OutputDebugStringA(outString);
    //       free(outString);
    //   }

    // Build a new task queue containing all task currently needed. Tasks
    // are added in order of priority, highest priority task first.
    for (auto& prioritized_tile : tiles_that_need_to_be_rasterized) {
        Tile* tile = prioritized_tile.tile();

        DCHECK(tile->draw_info().requires_resource());
        DCHECK(!tile->draw_info().resource_);

        if (!tile->raster_task_.get())
            tile->raster_task_ = CreateRasterTask(prioritized_tile);

        TaskSetCollection task_sets;
        if (tile->required_for_activation())
            task_sets.set(REQUIRED_FOR_ACTIVATION);
        if (tile->required_for_draw())
            task_sets.set(REQUIRED_FOR_DRAW);
        task_sets.set(ALL);
        raster_queue_.items.push_back(
            TileTaskQueue::Item(tile->raster_task_.get(), task_sets));
    }

    // We must reduce the amount of unused resoruces before calling
    // ScheduleTasks to prevent usage from rising above limits.
    resource_pool_->ReduceResourceUsage();

    // Schedule running of |raster_queue_|. This replaces any previously
    // scheduled tasks and effectively cancels all tasks not present
    // in |raster_queue_|.
    tile_task_runner_->ScheduleTasks(&raster_queue_);

    // It's now safe to clean up orphan tasks as raster worker pool is not
    // allowed to keep around unreferenced raster tasks after ScheduleTasks() has
    // been called.
    orphan_raster_tasks_.clear();

    did_check_for_completed_tasks_since_last_schedule_tasks_ = false;
}

scoped_refptr<RasterTask> TileManager::CreateRasterTask(
    const PrioritizedTile& prioritized_tile)
{
    Tile* tile = prioritized_tile.tile();
    uint64_t resource_content_id = 0;
    Resource* resource = nullptr;
    if (tile->invalidated_id()) {
        // TODO(danakj): For resources that are in use, we should still grab them
        // and copy from them instead of rastering everything. crbug.com/492754
        resource = resource_pool_->TryAcquireResourceWithContentId(tile->invalidated_id());
    }
    if (resource) {
        resource_content_id = tile->invalidated_id();
        DCHECK_EQ(DetermineResourceFormat(tile), resource->format());
        DCHECK_EQ(tile->desired_texture_size().ToString(),
            resource->size().ToString());
    } else {
        resource = resource_pool_->AcquireResource(tile->desired_texture_size(),
            DetermineResourceFormat(tile));
    }

    // Create and queue all image decode tasks that this tile depends on.
    ImageDecodeTask::Vector decode_tasks;
    std::vector<PositionImage> images;
    prioritized_tile.raster_source()->GetDiscardableImagesInRect(
        tile->enclosing_layer_rect(), &images);
    for (const auto& image : images) {
        decode_tasks.push_back(image_decode_controller_.GetTaskForImage(
            image, tile->layer_id(), prepare_tiles_count_));
    }

    return make_scoped_refptr(new RasterTaskImpl(
        resource, prioritized_tile.raster_source(), tile->content_rect(),
        tile->invalidated_content_rect(), tile->contents_scale(),
        prioritized_tile.priority().resolution, tile->layer_id(),
        prepare_tiles_count_, static_cast<const void*>(tile), tile->id(),
        tile->invalidated_id(), resource_content_id, tile->source_frame_number(),
        tile->use_picture_analysis(),
        base::Bind(&TileManager::OnRasterTaskCompleted, base::Unretained(this),
            tile->id(), resource),
        &decode_tasks));
}

void TileManager::OnRasterTaskCompleted(
    Tile::Id tile_id,
    Resource* resource,
    const RasterSource::SolidColorAnalysis& analysis,
    bool was_canceled)
{
    DCHECK(tiles_.find(tile_id) != tiles_.end());

    Tile* tile = tiles_[tile_id];
    DCHECK(tile->raster_task_.get());
    orphan_raster_tasks_.push_back(tile->raster_task_);
    tile->raster_task_ = nullptr;

    if (was_canceled) {
        ++flush_stats_.canceled_count;
        // TODO(ericrk): If more partial raster work is done in the future, it may
        // be worth returning the resource to the pool with its previous ID (not
        // currently tracked). crrev.com/1370333002/#ps40001 has a possible method
        // of achieving this.
        resource_pool_->ReleaseResource(resource, 0 /* content_id */);
        return;
    }

    UpdateTileDrawInfo(tile, resource, analysis);
}

void TileManager::UpdateTileDrawInfo(
    Tile* tile,
    Resource* resource,
    const RasterSource::SolidColorAnalysis& analysis)
{
    TileDrawInfo& draw_info = tile->draw_info();

    ++flush_stats_.completed_count;

    if (analysis.is_solid_color) {
        draw_info.set_solid_color(analysis.solid_color);
        if (resource) {
            // TODO(ericrk): If more partial raster work is done in the future, it may
            // be worth returning the resource to the pool with its previous ID (not
            // currently tracked). crrev.com/1370333002/#ps40001 has a possible method
            // of achieving this.
            resource_pool_->ReleaseResource(resource, 0 /* content_id */);
        }
    } else {
        DCHECK(resource);
        draw_info.set_use_resource();
        draw_info.resource_ = resource;
        draw_info.contents_swizzled_ = DetermineResourceRequiresSwizzle(tile);
    }
    DCHECK(draw_info.IsReadyToDraw());
    draw_info.set_was_ever_ready_to_draw();

    client_->NotifyTileStateChanged(tile);
}

ScopedTilePtr TileManager::CreateTile(const Tile::CreateInfo& info,
    int layer_id,
    int source_frame_number,
    int flags)
{
    // We need to have a tile task worker pool to do anything meaningful with
    // tiles.
    DCHECK(tile_task_runner_);
    ScopedTilePtr tile(
        new Tile(this, info, layer_id, source_frame_number, flags));
    DCHECK(tiles_.find(tile->id()) == tiles_.end());

    tiles_[tile->id()] = tile.get();
    image_decode_controller_.AddLayerUsedCount(tile->layer_id());
    return tile;
}

void TileManager::SetTileTaskRunnerForTesting(
    TileTaskRunner* tile_task_runner)
{
    tile_task_runner_ = tile_task_runner;
    tile_task_runner_->SetClient(this);
}

bool TileManager::AreRequiredTilesReadyToDraw(
    RasterTilePriorityQueue::Type type) const
{
    scoped_ptr<RasterTilePriorityQueue> raster_priority_queue(
        client_->BuildRasterQueue(global_state_.tree_priority, type));
    // It is insufficient to check whether the raster queue we constructed is
    // empty. The reason for this is that there are situations (rasterize on
    // demand) when the tile both needs raster and it's ready to draw. Hence, we
    // have to iterate the queue to check whether the required tiles are ready to
    // draw.
    for (; !raster_priority_queue->IsEmpty(); raster_priority_queue->Pop()) {
        if (!raster_priority_queue->Top().tile()->draw_info().IsReadyToDraw())
            return false;
    }

#if DCHECK_IS_ON()
    scoped_ptr<RasterTilePriorityQueue> all_queue(
        client_->BuildRasterQueue(global_state_.tree_priority, type));
    for (; !all_queue->IsEmpty(); all_queue->Pop()) {
        Tile* tile = all_queue->Top().tile();
        DCHECK_IMPLIES(tile->required_for_activation(),
            tile->draw_info().IsReadyToDraw());
    }
#endif
    return true;
}

bool TileManager::IsReadyToActivate() const
{
    TRACE_EVENT0("cc", "TileManager::IsReadyToActivate");
    return AreRequiredTilesReadyToDraw(
        RasterTilePriorityQueue::Type::REQUIRED_FOR_ACTIVATION);
}

bool TileManager::IsReadyToDraw() const
{
    TRACE_EVENT0("cc", "TileManager::IsReadyToDraw");
    return AreRequiredTilesReadyToDraw(
        RasterTilePriorityQueue::Type::REQUIRED_FOR_DRAW);
}

void TileManager::CheckAndIssueSignals()
{
    TRACE_EVENT0("cc", "TileManager::CheckAndIssueSignals");
    tile_task_runner_->CheckForCompletedTasks();
    did_check_for_completed_tasks_since_last_schedule_tasks_ = true;

    // Ready to activate.
    if (signals_.ready_to_activate && !signals_.did_notify_ready_to_activate) {
        signals_.ready_to_activate = false;
        if (IsReadyToActivate()) {
            TRACE_EVENT0("cc",
                "TileManager::CheckAndIssueSignals - ready to activate");
            signals_.did_notify_ready_to_activate = true;
            client_->NotifyReadyToActivate();
        }
    }

    // Ready to draw.
    if (signals_.ready_to_draw && !signals_.did_notify_ready_to_draw) {
        signals_.ready_to_draw = false;
        if (IsReadyToDraw()) {
            TRACE_EVENT0("cc", "TileManager::CheckAndIssueSignals - ready to draw");
            signals_.did_notify_ready_to_draw = true;
            client_->NotifyReadyToDraw();
        }
    }

    // All tile tasks completed.
    if (signals_.all_tile_tasks_completed && !signals_.did_notify_all_tile_tasks_completed) {
        signals_.all_tile_tasks_completed = false;
        if (!has_scheduled_tile_tasks_) {
            TRACE_EVENT0(
                "cc", "TileManager::CheckAndIssueSignals - all tile tasks completed");
            signals_.did_notify_all_tile_tasks_completed = true;
            client_->NotifyAllTileTasksCompleted();
        }
    }
}

void TileManager::CheckIfMoreTilesNeedToBePrepared()
{
    tile_task_runner_->CheckForCompletedTasks();
    did_check_for_completed_tasks_since_last_schedule_tasks_ = true;

    // When OOM, keep re-assigning memory until we reach a steady state
    // where top-priority tiles are initialized.
    PrioritizedTileVector tiles_that_need_to_be_rasterized;
    scoped_ptr<RasterTilePriorityQueue> raster_priority_queue(
        client_->BuildRasterQueue(global_state_.tree_priority,
            RasterTilePriorityQueue::Type::ALL));
    AssignGpuMemoryToTiles(raster_priority_queue.get(),
        scheduled_raster_task_limit_,
        &tiles_that_need_to_be_rasterized);

    // Inform the client that will likely require a draw if the highest priority
    // tile that will be rasterized is required for draw.
    client_->SetIsLikelyToRequireADraw(
        !tiles_that_need_to_be_rasterized.empty() && tiles_that_need_to_be_rasterized.front().tile()->required_for_draw());

    // |tiles_that_need_to_be_rasterized| will be empty when we reach a
    // steady memory state. Keep scheduling tasks until we reach this state.
    if (!tiles_that_need_to_be_rasterized.empty()) {
        ScheduleTasks(tiles_that_need_to_be_rasterized);
        return;
    }

    FreeResourcesForReleasedTiles();

    resource_pool_->ReduceResourceUsage();

    signals_.all_tile_tasks_completed = true;
    signals_check_notifier_.Schedule();

    // We don't reserve memory for required-for-activation tiles during
    // accelerated gestures, so we just postpone activation when we don't
    // have these tiles, and activate after the accelerated gesture.
    // Likewise if we don't allow any tiles (as is the case when we're
    // invisible), if we have tiles that aren't ready, then we shouldn't
    // activate as activation can cause checkerboards.
    bool wait_for_all_required_tiles = global_state_.tree_priority == SMOOTHNESS_TAKES_PRIORITY || global_state_.memory_limit_policy == ALLOW_NOTHING;

    // Mark any required-for-activation tiles that have not been been assigned
    // memory after reaching a steady memory state as OOM. This ensures that we
    // activate even when OOM. Note that we can't reuse the queue we used for
    // AssignGpuMemoryToTiles, since the AssignGpuMemoryToTiles call could have
    // evicted some tiles that would not be picked up by the old raster queue.
    scoped_ptr<RasterTilePriorityQueue> required_for_activation_queue(
        client_->BuildRasterQueue(
            global_state_.tree_priority,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_ACTIVATION));

    // If we have tiles left to raster for activation, and we don't allow
    // activating without them, then skip activation and return early.
    if (!required_for_activation_queue->IsEmpty() && wait_for_all_required_tiles)
        return;

    // Mark required tiles as OOM so that we can activate without them.
    for (; !required_for_activation_queue->IsEmpty();
         required_for_activation_queue->Pop()) {
        Tile* tile = required_for_activation_queue->Top().tile();
        tile->draw_info().set_oom();
        client_->NotifyTileStateChanged(tile);
    }

    DCHECK(IsReadyToActivate());
    // TODO(ericrk): Investigate why we need to schedule this (not just call it
    // inline). http://crbug.com/498439
    signals_.ready_to_activate = true;
    signals_check_notifier_.Schedule();
}

ResourceFormat TileManager::DetermineResourceFormat(const Tile* tile) const
{
    return tile_task_runner_->GetResourceFormat(!tile->is_opaque());
}

bool TileManager::DetermineResourceRequiresSwizzle(const Tile* tile) const
{
    return tile_task_runner_->GetResourceRequiresSwizzle(!tile->is_opaque());
}

TileManager::MemoryUsage::MemoryUsage()
    : memory_bytes_(0)
    , resource_count_(0)
{
}

TileManager::MemoryUsage::MemoryUsage(size_t memory_bytes,
    size_t resource_count)
    : memory_bytes_(static_cast<int64>(memory_bytes))
    , resource_count_(static_cast<int>(resource_count))
{
    // MemoryUsage is constructed using size_ts, since it deals with memory and
    // the inputs are typically size_t. However, during the course of usage (in
    // particular operator-=) can cause internal values to become negative. Thus,
    // member variables are signed.
    DCHECK_LE(memory_bytes,
        static_cast<size_t>(std::numeric_limits<int64>::max()));
    DCHECK_LE(resource_count,
        static_cast<size_t>(std::numeric_limits<int>::max()));
}

// static
TileManager::MemoryUsage TileManager::MemoryUsage::FromConfig(
    const gfx::Size& size,
    ResourceFormat format)
{
    // We can use UncheckedSizeInBytes here since this is used with a tile
    // size which is determined by the compositor (it's at most max texture size).
    return MemoryUsage(ResourceUtil::UncheckedSizeInBytes<size_t>(size, format),
        1);
}

// static
TileManager::MemoryUsage TileManager::MemoryUsage::FromTile(const Tile* tile)
{
    const TileDrawInfo& draw_info = tile->draw_info();
    if (draw_info.resource_) {
        return MemoryUsage::FromConfig(draw_info.resource_->size(),
            draw_info.resource_->format());
    }
    return MemoryUsage();
}

TileManager::MemoryUsage& TileManager::MemoryUsage::operator+=(
    const MemoryUsage& other)
{
    memory_bytes_ += other.memory_bytes_;
    resource_count_ += other.resource_count_;
    return *this;
}

TileManager::MemoryUsage& TileManager::MemoryUsage::operator-=(
    const MemoryUsage& other)
{
    memory_bytes_ -= other.memory_bytes_;
    resource_count_ -= other.resource_count_;
    return *this;
}

TileManager::MemoryUsage TileManager::MemoryUsage::operator-(
    const MemoryUsage& other)
{
    MemoryUsage result = *this;
    result -= other;
    return result;
}

bool TileManager::MemoryUsage::Exceeds(const MemoryUsage& limit) const
{
    return memory_bytes_ > limit.memory_bytes_ || resource_count_ > limit.resource_count_;
}

TileManager::Signals::Signals()
{
    reset();
}

void TileManager::Signals::reset()
{
    ready_to_activate = false;
    did_notify_ready_to_activate = false;
    ready_to_draw = false;
    did_notify_ready_to_draw = false;
    all_tile_tasks_completed = false;
    did_notify_all_tile_tasks_completed = false;
}

} // namespace cc
