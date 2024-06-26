// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/tiles/raster_tile_priority_queue.h"

#include "cc/tiles/raster_tile_priority_queue_all.h"
#include "cc/tiles/raster_tile_priority_queue_required.h"

namespace cc {

// static
scoped_ptr<RasterTilePriorityQueue> RasterTilePriorityQueue::Create(
    const std::vector<PictureLayerImpl*>& active_layers,
    const std::vector<PictureLayerImpl*>& pending_layers,
    TreePriority tree_priority,
    Type type)
{
    switch (type) {
    case Type::ALL: {
        scoped_ptr<RasterTilePriorityQueueAll> queue(
            new RasterTilePriorityQueueAll);
        queue->Build(active_layers, pending_layers, tree_priority);
        return queue.Pass();
    }
    case Type::REQUIRED_FOR_ACTIVATION:
    case Type::REQUIRED_FOR_DRAW: {
        scoped_ptr<RasterTilePriorityQueueRequired> queue(
            new RasterTilePriorityQueueRequired);
        queue->Build(active_layers, pending_layers, type);
        return queue.Pass();
    }
    }
    NOTREACHED();
    return nullptr;
}

} // namespace cc
