// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_PICTURE_PILE_IMPL_H_
#define CC_TEST_FAKE_PICTURE_PILE_IMPL_H_

#include "base/memory/ref_counted.h"
#include "cc/playback/picture_pile_impl.h"
#include "cc/test/fake_picture_pile.h"

namespace base {
class WaitableEvent;
}

namespace cc {

class FakePicturePileImpl : public PicturePileImpl {
public:
    static scoped_refptr<FakePicturePileImpl> CreatePile(
        const gfx::Size& tile_size,
        const gfx::Size& layer_bounds,
        bool is_filled);

    static scoped_refptr<FakePicturePileImpl> CreateFilledPileWithDefaultTileSize(
        const gfx::Size& layer_bounds)
    {
        return CreateFilledPile(gfx::Size(512, 512), layer_bounds);
    }
    static scoped_refptr<FakePicturePileImpl> CreateEmptyPileWithDefaultTileSize(
        const gfx::Size& layer_bounds)
    {
        return CreateEmptyPile(gfx::Size(512, 512), layer_bounds);
    }
    static scoped_refptr<FakePicturePileImpl> CreateFilledPile(
        const gfx::Size& tile_size,
        const gfx::Size& layer_bounds);
    static scoped_refptr<FakePicturePileImpl> CreateEmptyPile(
        const gfx::Size& tile_size,
        const gfx::Size& layer_bounds);
    static scoped_refptr<FakePicturePileImpl>
    CreateEmptyPileThatThinksItHasRecordings(const gfx::Size& tile_size,
        const gfx::Size& layer_bounds,
        bool is_solid_color);
    static scoped_refptr<FakePicturePileImpl> CreateInfiniteFilledPile();
    static scoped_refptr<FakePicturePileImpl> CreateFromPile(
        const PicturePile* other,
        base::WaitableEvent* playback_allowed_event);

    // Hi-jack the PlaybackToCanvas method to delay its completion.
    void PlaybackToCanvas(SkCanvas* canvas,
        const gfx::Rect& canvas_bitmap_rect,
        const gfx::Rect& canvas_playback_rect,
        float contents_scale) const override;

    const TilingData& tiling() { return tiling_; }

    bool HasRecordingAt(int x, int y) const;
    int num_tiles_x() const { return tiling_.num_tiles_x(); }
    int num_tiles_y() const { return tiling_.num_tiles_y(); }

protected:
    FakePicturePileImpl();
    FakePicturePileImpl(const PicturePile* other,
        base::WaitableEvent* playback_allowed_event);
    ~FakePicturePileImpl() override;

    base::WaitableEvent* playback_allowed_event_;
    gfx::Size tile_grid_size_;
};

} // namespace cc

#endif // CC_TEST_FAKE_PICTURE_PILE_IMPL_H_
