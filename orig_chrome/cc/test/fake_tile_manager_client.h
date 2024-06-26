// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_TILE_MANAGER_CLIENT_H_
#define CC_TEST_FAKE_TILE_MANAGER_CLIENT_H_

#include <vector>

#include "cc/tiles/tile_manager.h"

namespace cc {

class FakeTileManagerClient : public TileManagerClient {
public:
    FakeTileManagerClient();
    ~FakeTileManagerClient() override;

    // TileManagerClient implementation.
    void NotifyReadyToActivate() override { }
    void NotifyReadyToDraw() override { }
    void NotifyAllTileTasksCompleted() override { }
    void NotifyTileStateChanged(const Tile* tile) override { }
    scoped_ptr<RasterTilePriorityQueue> BuildRasterQueue(
        TreePriority tree_priority,
        RasterTilePriorityQueue::Type type) override;
    scoped_ptr<EvictionTilePriorityQueue> BuildEvictionQueue(
        TreePriority tree_priority) override;
    void SetIsLikelyToRequireADraw(bool is_likely_to_require_a_draw) override { }
};

} // namespace cc

#endif // CC_TEST_FAKE_TILE_MANAGER_CLIENT_H_
