// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/discardable_image_map.h"

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/values.h"
#include "cc/base/region.h"
#include "cc/playback/raster_source.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/fake_display_list_recording_source.h"
#include "cc/test/skia_common.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "third_party/skia/include/core/SkImageGenerator.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/skia_util.h"

namespace cc {
namespace {

    class TestImageGenerator : public SkImageGenerator {
    public:
        explicit TestImageGenerator(const SkImageInfo& info)
            : SkImageGenerator(info)
        {
        }
    };

    skia::RefPtr<SkImage> CreateDiscardableImage(const gfx::Size& size)
    {
        const SkImageInfo info = SkImageInfo::MakeN32Premul(size.width(), size.height());
        return skia::AdoptRef(
            SkImage::NewFromGenerator(new TestImageGenerator(info)));
    }

    TEST(DiscardableImageMapTest, GetDiscardableImagesInRect)
    {
        gfx::Rect visible_rect(2048, 2048);
        FakeContentLayerClient content_layer_client;

        // Discardable pixel refs are found in the following grids:
        // |---|---|---|---|
        // |   | x |   | x |
        // |---|---|---|---|
        // | x |   | x |   |
        // |---|---|---|---|
        // |   | x |   | x |
        // |---|---|---|---|
        // | x |   | x |   |
        // |---|---|---|---|
        skia::RefPtr<SkImage> discardable_image[4][4];
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if ((x + y) & 1) {
                    discardable_image[y][x] = CreateDiscardableImage(gfx::Size(500, 500));
                    SkPaint paint;
                    content_layer_client.add_draw_image(
                        discardable_image[y][x].get(), gfx::Point(x * 512 + 6, y * 512 + 6),
                        paint);
                }
            }
        }

        FakeDisplayListRecordingSource recording_source;
        Region invalidation(visible_rect);
        recording_source.SetGenerateDiscardableImagesMetadata(true);
        recording_source.UpdateAndExpandInvalidation(
            &content_layer_client, &invalidation, visible_rect.size(), visible_rect,
            1, RecordingSource::RECORD_NORMALLY);
        DisplayItemList* display_list = recording_source.display_list();

        DiscardableImageMap image_map;
        {
            DiscardableImageMap::ScopedMetadataGenerator generator(&image_map,
                visible_rect.size());
            display_list->Raster(generator.canvas(), nullptr, visible_rect, 1.f);
        }

        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                std::vector<PositionImage> images;
                image_map.GetDiscardableImagesInRect(
                    gfx::Rect(x * 512, y * 512, 500, 500), &images);
                if ((x + y) & 1) {
                    EXPECT_EQ(1u, images.size()) << x << " " << y;
                    EXPECT_TRUE(images[0].image == discardable_image[y][x].get())
                        << x << " " << y;
                    EXPECT_EQ(gfx::RectF(x * 512 + 6, y * 512 + 6, 500, 500).ToString(),
                        gfx::SkRectToRectF(images[0].image_rect).ToString());
                } else {
                    EXPECT_EQ(0u, images.size()) << x << " " << y;
                }
            }
        }

        // Capture 4 pixel refs.
        std::vector<PositionImage> images;
        image_map.GetDiscardableImagesInRect(gfx::Rect(512, 512, 2048, 2048),
            &images);
        EXPECT_EQ(4u, images.size());
        EXPECT_TRUE(images[0].image == discardable_image[1][2].get());
        EXPECT_EQ(gfx::RectF(2 * 512 + 6, 512 + 6, 500, 500).ToString(),
            gfx::SkRectToRectF(images[0].image_rect).ToString());
        EXPECT_TRUE(images[1].image == discardable_image[2][1].get());
        EXPECT_EQ(gfx::RectF(512 + 6, 2 * 512 + 6, 500, 500).ToString(),
            gfx::SkRectToRectF(images[1].image_rect).ToString());
        EXPECT_TRUE(images[2].image == discardable_image[2][3].get());
        EXPECT_EQ(gfx::RectF(3 * 512 + 6, 2 * 512 + 6, 500, 500).ToString(),
            gfx::SkRectToRectF(images[2].image_rect).ToString());
        EXPECT_TRUE(images[3].image == discardable_image[3][2].get());
        EXPECT_EQ(gfx::RectF(2 * 512 + 6, 3 * 512 + 6, 500, 500).ToString(),
            gfx::SkRectToRectF(images[3].image_rect).ToString());
    }

    TEST(DiscardableImageMapTest, GetDiscardableImagesInRectNonZeroLayer)
    {
        gfx::Rect visible_rect(1024, 0, 2048, 2048);
        // Make sure visible rect fits into the layer size.
        gfx::Size layer_size(visible_rect.right(), visible_rect.bottom());
        FakeContentLayerClient content_layer_client;

        // Discardable pixel refs are found in the following grids:
        // |---|---|---|---|
        // |   | x |   | x |
        // |---|---|---|---|
        // | x |   | x |   |
        // |---|---|---|---|
        // |   | x |   | x |
        // |---|---|---|---|
        // | x |   | x |   |
        // |---|---|---|---|
        skia::RefPtr<SkImage> discardable_image[4][4];
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if ((x + y) & 1) {
                    discardable_image[y][x] = CreateDiscardableImage(gfx::Size(500, 500));
                    SkPaint paint;
                    content_layer_client.add_draw_image(
                        discardable_image[y][x].get(),
                        gfx::Point(1024 + x * 512 + 6, y * 512 + 6), paint);
                }
            }
        }

        FakeDisplayListRecordingSource recording_source;
        Region invalidation(visible_rect);
        recording_source.set_pixel_record_distance(0);
        recording_source.SetGenerateDiscardableImagesMetadata(true);
        recording_source.UpdateAndExpandInvalidation(
            &content_layer_client, &invalidation, layer_size, visible_rect, 1,
            RecordingSource::RECORD_NORMALLY);
        DisplayItemList* display_list = recording_source.display_list();

        DiscardableImageMap image_map;
        {
            DiscardableImageMap::ScopedMetadataGenerator generator(&image_map,
                layer_size);
            display_list->Raster(generator.canvas(), nullptr, visible_rect, 1.f);
        }

        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                std::vector<PositionImage> images;
                image_map.GetDiscardableImagesInRect(
                    gfx::Rect(1024 + x * 512, y * 512, 500, 500), &images);
                if ((x + y) & 1) {
                    EXPECT_EQ(1u, images.size()) << x << " " << y;
                    EXPECT_TRUE(images[0].image == discardable_image[y][x].get())
                        << x << " " << y;
                    EXPECT_EQ(
                        gfx::RectF(1024 + x * 512 + 6, y * 512 + 6, 500, 500).ToString(),
                        gfx::SkRectToRectF(images[0].image_rect).ToString());
                } else {
                    EXPECT_EQ(0u, images.size()) << x << " " << y;
                }
            }
        }
        // Capture 4 pixel refs.
        {
            std::vector<PositionImage> images;
            image_map.GetDiscardableImagesInRect(gfx::Rect(1024 + 512, 512, 2048, 2048),
                &images);
            EXPECT_EQ(4u, images.size());
            EXPECT_TRUE(images[0].image == discardable_image[1][2].get());
            EXPECT_EQ(gfx::RectF(1024 + 2 * 512 + 6, 512 + 6, 500, 500).ToString(),
                gfx::SkRectToRectF(images[0].image_rect).ToString());
            EXPECT_TRUE(images[1].image == discardable_image[2][1].get());
            EXPECT_EQ(gfx::RectF(1024 + 512 + 6, 2 * 512 + 6, 500, 500).ToString(),
                gfx::SkRectToRectF(images[1].image_rect).ToString());
            EXPECT_TRUE(images[2].image == discardable_image[2][3].get());
            EXPECT_EQ(gfx::RectF(1024 + 3 * 512 + 6, 2 * 512 + 6, 500, 500).ToString(),
                gfx::SkRectToRectF(images[2].image_rect).ToString());
            EXPECT_TRUE(images[3].image == discardable_image[3][2].get());
            EXPECT_EQ(gfx::RectF(1024 + 2 * 512 + 6, 3 * 512 + 6, 500, 500).ToString(),
                gfx::SkRectToRectF(images[3].image_rect).ToString());
        }

        // Non intersecting rects
        {
            std::vector<PositionImage> images;
            image_map.GetDiscardableImagesInRect(gfx::Rect(0, 0, 1000, 1000), &images);
            EXPECT_EQ(0u, images.size());
        }
        {
            std::vector<PositionImage> images;
            image_map.GetDiscardableImagesInRect(gfx::Rect(3500, 0, 1000, 1000),
                &images);
            EXPECT_EQ(0u, images.size());
        }
        {
            std::vector<PositionImage> images;
            image_map.GetDiscardableImagesInRect(gfx::Rect(0, 1100, 1000, 1000),
                &images);
            EXPECT_EQ(0u, images.size());
        }
        {
            std::vector<PositionImage> images;
            image_map.GetDiscardableImagesInRect(gfx::Rect(3500, 1100, 1000, 1000),
                &images);
            EXPECT_EQ(0u, images.size());
        }
    }

    TEST(DiscardableImageMapTest, GetDiscardableImagesInRectOnePixelQuery)
    {
        gfx::Rect visible_rect(2048, 2048);
        FakeContentLayerClient content_layer_client;

        // Discardable pixel refs are found in the following grids:
        // |---|---|---|---|
        // |   | x |   | x |
        // |---|---|---|---|
        // | x |   | x |   |
        // |---|---|---|---|
        // |   | x |   | x |
        // |---|---|---|---|
        // | x |   | x |   |
        // |---|---|---|---|
        skia::RefPtr<SkImage> discardable_image[4][4];
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if ((x + y) & 1) {
                    discardable_image[y][x] = CreateDiscardableImage(gfx::Size(500, 500));
                    SkPaint paint;
                    content_layer_client.add_draw_image(
                        discardable_image[y][x].get(), gfx::Point(x * 512 + 6, y * 512 + 6),
                        paint);
                }
            }
        }

        FakeDisplayListRecordingSource recording_source;
        Region invalidation(visible_rect);
        recording_source.SetGenerateDiscardableImagesMetadata(true);
        recording_source.UpdateAndExpandInvalidation(
            &content_layer_client, &invalidation, visible_rect.size(), visible_rect,
            1, RecordingSource::RECORD_NORMALLY);
        DisplayItemList* display_list = recording_source.display_list();

        DiscardableImageMap image_map;
        {
            DiscardableImageMap::ScopedMetadataGenerator generator(&image_map,
                visible_rect.size());
            display_list->Raster(generator.canvas(), nullptr, visible_rect, 1.f);
        }

        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                std::vector<PositionImage> images;
                image_map.GetDiscardableImagesInRect(
                    gfx::Rect(x * 512 + 256, y * 512 + 256, 1, 1), &images);
                if ((x + y) & 1) {
                    EXPECT_EQ(1u, images.size()) << x << " " << y;
                    EXPECT_TRUE(images[0].image == discardable_image[y][x].get())
                        << x << " " << y;
                    EXPECT_EQ(gfx::RectF(x * 512 + 6, y * 512 + 6, 500, 500).ToString(),
                        gfx::SkRectToRectF(images[0].image_rect).ToString());
                } else {
                    EXPECT_EQ(0u, images.size()) << x << " " << y;
                }
            }
        }
    }

    TEST(DiscardableImageMapTest, GetDiscardableImagesInRectMassiveImage)
    {
        gfx::Rect visible_rect(2048, 2048);
        FakeContentLayerClient content_layer_client;

        skia::RefPtr<SkImage> discardable_image;
        discardable_image = CreateDiscardableImage(gfx::Size(1 << 25, 1 << 25));
        SkPaint paint;
        content_layer_client.add_draw_image(discardable_image.get(), gfx::Point(0, 0),
            paint);

        FakeDisplayListRecordingSource recording_source;
        Region invalidation(visible_rect);
        recording_source.SetGenerateDiscardableImagesMetadata(true);
        recording_source.UpdateAndExpandInvalidation(
            &content_layer_client, &invalidation, visible_rect.size(), visible_rect,
            1, RecordingSource::RECORD_NORMALLY);
        DisplayItemList* display_list = recording_source.display_list();

        DiscardableImageMap image_map;
        {
            DiscardableImageMap::ScopedMetadataGenerator generator(&image_map,
                visible_rect.size());
            display_list->Raster(generator.canvas(), nullptr, visible_rect, 1.f);
        }
        std::vector<PositionImage> images;
        image_map.GetDiscardableImagesInRect(gfx::Rect(0, 0, 1, 1), &images);
        EXPECT_EQ(1u, images.size());
        EXPECT_TRUE(images[0].image == discardable_image.get());
        EXPECT_EQ(gfx::RectF(0, 0, 1 << 25, 1 << 25).ToString(),
            gfx::SkRectToRectF(images[0].image_rect).ToString());
    }

} // namespace
} // namespace cc
