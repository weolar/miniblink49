// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/pixel_ref_map.h"

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/values.h"
#include "cc/playback/picture.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/skia_common.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/skia_util.h"

namespace cc {
namespace {

    TEST(PixelRefMapTest, PixelRefMapIterator)
    {
        gfx::Rect layer_rect(2048, 2048);

        gfx::Size tile_grid_size(512, 512);

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
        SkBitmap discardable_bitmap[4][4];
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if ((x + y) & 1) {
                    CreateBitmap(gfx::Size(500, 500), "discardable",
                        &discardable_bitmap[y][x]);
                    SkPaint paint;
                    content_layer_client.add_draw_bitmap(
                        discardable_bitmap[y][x], gfx::Point(x * 512 + 6, y * 512 + 6),
                        paint);
                }
            }
        }

        scoped_refptr<Picture> picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, true,
            RecordingSource::RECORD_NORMALLY);

        // Default iterator does not have any pixel refs.
        {
            PixelRefMap::Iterator iterator;
            EXPECT_FALSE(iterator);
        }

        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                PixelRefMap::Iterator iterator(gfx::Rect(x * 512, y * 512, 500, 500),
                    picture.get());
                if ((x + y) & 1) {
                    EXPECT_TRUE(iterator) << x << " " << y;
                    EXPECT_TRUE(*iterator == discardable_bitmap[y][x].pixelRef())
                        << x << " " << y;
                    EXPECT_FALSE(++iterator) << x << " " << y;
                } else {
                    EXPECT_FALSE(iterator) << x << " " << y;
                }
            }
        }
        // Capture 4 pixel refs.
        {
            PixelRefMap::Iterator iterator(gfx::Rect(512, 512, 2048, 2048),
                picture.get());
            EXPECT_TRUE(iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[1][2].pixelRef());
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[2][1].pixelRef());
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[2][3].pixelRef());
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[3][2].pixelRef());
            EXPECT_FALSE(++iterator);
        }

        // Copy test.
        PixelRefMap::Iterator iterator(gfx::Rect(512, 512, 2048, 2048),
            picture.get());
        EXPECT_TRUE(iterator);
        EXPECT_TRUE(*iterator == discardable_bitmap[1][2].pixelRef());
        EXPECT_TRUE(++iterator);
        EXPECT_TRUE(*iterator == discardable_bitmap[2][1].pixelRef());

        // copy now points to the same spot as iterator,
        // but both can be incremented independently.
        PixelRefMap::Iterator copy = iterator;
        EXPECT_TRUE(++iterator);
        EXPECT_TRUE(*iterator == discardable_bitmap[2][3].pixelRef());
        EXPECT_TRUE(++iterator);
        EXPECT_TRUE(*iterator == discardable_bitmap[3][2].pixelRef());
        EXPECT_FALSE(++iterator);

        EXPECT_TRUE(copy);
        EXPECT_TRUE(*copy == discardable_bitmap[2][1].pixelRef());
        EXPECT_TRUE(++copy);
        EXPECT_TRUE(*copy == discardable_bitmap[2][3].pixelRef());
        EXPECT_TRUE(++copy);
        EXPECT_TRUE(*copy == discardable_bitmap[3][2].pixelRef());
        EXPECT_FALSE(++copy);
    }

    TEST(PixelRefMapTest, PixelRefMapIteratorNonZeroLayer)
    {
        gfx::Rect layer_rect(1024, 0, 2048, 2048);

        gfx::Size tile_grid_size(512, 512);

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
        SkBitmap discardable_bitmap[4][4];
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if ((x + y) & 1) {
                    CreateBitmap(gfx::Size(500, 500), "discardable",
                        &discardable_bitmap[y][x]);
                    SkPaint paint;
                    content_layer_client.add_draw_bitmap(
                        discardable_bitmap[y][x],
                        gfx::Point(1024 + x * 512 + 6, y * 512 + 6), paint);
                }
            }
        }

        scoped_refptr<Picture> picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, true,
            RecordingSource::RECORD_NORMALLY);

        // Default iterator does not have any pixel refs.
        {
            PixelRefMap::Iterator iterator;
            EXPECT_FALSE(iterator);
        }

        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                PixelRefMap::Iterator iterator(
                    gfx::Rect(1024 + x * 512, y * 512, 500, 500), picture.get());
                if ((x + y) & 1) {
                    EXPECT_TRUE(iterator) << x << " " << y;
                    EXPECT_TRUE(*iterator == discardable_bitmap[y][x].pixelRef());
                    EXPECT_FALSE(++iterator) << x << " " << y;
                } else {
                    EXPECT_FALSE(iterator) << x << " " << y;
                }
            }
        }
        // Capture 4 pixel refs.
        {
            PixelRefMap::Iterator iterator(gfx::Rect(1024 + 512, 512, 2048, 2048),
                picture.get());
            EXPECT_TRUE(iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[1][2].pixelRef());
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[2][1].pixelRef());
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[2][3].pixelRef());
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[3][2].pixelRef());
            EXPECT_FALSE(++iterator);
        }

        // Copy test.
        {
            PixelRefMap::Iterator iterator(gfx::Rect(1024 + 512, 512, 2048, 2048),
                picture.get());
            EXPECT_TRUE(iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[1][2].pixelRef());
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[2][1].pixelRef());

            // copy now points to the same spot as iterator,
            // but both can be incremented independently.
            PixelRefMap::Iterator copy = iterator;
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[2][3].pixelRef());
            EXPECT_TRUE(++iterator);
            EXPECT_TRUE(*iterator == discardable_bitmap[3][2].pixelRef());
            EXPECT_FALSE(++iterator);

            EXPECT_TRUE(copy);
            EXPECT_TRUE(*copy == discardable_bitmap[2][1].pixelRef());
            EXPECT_TRUE(++copy);
            EXPECT_TRUE(*copy == discardable_bitmap[2][3].pixelRef());
            EXPECT_TRUE(++copy);
            EXPECT_TRUE(*copy == discardable_bitmap[3][2].pixelRef());
            EXPECT_FALSE(++copy);
        }

        // Non intersecting rects
        {
            PixelRefMap::Iterator iterator(gfx::Rect(0, 0, 1000, 1000), picture.get());
            EXPECT_FALSE(iterator);
        }
        {
            PixelRefMap::Iterator iterator(gfx::Rect(3500, 0, 1000, 1000),
                picture.get());
            EXPECT_FALSE(iterator);
        }
        {
            PixelRefMap::Iterator iterator(gfx::Rect(0, 1100, 1000, 1000),
                picture.get());
            EXPECT_FALSE(iterator);
        }
        {
            PixelRefMap::Iterator iterator(gfx::Rect(3500, 1100, 1000, 1000),
                picture.get());
            EXPECT_FALSE(iterator);
        }
    }

    TEST(PixelRefMapTest, PixelRefMapIteratorOnePixelQuery)
    {
        gfx::Rect layer_rect(2048, 2048);

        gfx::Size tile_grid_size(512, 512);

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
        SkBitmap discardable_bitmap[4][4];
        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                if ((x + y) & 1) {
                    CreateBitmap(gfx::Size(500, 500), "discardable",
                        &discardable_bitmap[y][x]);
                    SkPaint paint;
                    content_layer_client.add_draw_bitmap(
                        discardable_bitmap[y][x], gfx::Point(x * 512 + 6, y * 512 + 6),
                        paint);
                }
            }
        }

        scoped_refptr<Picture> picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, true,
            RecordingSource::RECORD_NORMALLY);

        // Default iterator does not have any pixel refs.
        {
            PixelRefMap::Iterator iterator;
            EXPECT_FALSE(iterator);
        }

        for (int y = 0; y < 4; ++y) {
            for (int x = 0; x < 4; ++x) {
                PixelRefMap::Iterator iterator(gfx::Rect(x * 512, y * 512 + 256, 1, 1),
                    picture.get());
                if ((x + y) & 1) {
                    EXPECT_TRUE(iterator) << x << " " << y;
                    EXPECT_TRUE(*iterator == discardable_bitmap[y][x].pixelRef());
                    EXPECT_FALSE(++iterator) << x << " " << y;
                } else {
                    EXPECT_FALSE(iterator) << x << " " << y;
                }
            }
        }
    }

} // namespace
} // namespace cc
