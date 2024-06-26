// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/scoped_ptr.h"
#include "cc/playback/display_list_raster_source.h"
#include "cc/test/fake_display_list_recording_source.h"
#include "cc/test/skia_common.h"
#include "skia/ext/refptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkPixelRef.h"
#include "third_party/skia/include/core/SkShader.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {
namespace {

    TEST(DisplayListRasterSourceTest, AnalyzeIsSolidUnscaled)
    {
        gfx::Size layer_bounds(400, 400);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);

        SkPaint solid_paint;
        SkColor solid_color = SkColorSetARGB(255, 12, 23, 34);
        solid_paint.setColor(solid_color);

        SkColor non_solid_color = SkColorSetARGB(128, 45, 56, 67);
        SkPaint non_solid_paint;
        non_solid_paint.setColor(non_solid_color);

        recording_source->add_draw_rect_with_paint(gfx::Rect(layer_bounds),
            solid_paint);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        // Ensure everything is solid.
        for (int y = 0; y <= 300; y += 100) {
            for (int x = 0; x <= 300; x += 100) {
                RasterSource::SolidColorAnalysis analysis;
                gfx::Rect rect(x, y, 100, 100);
                raster->PerformSolidColorAnalysis(rect, 1.0, &analysis);
                EXPECT_TRUE(analysis.is_solid_color) << rect.ToString();
                EXPECT_EQ(solid_color, analysis.solid_color) << rect.ToString();
            }
        }

        // Add one non-solid pixel and recreate the raster source.
        recording_source->add_draw_rect_with_paint(gfx::Rect(50, 50, 1, 1),
            non_solid_paint);
        recording_source->Rerecord();
        raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        RasterSource::SolidColorAnalysis analysis;
        raster->PerformSolidColorAnalysis(gfx::Rect(0, 0, 100, 100), 1.0, &analysis);
        EXPECT_FALSE(analysis.is_solid_color);

        raster->PerformSolidColorAnalysis(gfx::Rect(100, 0, 100, 100), 1.0,
            &analysis);
        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(solid_color, analysis.solid_color);

        // Boundaries should be clipped.
        analysis.is_solid_color = false;
        raster->PerformSolidColorAnalysis(gfx::Rect(350, 0, 100, 100), 1.0,
            &analysis);
        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(solid_color, analysis.solid_color);

        analysis.is_solid_color = false;
        raster->PerformSolidColorAnalysis(gfx::Rect(0, 350, 100, 100), 1.0,
            &analysis);
        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(solid_color, analysis.solid_color);

        analysis.is_solid_color = false;
        raster->PerformSolidColorAnalysis(gfx::Rect(350, 350, 100, 100), 1.0,
            &analysis);
        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(solid_color, analysis.solid_color);
    }

    TEST(DisplayListRasterSourceTest, AnalyzeIsSolidScaled)
    {
        gfx::Size layer_bounds(400, 400);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);

        SkColor solid_color = SkColorSetARGB(255, 12, 23, 34);
        SkPaint solid_paint;
        solid_paint.setColor(solid_color);

        SkColor non_solid_color = SkColorSetARGB(128, 45, 56, 67);
        SkPaint non_solid_paint;
        non_solid_paint.setColor(non_solid_color);

        recording_source->add_draw_rect_with_paint(gfx::Rect(0, 0, 400, 400),
            solid_paint);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        // Ensure everything is solid.
        for (int y = 0; y <= 30; y += 10) {
            for (int x = 0; x <= 30; x += 10) {
                RasterSource::SolidColorAnalysis analysis;
                gfx::Rect rect(x, y, 10, 10);
                raster->PerformSolidColorAnalysis(rect, 0.1f, &analysis);
                EXPECT_TRUE(analysis.is_solid_color) << rect.ToString();
                EXPECT_EQ(analysis.solid_color, solid_color) << rect.ToString();
            }
        }

        // Add one non-solid pixel and recreate the raster source.
        recording_source->add_draw_rect_with_paint(gfx::Rect(50, 50, 1, 1),
            non_solid_paint);
        recording_source->Rerecord();
        raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        RasterSource::SolidColorAnalysis analysis;
        raster->PerformSolidColorAnalysis(gfx::Rect(0, 0, 10, 10), 0.1f, &analysis);
        EXPECT_FALSE(analysis.is_solid_color);

        raster->PerformSolidColorAnalysis(gfx::Rect(10, 0, 10, 10), 0.1f, &analysis);
        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(analysis.solid_color, solid_color);

        // Boundaries should be clipped.
        analysis.is_solid_color = false;
        raster->PerformSolidColorAnalysis(gfx::Rect(35, 0, 10, 10), 0.1f, &analysis);
        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(analysis.solid_color, solid_color);

        analysis.is_solid_color = false;
        raster->PerformSolidColorAnalysis(gfx::Rect(0, 35, 10, 10), 0.1f, &analysis);
        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(analysis.solid_color, solid_color);

        analysis.is_solid_color = false;
        raster->PerformSolidColorAnalysis(gfx::Rect(35, 35, 10, 10), 0.1f, &analysis);
        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(analysis.solid_color, solid_color);
    }

    TEST(DisplayListRasterSourceTest, AnalyzeIsSolidEmpty)
    {
        gfx::Size layer_bounds(400, 400);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);
        RasterSource::SolidColorAnalysis analysis;
        EXPECT_FALSE(analysis.is_solid_color);

        raster->PerformSolidColorAnalysis(gfx::Rect(0, 0, 400, 400), 1.f, &analysis);

        EXPECT_TRUE(analysis.is_solid_color);
        EXPECT_EQ(analysis.solid_color, SkColorSetARGB(0, 0, 0, 0));
    }

    TEST(DisplayListRasterSourceTest, PixelRefIteratorDiscardableRefsOneTile)
    {
        gfx::Size layer_bounds(512, 512);

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);

        skia::RefPtr<SkImage> discardable_image[2][2];
        discardable_image[0][0] = CreateDiscardableImage(gfx::Size(32, 32));
        discardable_image[0][1] = CreateDiscardableImage(gfx::Size(32, 32));
        discardable_image[1][1] = CreateDiscardableImage(gfx::Size(32, 32));

        // Discardable pixel refs are found in the following cells:
        // |---|---|
        // | x | x |
        // |---|---|
        // |   | x |
        // |---|---|
        recording_source->add_draw_image(discardable_image[0][0].get(),
            gfx::Point(0, 0));
        recording_source->add_draw_image(discardable_image[0][1].get(),
            gfx::Point(260, 0));
        recording_source->add_draw_image(discardable_image[1][1].get(),
            gfx::Point(260, 260));
        recording_source->SetGenerateDiscardableImagesMetadata(true);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        // Tile sized iterators. These should find only one pixel ref.
        {
            std::vector<PositionImage> images;
            raster->GetDiscardableImagesInRect(gfx::Rect(0, 0, 256, 256), &images);
            EXPECT_EQ(1u, images.size());
            EXPECT_EQ(discardable_image[0][0].get(), images[0].image);
            EXPECT_EQ(gfx::RectF(32, 32).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
        }
        // Shifted tile sized iterators. These should find only one pixel ref.
        {
            std::vector<PositionImage> images;
            raster->GetDiscardableImagesInRect(gfx::Rect(260, 260, 256, 256), &images);
            EXPECT_EQ(1u, images.size());
            EXPECT_EQ(discardable_image[1][1].get(), images[0].image);
            EXPECT_EQ(gfx::RectF(260, 260, 32, 32).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
        }
        // Ensure there's no discardable pixel refs in the empty cell
        {
            std::vector<PositionImage> images;
            raster->GetDiscardableImagesInRect(gfx::Rect(0, 256, 256, 256), &images);
            EXPECT_EQ(0u, images.size());
        }
        // Layer sized iterators. These should find three pixel ref.
        {
            std::vector<PositionImage> images;
            raster->GetDiscardableImagesInRect(gfx::Rect(0, 0, 512, 512), &images);
            EXPECT_EQ(3u, images.size());
            EXPECT_EQ(discardable_image[0][0].get(), images[0].image);
            EXPECT_EQ(discardable_image[0][1].get(), images[1].image);
            EXPECT_EQ(discardable_image[1][1].get(), images[2].image);
            EXPECT_EQ(gfx::RectF(32, 32).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
            EXPECT_EQ(gfx::RectF(260, 0, 32, 32).ToString(),
                gfx::SkRectToRectF(images[1].image_rect).ToString());
            EXPECT_EQ(gfx::RectF(260, 260, 32, 32).ToString(),
                gfx::SkRectToRectF(images[2].image_rect).ToString());
        }
    }

    TEST(DisplayListRasterSourceTest, RasterFullContents)
    {
        gfx::Size layer_bounds(3, 5);
        float contents_scale = 1.5f;
        float raster_divisions = 2.f;

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);
        recording_source->SetBackgroundColor(SK_ColorBLACK);
        recording_source->SetClearCanvasWithDebugColor(false);

        // Because the caller sets content opaque, it also promises that it
        // has at least filled in layer_bounds opaquely.
        SkPaint white_paint;
        white_paint.setColor(SK_ColorWHITE);
        recording_source->add_draw_rect_with_paint(gfx::Rect(layer_bounds),
            white_paint);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        gfx::Size content_bounds(
            gfx::ScaleToCeiledSize(layer_bounds, contents_scale));

        // Simulate drawing into different tiles at different offsets.
        int step_x = std::ceil(content_bounds.width() / raster_divisions);
        int step_y = std::ceil(content_bounds.height() / raster_divisions);
        for (int offset_x = 0; offset_x < content_bounds.width();
             offset_x += step_x) {
            for (int offset_y = 0; offset_y < content_bounds.height();
                 offset_y += step_y) {
                gfx::Rect content_rect(offset_x, offset_y, step_x, step_y);
                content_rect.Intersect(gfx::Rect(content_bounds));

                // Simulate a canvas rect larger than the content rect.  Every pixel
                // up to one pixel outside the content rect is guaranteed to be opaque.
                // Outside of that is undefined.
                gfx::Rect canvas_rect(content_rect);
                canvas_rect.Inset(0, 0, -1, -1);

                SkBitmap bitmap;
                bitmap.allocN32Pixels(canvas_rect.width(), canvas_rect.height());
                SkCanvas canvas(bitmap);
                canvas.clear(SK_ColorTRANSPARENT);

                raster->PlaybackToCanvas(&canvas, canvas_rect, canvas_rect,
                    contents_scale);

                SkColor* pixels = reinterpret_cast<SkColor*>(bitmap.getPixels());
                int num_pixels = bitmap.width() * bitmap.height();
                bool all_white = true;
                for (int i = 0; i < num_pixels; ++i) {
                    EXPECT_EQ(SkColorGetA(pixels[i]), 255u);
                    all_white &= (SkColorGetR(pixels[i]) == 255);
                    all_white &= (SkColorGetG(pixels[i]) == 255);
                    all_white &= (SkColorGetB(pixels[i]) == 255);
                }

                // If the canvas doesn't extend past the edge of the content,
                // it should be entirely white. Otherwise, the edge of the content
                // will be non-white.
                EXPECT_EQ(all_white, gfx::Rect(content_bounds).Contains(canvas_rect));
            }
        }
    }

    TEST(DisplayListRasterSourceTest, RasterPartialContents)
    {
        gfx::Size layer_bounds(3, 5);
        float contents_scale = 1.5f;

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);
        recording_source->SetBackgroundColor(SK_ColorGREEN);
        recording_source->SetClearCanvasWithDebugColor(false);

        // First record everything as white.
        SkPaint white_paint;
        white_paint.setColor(SK_ColorWHITE);
        recording_source->add_draw_rect_with_paint(gfx::Rect(layer_bounds),
            white_paint);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        gfx::Size content_bounds(
            gfx::ScaleToCeiledSize(layer_bounds, contents_scale));

        SkBitmap bitmap;
        bitmap.allocN32Pixels(content_bounds.width(), content_bounds.height());
        SkCanvas canvas(bitmap);
        canvas.clear(SK_ColorTRANSPARENT);

        // Playback the full rect which should make everything white.
        gfx::Rect raster_full_rect(content_bounds);
        gfx::Rect playback_rect(content_bounds);
        raster->PlaybackToCanvas(&canvas, raster_full_rect, playback_rect,
            contents_scale);

        {
            SkColor* pixels = reinterpret_cast<SkColor*>(bitmap.getPixels());
            for (int i = 0; i < bitmap.width(); ++i) {
                for (int j = 0; j < bitmap.height(); ++j) {
                    SCOPED_TRACE(i);
                    SCOPED_TRACE(j);
                    EXPECT_EQ(255u, SkColorGetA(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(255u, SkColorGetR(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(255u, SkColorGetG(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(255u, SkColorGetB(pixels[i + j * bitmap.width()]));
                }
            }
        }

        // Re-record everything as black.
        SkPaint black_paint;
        black_paint.setColor(SK_ColorBLACK);
        recording_source->add_draw_rect_with_paint(gfx::Rect(layer_bounds),
            black_paint);
        recording_source->Rerecord();

        // Make a new RasterSource from the new recording.
        raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        // We're going to playback from "everything is black" into a smaller area,
        // that touches the edge pixels of the recording.
        playback_rect.Inset(1, 2, 0, 1);
        raster->PlaybackToCanvas(&canvas, raster_full_rect, playback_rect,
            contents_scale);

        SkColor* pixels = reinterpret_cast<SkColor*>(bitmap.getPixels());
        int num_black = 0;
        int num_white = 0;
        for (int i = 0; i < bitmap.width(); ++i) {
            for (int j = 0; j < bitmap.height(); ++j) {
                SCOPED_TRACE(j);
                SCOPED_TRACE(i);
                bool expect_black = playback_rect.Contains(i, j);
                if (expect_black) {
                    EXPECT_EQ(255u, SkColorGetA(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(0u, SkColorGetR(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(0u, SkColorGetG(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(0u, SkColorGetB(pixels[i + j * bitmap.width()]));
                    ++num_black;
                } else {
                    EXPECT_EQ(255u, SkColorGetA(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(255u, SkColorGetR(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(255u, SkColorGetG(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(255u, SkColorGetB(pixels[i + j * bitmap.width()]));
                    ++num_white;
                }
            }
        }
        EXPECT_GT(num_black, 0);
        EXPECT_GT(num_white, 0);
    }

    TEST(DisplayListRasterSourceTest, RasterPartialClear)
    {
        gfx::Size layer_bounds(3, 5);
        gfx::Size partial_bounds(2, 4);
        float contents_scale = 1.5f;

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);
        recording_source->SetBackgroundColor(SK_ColorGREEN);
        recording_source->SetRequiresClear(true);
        recording_source->SetClearCanvasWithDebugColor(false);

        // First record everything as white.
        const unsigned alpha_dark = 10u;
        SkPaint white_paint;
        white_paint.setColor(SK_ColorWHITE);
        white_paint.setAlpha(alpha_dark);
        recording_source->add_draw_rect_with_paint(gfx::Rect(layer_bounds),
            white_paint);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);

        gfx::Size content_bounds(
            gfx::ScaleToCeiledSize(layer_bounds, contents_scale));

        SkBitmap bitmap;
        bitmap.allocN32Pixels(content_bounds.width(), content_bounds.height());
        SkCanvas canvas(bitmap);
        canvas.clear(SK_ColorTRANSPARENT);

        // Playback the full rect which should make everything light gray (alpha=10).
        gfx::Rect raster_full_rect(content_bounds);
        gfx::Rect playback_rect(content_bounds);
        raster->PlaybackToCanvas(&canvas, raster_full_rect, playback_rect,
            contents_scale);

        {
            SkColor* pixels = reinterpret_cast<SkColor*>(bitmap.getPixels());
            for (int i = 0; i < bitmap.width(); ++i) {
                for (int j = 0; j < bitmap.height(); ++j) {
                    SCOPED_TRACE(i);
                    SCOPED_TRACE(j);
                    EXPECT_EQ(alpha_dark, SkColorGetA(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(alpha_dark, SkColorGetR(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(alpha_dark, SkColorGetG(pixels[i + j * bitmap.width()]));
                    EXPECT_EQ(alpha_dark, SkColorGetB(pixels[i + j * bitmap.width()]));
                }
            }
        }

        scoped_ptr<FakeDisplayListRecordingSource> recording_source_light = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);
        recording_source_light->SetBackgroundColor(SK_ColorGREEN);
        recording_source_light->SetRequiresClear(true);
        recording_source_light->SetClearCanvasWithDebugColor(false);

        // Record everything as a slightly lighter white.
        const unsigned alpha_light = 18u;
        white_paint.setAlpha(alpha_light);
        recording_source_light->add_draw_rect_with_paint(gfx::Rect(layer_bounds),
            white_paint);
        recording_source_light->Rerecord();

        // Make a new RasterSource from the new recording.
        raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source_light.get(), false);

        // We're going to playback from alpha(18) white rectangle into a smaller area
        // of the recording resulting in a smaller lighter white rectangle over a
        // darker white background rectangle.
        playback_rect = gfx::Rect(gfx::ScaleToCeiledSize(partial_bounds, contents_scale));
        raster->PlaybackToCanvas(&canvas, raster_full_rect, playback_rect,
            contents_scale);

        // Test that the whole playback_rect was cleared and repainted with new alpha.
        SkColor* pixels = reinterpret_cast<SkColor*>(bitmap.getPixels());
        for (int i = 0; i < playback_rect.width(); ++i) {
            for (int j = 0; j < playback_rect.height(); ++j) {
                SCOPED_TRACE(j);
                SCOPED_TRACE(i);
                EXPECT_EQ(alpha_light, SkColorGetA(pixels[i + j * bitmap.width()]));
                EXPECT_EQ(alpha_light, SkColorGetR(pixels[i + j * bitmap.width()]));
                EXPECT_EQ(alpha_light, SkColorGetG(pixels[i + j * bitmap.width()]));
                EXPECT_EQ(alpha_light, SkColorGetB(pixels[i + j * bitmap.width()]));
            }
        }
    }

    TEST(DisplayListRasterSourceTest, RasterContentsTransparent)
    {
        gfx::Size layer_bounds(5, 3);
        float contents_scale = 0.5f;

        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);
        recording_source->SetBackgroundColor(SK_ColorTRANSPARENT);
        recording_source->SetRequiresClear(true);
        recording_source->SetClearCanvasWithDebugColor(false);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);
        gfx::Size content_bounds(
            gfx::ScaleToCeiledSize(layer_bounds, contents_scale));

        gfx::Rect canvas_rect(content_bounds);
        canvas_rect.Inset(0, 0, -1, -1);

        SkBitmap bitmap;
        bitmap.allocN32Pixels(canvas_rect.width(), canvas_rect.height());
        SkCanvas canvas(bitmap);

        raster->PlaybackToCanvas(&canvas, canvas_rect, canvas_rect, contents_scale);

        SkColor* pixels = reinterpret_cast<SkColor*>(bitmap.getPixels());
        int num_pixels = bitmap.width() * bitmap.height();
        for (int i = 0; i < num_pixels; ++i) {
            EXPECT_EQ(SkColorGetA(pixels[i]), 0u);
        }
    }

    TEST(DisplayListRasterSourceTest,
        GetPictureMemoryUsageIncludesClientReportedMemory)
    {
        const size_t kReportedMemoryUsageInBytes = 100 * 1024 * 1024;
        gfx::Size layer_bounds(5, 3);
        scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(layer_bounds);
        recording_source->set_reported_memory_usage(kReportedMemoryUsageInBytes);
        recording_source->Rerecord();

        scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
            recording_source.get(), false);
        size_t total_memory_usage = raster->GetPictureMemoryUsage();
        EXPECT_GE(total_memory_usage, kReportedMemoryUsageInBytes);
        EXPECT_LT(total_memory_usage, 2 * kReportedMemoryUsageInBytes);
    }

} // namespace
} // namespace cc
