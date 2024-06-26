// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "cc/playback/display_list_raster_source.h"
#include "cc/test/fake_display_list_recording_source.h"
#include "cc/test/fake_picture_pile.h"
#include "cc/test/fake_picture_pile_impl.h"
#include "cc/test/skia_common.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    template <class T>
    scoped_ptr<T> CreateRecordingSource(const gfx::Rect& viewport,
        const gfx::Size& grid_cell_size);

    template <>
    scoped_ptr<FakePicturePile> CreateRecordingSource<FakePicturePile>(
        const gfx::Rect& viewport,
        const gfx::Size& grid_cell_size)
    {
        return FakePicturePile::CreateFilledPile(grid_cell_size, viewport.size());
    }

    template <>
    scoped_ptr<FakeDisplayListRecordingSource> CreateRecordingSource<
        FakeDisplayListRecordingSource>(const gfx::Rect& viewport,
        const gfx::Size& grid_cell_size)
    {
        gfx::Rect layer_rect(viewport.right(), viewport.bottom());
        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateRecordingSource(viewport,
            layer_rect.size());
        recording_source->SetGridCellSize(grid_cell_size);

        return recording_source.Pass();
    }

    template <class T>
    scoped_refptr<RasterSource> CreateRasterSource(T* recording_source);

    template <>
    scoped_refptr<RasterSource> CreateRasterSource(
        FakePicturePile* recording_source)
    {
        return FakePicturePileImpl::CreateFromPile(recording_source, nullptr);
    }

    template <>
    scoped_refptr<RasterSource> CreateRasterSource(
        FakeDisplayListRecordingSource* recording_source)
    {
        bool can_use_lcd_text = true;
        return DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source, can_use_lcd_text);
    }

    template <typename T>
    class RecordingSourceTest : public testing::Test {
    };

    using testing::Types;

    typedef Types<FakePicturePile, FakeDisplayListRecordingSource>
        RecordingSourceImplementations;

    TYPED_TEST_CASE(RecordingSourceTest, RecordingSourceImplementations);

    TYPED_TEST(RecordingSourceTest, NoGatherPixelRefEmptyPixelRefs)
    {
        gfx::Size grid_cell_size(128, 128);
        gfx::Rect recorded_viewport(0, 0, 256, 256);

        scoped_ptr<TypeParam> recording_source = CreateRecordingSource<TypeParam>(recorded_viewport, grid_cell_size);
        recording_source->SetGatherPixelRefs(false);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource<TypeParam>(recording_source.get());

        // If recording source do not gather pixel ref, raster source is not going to
        // get pixel refs.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(recorded_viewport, 1.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
    }

    TYPED_TEST(RecordingSourceTest, EmptyPixelRefs)
    {
        gfx::Size grid_cell_size(128, 128);
        gfx::Rect recorded_viewport(0, 0, 256, 256);

        scoped_ptr<TypeParam> recording_source = CreateRecordingSource<TypeParam>(recorded_viewport, grid_cell_size);
        recording_source->SetGatherPixelRefs(true);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource<TypeParam>(recording_source.get());

        // Tile sized iterators.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 128, 128), 1.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 256, 256), 2.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 64, 64), 0.5, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        // Shifted tile sized iterators.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(140, 140, 128, 128), 1.0,
                &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(280, 280, 256, 256), 2.0,
                &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(70, 70, 64, 64), 0.5, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        // Layer sized iterators.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 256, 256), 1.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 512, 512), 2.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 128, 128), 0.5, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
    }

    TYPED_TEST(RecordingSourceTest, NoDiscardablePixelRefs)
    {
        gfx::Size grid_cell_size(128, 128);
        gfx::Rect recorded_viewport(0, 0, 256, 256);

        scoped_ptr<TypeParam> recording_source = CreateRecordingSource<TypeParam>(recorded_viewport, grid_cell_size);

        SkPaint simple_paint;
        simple_paint.setColor(SkColorSetARGB(255, 12, 23, 34));

        SkBitmap non_discardable_bitmap;
        CreateBitmap(gfx::Size(128, 128), "notdiscardable", &non_discardable_bitmap);

        recording_source->add_draw_rect_with_paint(gfx::Rect(0, 0, 256, 256),
            simple_paint);
        recording_source->add_draw_rect_with_paint(gfx::Rect(128, 128, 512, 512),
            simple_paint);
        recording_source->add_draw_rect_with_paint(gfx::Rect(512, 0, 256, 256),
            simple_paint);
        recording_source->add_draw_rect_with_paint(gfx::Rect(0, 512, 256, 256),
            simple_paint);
        recording_source->add_draw_bitmap(non_discardable_bitmap, gfx::Point(128, 0));
        recording_source->add_draw_bitmap(non_discardable_bitmap, gfx::Point(0, 128));
        recording_source->add_draw_bitmap(non_discardable_bitmap,
            gfx::Point(150, 150));
        recording_source->SetGatherPixelRefs(true);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource<TypeParam>(recording_source.get());

        // Tile sized iterators.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 128, 128), 1.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 256, 256), 2.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 64, 64), 0.5, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        // Shifted tile sized iterators.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(140, 140, 128, 128), 1.0,
                &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(280, 280, 256, 256), 2.0,
                &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(70, 70, 64, 64), 0.5, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        // Layer sized iterators.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 256, 256), 1.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 512, 512), 2.0, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 128, 128), 0.5, &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
    }

    TYPED_TEST(RecordingSourceTest, DiscardablePixelRefs)
    {
        gfx::Size grid_cell_size(128, 128);
        gfx::Rect recorded_viewport(0, 0, 256, 256);

        scoped_ptr<TypeParam> recording_source = CreateRecordingSource<TypeParam>(recorded_viewport, grid_cell_size);

        SkBitmap discardable_bitmap[2][2];
        CreateBitmap(gfx::Size(32, 32), "discardable", &discardable_bitmap[0][0]);
        CreateBitmap(gfx::Size(32, 32), "discardable", &discardable_bitmap[1][0]);
        CreateBitmap(gfx::Size(32, 32), "discardable", &discardable_bitmap[1][1]);

        // Discardable pixel refs are found in the following cells:
        // |---|---|
        // | x |   |
        // |---|---|
        // | x | x |
        // |---|---|
        recording_source->add_draw_bitmap(discardable_bitmap[0][0], gfx::Point(0, 0));
        recording_source->add_draw_bitmap(discardable_bitmap[1][0],
            gfx::Point(0, 130));
        recording_source->add_draw_bitmap(discardable_bitmap[1][1],
            gfx::Point(140, 140));
        recording_source->SetGatherPixelRefs(true);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource<TypeParam>(recording_source.get());

        // Tile sized iterators. These should find only one pixel ref.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 128, 128), 1.0, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 256, 256), 2.0, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 64, 64), 0.5, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }

        // Shifted tile sized iterators. These should find only one pixel ref.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(140, 140, 128, 128), 1.0,
                &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(280, 280, 256, 256), 2.0,
                &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(70, 70, 64, 64), 0.5, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }

        // Ensure there's no discardable pixel refs in the empty cell
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(140, 0, 128, 128), 1.0,
                &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }

        // Layer sized iterators. These should find all 3 pixel refs.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 256, 256), 1.0, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_TRUE(pixel_refs[1] == discardable_bitmap[1][0].pixelRef());
            EXPECT_TRUE(pixel_refs[2] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(3u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 512, 512), 2.0, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_TRUE(pixel_refs[1] == discardable_bitmap[1][0].pixelRef());
            EXPECT_TRUE(pixel_refs[2] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(3u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 128, 128), 0.5, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_TRUE(pixel_refs[1] == discardable_bitmap[1][0].pixelRef());
            EXPECT_TRUE(pixel_refs[2] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(3u, pixel_refs.size());
        }
    }

    TYPED_TEST(RecordingSourceTest, DiscardablePixelRefsBaseNonDiscardable)
    {
        gfx::Size grid_cell_size(256, 256);
        gfx::Rect recorded_viewport(0, 0, 512, 512);

        scoped_ptr<TypeParam> recording_source = CreateRecordingSource<TypeParam>(recorded_viewport, grid_cell_size);

        SkBitmap non_discardable_bitmap;
        CreateBitmap(gfx::Size(512, 512), "notdiscardable", &non_discardable_bitmap);

        SkBitmap discardable_bitmap[2][2];
        CreateBitmap(gfx::Size(128, 128), "discardable", &discardable_bitmap[0][0]);
        CreateBitmap(gfx::Size(128, 128), "discardable", &discardable_bitmap[0][1]);
        CreateBitmap(gfx::Size(128, 128), "discardable", &discardable_bitmap[1][1]);

        // One large non-discardable bitmap covers the whole grid.
        // Discardable pixel refs are found in the following cells:
        // |---|---|
        // | x | x |
        // |---|---|
        // |   | x |
        // |---|---|
        recording_source->add_draw_bitmap(non_discardable_bitmap, gfx::Point(0, 0));
        recording_source->add_draw_bitmap(discardable_bitmap[0][0], gfx::Point(0, 0));
        recording_source->add_draw_bitmap(discardable_bitmap[0][1],
            gfx::Point(260, 0));
        recording_source->add_draw_bitmap(discardable_bitmap[1][1],
            gfx::Point(260, 260));
        recording_source->SetGatherPixelRefs(true);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource<TypeParam>(recording_source.get());

        // Tile sized iterators. These should find only one pixel ref.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 256, 256), 1.0, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 512, 512), 2.0, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 128, 128), 0.5, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        // Shifted tile sized iterators. These should find only one pixel ref.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(260, 260, 256, 256), 1.0,
                &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(520, 520, 512, 512), 2.0,
                &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(130, 130, 128, 128), 0.5,
                &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(1u, pixel_refs.size());
        }
        // Ensure there's no discardable pixel refs in the empty cell
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 256, 256, 256), 1.0,
                &pixel_refs);
            EXPECT_TRUE(pixel_refs.empty());
        }
        // Layer sized iterators. These should find three pixel ref.
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 512, 512), 1.0, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_TRUE(pixel_refs[1] == discardable_bitmap[0][1].pixelRef());
            EXPECT_TRUE(pixel_refs[2] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(3u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 1024, 1024), 2.0,
                &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_TRUE(pixel_refs[1] == discardable_bitmap[0][1].pixelRef());
            EXPECT_TRUE(pixel_refs[2] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(3u, pixel_refs.size());
        }
        {
            std::vector<SkPixelRef*> pixel_refs;
            raster_source->GatherPixelRefs(gfx::Rect(0, 0, 256, 256), 0.5, &pixel_refs);
            EXPECT_FALSE(pixel_refs.empty());
            EXPECT_TRUE(pixel_refs[0] == discardable_bitmap[0][0].pixelRef());
            EXPECT_TRUE(pixel_refs[1] == discardable_bitmap[0][1].pixelRef());
            EXPECT_TRUE(pixel_refs[2] == discardable_bitmap[1][1].pixelRef());
            EXPECT_EQ(3u, pixel_refs.size());
        }
    }

} // namespace
} // namespace cc
