// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <utility>

#include "cc/playback/picture_pile.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/fake_picture_pile.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {
namespace {

    class PicturePileTestBase {
    public:
        PicturePileTestBase()
            : min_scale_(0.125)
            , pile_(min_scale_, gfx::Size(1000, 1000))
            , frame_number_(0)
        {
        }

        void InitializeData()
        {
            pile_.SetTileGridSize(gfx::Size(1000, 1000));
            pile_.SetMinContentsScale(min_scale_);
            client_ = FakeContentLayerClient();
            SetTilingSize(pile_.tiling().max_texture_size());
        }

        void SetTilingSize(const gfx::Size& tiling_size)
        {
            Region invalidation;
            gfx::Rect viewport_rect(tiling_size);
            UpdateAndExpandInvalidation(&invalidation, tiling_size, viewport_rect);
        }

        gfx::Size tiling_size() const { return pile_.GetSize(); }
        gfx::Rect tiling_rect() const { return gfx::Rect(pile_.GetSize()); }

        bool UpdateAndExpandInvalidation(Region* invalidation,
            const gfx::Size& layer_size,
            const gfx::Rect& visible_layer_rect)
        {
            frame_number_++;
            return pile_.UpdateAndExpandInvalidation(&client_, invalidation, layer_size,
                visible_layer_rect, frame_number_,
                RecordingSource::RECORD_NORMALLY);
        }

        bool UpdateWholePile()
        {
            Region invalidation = tiling_rect();
            bool result = UpdateAndExpandInvalidation(&invalidation, tiling_size(),
                tiling_rect());
            EXPECT_EQ(tiling_rect().ToString(), invalidation.ToString());
            return result;
        }

        FakeContentLayerClient client_;
        float min_scale_;
        FakePicturePile pile_;
        int frame_number_;
    };

    class PicturePileTest : public PicturePileTestBase, public testing::Test {
    public:
        void SetUp() override { InitializeData(); }
    };

    TEST_F(PicturePileTest, InvalidationOnTileBorderOutsideInterestRect)
    {
        // Don't expand the interest rect past what we invalidate.
        pile_.SetPixelRecordDistance(0);

        gfx::Size tile_size(100, 100);
        pile_.tiling().SetMaxTextureSize(tile_size);

        gfx::Size pile_size(400, 400);
        SetTilingSize(pile_size);

        // We have multiple tiles.
        EXPECT_GT(pile_.tiling().num_tiles_x(), 2);
        EXPECT_GT(pile_.tiling().num_tiles_y(), 2);

        // Record everything.
        Region invalidation(tiling_rect());
        UpdateAndExpandInvalidation(&invalidation, tiling_size(), tiling_rect());

        // +----------+-----------------+-----------+
        // |          |     VVVV     1,0|           |
        // |          |     VVVV        |           |
        // |          |     VVVV        |           |
        // |       ...|.................|...        |
        // |       ...|.................|...        |
        // +----------+-----------------+-----------+
        // |       ...|                 |...        |
        // |       ...|                 |...        |
        // |       ...|                 |...        |
        // |       ...|                 |...        |
        // |       ...|              1,1|...        |
        // +----------+-----------------+-----------+
        // |       ...|.................|...        |
        // |       ...|.................|...        |
        // +----------+-----------------+-----------+
        //
        // .. = border pixels for tile 1,1
        // VV = interest rect (what we will record)
        //
        // The first invalidation is inside VV, so it does not touch border pixels of
        // tile 1,1.
        //
        // The second invalidation goes below VV into the .. border pixels of 1,1.

        // This is the VV interest rect which will be entirely inside 1,0 and not
        // touch the border of 1,1.
        gfx::Rect interest_rect(
            pile_.tiling().TilePositionX(1) + pile_.tiling().border_texels(),
            0,
            10,
            pile_.tiling().TileSizeY(0) - pile_.tiling().border_texels());

        // Invalidate tile 1,0 only. This is a rect that avoids the borders of any
        // other tiles.
        gfx::Rect invalidate_tile = interest_rect;
        // This should cause the tile 1,0 to be invalidated and re-recorded. The
        // invalidation did not need to be expanded.
        invalidation = invalidate_tile;
        UpdateAndExpandInvalidation(&invalidation, tiling_size(), interest_rect);
        EXPECT_EQ(invalidate_tile, invalidation);

        // Invalidate tile 1,0 and 1,1 by invalidating something that only touches the
        // border of 1,1 (and is inside the tile bounds of 1,0). This is a 10px wide
        // strip from the top of the tiling onto the border pixels of tile 1,1 that
        // avoids border pixels of any other tiles.
        gfx::Rect invalidate_border = interest_rect;
        invalidate_border.Inset(0, 0, 0, -1);
        // This should cause the tile 1,0 and 1,1 to be invalidated. The 1,1 tile will
        // not be re-recorded since it does not touch the interest rect, so the
        // invalidation should be expanded to cover all of 1,1.
        invalidation = invalidate_border;
        UpdateAndExpandInvalidation(&invalidation, tiling_size(), interest_rect);
        Region expected_invalidation = invalidate_border;
        expected_invalidation.Union(pile_.tiling().TileBounds(1, 1));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
    }

    TEST_F(PicturePileTest, SmallInvalidateInflated)
    {
        // Invalidate something inside a tile.
        Region invalidate_rect(gfx::Rect(50, 50, 1, 1));
        UpdateAndExpandInvalidation(&invalidate_rect, tiling_size(), tiling_rect());
        EXPECT_EQ(gfx::Rect(50, 50, 1, 1).ToString(), invalidate_rect.ToString());

        EXPECT_EQ(1, pile_.tiling().num_tiles_x());
        EXPECT_EQ(1, pile_.tiling().num_tiles_y());

        PicturePile::PictureMapKey key = FakePicturePile::PictureMapKey(0, 0);
        PicturePile::PictureMap::iterator it = pile_.picture_map().find(key);
        EXPECT_TRUE(it != pile_.picture_map().end());
        const Picture* picture = it->second.get();
        EXPECT_TRUE(picture);

        gfx::Rect picture_rect = gfx::ScaleToEnclosedRect(picture->LayerRect(), min_scale_);

        // The the picture should be large enough that scaling it never makes a rect
        // smaller than 1 px wide or tall.
        EXPECT_FALSE(picture_rect.IsEmpty()) << "Picture rect "
                                             << picture_rect.ToString();
    }

    TEST_F(PicturePileTest, LargeInvalidateInflated)
    {
        // Invalidate something inside a tile.
        Region invalidate_rect(gfx::Rect(50, 50, 100, 100));
        UpdateAndExpandInvalidation(&invalidate_rect, tiling_size(), tiling_rect());
        EXPECT_EQ(gfx::Rect(50, 50, 100, 100).ToString(), invalidate_rect.ToString());

        EXPECT_EQ(1, pile_.tiling().num_tiles_x());
        EXPECT_EQ(1, pile_.tiling().num_tiles_y());

        PicturePile::PictureMapKey key = FakePicturePile::PictureMapKey(0, 0);
        PicturePile::PictureMap::iterator it = pile_.picture_map().find(key);
        EXPECT_TRUE(it != pile_.picture_map().end());
        const Picture* picture = it->second.get();
        EXPECT_TRUE(picture);

        int expected_inflation = pile_.buffer_pixels();

        gfx::Rect base_picture_rect(tiling_size());
        base_picture_rect.Inset(-expected_inflation, -expected_inflation);
        EXPECT_EQ(base_picture_rect.ToString(), picture->LayerRect().ToString());
    }

    TEST_F(PicturePileTest, ClearingInvalidatesRecordedRect)
    {
        gfx::Rect rect(0, 0, 5, 5);
        EXPECT_TRUE(pile_.CanRasterLayerRect(rect));
        EXPECT_TRUE(pile_.CanRasterSlowTileCheck(rect));

        pile_.Clear();

        // Make sure both the cache-aware check (using recorded region) and the normal
        // check are both false after clearing.
        EXPECT_FALSE(pile_.CanRasterLayerRect(rect));
        EXPECT_FALSE(pile_.CanRasterSlowTileCheck(rect));
    }

    TEST_F(PicturePileTest, NoInvalidationValidViewport)
    {
        // This test validates that the recorded_viewport cache of full tiles
        // is still valid for some use cases.  If it's not, it's a performance
        // issue because CanRaster checks will go down the slow path.
        EXPECT_TRUE(!pile_.recorded_viewport().IsEmpty());

        // No invalidation, same viewport.
        Region invalidation;
        UpdateAndExpandInvalidation(&invalidation, tiling_size(), tiling_rect());
        EXPECT_TRUE(!pile_.recorded_viewport().IsEmpty());
        EXPECT_EQ(Region().ToString(), invalidation.ToString());

        // Partial invalidation, same viewport.
        invalidation = gfx::Rect(0, 0, 1, 1);
        UpdateAndExpandInvalidation(&invalidation, tiling_size(), tiling_rect());
        EXPECT_TRUE(!pile_.recorded_viewport().IsEmpty());
        EXPECT_EQ(gfx::Rect(0, 0, 1, 1).ToString(), invalidation.ToString());

        // No invalidation, changing viewport.
        invalidation = Region();
        UpdateAndExpandInvalidation(&invalidation, tiling_size(),
            gfx::Rect(5, 5, 5, 5));
        EXPECT_TRUE(!pile_.recorded_viewport().IsEmpty());
        EXPECT_EQ(Region().ToString(), invalidation.ToString());
    }

    TEST_F(PicturePileTest, BigFullLayerInvalidation)
    {
        gfx::Size huge_layer_size(100000000, 100000000);
        gfx::Rect viewport(300000, 400000, 5000, 6000);

        // Resize the pile.
        Region invalidation;
        UpdateAndExpandInvalidation(&invalidation, huge_layer_size, viewport);

        // Invalidating a huge layer should be fast.
        base::TimeTicks start = base::TimeTicks::Now();
        invalidation = gfx::Rect(huge_layer_size);
        UpdateAndExpandInvalidation(&invalidation, huge_layer_size, viewport);
        base::TimeTicks end = base::TimeTicks::Now();
        base::TimeDelta length = end - start;
        // This is verrrry generous to avoid flake.
        EXPECT_LT(length.InSeconds(), 5);
    }

    TEST_F(PicturePileTest, BigFullLayerInvalidationWithResizeGrow)
    {
        gfx::Size huge_layer_size(100000000, 100000000);
        gfx::Rect viewport(300000, 400000, 5000, 6000);

        // Resize the pile.
        Region invalidation;
        UpdateAndExpandInvalidation(&invalidation, huge_layer_size, viewport);

        // Resize the pile even larger, while invalidating everything in the old size.
        // Invalidating the whole thing should be fast.
        base::TimeTicks start = base::TimeTicks::Now();
        gfx::Size bigger_layer_size(huge_layer_size.width() * 2,
            huge_layer_size.height() * 2);
        invalidation = gfx::Rect(huge_layer_size);
        UpdateAndExpandInvalidation(&invalidation, bigger_layer_size, viewport);
        base::TimeTicks end = base::TimeTicks::Now();
        base::TimeDelta length = end - start;
        // This is verrrry generous to avoid flake.
        EXPECT_LT(length.InSeconds(), 5);
    }

    TEST_F(PicturePileTest, BigFullLayerInvalidationWithResizeShrink)
    {
        gfx::Size huge_layer_size(100000000, 100000000);
        gfx::Rect viewport(300000, 400000, 5000, 6000);

        // Resize the pile.
        Region invalidation;
        UpdateAndExpandInvalidation(&invalidation, huge_layer_size, viewport);

        // Resize the pile smaller, while invalidating everything in the new size.
        // Invalidating the whole thing should be fast.
        base::TimeTicks start = base::TimeTicks::Now();
        gfx::Size smaller_layer_size(huge_layer_size.width() - 1000,
            huge_layer_size.height() - 1000);
        invalidation = gfx::Rect(smaller_layer_size);
        UpdateAndExpandInvalidation(&invalidation, smaller_layer_size, viewport);
        base::TimeTicks end = base::TimeTicks::Now();
        base::TimeDelta length = end - start;
        // This is verrrry generous to avoid flake.
        EXPECT_LT(length.InSeconds(), 5);
    }

    TEST_F(PicturePileTest, InvalidationOutsideRecordingRect)
    {
        gfx::Size huge_layer_size(10000000, 20000000);
        gfx::Rect viewport(300000, 400000, 5000, 6000);

        // Resize the pile and set up the interest rect.
        Region invalidation;
        UpdateAndExpandInvalidation(&invalidation, huge_layer_size, viewport);

        // Invalidation inside the recording rect does not need to be expanded.
        invalidation = viewport;
        UpdateAndExpandInvalidation(&invalidation, huge_layer_size, viewport);
        EXPECT_EQ(viewport.ToString(), invalidation.ToString());

        // Invalidation outside the recording rect should expand to the tiles it
        // covers.
        gfx::Rect recorded_over_tiles = pile_.tiling().ExpandRectToTileBounds(pile_.recorded_viewport());
        gfx::Rect invalidation_outside(
            recorded_over_tiles.right(), recorded_over_tiles.y(), 30, 30);
        invalidation = invalidation_outside;
        UpdateAndExpandInvalidation(&invalidation, huge_layer_size, viewport);
        gfx::Rect expanded_recorded_viewport = pile_.tiling().ExpandRectToTileBounds(pile_.recorded_viewport());
        Region expected_invalidation = pile_.tiling().ExpandRectToTileBounds(invalidation_outside);
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
    }

    enum Corner {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
    };

    class PicturePileResizeCornerTest : public PicturePileTestBase,
                                        public testing::TestWithParam<Corner> {
    protected:
        void SetUp() override { InitializeData(); }

        static gfx::Rect CornerSinglePixelRect(Corner corner, const gfx::Size& s)
        {
            switch (corner) {
            case TOP_LEFT:
                return gfx::Rect(0, 0, 1, 1);
            case TOP_RIGHT:
                return gfx::Rect(s.width() - 1, 0, 1, 1);
            case BOTTOM_LEFT:
                return gfx::Rect(0, s.height() - 1, 1, 1);
            case BOTTOM_RIGHT:
                return gfx::Rect(s.width() - 1, s.height() - 1, 1, 1);
            }
            NOTREACHED();
            return gfx::Rect();
        }
    };

    TEST_P(PicturePileResizeCornerTest, ResizePileOutsideInterestRect)
    {
        Corner corner = GetParam();

        // This size chosen to be larger than the interest rect size, which is
        // at least kPixelDistanceToRecord * 2 in each dimension.
        int tile_size = 100000;
        // The small number subtracted keeps the last tile in each axis larger than
        // the interest rect also.
        int offset = -100;
        gfx::Size base_tiling_size(6 * tile_size + offset, 6 * tile_size + offset);
        gfx::Size grow_down_tiling_size(6 * tile_size + offset,
            8 * tile_size + offset);
        gfx::Size grow_right_tiling_size(8 * tile_size + offset,
            6 * tile_size + offset);
        gfx::Size grow_both_tiling_size(8 * tile_size + offset,
            8 * tile_size + offset);

        Region invalidation;
        Region expected_invalidation;

        pile_.tiling().SetMaxTextureSize(gfx::Size(tile_size, tile_size));
        SetTilingSize(base_tiling_size);

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        UpdateAndExpandInvalidation(
            &invalidation,
            grow_down_tiling_size,
            CornerSinglePixelRect(corner, grow_down_tiling_size));

        // We should have lost all of the recordings in the bottom row as none of them
        // are in the current interest rect (which is either the above or below it).
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(8, pile_.tiling().num_tiles_y());
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_EQ(j < 5, it != map.end() && it->second.get());
            }
        }

        // We invalidated all new pixels in the recording.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_down_tiling_size),
            gfx::Rect(base_tiling_size));
        // But the new pixels don't cover the whole bottom row.
        gfx::Rect bottom_row = gfx::UnionRects(pile_.tiling().TileBounds(0, 5),
            pile_.tiling().TileBounds(5, 5));
        EXPECT_FALSE(expected_invalidation.Contains(bottom_row));
        // We invalidated the entire old bottom row.
        expected_invalidation.Union(bottom_row);
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation,
            base_tiling_size,
            CornerSinglePixelRect(corner, base_tiling_size));

        // When shrinking, we should have lost all the recordings in the bottom row
        // not touching the interest rect.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                bool expect_tile;
                switch (corner) {
                case TOP_LEFT:
                case TOP_RIGHT:
                    expect_tile = j < 5;
                    break;
                case BOTTOM_LEFT:
                    // The interest rect in the bottom left tile means we'll record it.
                    expect_tile = j < 5 || (j == 5 && i == 0);
                    break;
                case BOTTOM_RIGHT:
                    // The interest rect in the bottom right tile means we'll record it.
                    expect_tile = j < 5 || (j == 5 && i == 5);
                    break;
                }
                EXPECT_EQ(expect_tile, it != map.end() && it->second.get());
            }
        }

        // When shrinking, the previously exposed region is invalidated.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_down_tiling_size),
            gfx::Rect(base_tiling_size));
        // The whole bottom row of tiles (except any with the interest rect) are
        // dropped.
        gfx::Rect bottom_row_minus_existing_corner = gfx::UnionRects(
            pile_.tiling().TileBounds(0, 5), pile_.tiling().TileBounds(5, 5));
        switch (corner) {
        case TOP_LEFT:
        case TOP_RIGHT:
            // No tiles are kept in the changed region because it doesn't
            // intersect with the interest rect.
            break;
        case BOTTOM_LEFT:
            bottom_row_minus_existing_corner.Subtract(
                pile_.tiling().TileBounds(0, 5));
            break;
        case BOTTOM_RIGHT:
            bottom_row_minus_existing_corner.Subtract(
                pile_.tiling().TileBounds(5, 5));
            break;
        }

        expected_invalidation.Union(bottom_row_minus_existing_corner);
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(
            &invalidation,
            grow_right_tiling_size,
            CornerSinglePixelRect(corner, grow_right_tiling_size));

        // We should have lost all of the recordings in the right column as none of
        // them are in the current interest rect (which is either entirely left or
        // right of it).
        EXPECT_EQ(8, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_EQ(i < 5, it != map.end() && it->second.get());
            }
        }

        // We invalidated all new pixels in the recording.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_right_tiling_size),
            gfx::Rect(base_tiling_size));
        // But the new pixels don't cover the whole right_column.
        gfx::Rect right_column = gfx::UnionRects(pile_.tiling().TileBounds(5, 0),
            pile_.tiling().TileBounds(5, 5));
        EXPECT_FALSE(expected_invalidation.Contains(right_column));
        // We invalidated the entire old right column.
        expected_invalidation.Union(right_column);
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation,
            base_tiling_size,
            CornerSinglePixelRect(corner, base_tiling_size));

        // When shrinking, we should have lost all the recordings in the right column
        // not touching the interest rect.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                bool expect_tile;
                switch (corner) {
                case TOP_LEFT:
                case BOTTOM_LEFT:
                    // No tiles are kept in the changed region because it doesn't
                    // intersect with the interest rect.
                    expect_tile = i < 5;
                    break;
                case TOP_RIGHT:
                    // The interest rect in the top right tile means we'll record it.
                    expect_tile = i < 5 || (j == 0 && i == 5);
                    break;
                case BOTTOM_RIGHT:
                    // The interest rect in the bottom right tile means we'll record it.
                    expect_tile = i < 5 || (j == 5 && i == 5);
                    break;
                }
                EXPECT_EQ(expect_tile, it != map.end() && it->second.get());
            }
        }

        // When shrinking, the previously exposed region is invalidated.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_right_tiling_size),
            gfx::Rect(base_tiling_size));
        // The whole right column of tiles (except for ones with the interest rect)
        // are dropped.
        gfx::Rect right_column_minus_existing_corner = gfx::UnionRects(
            pile_.tiling().TileBounds(5, 0), pile_.tiling().TileBounds(5, 5));
        switch (corner) {
        case TOP_LEFT:
        case BOTTOM_LEFT:
            break;
        case TOP_RIGHT:
            right_column_minus_existing_corner.Subtract(
                pile_.tiling().TileBounds(5, 0));
            break;
        case BOTTOM_RIGHT:
            right_column_minus_existing_corner.Subtract(
                pile_.tiling().TileBounds(5, 5));
            break;
        }
        expected_invalidation.Union(right_column_minus_existing_corner);
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(
            &invalidation,
            grow_both_tiling_size,
            CornerSinglePixelRect(corner, grow_both_tiling_size));

        // We should have lost the recordings in the right column and bottom row.
        EXPECT_EQ(8, pile_.tiling().num_tiles_x());
        EXPECT_EQ(8, pile_.tiling().num_tiles_y());
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 6; ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_EQ(i < 5 && j < 5, it != map.end() && it->second.get());
            }
        }

        // We invalidated all new pixels in the recording.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_both_tiling_size),
            gfx::Rect(base_tiling_size));
        // But the new pixels don't cover the whole right column or bottom row.
        Region right_column_and_bottom_row = UnionRegions(gfx::UnionRects(pile_.tiling().TileBounds(5, 0),
                                                              pile_.tiling().TileBounds(5, 5)),
            gfx::UnionRects(pile_.tiling().TileBounds(0, 5),
                pile_.tiling().TileBounds(5, 5)));
        EXPECT_FALSE(expected_invalidation.Contains(right_column_and_bottom_row));
        // We invalidated the entire old right column and the old bottom row.
        expected_invalidation.Union(right_column_and_bottom_row);
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation, base_tiling_size,
            CornerSinglePixelRect(corner, base_tiling_size));

        // We should have lost the recordings in the right column and bottom row,
        // except where it intersects the interest rect.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                bool expect_tile;
                switch (corner) {
                case TOP_LEFT:
                    expect_tile = i < 5 && j < 5;
                    break;
                case TOP_RIGHT:
                    // The interest rect in the top right tile means we'll record it.
                    expect_tile = (i < 5 && j < 5) || (j == 0 && i == 5);
                    break;
                case BOTTOM_LEFT:
                    // The interest rect in the bottom left tile means we'll record it.
                    expect_tile = (i < 5 && j < 5) || (j == 5 && i == 0);
                    break;
                case BOTTOM_RIGHT:
                    // The interest rect in the bottom right tile means we'll record it.
                    expect_tile = (i < 5 && j < 5) || (j == 5 && i == 5);
                    break;
                }
                EXPECT_EQ(expect_tile, it != map.end() && it->second.get()) << i << ","
                                                                            << j;
            }
        }

        // We invalidated all previous pixels in the recording.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_both_tiling_size),
            gfx::Rect(base_tiling_size));
        // The whole right column and bottom row of tiles (except for ones with the
        // interest rect) are dropped.
        Region right_column_and_bottom_row_minus_existing_corner = right_column_and_bottom_row;
        switch (corner) {
        case TOP_LEFT:
            break;
        case BOTTOM_LEFT:
            right_column_and_bottom_row_minus_existing_corner.Subtract(
                pile_.tiling().TileBounds(0, 5));
            break;
        case TOP_RIGHT:
            right_column_and_bottom_row_minus_existing_corner.Subtract(
                pile_.tiling().TileBounds(5, 0));
            break;
        case BOTTOM_RIGHT:
            right_column_and_bottom_row_minus_existing_corner.Subtract(
                pile_.tiling().TileBounds(5, 5));
            break;
        }
        expected_invalidation.Union(
            right_column_and_bottom_row_minus_existing_corner);
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();
    }

    TEST_P(PicturePileResizeCornerTest, SmallResizePileOutsideInterestRect)
    {
        Corner corner = GetParam();

        // This size chosen to be larger than the interest rect size, which is
        // at least kPixelDistanceToRecord * 2 in each dimension.
        int tile_size = 100000;
        // The small number subtracted keeps the last tile in each axis larger than
        // the interest rect also.
        int offset = -100;
        gfx::Size base_tiling_size(6 * tile_size + offset, 6 * tile_size + offset);
        gfx::Size grow_down_tiling_size(6 * tile_size + offset,
            6 * tile_size + offset + 5);
        gfx::Size grow_right_tiling_size(6 * tile_size + offset + 5,
            6 * tile_size + offset);
        gfx::Size grow_both_tiling_size(6 * tile_size + offset + 5,
            6 * tile_size + offset + 5);

        Region invalidation;
        Region expected_invalidation;

        pile_.tiling().SetMaxTextureSize(gfx::Size(tile_size, tile_size));
        SetTilingSize(base_tiling_size);

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // In this test (unlike the large resize test), as all growing and shrinking
        // happens within tiles, the resulting invalidation is symmetrical, so use
        // this enum to repeat the test both ways.
        enum ChangeDirection { GROW,
            SHRINK,
            LAST_DIRECTION = SHRINK };

        // Grow downward.
        for (int dir = 0; dir <= LAST_DIRECTION; ++dir) {
            gfx::Size new_tiling_size = dir == GROW ? grow_down_tiling_size : base_tiling_size;
            UpdateWholePile();
            UpdateAndExpandInvalidation(&invalidation, new_tiling_size,
                CornerSinglePixelRect(corner, new_tiling_size));

            // We should have lost the recordings in the bottom row that do not
            // intersect the interest rect.
            EXPECT_EQ(6, pile_.tiling().num_tiles_x());
            EXPECT_EQ(6, pile_.tiling().num_tiles_y());
            for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
                for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                    FakePicturePile::PictureMapKey key(i, j);
                    FakePicturePile::PictureMap& map = pile_.picture_map();
                    FakePicturePile::PictureMap::iterator it = map.find(key);
                    bool expect_tile;
                    switch (corner) {
                    case TOP_LEFT:
                    case TOP_RIGHT:
                        expect_tile = j < 5;
                        break;
                    case BOTTOM_LEFT:
                        // The interest rect in the bottom left tile means we'll record it.
                        expect_tile = j < 5 || (j == 5 && i == 0);
                        break;
                    case BOTTOM_RIGHT:
                        // The interest rect in the bottom right tile means we'll record it.
                        expect_tile = j < 5 || (j == 5 && i == 5);
                        break;
                    }
                    EXPECT_EQ(expect_tile, it != map.end() && it->second.get());
                }
            }

            // We invalidated the bottom row outside the new interest rect. The tile
            // that insects the interest rect in invalidated only on its newly
            // exposed or previously exposed pixels.
            if (dir == GROW) {
                // Only calculate the expected invalidation while growing, as the tile
                // bounds post-growing is the newly exposed / previously exposed sizes.
                // Post-shrinking, the tile bounds are smaller, so can't be used.
                switch (corner) {
                case TOP_LEFT:
                case TOP_RIGHT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(0, 5), pile_.tiling().TileBounds(5, 5));
                    break;
                case BOTTOM_LEFT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(1, 5), pile_.tiling().TileBounds(5, 5));
                    expected_invalidation.Union(SubtractRects(
                        pile_.tiling().TileBounds(0, 5), gfx::Rect(base_tiling_size)));
                    break;
                case BOTTOM_RIGHT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(0, 5), pile_.tiling().TileBounds(4, 5));
                    expected_invalidation.Union(SubtractRects(
                        pile_.tiling().TileBounds(5, 5), gfx::Rect(base_tiling_size)));
                    break;
                }
            }
            EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
            invalidation.Clear();
        }

        // Grow right.
        for (int dir = 0; dir <= LAST_DIRECTION; ++dir) {
            gfx::Size new_tiling_size = dir == GROW ? grow_right_tiling_size : base_tiling_size;
            UpdateWholePile();
            UpdateAndExpandInvalidation(&invalidation, new_tiling_size,
                CornerSinglePixelRect(corner, new_tiling_size));

            // We should have lost the recordings in the right column.
            EXPECT_EQ(6, pile_.tiling().num_tiles_x());
            EXPECT_EQ(6, pile_.tiling().num_tiles_y());
            for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
                for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                    FakePicturePile::PictureMapKey key(i, j);
                    FakePicturePile::PictureMap& map = pile_.picture_map();
                    FakePicturePile::PictureMap::iterator it = map.find(key);
                    bool expect_tile;
                    switch (corner) {
                    case TOP_LEFT:
                    case BOTTOM_LEFT:
                        expect_tile = i < 5;
                        break;
                    case TOP_RIGHT:
                        // The interest rect in the top right tile means we'll record it.
                        expect_tile = i < 5 || (j == 0 && i == 5);
                        break;
                    case BOTTOM_RIGHT:
                        // The interest rect in the bottom right tile means we'll record it.
                        expect_tile = i < 5 || (j == 5 && i == 5);
                        break;
                    }
                    EXPECT_EQ(expect_tile, it != map.end() && it->second.get());
                }
            }

            // We invalidated the right column outside the new interest rect. The tile
            // that insects the interest rect in invalidated only on its new or
            // previously exposed pixels.
            if (dir == GROW) {
                // Calculate the expected invalidation the first time through the loop.
                switch (corner) {
                case TOP_LEFT:
                case BOTTOM_LEFT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(5, 0), pile_.tiling().TileBounds(5, 5));
                    break;
                case TOP_RIGHT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(5, 1), pile_.tiling().TileBounds(5, 5));
                    expected_invalidation.Union(SubtractRects(
                        pile_.tiling().TileBounds(5, 0), gfx::Rect(base_tiling_size)));
                    break;
                case BOTTOM_RIGHT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(5, 0), pile_.tiling().TileBounds(5, 4));
                    expected_invalidation.Union(SubtractRects(
                        pile_.tiling().TileBounds(5, 5), gfx::Rect(base_tiling_size)));
                    break;
                }
            }
            EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
            invalidation.Clear();
        }

        // Grow both.
        for (int dir = 0; dir <= LAST_DIRECTION; ++dir) {
            gfx::Size new_tiling_size = dir == GROW ? grow_both_tiling_size : base_tiling_size;
            UpdateWholePile();
            UpdateAndExpandInvalidation(&invalidation, new_tiling_size,
                CornerSinglePixelRect(corner, new_tiling_size));

            // We should have lost the recordings in the right column and bottom row.
            // The tile that insects the interest rect in invalidated only on its new
            // or previously exposed pixels.
            EXPECT_EQ(6, pile_.tiling().num_tiles_x());
            EXPECT_EQ(6, pile_.tiling().num_tiles_y());
            for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
                for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                    FakePicturePile::PictureMapKey key(i, j);
                    FakePicturePile::PictureMap& map = pile_.picture_map();
                    FakePicturePile::PictureMap::iterator it = map.find(key);
                    bool expect_tile;
                    switch (corner) {
                    case TOP_LEFT:
                        expect_tile = i < 5 && j < 5;
                        break;
                    case TOP_RIGHT:
                        // The interest rect in the top right tile means we'll record it.
                        expect_tile = (i < 5 && j < 5) || (j == 0 && i == 5);
                        break;
                    case BOTTOM_LEFT:
                        // The interest rect in the bottom left tile means we'll record it.
                        expect_tile = (i < 5 && j < 5) || (j == 5 && i == 0);
                        break;
                    case BOTTOM_RIGHT:
                        // The interest rect in the bottom right tile means we'll record it.
                        expect_tile = (i < 5 && j < 5) || (j == 5 && i == 5);
                        break;
                    }
                    EXPECT_EQ(expect_tile, it != map.end() && it->second.get()) << i << ","
                                                                                << j;
                }
            }

            // We invalidated the right column and the bottom row outside the new
            // interest rect. The tile that insects the interest rect in invalidated
            // only on its new or previous exposed pixels.
            if (dir == GROW) {
                // Calculate the expected invalidation the first time through the loop.
                switch (corner) {
                case TOP_LEFT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(5, 0), pile_.tiling().TileBounds(5, 5));
                    expected_invalidation.Union(
                        gfx::UnionRects(pile_.tiling().TileBounds(0, 5),
                            pile_.tiling().TileBounds(5, 5)));
                    break;
                case TOP_RIGHT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(5, 1), pile_.tiling().TileBounds(5, 5));
                    expected_invalidation.Union(
                        gfx::UnionRects(pile_.tiling().TileBounds(0, 5),
                            pile_.tiling().TileBounds(5, 5)));
                    expected_invalidation.Union(SubtractRects(
                        pile_.tiling().TileBounds(5, 0), gfx::Rect(base_tiling_size)));
                    break;
                case BOTTOM_LEFT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(5, 0), pile_.tiling().TileBounds(5, 5));
                    expected_invalidation.Union(
                        gfx::UnionRects(pile_.tiling().TileBounds(1, 5),
                            pile_.tiling().TileBounds(5, 5)));
                    expected_invalidation.Union(SubtractRects(
                        pile_.tiling().TileBounds(0, 5), gfx::Rect(base_tiling_size)));
                    break;
                case BOTTOM_RIGHT:
                    expected_invalidation = gfx::UnionRects(
                        pile_.tiling().TileBounds(5, 0), pile_.tiling().TileBounds(5, 4));
                    expected_invalidation.Union(
                        gfx::UnionRects(pile_.tiling().TileBounds(0, 5),
                            pile_.tiling().TileBounds(4, 5)));
                    expected_invalidation.Union(SubtractRegions(
                        pile_.tiling().TileBounds(5, 5), gfx::Rect(base_tiling_size)));
                    break;
                }
            }
            EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
            invalidation.Clear();
        }
    }

    INSTANTIATE_TEST_CASE_P(
        PicturePileResizeCornerTests,
        PicturePileResizeCornerTest,
        ::testing::Values(TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT));

    TEST_F(PicturePileTest, ResizePileInsideInterestRect)
    {
        // This size chosen to be small enough that all the rects below fit inside the
        // the interest rect, so they are smaller than kPixelDistanceToRecord in each
        // dimension.
        int tile_size = 100;
        gfx::Size base_tiling_size(5 * tile_size, 5 * tile_size);
        gfx::Size grow_down_tiling_size(5 * tile_size, 7 * tile_size);
        gfx::Size grow_right_tiling_size(7 * tile_size, 5 * tile_size);
        gfx::Size grow_both_tiling_size(7 * tile_size, 7 * tile_size);

        Region invalidation;
        Region expected_invalidation;

        pile_.tiling().SetMaxTextureSize(gfx::Size(tile_size, tile_size));
        SetTilingSize(base_tiling_size);

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        UpdateAndExpandInvalidation(
            &invalidation, grow_down_tiling_size, gfx::Rect(1, 1));

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(8, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the newly exposed pixels on the bottom row of tiles.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_down_tiling_size),
            gfx::Rect(base_tiling_size));
        Region bottom_row_new_pixels = SubtractRegions(gfx::UnionRects(pile_.tiling().TileBounds(0, 5),
                                                           pile_.tiling().TileBounds(5, 5)),
            gfx::Rect(base_tiling_size));
        EXPECT_TRUE(expected_invalidation.Contains(bottom_row_new_pixels));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation, base_tiling_size, gfx::Rect(1, 1));

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the previously exposed pixels on the bottom row of tiles.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_down_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_TRUE(expected_invalidation.Contains(bottom_row_new_pixels));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(
            &invalidation, grow_right_tiling_size, gfx::Rect(1, 1));

        // We should have a recording for every tile.
        EXPECT_EQ(8, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the newly exposed pixels on the right column of tiles.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_right_tiling_size),
            gfx::Rect(base_tiling_size));
        Region right_column_new_pixels = SubtractRegions(gfx::UnionRects(pile_.tiling().TileBounds(5, 0),
                                                             pile_.tiling().TileBounds(5, 5)),
            gfx::Rect(base_tiling_size));
        EXPECT_TRUE(expected_invalidation.Contains(right_column_new_pixels));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation, base_tiling_size, gfx::Rect(1, 1));

        // We should have lost the recordings that are now outside the tiling only.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the previously exposed pixels on the right column of tiles.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_right_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_TRUE(expected_invalidation.Contains(right_column_new_pixels));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(
            &invalidation, grow_both_tiling_size, gfx::Rect(1, 1));

        // We should have a recording for every tile.
        EXPECT_EQ(8, pile_.tiling().num_tiles_x());
        EXPECT_EQ(8, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the newly exposed pixels on the bottom row and right column
        // of tiles.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_both_tiling_size),
            gfx::Rect(base_tiling_size));
        Region bottom_row_and_right_column_new_pixels = SubtractRegions(
            UnionRegions(gfx::UnionRects(pile_.tiling().TileBounds(0, 5),
                             pile_.tiling().TileBounds(5, 5)),
                gfx::UnionRects(pile_.tiling().TileBounds(5, 0),
                    pile_.tiling().TileBounds(5, 5))),
            gfx::Rect(base_tiling_size));
        EXPECT_TRUE(
            expected_invalidation.Contains(bottom_row_and_right_column_new_pixels));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation, base_tiling_size, gfx::Rect());

        // We should have lost the recordings that are now outside the tiling only.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the previously exposed pixels on the bottom row and right
        // column of tiles.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_both_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_TRUE(
            expected_invalidation.Contains(bottom_row_and_right_column_new_pixels));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();
    }

    TEST_F(PicturePileTest, SmallResizePileInsideInterestRect)
    {
        // This size chosen to be small enough that all the rects below fit inside the
        // the interest rect, so they are smaller than kPixelDistanceToRecord in each
        // dimension.
        int tile_size = 100;
        gfx::Size base_tiling_size(5 * tile_size, 5 * tile_size);
        gfx::Size grow_down_tiling_size(5 * tile_size, 5 * tile_size + 5);
        gfx::Size grow_right_tiling_size(5 * tile_size + 5, 5 * tile_size);
        gfx::Size grow_both_tiling_size(5 * tile_size + 5, 5 * tile_size + 5);

        Region invalidation;
        Region expected_invalidation;

        pile_.tiling().SetMaxTextureSize(gfx::Size(tile_size, tile_size));
        SetTilingSize(base_tiling_size);

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        UpdateAndExpandInvalidation(
            &invalidation, grow_down_tiling_size, gfx::Rect(1, 1));

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the newly exposed pixels.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_down_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation, base_tiling_size, gfx::Rect(1, 1));

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the previously exposed pixels.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_down_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(
            &invalidation, grow_right_tiling_size, gfx::Rect(1, 1));

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the newly exposed pixels.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_right_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation, base_tiling_size, gfx::Rect(1, 1));

        // We should have lost the recordings that are now outside the tiling only.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the previously exposed pixels.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_right_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(
            &invalidation, grow_both_tiling_size, gfx::Rect(1, 1));

        // We should have a recording for every tile.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the newly exposed pixels.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_both_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();

        UpdateWholePile();
        UpdateAndExpandInvalidation(&invalidation, base_tiling_size, gfx::Rect());

        // We should have lost the recordings that are now outside the tiling only.
        EXPECT_EQ(6, pile_.tiling().num_tiles_x());
        EXPECT_EQ(6, pile_.tiling().num_tiles_y());
        for (int i = 0; i < pile_.tiling().num_tiles_x(); ++i) {
            for (int j = 0; j < pile_.tiling().num_tiles_y(); ++j) {
                FakePicturePile::PictureMapKey key(i, j);
                FakePicturePile::PictureMap& map = pile_.picture_map();
                FakePicturePile::PictureMap::iterator it = map.find(key);
                EXPECT_TRUE(it != map.end() && it->second.get());
            }
        }

        // We invalidated the previously exposed pixels.
        expected_invalidation = SubtractRegions(gfx::Rect(grow_both_tiling_size),
            gfx::Rect(base_tiling_size));
        EXPECT_EQ(expected_invalidation.ToString(), invalidation.ToString());
        invalidation.Clear();
    }

    TEST_F(PicturePileTest, SolidRectangleIsSolid)
    {
        // If the client has no contents, the solid state will be true.
        Region invalidation1(tiling_rect());
        UpdateAndExpandInvalidation(&invalidation1, tiling_size(), tiling_rect());
        EXPECT_TRUE(pile_.is_solid_color());
        EXPECT_EQ(static_cast<SkColor>(SK_ColorTRANSPARENT), pile_.solid_color());

        // If there is a single rect that covers the view, the solid
        // state will be true.
        SkPaint paint;
        paint.setColor(SK_ColorCYAN);
        client_.add_draw_rect(tiling_rect(), paint);
        Region invalidation2(tiling_rect());
        UpdateAndExpandInvalidation(&invalidation2, tiling_size(), tiling_rect());
        EXPECT_TRUE(pile_.is_solid_color());
        EXPECT_EQ(SK_ColorCYAN, pile_.solid_color());

        // If a second smaller rect is draw that doesn't cover the viewport
        // completely, the solid state will be false.
        gfx::Rect smallRect = tiling_rect();
        smallRect.Inset(10, 10, 10, 10);
        client_.add_draw_rect(smallRect, paint);
        Region invalidation3(tiling_rect());
        UpdateAndExpandInvalidation(&invalidation3, tiling_size(), tiling_rect());
        EXPECT_FALSE(pile_.is_solid_color());

        // If a third rect is drawn over everything, we should be solid again.
        paint.setColor(SK_ColorRED);
        client_.add_draw_rect(tiling_rect(), paint);
        Region invalidation4(tiling_rect());
        UpdateAndExpandInvalidation(&invalidation4, tiling_size(), tiling_rect());
        EXPECT_TRUE(pile_.is_solid_color());
        EXPECT_EQ(SK_ColorRED, pile_.solid_color());

        // If we draw too many, we don't bother doing the analysis and we should no
        // longer be in a solid state.  There are 8 rects, two clips and a translate.
        client_.add_draw_rect(tiling_rect(), paint);
        client_.add_draw_rect(tiling_rect(), paint);
        client_.add_draw_rect(tiling_rect(), paint);
        client_.add_draw_rect(tiling_rect(), paint);
        client_.add_draw_rect(tiling_rect(), paint);
        Region invalidation5(tiling_rect());
        UpdateAndExpandInvalidation(&invalidation5, tiling_size(), tiling_rect());
        EXPECT_FALSE(pile_.is_solid_color());
    }

    TEST_F(PicturePileTest, NonSolidRectangleOnOffsettedLayerIsNonSolid)
    {
        gfx::Rect visible_rect(tiling_rect());
        visible_rect.Offset(gfx::Vector2d(1000, 1000));
        // The picture pile requires that the tiling completely encompass the viewport
        // to make this test work correctly since the recorded viewport is an
        // intersection of the tile size and viewport rect.  This is possibly a flaw
        // in |PicturePile|.
        gfx::Size tiling_size(visible_rect.right(), visible_rect.bottom());
        // |Setup()| will create pictures here that mess with the test, clear it!
        pile_.Clear();

        SkPaint paint;
        paint.setColor(SK_ColorCYAN);

        // Add a rect that doesn't cover the viewport completely, the solid state
        // will be false.
        gfx::Rect smallRect = visible_rect;
        smallRect.Inset(10, 10, 10, 10);
        client_.add_draw_rect(smallRect, paint);
        Region invalidation(visible_rect);
        UpdateAndExpandInvalidation(&invalidation, tiling_size, visible_rect);
        EXPECT_FALSE(pile_.is_solid_color());
    }

    TEST_F(PicturePileTest, SetEmptyBounds)
    {
        EXPECT_TRUE(pile_.is_solid_color());
        EXPECT_FALSE(pile_.GetSize().IsEmpty());
        EXPECT_FALSE(pile_.picture_map().empty());
        EXPECT_TRUE(pile_.HasRecordings());
        pile_.SetEmptyBounds();
        EXPECT_FALSE(pile_.is_solid_color());
        EXPECT_TRUE(pile_.GetSize().IsEmpty());
        EXPECT_TRUE(pile_.picture_map().empty());
        EXPECT_FALSE(pile_.HasRecordings());
    }

} // namespace
} // namespace cc
