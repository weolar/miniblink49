// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_TILING_DATA_H_
#define CC_BASE_TILING_DATA_H_

#include <utility>

#include "base/basictypes.h"
#include "base/logging.h"
#include "cc/base/cc_export.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace gfx {
class Vector2d;
}

namespace cc {

class CC_EXPORT TilingData {
public:
    TilingData();
    TilingData(const gfx::Size& max_texture_size,
        const gfx::Size& tiling_size,
        bool has_border_texels);
    TilingData(const gfx::Size& max_texture_size,
        const gfx::Size& tiling_size,
        int border_texels);

    gfx::Size tiling_size() const { return tiling_size_; }
    void SetTilingSize(const gfx::Size& tiling_size);

    gfx::Size max_texture_size() const { return max_texture_size_; }
    void SetMaxTextureSize(const gfx::Size& max_texture_size);

    int border_texels() const { return border_texels_; }
    void SetHasBorderTexels(bool has_border_texels);
    void SetBorderTexels(int border_texels);

    bool has_empty_bounds() const { return !num_tiles_x_ || !num_tiles_y_; }
    int num_tiles_x() const { return num_tiles_x_; }
    int num_tiles_y() const { return num_tiles_y_; }
    // Return the tile index whose non-border texels include src_position.
    int TileXIndexFromSrcCoord(int src_position) const;
    int TileYIndexFromSrcCoord(int src_position) const;
    // Return the lowest tile index whose border texels include src_position.
    int FirstBorderTileXIndexFromSrcCoord(int src_position) const;
    int FirstBorderTileYIndexFromSrcCoord(int src_position) const;
    // Return the highest tile index whose border texels include src_position.
    int LastBorderTileXIndexFromSrcCoord(int src_position) const;
    int LastBorderTileYIndexFromSrcCoord(int src_position) const;

    gfx::Rect ExpandRectIgnoringBordersToTileBounds(const gfx::Rect& rect) const;
    gfx::Rect ExpandRectToTileBounds(const gfx::Rect& rect) const;

    gfx::Rect TileBounds(int i, int j) const;
    gfx::Rect TileBoundsWithBorder(int i, int j) const;
    int TilePositionX(int x_index) const;
    int TilePositionY(int y_index) const;
    int TileSizeX(int x_index) const;
    int TileSizeY(int y_index) const;

    // Difference between TileBound's and TileBoundWithBorder's origin().
    gfx::Vector2d TextureOffset(int x_index, int y_index) const;

    class CC_EXPORT BaseIterator {
    public:
        operator bool() const { return index_x_ != -1 && index_y_ != -1; }

        int index_x() const { return index_x_; }
        int index_y() const { return index_y_; }
        std::pair<int, int> index() const
        {
            return std::make_pair(index_x_, index_y_);
        }

    protected:
        BaseIterator();
        void done()
        {
            index_x_ = -1;
            index_y_ = -1;
        }

        int index_x_;
        int index_y_;
    };

    // Iterate through tiles whose bounds + optional border intersect with |rect|.
    class CC_EXPORT Iterator : public BaseIterator {
    public:
        Iterator();
        Iterator(const TilingData* tiling_data,
            const gfx::Rect& consider_rect,
            bool include_borders);
        Iterator& operator++();

    private:
        int left_;
        int right_;
        int bottom_;
    };

    class CC_EXPORT BaseDifferenceIterator : public BaseIterator {
    protected:
        BaseDifferenceIterator();
        BaseDifferenceIterator(const TilingData* tiling_data,
            const gfx::Rect& consider_rect,
            const gfx::Rect& ignore_rect);

        bool HasConsiderRect() const;
        bool in_consider_rect() const
        {
            return index_x_ >= consider_left_ && index_x_ <= consider_right_ && index_y_ >= consider_top_ && index_y_ <= consider_bottom_;
        }
        bool in_ignore_rect() const
        {
            return index_x_ >= ignore_left_ && index_x_ <= ignore_right_ && index_y_ >= ignore_top_ && index_y_ <= ignore_bottom_;
        }

        int consider_left_;
        int consider_top_;
        int consider_right_;
        int consider_bottom_;
        int ignore_left_;
        int ignore_top_;
        int ignore_right_;
        int ignore_bottom_;
    };

    // Iterate through all indices whose bounds (not including borders) intersect
    // with |consider| but which also do not intersect with |ignore|.
    class CC_EXPORT DifferenceIterator : public BaseDifferenceIterator {
    public:
        DifferenceIterator();
        DifferenceIterator(const TilingData* tiling_data,
            const gfx::Rect& consider_rect,
            const gfx::Rect& ignore_rect);
        DifferenceIterator& operator++();
    };

    // Iterate through all indices whose bounds + border intersect with
    // |consider| but which also do not intersect with |ignore|. The iterator
    // order is a counterclockwise spiral around the given center.
    class CC_EXPORT SpiralDifferenceIterator : public BaseDifferenceIterator {
    public:
        SpiralDifferenceIterator();
        SpiralDifferenceIterator(const TilingData* tiling_data,
            const gfx::Rect& consider_rect,
            const gfx::Rect& ignore_rect,
            const gfx::Rect& center_rect);
        SpiralDifferenceIterator& operator++();

    private:
        bool valid_column() const
        {
            return index_x_ >= consider_left_ && index_x_ <= consider_right_;
        }
        bool valid_row() const
        {
            return index_y_ >= consider_top_ && index_y_ <= consider_bottom_;
        }

        int current_step_count() const
        {
            return (direction_ == UP || direction_ == DOWN) ? vertical_step_count_
                                                            : horizontal_step_count_;
        }

        bool needs_direction_switch() const;
        void switch_direction();

        enum Direction { UP,
            LEFT,
            DOWN,
            RIGHT };

        Direction direction_;
        int delta_x_;
        int delta_y_;
        int current_step_;
        int horizontal_step_count_;
        int vertical_step_count_;
    };

    class CC_EXPORT ReverseSpiralDifferenceIterator
        : public BaseDifferenceIterator {
    public:
        ReverseSpiralDifferenceIterator();
        ReverseSpiralDifferenceIterator(const TilingData* tiling_data,
            const gfx::Rect& consider_rect,
            const gfx::Rect& ignore_rect,
            const gfx::Rect& center_rect);
        ReverseSpiralDifferenceIterator& operator++();

    private:
        bool in_around_rect() const
        {
            return index_x_ >= around_left_ && index_x_ <= around_right_ && index_y_ >= around_top_ && index_y_ <= around_bottom_;
        }
        bool valid_column() const
        {
            return index_x_ >= consider_left_ && index_x_ <= consider_right_;
        }
        bool valid_row() const
        {
            return index_y_ >= consider_top_ && index_y_ <= consider_bottom_;
        }

        int current_step_count() const
        {
            return (direction_ == UP || direction_ == DOWN) ? vertical_step_count_
                                                            : horizontal_step_count_;
        }

        bool needs_direction_switch() const;
        void switch_direction();

        int around_left_;
        int around_top_;
        int around_right_;
        int around_bottom_;

        enum Direction { LEFT,
            UP,
            RIGHT,
            DOWN };

        Direction direction_;
        int delta_x_;
        int delta_y_;
        int current_step_;
        int horizontal_step_count_;
        int vertical_step_count_;
    };

private:
    void AssertTile(int i, int j) const
    {
        DCHECK_GE(i, 0);
        DCHECK_LT(i, num_tiles_x_);
        DCHECK_GE(j, 0);
        DCHECK_LT(j, num_tiles_y_);
    }

    void RecomputeNumTiles();

    gfx::Size max_texture_size_;
    gfx::Size tiling_size_;
    int border_texels_;

    // These are computed values.
    int num_tiles_x_;
    int num_tiles_y_;
};

} // namespace cc

#endif // CC_BASE_TILING_DATA_H_
