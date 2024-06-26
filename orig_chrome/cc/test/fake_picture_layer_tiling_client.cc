// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_picture_layer_tiling_client.h"

#include <limits>

#include "base/thread_task_runner_handle.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_tile_manager.h"

namespace cc {

FakePictureLayerTilingClient::FakePictureLayerTilingClient()
    : tile_manager_(new FakeTileManager(&tile_manager_client_))
    , raster_source_(FakeDisplayListRasterSource::CreateInfiniteFilled())
    , twin_set_(nullptr)
    , twin_tiling_(nullptr)
    , has_valid_tile_priorities_(true)
{
}

FakePictureLayerTilingClient::FakePictureLayerTilingClient(
    ResourceProvider* resource_provider)
    : resource_pool_(
        ResourcePool::Create(resource_provider,
            base::ThreadTaskRunnerHandle::Get().get(),
            GL_TEXTURE_2D))
    , tile_manager_(
          new FakeTileManager(&tile_manager_client_, resource_pool_.get()))
    , raster_source_(FakeDisplayListRasterSource::CreateInfiniteFilled())
    , twin_set_(nullptr)
    , twin_tiling_(nullptr)
    , has_valid_tile_priorities_(true)
{
}

FakePictureLayerTilingClient::~FakePictureLayerTilingClient()
{
}

ScopedTilePtr FakePictureLayerTilingClient::CreateTile(
    const Tile::CreateInfo& info)
{
    return tile_manager_->CreateTile(info, 0, 0, 0);
}

void FakePictureLayerTilingClient::SetTileSize(const gfx::Size& tile_size)
{
    tile_size_ = tile_size;
}

gfx::Size FakePictureLayerTilingClient::CalculateTileSize(
    const gfx::Size& /* content_bounds */) const
{
    return tile_size_;
}

bool FakePictureLayerTilingClient::HasValidTilePriorities() const
{
    return has_valid_tile_priorities_;
}

const Region* FakePictureLayerTilingClient::GetPendingInvalidation()
{
    return &invalidation_;
}

const PictureLayerTiling*
FakePictureLayerTilingClient::GetPendingOrActiveTwinTiling(
    const PictureLayerTiling* tiling) const
{
    if (!twin_set_)
        return twin_tiling_;
    for (size_t i = 0; i < twin_set_->num_tilings(); ++i) {
        if (twin_set_->tiling_at(i)->contents_scale() == tiling->contents_scale())
            return twin_set_->tiling_at(i);
    }
    return nullptr;
}

bool FakePictureLayerTilingClient::RequiresHighResToDraw() const
{
    return false;
}

} // namespace cc
