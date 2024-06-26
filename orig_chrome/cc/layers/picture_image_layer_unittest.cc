// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/picture_image_layer.h"

#include "cc/playback/display_item.h"
#include "cc/test/skia_common.h"
#include "cc/trees/layer_tree_settings.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace cc {
namespace {

    TEST(PictureImageLayerTest, PaintContentsToDisplayList)
    {
        scoped_refptr<PictureImageLayer> layer = PictureImageLayer::Create(LayerSettings());
        gfx::Rect layer_rect(200, 200);

        unsigned char image_pixels[4 * 200 * 200] = { 0 };
        SkImageInfo info = SkImageInfo::MakeN32Premul(layer_rect.width(), layer_rect.height());
        skia::RefPtr<SkSurface> image_surface = skia::AdoptRef(
            SkSurface::NewRasterDirect(info, image_pixels, info.minRowBytes()));
        SkCanvas* image_canvas = image_surface->getCanvas();
        image_canvas->clear(SK_ColorRED);
        SkPaint blue_paint;
        blue_paint.setColor(SK_ColorBLUE);
        image_canvas->drawRectCoords(0.f, 0.f, 100.f, 100.f, blue_paint);
        image_canvas->drawRectCoords(100.f, 100.f, 200.f, 200.f, blue_paint);

        skia::RefPtr<const SkImage> image = skia::AdoptRef(image_surface->newImageSnapshot());
        layer->SetImage(image.Pass());
        layer->SetBounds(gfx::Size(layer_rect.width(), layer_rect.height()));

        scoped_refptr<DisplayItemList> display_list = layer->PaintContentsToDisplayList(
            layer_rect, ContentLayerClient::PAINTING_BEHAVIOR_NORMAL);
        unsigned char actual_pixels[4 * 200 * 200] = { 0 };
        DrawDisplayList(actual_pixels, layer_rect, display_list);

        EXPECT_EQ(0, memcmp(actual_pixels, image_pixels, 4 * 200 * 200));
    }

} // namespace
} // namespace cc
