// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_picture_pile_impl.h"

#include <algorithm>
#include <limits>
#include <utility>

#include "base/synchronization/waitable_event.h"
#include "cc/playback/picture_pile.h"
#include "cc/trees/layer_tree_settings.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

FakePicturePileImpl::FakePicturePileImpl()
    : playback_allowed_event_(nullptr)
{
}

FakePicturePileImpl::FakePicturePileImpl(
    const PicturePile* other,
    base::WaitableEvent* playback_allowed_event)
    : PicturePileImpl(other, true)
    , playback_allowed_event_(playback_allowed_event)
    , tile_grid_size_(other->GetTileGridSizeForTesting())
{
}

FakePicturePileImpl::~FakePicturePileImpl() { }

scoped_refptr<FakePicturePileImpl> FakePicturePileImpl::CreatePile(
    const gfx::Size& tile_size,
    const gfx::Size& layer_bounds,
    bool is_filled)
{
    FakePicturePile pile(LayerTreeSettings().minimum_contents_scale,
        LayerTreeSettings().default_tile_grid_size);
    pile.tiling().SetBorderTexels(0);
    pile.tiling().SetTilingSize(layer_bounds);
    pile.tiling().SetMaxTextureSize(tile_size);
    pile.SetRecordedViewport(is_filled ? gfx::Rect(layer_bounds) : gfx::Rect());
    pile.SetHasAnyRecordings(is_filled);
    if (is_filled) {
        for (int x = 0; x < pile.tiling().num_tiles_x(); ++x) {
            for (int y = 0; y < pile.tiling().num_tiles_y(); ++y)
                pile.AddRecordingAt(x, y);
        }
    }
    scoped_refptr<FakePicturePileImpl> pile_impl(
        new FakePicturePileImpl(&pile, nullptr));
    return pile_impl;
}

scoped_refptr<FakePicturePileImpl> FakePicturePileImpl::CreateFilledPile(
    const gfx::Size& tile_size,
    const gfx::Size& layer_bounds)
{
    bool is_filled = true;
    return CreatePile(tile_size, layer_bounds, is_filled);
}

scoped_refptr<FakePicturePileImpl> FakePicturePileImpl::CreateEmptyPile(
    const gfx::Size& tile_size,
    const gfx::Size& layer_bounds)
{
    bool is_filled = false;
    return CreatePile(tile_size, layer_bounds, is_filled);
}

scoped_refptr<FakePicturePileImpl>
FakePicturePileImpl::CreateEmptyPileThatThinksItHasRecordings(
    const gfx::Size& tile_size,
    const gfx::Size& layer_bounds,
    bool is_solid_color)
{
    FakePicturePile pile(LayerTreeSettings().minimum_contents_scale,
        LayerTreeSettings().default_tile_grid_size);
    pile.tiling().SetBorderTexels(0);
    pile.tiling().SetTilingSize(layer_bounds);
    pile.tiling().SetMaxTextureSize(tile_size);
    // This simulates a false positive for this flag.
    pile.SetRecordedViewport(gfx::Rect());
    pile.SetHasAnyRecordings(true);
    pile.SetIsSolidColor(is_solid_color);
    return make_scoped_refptr(new FakePicturePileImpl(&pile, nullptr));
}

scoped_refptr<FakePicturePileImpl>
FakePicturePileImpl::CreateInfiniteFilledPile()
{
    gfx::Size size(std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max());
    FakePicturePile pile(LayerTreeSettings().minimum_contents_scale, size);
    pile.tiling().SetBorderTexels(0);
    pile.tiling().SetTilingSize(size);
    pile.tiling().SetMaxTextureSize(size);
    pile.SetRecordedViewport(gfx::Rect(size));
    pile.SetHasAnyRecordings(true);
    pile.AddRecordingAt(0, 0);
    scoped_refptr<FakePicturePileImpl> pile_impl(
        new FakePicturePileImpl(&pile, nullptr));
    return pile_impl;
}

scoped_refptr<FakePicturePileImpl> FakePicturePileImpl::CreateFromPile(
    const PicturePile* other,
    base::WaitableEvent* playback_allowed_event)
{
    return make_scoped_refptr(
        new FakePicturePileImpl(other, playback_allowed_event));
}

void FakePicturePileImpl::PlaybackToCanvas(
    SkCanvas* canvas,
    const gfx::Rect& canvas_bitmap_rect,
    const gfx::Rect& canvas_playback_rect,
    float contents_scale) const
{
    if (playback_allowed_event_)
        playback_allowed_event_->Wait();
    PicturePileImpl::PlaybackToCanvas(canvas, canvas_bitmap_rect,
        canvas_playback_rect, contents_scale);
}

bool FakePicturePileImpl::HasRecordingAt(int x, int y) const
{
    PictureMap::const_iterator found = picture_map_.find(PictureMapKey(x, y));
    return found != picture_map_.end();
}

} // namespace cc
