// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#include "cc/layers/solid_color_layer.h"
#include "cc/test/layer_tree_pixel_test.h"
#include "cc/test/pixel_comparator.h"
#include "third_party/skia/include/effects/SkColorFilterImageFilter.h"
#include "third_party/skia/include/effects/SkColorMatrixFilter.h"

#if !defined(OS_ANDROID)

namespace cc {
namespace {

    class LayerTreeHostFiltersPixelTest : public LayerTreePixelTest {
    };

    TEST_F(LayerTreeHostFiltersPixelTest, BackgroundFilterBlur)
    {
        scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(
            gfx::Rect(200, 200), SK_ColorWHITE);

        // The green box is entirely behind a layer with background blur, so it
        // should appear blurred on its edges.
        scoped_refptr<SolidColorLayer> green = CreateSolidColorLayer(
            gfx::Rect(50, 50, 100, 100), kCSSGreen);
        scoped_refptr<SolidColorLayer> blur = CreateSolidColorLayer(
            gfx::Rect(30, 30, 140, 140), SK_ColorTRANSPARENT);
        background->AddChild(green);
        background->AddChild(blur);

        FilterOperations filters;
        filters.Append(FilterOperation::CreateBlurFilter(2.f));
        blur->SetBackgroundFilters(filters);

#if defined(OS_WIN)
        // Windows has 436 pixels off by 1: crbug.com/259915
        float percentage_pixels_large_error = 1.09f; // 436px / (200*200)
        float percentage_pixels_small_error = 0.0f;
        float average_error_allowed_in_bad_pixels = 1.f;
        int large_error_allowed = 1;
        int small_error_allowed = 0;
        pixel_comparator_.reset(new FuzzyPixelComparator(
            true, // discard_alpha
            percentage_pixels_large_error,
            percentage_pixels_small_error,
            average_error_allowed_in_bad_pixels,
            large_error_allowed,
            small_error_allowed));
#endif

        RunPixelTest(PIXEL_TEST_GL,
            background,
            base::FilePath(FILE_PATH_LITERAL("background_filter_blur.png")));
    }

    TEST_F(LayerTreeHostFiltersPixelTest, BackgroundFilterBlurOutsets)
    {
        scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(
            gfx::Rect(200, 200), SK_ColorWHITE);

        // The green border is outside the layer with background blur, but the
        // background blur should use pixels from outside its layer borders, up to the
        // radius of the blur effect. So the border should be blurred underneath the
        // top layer causing the green to bleed under the transparent layer, but not
        // in the 1px region between the transparent layer and the green border.
        scoped_refptr<SolidColorLayer> green_border = CreateSolidColorLayerWithBorder(
            gfx::Rect(1, 1, 198, 198), SK_ColorWHITE, 10, kCSSGreen);
        scoped_refptr<SolidColorLayer> blur = CreateSolidColorLayer(
            gfx::Rect(12, 12, 176, 176), SK_ColorTRANSPARENT);
        background->AddChild(green_border);
        background->AddChild(blur);

        FilterOperations filters;
        filters.Append(FilterOperation::CreateBlurFilter(5.f));
        blur->SetBackgroundFilters(filters);

#if defined(OS_WIN)
        // Windows has 2596 pixels off by at most 2: crbug.com/259922
        float percentage_pixels_large_error = 6.5f; // 2596px / (200*200), rounded up
        float percentage_pixels_small_error = 0.0f;
        float average_error_allowed_in_bad_pixels = 1.f;
        int large_error_allowed = 2;
        int small_error_allowed = 0;
        pixel_comparator_.reset(new FuzzyPixelComparator(
            true, // discard_alpha
            percentage_pixels_large_error,
            percentage_pixels_small_error,
            average_error_allowed_in_bad_pixels,
            large_error_allowed,
            small_error_allowed));
#endif

        RunPixelTest(
            PIXEL_TEST_GL,
            background,
            base::FilePath(FILE_PATH_LITERAL("background_filter_blur_outsets.png")));
    }

    TEST_F(LayerTreeHostFiltersPixelTest, BackgroundFilterBlurOffAxis)
    {
        scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(gfx::Rect(200, 200), SK_ColorTRANSPARENT);

        // This verifies that the perspective of the clear layer (with black border)
        // does not influence the blending of the green box behind it. Also verifies
        // that the blur is correctly clipped inside the transformed clear layer.
        scoped_refptr<SolidColorLayer> green = CreateSolidColorLayer(
            gfx::Rect(50, 50, 100, 100), kCSSGreen);
        scoped_refptr<SolidColorLayer> blur = CreateSolidColorLayerWithBorder(
            gfx::Rect(30, 30, 120, 120), SK_ColorTRANSPARENT, 1, SK_ColorBLACK);
        background->AddChild(green);
        background->AddChild(blur);

        background->SetShouldFlattenTransform(false);
        background->Set3dSortingContextId(1);
        green->SetShouldFlattenTransform(false);
        green->Set3dSortingContextId(1);
        gfx::Transform background_transform;
        background_transform.ApplyPerspectiveDepth(200.0);
        background->SetTransform(background_transform);

        blur->SetShouldFlattenTransform(false);
        blur->Set3dSortingContextId(1);
        for (size_t i = 0; i < blur->children().size(); ++i)
            blur->children()[i]->Set3dSortingContextId(1);

        gfx::Transform blur_transform;
        blur_transform.Translate(55.0, 65.0);
        blur_transform.RotateAboutXAxis(85.0);
        blur_transform.RotateAboutYAxis(180.0);
        blur_transform.RotateAboutZAxis(20.0);
        blur_transform.Translate(-60.0, -60.0);
        blur->SetTransform(blur_transform);

        FilterOperations filters;
        filters.Append(FilterOperation::CreateBlurFilter(2.f));
        blur->SetBackgroundFilters(filters);

#if defined(OS_WIN)
        // Windows has 116 pixels off by at most 2: crbug.com/225027
        float percentage_pixels_large_error = 0.3f; // 116px / (200*200), rounded up
        float percentage_pixels_small_error = 0.0f;
        float average_error_allowed_in_bad_pixels = 1.f;
        int large_error_allowed = 2;
        int small_error_allowed = 0;
        pixel_comparator_.reset(new FuzzyPixelComparator(
            true, // discard_alpha
            percentage_pixels_large_error,
            percentage_pixels_small_error,
            average_error_allowed_in_bad_pixels,
            large_error_allowed,
            small_error_allowed));
#endif

        RunPixelTest(
            PIXEL_TEST_GL,
            background,
            base::FilePath(FILE_PATH_LITERAL("background_filter_blur_off_axis.png")));
    }

    class LayerTreeHostFiltersScaledPixelTest
        : public LayerTreeHostFiltersPixelTest {
        void InitializeSettings(LayerTreeSettings* settings) override
        {
            // Required so that device scale is inherited by content scale.
            settings->layer_transforms_should_scale_layer_contents = true;
        }

        void SetupTree() override
        {
            layer_tree_host()->SetDeviceScaleFactor(device_scale_factor_);
            LayerTreePixelTest::SetupTree();
        }

    protected:
        void RunPixelTestType(int content_size,
            float device_scale_factor,
            PixelTestType test_type)
        {
            int half_content = content_size / 2;

            scoped_refptr<SolidColorLayer> root = CreateSolidColorLayer(
                gfx::Rect(0, 0, content_size, content_size), SK_ColorWHITE);

            scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(
                gfx::Rect(0, 0, content_size, content_size), SK_ColorGREEN);
            root->AddChild(background);

            // Add a blue layer that completely covers the green layer.
            scoped_refptr<SolidColorLayer> foreground = CreateSolidColorLayer(
                gfx::Rect(0, 0, content_size, content_size), SK_ColorBLUE);
            background->AddChild(foreground);

            // Add an alpha threshold filter to the blue layer which will filter out
            // everything except the lower right corner.
            FilterOperations filters;
            SkRegion alpha_region;
            alpha_region.setRect(
                half_content, half_content, content_size, content_size);
            filters.Append(
                FilterOperation::CreateAlphaThresholdFilter(alpha_region, 1.f, 0.f));
            foreground->SetFilters(filters);

            device_scale_factor_ = device_scale_factor;
            RunPixelTest(
                test_type,
                background,
                base::FilePath(FILE_PATH_LITERAL("green_small_with_blue_corner.png")));
        }

        float device_scale_factor_;
    };

    TEST_F(LayerTreeHostFiltersScaledPixelTest, StandardDpi_GL)
    {
        RunPixelTestType(100, 1.f, PIXEL_TEST_GL);
    }

    TEST_F(LayerTreeHostFiltersScaledPixelTest, StandardDpi_Software)
    {
        RunPixelTestType(100, 1.f, PIXEL_TEST_SOFTWARE);
    }

    TEST_F(LayerTreeHostFiltersScaledPixelTest, HiDpi_GL)
    {
        RunPixelTestType(50, 2.f, PIXEL_TEST_GL);
    }

    TEST_F(LayerTreeHostFiltersScaledPixelTest, HiDpi_Software)
    {
        RunPixelTestType(50, 2.f, PIXEL_TEST_SOFTWARE);
    }

    class ImageFilterClippedPixelTest : public LayerTreeHostFiltersPixelTest {
    protected:
        void RunPixelTestType(PixelTestType test_type)
        {
            scoped_refptr<SolidColorLayer> root = CreateSolidColorLayer(gfx::Rect(200, 200), SK_ColorBLACK);

            scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(gfx::Rect(200, 200), SK_ColorYELLOW);
            root->AddChild(background);

            scoped_refptr<SolidColorLayer> foreground = CreateSolidColorLayer(gfx::Rect(200, 200), SK_ColorRED);
            background->AddChild(foreground);

            SkScalar matrix[20];
            memset(matrix, 0, 20 * sizeof(matrix[0]));
            // This filter does a red-blue swap, so the foreground becomes blue.
            matrix[2] = matrix[6] = matrix[10] = matrix[18] = SK_Scalar1;
            skia::RefPtr<SkColorFilter> colorFilter(
                skia::AdoptRef(SkColorMatrixFilter::Create(matrix)));
            // We filter only the bottom 200x100 pixels of the foreground.
            SkImageFilter::CropRect crop_rect(SkRect::MakeXYWH(0, 100, 200, 100));
            skia::RefPtr<SkImageFilter> filter = skia::AdoptRef(
                SkColorFilterImageFilter::Create(colorFilter.get(), NULL, &crop_rect));
            FilterOperations filters;
            filters.Append(FilterOperation::CreateReferenceFilter(filter));

            // Make the foreground layer's render surface be clipped by the background
            // layer.
            background->SetMasksToBounds(true);
            foreground->SetFilters(filters);

            // Then we translate the foreground up by 100 pixels in Y, so the cropped
            // region is moved to to the top. This ensures that the crop rect is being
            // correctly transformed in skia by the amount of clipping that the
            // compositor performs.
            gfx::Transform transform;
            transform.Translate(0.0, -100.0);
            foreground->SetTransform(transform);

            RunPixelTest(test_type,
                background,
                base::FilePath(FILE_PATH_LITERAL("blue_yellow.png")));
        }
    };

    TEST_F(ImageFilterClippedPixelTest, ImageFilterClipped_GL)
    {
        RunPixelTestType(PIXEL_TEST_GL);
    }

    TEST_F(ImageFilterClippedPixelTest, ImageFilterClipped_Software)
    {
        RunPixelTestType(PIXEL_TEST_SOFTWARE);
    }

    class ImageScaledBackgroundFilter : public LayerTreeHostFiltersPixelTest {
    protected:
        void RunPixelTestType(PixelTestType test_type, base::FilePath image_name)
        {
            scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(gfx::Rect(200, 200), SK_ColorWHITE);

            gfx::Rect rect(50, 50, 100, 100);

            const int kInset = 3;
            for (int i = 0; !rect.IsEmpty(); ++i) {
                scoped_refptr<SolidColorLayer> layer = CreateSolidColorLayer(rect, (i & 1) ? SK_ColorWHITE : SK_ColorRED);

                gfx::Transform transform;
                transform.Translate(rect.width() / 2.0, rect.height() / 2.0);
                transform.RotateAboutZAxis(30.0);
                transform.Translate(-rect.width() / 2.0, -rect.height() / 2.0);
                layer->SetTransform(transform);

                background->AddChild(layer);

                rect.Inset(kInset, kInset);
            }

            scoped_refptr<SolidColorLayer> filter = CreateSolidColorLayer(gfx::Rect(100, 0, 100, 200), SK_ColorTRANSPARENT);

            background->AddChild(filter);

            // Apply a scale to |background| so that we can see any scaling artifacts
            // that may appear.
            gfx::Transform background_transform;
            static float scale = 1.1f;
            background_transform.Scale(scale, scale);
            background->SetTransform(background_transform);

            FilterOperations filters;
            filters.Append(FilterOperation::CreateGrayscaleFilter(1.0f));
            filter->SetBackgroundFilters(filters);

#if defined(OS_WIN)
            // Windows has 153 pixels off by at most 2: crbug.com/225027
            float percentage_pixels_large_error = 0.3825f; // 153px / (200*200)
            float percentage_pixels_small_error = 0.0f;
            float average_error_allowed_in_bad_pixels = 1.f;
            int large_error_allowed = 2;
            int small_error_allowed = 0;
            pixel_comparator_.reset(new FuzzyPixelComparator(
                true, // discard_alpha
                percentage_pixels_large_error, percentage_pixels_small_error,
                average_error_allowed_in_bad_pixels, large_error_allowed,
                small_error_allowed));
#endif

            RunPixelTest(test_type, background, image_name);
        }
    };

    TEST_F(ImageScaledBackgroundFilter, ImageFilterScaled_GL)
    {
        RunPixelTestType(PIXEL_TEST_GL,
            base::FilePath(FILE_PATH_LITERAL(
                "background_filter_on_scaled_layer_gl.png")));
    }

    TEST_F(ImageScaledBackgroundFilter, ImageFilterScaled_Software)
    {
        RunPixelTestType(PIXEL_TEST_SOFTWARE,
            base::FilePath(FILE_PATH_LITERAL(
                "background_filter_on_scaled_layer_sw.png")));
    }

    class ImageBackgroundFilter : public LayerTreeHostFiltersPixelTest {
    protected:
        void RunPixelTestType(PixelTestType test_type, base::FilePath image_name)
        {
            // Add a white background with a rotated red rect in the center.
            scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(gfx::Rect(200, 200), SK_ColorWHITE);

            gfx::Rect rect(50, 50, 100, 100);

            scoped_refptr<SolidColorLayer> layer = CreateSolidColorLayer(rect, SK_ColorRED);

            gfx::Transform transform;
            transform.Translate(rect.width() / 2.0, rect.height() / 2.0);
            transform.RotateAboutZAxis(30.0);
            transform.Translate(-rect.width() / 2.0, -rect.height() / 2.0);
            layer->SetTransform(transform);

            background->AddChild(layer);

            // Add a slightly transparent blue layer.
            scoped_refptr<SolidColorLayer> filter = CreateSolidColorLayer(gfx::Rect(100, 0, 100, 200), 0x220000FF);

            // Add some rotation so that we can see that it blurs only under the layer.
            gfx::Transform transform_filter;
            transform_filter.RotateAboutZAxis(10.0);
            filter->SetTransform(transform_filter);

            background->AddChild(filter);

            // Add a blur filter to the blue layer.
            FilterOperations filters;
            filters.Append(FilterOperation::CreateBlurFilter(5.0f));
            filter->SetBackgroundFilters(filters);

#if defined(OS_WIN)
            // Windows has 994 pixels off by at most 2: crbug.com/225027
            float percentage_pixels_large_error = 2.4825f; // 994px / (200*200)
            float percentage_pixels_small_error = 0.0f;
            float average_error_allowed_in_bad_pixels = 1.f;
            int large_error_allowed = 2;
            int small_error_allowed = 0;
            pixel_comparator_.reset(new FuzzyPixelComparator(
                true, // discard_alpha
                percentage_pixels_large_error, percentage_pixels_small_error,
                average_error_allowed_in_bad_pixels, large_error_allowed,
                small_error_allowed));
#endif

            RunPixelTest(test_type, background, image_name);
        }
    };

    TEST_F(ImageBackgroundFilter, BackgroundFilterRotated_GL)
    {
        RunPixelTestType(
            PIXEL_TEST_GL,
            base::FilePath(FILE_PATH_LITERAL("background_filter_rotated_gl.png")));
    }

    TEST_F(ImageBackgroundFilter, BackgroundFilterRotated_Software)
    {
        RunPixelTestType(
            PIXEL_TEST_SOFTWARE,
            base::FilePath(FILE_PATH_LITERAL("background_filter_rotated_sw.png")));
    }

    class ImageScaledRenderSurface : public LayerTreeHostFiltersPixelTest {
    protected:
        void RunPixelTestType(PixelTestType test_type, base::FilePath image_name)
        {
            // A filter will cause a render surface to be used.  Here we force the
            // render surface on, and scale the result to make sure that we rasterize at
            // the correct resolution.
            scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(gfx::Rect(300, 300), SK_ColorBLUE);

            scoped_refptr<SolidColorLayer> render_surface_layer = CreateSolidColorLayer(gfx::Rect(0, 0, 200, 200), SK_ColorWHITE);

            gfx::Rect rect(50, 50, 100, 100);

            scoped_refptr<SolidColorLayer> child = CreateSolidColorLayer(rect, SK_ColorRED);

            gfx::Transform transform;
            transform.Translate(rect.width() / 2.0, rect.height() / 2.0);
            transform.RotateAboutZAxis(30.0);
            transform.Translate(-rect.width() / 2.0, -rect.height() / 2.0);
            child->SetTransform(transform);

            render_surface_layer->AddChild(child);

            gfx::Transform render_surface_transform;
            render_surface_transform.Scale(1.5f, 1.5f);
            render_surface_layer->SetTransform(render_surface_transform);
            render_surface_layer->SetForceRenderSurface(true);

            background->AddChild(render_surface_layer);

            // Software has some huge differences in the AA'd pixels on the different
            // trybots. See crbug.com/452198.
            float percentage_pixels_large_error = 0.686f;
            float percentage_pixels_small_error = 0.0f;
            float average_error_allowed_in_bad_pixels = 16.f;
            int large_error_allowed = 17;
            int small_error_allowed = 0;
            pixel_comparator_.reset(new FuzzyPixelComparator(
                true, // discard_alpha
                percentage_pixels_large_error, percentage_pixels_small_error,
                average_error_allowed_in_bad_pixels, large_error_allowed,
                small_error_allowed));

            RunPixelTest(test_type, background, image_name);
        }
    };

    TEST_F(ImageScaledRenderSurface, ImageRenderSurfaceScaled_GL)
    {
        RunPixelTestType(
            PIXEL_TEST_GL,
            base::FilePath(FILE_PATH_LITERAL("scaled_render_surface_layer_gl.png")));
    }

    TEST_F(ImageScaledRenderSurface, ImageRenderSurfaceScaled_Software)
    {
        RunPixelTestType(
            PIXEL_TEST_SOFTWARE,
            base::FilePath(FILE_PATH_LITERAL("scaled_render_surface_layer_sw.png")));
    }

    class EnlargedTextureWithAlphaThresholdFilter
        : public LayerTreeHostFiltersPixelTest {
    protected:
        void RunPixelTestType(PixelTestType test_type, base::FilePath image_name)
        {
            // Rectangles choosen so that if flipped, the test will fail.
            gfx::Rect rect1(10, 10, 10, 15);
            gfx::Rect rect2(20, 25, 70, 65);

            scoped_refptr<SolidColorLayer> child1 = CreateSolidColorLayer(rect1, SK_ColorRED);
            scoped_refptr<SolidColorLayer> child2 = CreateSolidColorLayer(rect2, SK_ColorGREEN);
            scoped_refptr<SolidColorLayer> background = CreateSolidColorLayer(gfx::Rect(200, 200), SK_ColorBLUE);
            scoped_refptr<SolidColorLayer> filter_layer = CreateSolidColorLayer(gfx::Rect(100, 100), SK_ColorWHITE);

            // Make sure a transformation does not cause misregistration of the filter
            // and source texture.
            gfx::Transform filter_transform;
            filter_transform.Scale(2.f, 2.f);
            filter_layer->SetTransform(filter_transform);
            filter_layer->AddChild(child1);
            filter_layer->AddChild(child2);

            rect1.Inset(-5, -5);
            rect2.Inset(-5, -5);
            SkRegion alpha_region;
            SkIRect rects[2] = { gfx::RectToSkIRect(rect1), gfx::RectToSkIRect(rect2) };
            alpha_region.setRects(rects, 2);
            FilterOperations filters;
            filters.Append(
                FilterOperation::CreateAlphaThresholdFilter(alpha_region, 0.f, 0.f));
            filter_layer->SetFilters(filters);

            background->AddChild(filter_layer);

            // Force the allocation a larger textures.
            set_enlarge_texture_amount(gfx::Vector2d(50, 50));

            RunPixelTest(test_type, background, image_name);
        }
    };

    TEST_F(EnlargedTextureWithAlphaThresholdFilter, GL)
    {
        RunPixelTestType(
            PIXEL_TEST_GL,
            base::FilePath(FILE_PATH_LITERAL("enlarged_texture_on_threshold.png")));
    }

    TEST_F(EnlargedTextureWithAlphaThresholdFilter, Software)
    {
        RunPixelTestType(
            PIXEL_TEST_SOFTWARE,
            base::FilePath(FILE_PATH_LITERAL("enlarged_texture_on_threshold.png")));
    }

} // namespace
} // namespace cc

#endif // OS_ANDROID
