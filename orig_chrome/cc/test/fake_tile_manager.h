// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_TILE_MANAGER_H_
#define CC_TEST_FAKE_TILE_MANAGER_H_

#include <set>
#include <vector>

#include "cc/tiles/tile_manager.h"

namespace cc {

class FakeTileManager : public TileManager {
public:
    explicit FakeTileManager(TileManagerClient* client);
    FakeTileManager(TileManagerClient* client, ResourcePool* resource_pool);
    ~FakeTileManager() override;

    bool HasBeenAssignedMemory(Tile* tile);
    void AssignMemoryToTiles(
        const GlobalStateThatImpactsTilePriority& state);

    void Release(Tile* tile) override;

    std::vector<Tile*> tiles_for_raster;
};

} // namespace cc

#endif // CC_TEST_FAKE_TILE_MANAGER_H_
