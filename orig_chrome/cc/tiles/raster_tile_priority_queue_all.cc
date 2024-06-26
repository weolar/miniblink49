// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/tiles/raster_tile_priority_queue_all.h"

#include "cc/tiles/tiling_set_raster_queue_all.h"

namespace cc {

namespace {

    class RasterOrderComparator {
    public:
        explicit RasterOrderComparator(TreePriority tree_priority)
            : tree_priority_(tree_priority)
        {
        }

        bool operator()(const TilingSetRasterQueueAll* a_queue,
            const TilingSetRasterQueueAll* b_queue) const
        {
            // Note that in this function, we have to return true if and only if
            // a is strictly lower priority than b.
            const TilePriority& a_priority = a_queue->Top().priority();
            const TilePriority& b_priority = b_queue->Top().priority();
            bool prioritize_low_res = tree_priority_ == SMOOTHNESS_TAKES_PRIORITY;

            // If the bin is the same but the resolution is not, then the order will be
            // determined by whether we prioritize low res or not.
            // TODO(vmpstr): Remove this when TilePriority is no longer a member of Tile
            // class but instead produced by the iterators.
            if (b_priority.priority_bin == a_priority.priority_bin && b_priority.resolution != a_priority.resolution) {
                // Non ideal resolution should be sorted lower than other resolutions.
                if (a_priority.resolution == NON_IDEAL_RESOLUTION)
                    return true;

                if (b_priority.resolution == NON_IDEAL_RESOLUTION)
                    return false;

                if (prioritize_low_res)
                    return b_priority.resolution == LOW_RESOLUTION;
                return b_priority.resolution == HIGH_RESOLUTION;
            }

            return b_priority.IsHigherPriorityThan(a_priority);
        }

    private:
        TreePriority tree_priority_;
    };

    void CreateTilingSetRasterQueues(
        const std::vector<PictureLayerImpl*>& layers,
        TreePriority tree_priority,
        ScopedPtrVector<TilingSetRasterQueueAll>* queues)
    {
        DCHECK(queues->empty());

        for (auto* layer : layers) {
            if (!layer->HasValidTilePriorities())
                continue;

            PictureLayerTilingSet* tiling_set = layer->picture_layer_tiling_set();
            bool prioritize_low_res = tree_priority == SMOOTHNESS_TAKES_PRIORITY;
            scoped_ptr<TilingSetRasterQueueAll> tiling_set_queue = make_scoped_ptr(
                new TilingSetRasterQueueAll(tiling_set, prioritize_low_res));
            // Queues will only contain non empty tiling sets.
            if (!tiling_set_queue->IsEmpty())
                queues->push_back(tiling_set_queue.Pass());
        }
        queues->make_heap(RasterOrderComparator(tree_priority));
    }

} // namespace

RasterTilePriorityQueueAll::RasterTilePriorityQueueAll()
{
}

RasterTilePriorityQueueAll::~RasterTilePriorityQueueAll()
{
}

void RasterTilePriorityQueueAll::Build(
    const std::vector<PictureLayerImpl*>& active_layers,
    const std::vector<PictureLayerImpl*>& pending_layers,
    TreePriority tree_priority)
{
    tree_priority_ = tree_priority;

    CreateTilingSetRasterQueues(active_layers, tree_priority_, &active_queues_);
    CreateTilingSetRasterQueues(pending_layers, tree_priority_, &pending_queues_);
}

bool RasterTilePriorityQueueAll::IsEmpty() const
{
    return active_queues_.empty() && pending_queues_.empty();
}

const PrioritizedTile& RasterTilePriorityQueueAll::Top() const
{
    DCHECK(!IsEmpty());
    const ScopedPtrVector<TilingSetRasterQueueAll>& next_queues = GetNextQueues();
    return next_queues.front()->Top();
}

void RasterTilePriorityQueueAll::Pop()
{
    DCHECK(!IsEmpty());

    ScopedPtrVector<TilingSetRasterQueueAll>& next_queues = GetNextQueues();
    next_queues.pop_heap(RasterOrderComparator(tree_priority_));
    TilingSetRasterQueueAll* queue = next_queues.back();
    queue->Pop();

    // Remove empty queues.
    if (queue->IsEmpty())
        next_queues.pop_back();
    else
        next_queues.push_heap(RasterOrderComparator(tree_priority_));
}

ScopedPtrVector<TilingSetRasterQueueAll>&
RasterTilePriorityQueueAll::GetNextQueues()
{
    return const_cast<ScopedPtrVector<TilingSetRasterQueueAll>&>(
        static_cast<const RasterTilePriorityQueueAll*>(this)->GetNextQueues());
}

const ScopedPtrVector<TilingSetRasterQueueAll>&
RasterTilePriorityQueueAll::GetNextQueues() const
{
    DCHECK(!IsEmpty());

    // If we only have one queue with tiles, return it.
    if (active_queues_.empty())
        return pending_queues_;
    if (pending_queues_.empty())
        return active_queues_;

    const PrioritizedTile& active_tile = active_queues_.front()->Top();
    const PrioritizedTile& pending_tile = pending_queues_.front()->Top();

    const TilePriority& active_priority = active_tile.priority();
    const TilePriority& pending_priority = pending_tile.priority();

    switch (tree_priority_) {
    case SMOOTHNESS_TAKES_PRIORITY: {
        // If we're down to eventually bin tiles on the active tree and there
        // is a pending tree, process the entire pending tree to allow tiles
        // required for activation to be initialized when memory policy only
        // allows prepaint. The eventually bin tiles on the active tree are
        // lowest priority since that work is likely to be thrown away when
        // we activate.
        if (active_priority.priority_bin == TilePriority::EVENTUALLY)
            return pending_queues_;
        return active_queues_;
    }
    case NEW_CONTENT_TAKES_PRIORITY: {
        // If we're down to soon bin tiles on the pending tree, process the
        // active tree to allow tiles required for activation to be initialized
        // when memory policy only allows prepaint. Note that active required for
        // activation tiles might come from either now or soon bins.
        if (pending_priority.priority_bin >= TilePriority::SOON && active_priority.priority_bin <= TilePriority::SOON) {
            return active_queues_;
        }
        return pending_queues_;
    }
    case SAME_PRIORITY_FOR_BOTH_TREES: {
        if (active_priority.IsHigherPriorityThan(pending_priority))
            return active_queues_;
        return pending_queues_;
    }
    default:
        NOTREACHED();
        return active_queues_;
    }
}

} // namespace cc
