// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_picture_pile.h"

#include <utility>

#include "cc/test/fake_picture_pile_impl.h"
#include "cc/trees/layer_tree_settings.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

namespace {

    scoped_ptr<FakePicturePile> CreatePile(const gfx::Size& tile_size,
        const gfx::Size& layer_bounds,
        bool is_filled)
    {
        scoped_ptr<FakePicturePile> pile(
            new FakePicturePile(LayerTreeSettings().minimum_contents_scale,
                LayerTreeSettings().default_tile_grid_size));
        pile->tiling().SetBorderTexels(0);
        pile->tiling().SetTilingSize(layer_bounds);
        pile->tiling().SetMaxTextureSize(tile_size);
        pile->SetRecordedViewport(is_filled ? gfx::Rect(layer_bounds) : gfx::Rect());
        pile->SetHasAnyRecordings(is_filled);
        if (is_filled) {
            for (int x = 0; x < pile->tiling().num_tiles_x(); ++x) {
                for (int y = 0; y < pile->tiling().num_tiles_y(); ++y)
                    pile->AddRecordingAt(x, y);
            }
        }
        return pile;
    }

} // namespace

scoped_ptr<FakePicturePile> FakePicturePile::CreateFilledPile(
    const gfx::Size& tile_size,
    const gfx::Size& layer_bounds)
{
    bool is_filled = true;
    return CreatePile(tile_size, layer_bounds, is_filled);
}

scoped_ptr<FakePicturePile> FakePicturePile::CreateEmptyPile(
    const gfx::Size& tile_size,
    const gfx::Size& layer_bounds)
{
    bool is_filled = false;
    return CreatePile(tile_size, layer_bounds, is_filled);
}

scoped_refptr<RasterSource> FakePicturePile::CreateRasterSource(
    bool can_use_lcd_text) const
{
    return FakePicturePileImpl::CreateFromPile(this, playback_allowed_event_);
}

void FakePicturePile::AddRecordingAt(int x, int y)
{
    EXPECT_GE(x, 0);
    EXPECT_GE(y, 0);
    EXPECT_LT(x, tiling_.num_tiles_x());
    EXPECT_LT(y, tiling_.num_tiles_y());

    if (HasRecordingAt(x, y))
        return;
    gfx::Rect bounds(tiling().TileBounds(x, y));
    bounds.Inset(-buffer_pixels(), -buffer_pixels());

    scoped_refptr<Picture> picture(
        Picture::Create(bounds, &client_, tile_grid_size_, gather_pixel_refs_,
            RecordingSource::RECORD_NORMALLY));
    picture_map_[std::pair<int, int>(x, y)] = picture;
    EXPECT_TRUE(HasRecordingAt(x, y));

    has_any_recordings_ = true;
}

void FakePicturePile::RemoveRecordingAt(int x, int y)
{
    EXPECT_GE(x, 0);
    EXPECT_GE(y, 0);
    EXPECT_LT(x, tiling_.num_tiles_x());
    EXPECT_LT(y, tiling_.num_tiles_y());

    if (!HasRecordingAt(x, y))
        return;
    picture_map_.erase(std::pair<int, int>(x, y));
    EXPECT_FALSE(HasRecordingAt(x, y));
}

bool FakePicturePile::HasRecordingAt(int x, int y) const
{
    PictureMap::const_iterator found = picture_map_.find(PictureMapKey(x, y));
    return found != picture_map_.end();
}

void FakePicturePile::Rerecord()
{
    for (int y = 0; y < num_tiles_y(); ++y) {
        for (int x = 0; x < num_tiles_x(); ++x) {
            RemoveRecordingAt(x, y);
            AddRecordingAt(x, y);
        }
    }
}

} // namespace cc
