// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TILES_EVICTION_TILE_PRIORITY_QUEUE_H_
#define CC_TILES_EVICTION_TILE_PRIORITY_QUEUE_H_

#include <set>
#include <utility>
#include <vector>

#include "cc/base/cc_export.h"
#include "cc/layers/picture_layer_impl.h"
#include "cc/tiles/tile_priority.h"
#include "cc/tiles/tiling_set_eviction_queue.h"

namespace cc {
class PrioritizedTile;

class CC_EXPORT EvictionTilePriorityQueue {
public:
    EvictionTilePriorityQueue();
    ~EvictionTilePriorityQueue();

    void Build(const std::vector<PictureLayerImpl*>& active_layers,
        const std::vector<PictureLayerImpl*>& pending_layers,
        TreePriority tree_priority);

    bool IsEmpty() const;
    const PrioritizedTile& Top() const;
    void Pop();

private:
    ScopedPtrVector<TilingSetEvictionQueue>& GetNextQueues();
    const ScopedPtrVector<TilingSetEvictionQueue>& GetNextQueues() const;

    ScopedPtrVector<TilingSetEvictionQueue> active_queues_;
    ScopedPtrVector<TilingSetEvictionQueue> pending_queues_;
    TreePriority tree_priority_;

    DISALLOW_COPY_AND_ASSIGN(EvictionTilePriorityQueue);
};

} // namespace cc

#endif // CC_TILES_EVICTION_TILE_PRIORITY_QUEUE_H_
