// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#include "cc/layers/content_layer_client.h"
#include "cc/layers/picture_image_layer.h"
#include "cc/layers/picture_layer.h"
#include "cc/layers/solid_color_layer.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/layer_tree_pixel_test.h"
#include "cc/test/pixel_comparator.h"

#if !defined(OS_ANDROID)

namespace cc {
namespace {

    class LayerTreeHostSynchronousPixelTest : public LayerTreePixelTest {
    public:
        void InitializeSettings(LayerTreeSettings* settings) override
        {
            LayerTreePixelTest::InitializeSettings(settings);
            settings->single_thread_proxy_scheduler = false;
            settings->use_zero_copy = true;
        }

        void BeginTest() override
        {
            LayerTreePixelTest::BeginTest();
            PostCompositeImmediatelyToMainThread();
        }
    };

    TEST_F(LayerTreeHostSynchronousPixelTest, OneContentLayer)
    {
        gfx::Size bounds(200, 200);

        FakeContentLayerClient client;
        SkPaint green_paint;
        green_paint.setColor(SkColorSetARGB(255, 0, 255, 0));
        client.add_draw_rect(gfx::Rect(bounds), green_paint);
        scoped_refptr<PictureLayer> root = PictureLayer::Create(layer_settings(), &client);
        root->SetBounds(bounds);
        root->SetIsDrawable(true);

        RunSingleThreadedPixelTest(
            PIXEL_TEST_GL, root, base::FilePath(FILE_PATH_LITERAL("green.png")));
    }

    class LayerTreeHostSynchronousGPUPixelTest : public LayerTreePixelTest {
    public:
        void InitializeSettings(LayerTreeSettings* settings) override
        {
            LayerTreePixelTest::InitializeSettings(settings);
            settings->single_thread_proxy_scheduler = false;
            settings->gpu_rasterization_enabled = true;
            settings->gpu_rasterization_forced = true;
        }

        void BeginTest() override
        {
            LayerTreePixelTest::BeginTest();
            PostCompositeImmediatelyToMainThread();
        }
    };

    TEST_F(LayerTreeHostSynchronousGPUPixelTest, OneContentLayer)
    {
        gfx::Size bounds(200, 200);

        FakeContentLayerClient client;
        SkPaint green_paint;
        green_paint.setColor(SkColorSetARGB(255, 0, 255, 0));
        client.add_draw_rect(gfx::Rect(bounds), green_paint);
        scoped_refptr<PictureLayer> root = PictureLayer::Create(layer_settings(), &client);
        root->SetBounds(bounds);
        root->SetIsDrawable(true);

        RunSingleThreadedPixelTest(PIXEL_TEST_GL, root,
            base::FilePath(FILE_PATH_LITERAL("green.png")));
    }

} // namespace
} // namespace cc

#endif // OS_ANDROID
