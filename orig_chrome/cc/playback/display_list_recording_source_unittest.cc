// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "cc/base/region.h"
#include "cc/playback/display_list_raster_source.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/fake_display_list_recording_source.h"
#include "cc/test/skia_common.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    scoped_ptr<FakeDisplayListRecordingSource> CreateRecordingSource(
        const gfx::Rect& viewport)
    {
        gfx::Rect layer_rect(viewport.right(), viewport.bottom());
        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateRecordingSource(viewport,
            layer_rect.size());
        return recording_source.Pass();
    }

    scoped_refptr<RasterSource> CreateRasterSource(
        FakeDisplayListRecordingSource* recording_source)
    {
        bool can_use_lcd_text = true;
        return DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source, can_use_lcd_text);
    }

    TEST(DisplayListRecordingSourceTest, DiscardableImagesWithTransform)
    {
        gfx::Rect recorded_viewport(256, 256);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            recorded_viewport.size());
        skia::RefPtr<SkImage> discardable_image[2][2];
        gfx::Transform identity_transform;
        discardable_image[0][0] = CreateDiscardableImage(gfx::Size(32, 32));
        // Translate transform is equivalent to moving using point.
        gfx::Transform translate_transform;
        translate_transform.Translate(0, 130);
        discardable_image[1][0] = CreateDiscardableImage(gfx::Size(32, 32));
        // This moves the bitmap to center of viewport and rotate, this would make
        // this bitmap in all four tile grids.
        gfx::Transform rotate_transform;
        rotate_transform.Translate(112, 112);
        rotate_transform.Rotate(45);
        discardable_image[1][1] = CreateDiscardableImage(gfx::Size(32, 32));

        gfx::RectF rect(0, 0, 32, 32);
        gfx::RectF translate_rect = rect;
        translate_transform.TransformRect(&translate_rect);
        gfx::RectF rotate_rect = rect;
        rotate_transform.TransformRect(&rotate_rect);

        recording_source->add_draw_image_with_transform(discardable_image[0][0].get(),
            identity_transform);
        recording_source->add_draw_image_with_transform(discardable_image[1][0].get(),
            translate_transform);
        recording_source->add_draw_image_with_transform(discardable_image[1][1].get(),
            rotate_transform);
        recording_source->SetGenerateDiscardableImagesMetadata(true);
        recording_source->Rerecord();

        bool can_use_lcd_text = true;
        scoped_refptr<DisplayListRasterSource> raster_source = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), can_use_lcd_text);

        // Tile sized iterators. These should find only one pixel ref.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 128, 128),
                &images);
            EXPECT_EQ(2u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[0][0].get());
            EXPECT_TRUE(images[1].image == discardable_image[1][1].get());
            EXPECT_EQ(rect.ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
            EXPECT_EQ(rotate_rect.ToString(),
                gfx::SkRectToRectF(images[1].image_rect).ToString());
        }

        // Shifted tile sized iterators. These should find only one pixel ref.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(130, 140, 128, 128),
                &images);
            EXPECT_EQ(1u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[1][1].get());
            EXPECT_EQ(rotate_rect.ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
        }

        // The rotated bitmap would still be in the top right tile.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(130, 0, 128, 128),
                &images);
            EXPECT_EQ(1u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[1][1].get());
            EXPECT_EQ(rotate_rect.ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
        }

        // Layer sized iterators. These should find all pixel refs.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 256, 256),
                &images);
            EXPECT_EQ(3u, images.size());
            // Top left tile with bitmap[0][0] and bitmap[1][1].
            EXPECT_TRUE(images[0].image == discardable_image[0][0].get());
            EXPECT_TRUE(images[1].image == discardable_image[1][0].get());
            EXPECT_TRUE(images[2].image == discardable_image[1][1].get());
            EXPECT_EQ(rect.ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
            EXPECT_EQ(translate_rect.ToString(),
                gfx::SkRectToRectF(images[1].image_rect).ToString());
            EXPECT_EQ(rotate_rect.ToString(),
                gfx::SkRectToRectF(images[2].image_rect).ToString());
        }
    }

    TEST(DisplayListRecordingSourceTest, ExposesEnoughNewAreaEmpty)
    {
        gfx::Size layer_size(1000, 1000);

        // Both empty means there is nothing to do.
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            gfx::Rect(), gfx::Rect(), layer_size));
        // Going from empty to non-empty means we must re-record because it could be
        // the first frame after construction or Clear.
        EXPECT_TRUE(DisplayListRecordingSource::ExposesEnoughNewArea(
            gfx::Rect(), gfx::Rect(1, 1), layer_size));

        // Going from non-empty to empty is not special-cased.
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            gfx::Rect(1, 1), gfx::Rect(), layer_size));
    }

    TEST(DisplayListRecordingSourceTest, ExposesEnoughNewAreaNotBigEnough)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Rect current_recorded_viewport(100, 100, 100, 100);
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, gfx::Rect(100, 100, 90, 90), layer_size));
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, gfx::Rect(100, 100, 100, 100), layer_size));
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, gfx::Rect(1, 1, 200, 200), layer_size));
    }

    TEST(DisplayListRecordingSourceTest,
        ExposesEnoughNewAreaNotBigEnoughButNewAreaTouchesEdge)
    {
        gfx::Size layer_size(500, 500);
        gfx::Rect current_recorded_viewport(100, 100, 100, 100);

        // Top edge.
        EXPECT_TRUE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, gfx::Rect(100, 0, 100, 200), layer_size));

        // Left edge.
        EXPECT_TRUE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, gfx::Rect(0, 100, 200, 100), layer_size));

        // Bottom edge.
        EXPECT_TRUE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, gfx::Rect(100, 100, 100, 400), layer_size));

        // Right edge.
        EXPECT_TRUE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, gfx::Rect(100, 100, 400, 100), layer_size));
    }

    // Verifies that having a current viewport that touches a layer edge does not
    // force re-recording.
    TEST(DisplayListRecordingSourceTest,
        ExposesEnoughNewAreaCurrentViewportTouchesEdge)
    {
        gfx::Size layer_size(500, 500);
        gfx::Rect potential_new_viewport(100, 100, 300, 300);

        // Top edge.
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            gfx::Rect(100, 0, 100, 100), potential_new_viewport, layer_size));

        // Left edge.
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            gfx::Rect(0, 100, 100, 100), potential_new_viewport, layer_size));

        // Bottom edge.
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            gfx::Rect(300, 400, 100, 100), potential_new_viewport, layer_size));

        // Right edge.
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            gfx::Rect(400, 300, 100, 100), potential_new_viewport, layer_size));
    }

    TEST(DisplayListRecordingSourceTest, ExposesEnoughNewAreaScrollScenarios)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Rect current_recorded_viewport(100, 100, 100, 100);

        gfx::Rect new_recorded_viewport(current_recorded_viewport);
        new_recorded_viewport.Offset(512, 0);
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, new_recorded_viewport, layer_size));
        new_recorded_viewport.Offset(0, 512);
        EXPECT_FALSE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, new_recorded_viewport, layer_size));

        new_recorded_viewport.Offset(1, 0);
        EXPECT_TRUE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, new_recorded_viewport, layer_size));

        new_recorded_viewport.Offset(-1, 1);
        EXPECT_TRUE(DisplayListRecordingSource::ExposesEnoughNewArea(
            current_recorded_viewport, new_recorded_viewport, layer_size));
    }

    // Verifies that UpdateAndExpandInvalidation calls ExposesEnoughNewArea with the
    // right arguments.
    TEST(DisplayListRecordingSourceTest,
        ExposesEnoughNewAreaCalledWithCorrectArguments)
    {
        DisplayListRecordingSource recording_source;
        FakeContentLayerClient client;
        Region invalidation;
        gfx::Size layer_size(9000, 9000);
        gfx::Rect visible_rect(0, 0, 256, 256);

        recording_source.UpdateAndExpandInvalidation(
            &client, &invalidation, layer_size, visible_rect, 0,
            RecordingSource::RECORD_NORMALLY);
        EXPECT_EQ(gfx::Rect(0, 0, 4256, 4256), recording_source.recorded_viewport());

        visible_rect.Offset(0, 512);
        recording_source.UpdateAndExpandInvalidation(
            &client, &invalidation, layer_size, visible_rect, 0,
            RecordingSource::RECORD_NORMALLY);
        EXPECT_EQ(gfx::Rect(0, 0, 4256, 4256), recording_source.recorded_viewport());

        // Move past the threshold for enough exposed new area.
        visible_rect.Offset(0, 1);
        recording_source.UpdateAndExpandInvalidation(
            &client, &invalidation, layer_size, visible_rect, 0,
            RecordingSource::RECORD_NORMALLY);
        EXPECT_EQ(gfx::Rect(0, 0, 4256, 4769), recording_source.recorded_viewport());

        // Make the bottom of the potential new recorded viewport coincide with the
        // layer's bottom edge.
        visible_rect.Offset(0, 231);
        recording_source.UpdateAndExpandInvalidation(
            &client, &invalidation, layer_size, visible_rect, 0,
            RecordingSource::RECORD_NORMALLY);
        EXPECT_EQ(gfx::Rect(0, 0, 4256, 4769), recording_source.recorded_viewport());
    }

    TEST(DisplayListRecordingSourceTest, NoGatherImageEmptyImages)
    {
        gfx::Rect recorded_viewport(0, 0, 256, 256);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = CreateRecordingSource(recorded_viewport);
        recording_source->SetGenerateDiscardableImagesMetadata(false);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource(recording_source.get());

        // If recording source do not gather images, raster source is not going to
        // get images.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(recorded_viewport, &images);
            EXPECT_TRUE(images.empty());
        }
    }

    TEST(DisplayListRecordingSourceTest, EmptyImages)
    {
        gfx::Rect recorded_viewport(0, 0, 256, 256);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = CreateRecordingSource(recorded_viewport);
        recording_source->SetGenerateDiscardableImagesMetadata(true);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource(recording_source.get());

        // Tile sized iterators.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 128, 128),
                &images);
            EXPECT_TRUE(images.empty());
        }
        // Shifted tile sized iterators.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(140, 140, 128, 128),
                &images);
            EXPECT_TRUE(images.empty());
        }
        // Layer sized iterators.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 256, 256),
                &images);
            EXPECT_TRUE(images.empty());
        }
    }

    TEST(DisplayListRecordingSourceTest, NoDiscardableImages)
    {
        gfx::Rect recorded_viewport(0, 0, 256, 256);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = CreateRecordingSource(recorded_viewport);

        SkPaint simple_paint;
        simple_paint.setColor(SkColorSetARGB(255, 12, 23, 34));

        SkBitmap non_discardable_bitmap;
        non_discardable_bitmap.allocN32Pixels(128, 128);
        non_discardable_bitmap.setImmutable();
        skia::RefPtr<SkImage> non_discardable_image = skia::AdoptRef(SkImage::NewFromBitmap(non_discardable_bitmap));

        recording_source->add_draw_rect_with_paint(gfx::Rect(0, 0, 256, 256),
            simple_paint);
        recording_source->add_draw_rect_with_paint(gfx::Rect(128, 128, 512, 512),
            simple_paint);
        recording_source->add_draw_rect_with_paint(gfx::Rect(512, 0, 256, 256),
            simple_paint);
        recording_source->add_draw_rect_with_paint(gfx::Rect(0, 512, 256, 256),
            simple_paint);
        recording_source->add_draw_image(non_discardable_image.get(),
            gfx::Point(128, 0));
        recording_source->add_draw_image(non_discardable_image.get(),
            gfx::Point(0, 128));
        recording_source->add_draw_image(non_discardable_image.get(),
            gfx::Point(150, 150));
        recording_source->SetGenerateDiscardableImagesMetadata(true);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource(recording_source.get());

        // Tile sized iterators.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 128, 128),
                &images);
            EXPECT_TRUE(images.empty());
        }
        // Shifted tile sized iterators.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(140, 140, 128, 128),
                &images);
            EXPECT_TRUE(images.empty());
        }
        // Layer sized iterators.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 256, 256),
                &images);
            EXPECT_TRUE(images.empty());
        }
    }

    TEST(DisplayListRecordingSourceTest, DiscardableImages)
    {
        gfx::Rect recorded_viewport(0, 0, 256, 256);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = CreateRecordingSource(recorded_viewport);

        skia::RefPtr<SkImage> discardable_image[2][2];
        discardable_image[0][0] = CreateDiscardableImage(gfx::Size(32, 32));
        discardable_image[1][0] = CreateDiscardableImage(gfx::Size(32, 32));
        discardable_image[1][1] = CreateDiscardableImage(gfx::Size(32, 32));

        // Discardable images are found in the following cells:
        // |---|---|
        // | x |   |
        // |---|---|
        // | x | x |
        // |---|---|
        recording_source->add_draw_image(discardable_image[0][0].get(),
            gfx::Point(0, 0));
        recording_source->add_draw_image(discardable_image[1][0].get(),
            gfx::Point(0, 130));
        recording_source->add_draw_image(discardable_image[1][1].get(),
            gfx::Point(140, 140));
        recording_source->SetGenerateDiscardableImagesMetadata(true);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource(recording_source.get());

        // Tile sized iterators. These should find only one image.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 128, 128),
                &images);
            EXPECT_EQ(1u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[0][0].get());
            EXPECT_EQ(gfx::RectF(32, 32).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
        }

        // Shifted tile sized iterators. These should find only one image.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(140, 140, 128, 128),
                &images);
            EXPECT_EQ(1u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[1][1].get());
            EXPECT_EQ(gfx::RectF(140, 140, 32, 32).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
        }

        // Ensure there's no discardable images in the empty cell
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(140, 0, 128, 128),
                &images);
            EXPECT_TRUE(images.empty());
        }

        // Layer sized iterators. These should find all 3 images.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 256, 256),
                &images);
            EXPECT_EQ(3u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[0][0].get());
            EXPECT_TRUE(images[1].image == discardable_image[1][0].get());
            EXPECT_TRUE(images[2].image == discardable_image[1][1].get());
            EXPECT_EQ(gfx::RectF(32, 32).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
            EXPECT_EQ(gfx::RectF(0, 130, 32, 32).ToString(),
                gfx::SkRectToRectF(images[1].image_rect).ToString());
            EXPECT_EQ(gfx::RectF(140, 140, 32, 32).ToString(),
                gfx::SkRectToRectF(images[2].image_rect).ToString());
        }
    }

    TEST(DisplayListRecordingSourceTest, DiscardableImagesBaseNonDiscardable)
    {
        gfx::Rect recorded_viewport(0, 0, 512, 512);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = CreateRecordingSource(recorded_viewport);

        SkBitmap non_discardable_bitmap;
        non_discardable_bitmap.allocN32Pixels(512, 512);
        non_discardable_bitmap.setImmutable();
        skia::RefPtr<SkImage> non_discardable_image = skia::AdoptRef(SkImage::NewFromBitmap(non_discardable_bitmap));

        skia::RefPtr<SkImage> discardable_image[2][2];
        discardable_image[0][0] = CreateDiscardableImage(gfx::Size(128, 128));
        discardable_image[0][1] = CreateDiscardableImage(gfx::Size(128, 128));
        discardable_image[1][1] = CreateDiscardableImage(gfx::Size(128, 128));

        // One large non-discardable image covers the whole grid.
        // Discardable images are found in the following cells:
        // |---|---|
        // | x | x |
        // |---|---|
        // |   | x |
        // |---|---|
        recording_source->add_draw_image(non_discardable_image.get(),
            gfx::Point(0, 0));
        recording_source->add_draw_image(discardable_image[0][0].get(),
            gfx::Point(0, 0));
        recording_source->add_draw_image(discardable_image[0][1].get(),
            gfx::Point(260, 0));
        recording_source->add_draw_image(discardable_image[1][1].get(),
            gfx::Point(260, 260));
        recording_source->SetGenerateDiscardableImagesMetadata(true);
        recording_source->Rerecord();

        scoped_refptr<RasterSource> raster_source = CreateRasterSource(recording_source.get());

        // Tile sized iterators. These should find only one image.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 256, 256),
                &images);
            EXPECT_EQ(1u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[0][0].get());
            EXPECT_EQ(gfx::RectF(128, 128).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
        }
        // Shifted tile sized iterators. These should find only one image.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(260, 260, 256, 256),
                &images);
            EXPECT_EQ(1u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[1][1].get());
            EXPECT_EQ(gfx::RectF(260, 260, 128, 128).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
        }
        // Ensure there's no discardable images in the empty cell
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 256, 256, 256),
                &images);
            EXPECT_TRUE(images.empty());
        }
        // Layer sized iterators. These should find three images.
        {
            std::vector<PositionImage> images;
            raster_source->GetDiscardableImagesInRect(gfx::Rect(0, 0, 512, 512),
                &images);
            EXPECT_EQ(3u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[0][0].get());
            EXPECT_TRUE(images[1].image == discardable_image[0][1].get());
            EXPECT_TRUE(images[2].image == discardable_image[1][1].get());
            EXPECT_EQ(gfx::RectF(128, 128).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
            EXPECT_EQ(gfx::RectF(260, 0, 128, 128).ToString(),
                gfx::SkRectToRectF(images[1].image_rect).ToString());
            EXPECT_EQ(gfx::RectF(260, 260, 128, 128).ToString(),
                gfx::SkRectToRectF(images[2].image_rect).ToString());
        }
    }

} // namespace
} // namespace cc
