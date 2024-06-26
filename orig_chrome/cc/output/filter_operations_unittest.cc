// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/filter_operations.h"
#include "skia/ext/refptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/effects/SkBlurImageFilter.h"
#include "third_party/skia/include/effects/SkDropShadowImageFilter.h"
#include "ui/gfx/geometry/point.h"

namespace cc {
namespace {

    TEST(FilterOperationsTest, GetOutsetsBlur)
    {
        FilterOperations ops;
        ops.Append(FilterOperation::CreateBlurFilter(20));
        int top, right, bottom, left;
        top = right = bottom = left = 0;
        ops.GetOutsets(&top, &right, &bottom, &left);
        EXPECT_EQ(57, top);
        EXPECT_EQ(57, right);
        EXPECT_EQ(57, bottom);
        EXPECT_EQ(57, left);
    }

    TEST(FilterOperationsTest, GetOutsetsDropShadowReferenceFilter)
    {
        // TODO(hendrikw): We need to make outsets for reference filters be in line
        // with non-reference filters. See crbug.com/523534
        skia::RefPtr<SkImageFilter> filter = skia::AdoptRef(SkDropShadowImageFilter::Create(
            SkIntToScalar(3), SkIntToScalar(8), SkIntToScalar(4),
            SkIntToScalar(9), SK_ColorBLACK,
            SkDropShadowImageFilter::kDrawShadowAndForeground_ShadowMode));
        FilterOperations ops;
        ops.Append(FilterOperation::CreateReferenceFilter(filter));

        int top, right, bottom, left;
        top = right = bottom = left = 0;
        ops.GetOutsets(&top, &right, &bottom, &left);
        EXPECT_EQ(35, top);
        EXPECT_EQ(9, right);
        EXPECT_EQ(19, bottom);
        EXPECT_EQ(15, left);
    }

    TEST(FilterOperationsTest, GetOutsetsDropShadow)
    {
        FilterOperations ops;
        ops.Append(FilterOperation::CreateDropShadowFilter(gfx::Point(3, 8), 20, 0));
        int top, right, bottom, left;
        top = right = bottom = left = 0;
        ops.GetOutsets(&top, &right, &bottom, &left);
        EXPECT_EQ(49, top);
        EXPECT_EQ(60, right);
        EXPECT_EQ(65, bottom);
        EXPECT_EQ(54, left);
    }

#define SAVE_RESTORE_AMOUNT(filter_name, filter_type, a)                      \
    {                                                                         \
        FilterOperation op = FilterOperation::Create##filter_name##Filter(a); \
        EXPECT_EQ(FilterOperation::filter_type, op.type());                   \
        EXPECT_EQ(a, op.amount());                                            \
                                                                              \
        FilterOperation op2 = FilterOperation::CreateEmptyFilter();           \
        op2.set_type(FilterOperation::filter_type);                           \
                                                                              \
        EXPECT_NE(a, op2.amount());                                           \
                                                                              \
        op2.set_amount(a);                                                    \
                                                                              \
        EXPECT_EQ(FilterOperation::filter_type, op2.type());                  \
        EXPECT_EQ(a, op2.amount());                                           \
    }

#define SAVE_RESTORE_OFFSET_AMOUNT_COLOR(filter_name, filter_type, a, b, c)         \
    {                                                                               \
        FilterOperation op = FilterOperation::Create##filter_name##Filter(a, b, c); \
        EXPECT_EQ(FilterOperation::filter_type, op.type());                         \
        EXPECT_EQ(a, op.drop_shadow_offset());                                      \
        EXPECT_EQ(b, op.amount());                                                  \
        EXPECT_EQ(c, op.drop_shadow_color());                                       \
                                                                                    \
        FilterOperation op2 = FilterOperation::CreateEmptyFilter();                 \
        op2.set_type(FilterOperation::filter_type);                                 \
                                                                                    \
        EXPECT_NE(a, op2.drop_shadow_offset());                                     \
        EXPECT_NE(b, op2.amount());                                                 \
        EXPECT_NE(c, op2.drop_shadow_color());                                      \
                                                                                    \
        op2.set_drop_shadow_offset(a);                                              \
        op2.set_amount(b);                                                          \
        op2.set_drop_shadow_color(c);                                               \
                                                                                    \
        EXPECT_EQ(FilterOperation::filter_type, op2.type());                        \
        EXPECT_EQ(a, op2.drop_shadow_offset());                                     \
        EXPECT_EQ(b, op2.amount());                                                 \
        EXPECT_EQ(c, op2.drop_shadow_color());                                      \
    }

#define SAVE_RESTORE_MATRIX(filter_name, filter_type, a)                      \
    {                                                                         \
        FilterOperation op = FilterOperation::Create##filter_name##Filter(a); \
        EXPECT_EQ(FilterOperation::filter_type, op.type());                   \
        for (size_t i = 0; i < 20; ++i)                                       \
            EXPECT_EQ(a[i], op.matrix()[i]);                                  \
                                                                              \
        FilterOperation op2 = FilterOperation::CreateEmptyFilter();           \
        op2.set_type(FilterOperation::filter_type);                           \
                                                                              \
        for (size_t i = 0; i < 20; ++i)                                       \
            EXPECT_NE(a[i], op2.matrix()[i]);                                 \
                                                                              \
        op2.set_matrix(a);                                                    \
                                                                              \
        EXPECT_EQ(FilterOperation::filter_type, op2.type());                  \
        for (size_t i = 0; i < 20; ++i)                                       \
            EXPECT_EQ(a[i], op.matrix()[i]);                                  \
    }

#define SAVE_RESTORE_AMOUNT_INSET(filter_name, filter_type, a, b)                \
    {                                                                            \
        FilterOperation op = FilterOperation::Create##filter_name##Filter(a, b); \
        EXPECT_EQ(FilterOperation::filter_type, op.type());                      \
        EXPECT_EQ(a, op.amount());                                               \
        EXPECT_EQ(b, op.zoom_inset());                                           \
                                                                                 \
        FilterOperation op2 = FilterOperation::CreateEmptyFilter();              \
        op2.set_type(FilterOperation::filter_type);                              \
                                                                                 \
        EXPECT_NE(a, op2.amount());                                              \
        EXPECT_NE(b, op2.zoom_inset());                                          \
                                                                                 \
        op2.set_amount(a);                                                       \
        op2.set_zoom_inset(b);                                                   \
                                                                                 \
        EXPECT_EQ(FilterOperation::filter_type, op2.type());                     \
        EXPECT_EQ(a, op2.amount());                                              \
        EXPECT_EQ(b, op2.zoom_inset());                                          \
    }

    TEST(FilterOperationsTest, SaveAndRestore)
    {
        SAVE_RESTORE_AMOUNT(Grayscale, GRAYSCALE, 0.6f);
        SAVE_RESTORE_AMOUNT(Sepia, SEPIA, 0.6f);
        SAVE_RESTORE_AMOUNT(Saturate, SATURATE, 0.6f);
        SAVE_RESTORE_AMOUNT(HueRotate, HUE_ROTATE, 0.6f);
        SAVE_RESTORE_AMOUNT(Invert, INVERT, 0.6f);
        SAVE_RESTORE_AMOUNT(Brightness, BRIGHTNESS, 0.6f);
        SAVE_RESTORE_AMOUNT(Contrast, CONTRAST, 0.6f);
        SAVE_RESTORE_AMOUNT(Opacity, OPACITY, 0.6f);
        SAVE_RESTORE_AMOUNT(Blur, BLUR, 0.6f);
        SAVE_RESTORE_AMOUNT(SaturatingBrightness, SATURATING_BRIGHTNESS, 0.6f);
        SAVE_RESTORE_OFFSET_AMOUNT_COLOR(
            DropShadow, DROP_SHADOW, gfx::Point(3, 4), 0.4f, 0xffffff00);

        SkScalar matrix[20] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
            17, 18, 19, 20 };
        SAVE_RESTORE_MATRIX(ColorMatrix, COLOR_MATRIX, matrix);

        SAVE_RESTORE_AMOUNT_INSET(Zoom, ZOOM, 0.5f, 32);
    }

    TEST(FilterOperationsTest, BlendGrayscaleFilters)
    {
        FilterOperation from = FilterOperation::CreateGrayscaleFilter(0.25f);
        FilterOperation to = FilterOperation::CreateGrayscaleFilter(0.75f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateGrayscaleFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateGrayscaleFilter(0.625f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.8);
        expected = FilterOperation::CreateGrayscaleFilter(1.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendGrayscaleWithNull)
    {
        FilterOperation filter = FilterOperation::CreateGrayscaleFilter(1.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateGrayscaleFilter(0.75f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateGrayscaleFilter(0.25f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendSepiaFilters)
    {
        FilterOperation from = FilterOperation::CreateSepiaFilter(0.25f);
        FilterOperation to = FilterOperation::CreateSepiaFilter(0.75f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateSepiaFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateSepiaFilter(0.625f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.8);
        expected = FilterOperation::CreateSepiaFilter(1.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendSepiaWithNull)
    {
        FilterOperation filter = FilterOperation::CreateSepiaFilter(1.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateSepiaFilter(0.75f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateSepiaFilter(0.25f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendSaturateFilters)
    {
        FilterOperation from = FilterOperation::CreateSaturateFilter(0.25f);
        FilterOperation to = FilterOperation::CreateSaturateFilter(0.75f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateSaturateFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateSaturateFilter(0.625f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 2.0);
        expected = FilterOperation::CreateSaturateFilter(1.25f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendSaturateWithNull)
    {
        FilterOperation filter = FilterOperation::CreateSaturateFilter(0.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateSaturateFilter(0.25f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateSaturateFilter(0.75f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendHueRotateFilters)
    {
        FilterOperation from = FilterOperation::CreateHueRotateFilter(3.f);
        FilterOperation to = FilterOperation::CreateHueRotateFilter(7.f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateHueRotateFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateHueRotateFilter(6.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.5);
        expected = FilterOperation::CreateHueRotateFilter(9.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendHueRotateWithNull)
    {
        FilterOperation filter = FilterOperation::CreateHueRotateFilter(1.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateHueRotateFilter(0.75f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateHueRotateFilter(0.25f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendInvertFilters)
    {
        FilterOperation from = FilterOperation::CreateInvertFilter(0.25f);
        FilterOperation to = FilterOperation::CreateInvertFilter(0.75f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateInvertFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateInvertFilter(0.625f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.8);
        expected = FilterOperation::CreateInvertFilter(1.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendInvertWithNull)
    {
        FilterOperation filter = FilterOperation::CreateInvertFilter(1.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateInvertFilter(0.75f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateInvertFilter(0.25f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendBrightnessFilters)
    {
        FilterOperation from = FilterOperation::CreateBrightnessFilter(3.f);
        FilterOperation to = FilterOperation::CreateBrightnessFilter(7.f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.9);
        FilterOperation expected = FilterOperation::CreateBrightnessFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateBrightnessFilter(6.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.5);
        expected = FilterOperation::CreateBrightnessFilter(9.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendBrightnessWithNull)
    {
        FilterOperation filter = FilterOperation::CreateBrightnessFilter(0.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateBrightnessFilter(0.25f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateBrightnessFilter(0.75f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendContrastFilters)
    {
        FilterOperation from = FilterOperation::CreateContrastFilter(3.f);
        FilterOperation to = FilterOperation::CreateContrastFilter(7.f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.9);
        FilterOperation expected = FilterOperation::CreateContrastFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateContrastFilter(6.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.5);
        expected = FilterOperation::CreateContrastFilter(9.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendContrastWithNull)
    {
        FilterOperation filter = FilterOperation::CreateContrastFilter(0.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateContrastFilter(0.25f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateContrastFilter(0.75f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendOpacityFilters)
    {
        FilterOperation from = FilterOperation::CreateOpacityFilter(0.25f);
        FilterOperation to = FilterOperation::CreateOpacityFilter(0.75f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateOpacityFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateOpacityFilter(0.625f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.8);
        expected = FilterOperation::CreateOpacityFilter(1.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendOpacityWithNull)
    {
        FilterOperation filter = FilterOperation::CreateOpacityFilter(0.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateOpacityFilter(0.25f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateOpacityFilter(0.75f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendBlurFilters)
    {
        FilterOperation from = FilterOperation::CreateBlurFilter(3.f);
        FilterOperation to = FilterOperation::CreateBlurFilter(7.f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.9);
        FilterOperation expected = FilterOperation::CreateBlurFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateBlurFilter(6.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.5);
        expected = FilterOperation::CreateBlurFilter(9.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendBlurWithNull)
    {
        FilterOperation filter = FilterOperation::CreateBlurFilter(1.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateBlurFilter(0.75f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateBlurFilter(0.25f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendDropShadowFilters)
    {
        FilterOperation from = FilterOperation::CreateDropShadowFilter(
            gfx::Point(0, 0), 2.f, SkColorSetARGB(15, 34, 68, 136));
        FilterOperation to = FilterOperation::CreateDropShadowFilter(
            gfx::Point(3, 5), 6.f, SkColorSetARGB(51, 30, 60, 120));

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateDropShadowFilter(
            gfx::Point(-2, -4), 0.f, SkColorSetARGB(0, 0, 0, 0));
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.25);
        expected = FilterOperation::CreateDropShadowFilter(
            gfx::Point(1, 1), 3.f, SkColorSetARGB(24, 32, 64, 128));
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateDropShadowFilter(
            gfx::Point(2, 4), 5.f, SkColorSetARGB(42, 30, 61, 121));
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.5);
        expected = FilterOperation::CreateDropShadowFilter(
            gfx::Point(5, 8), 8.f, SkColorSetARGB(69, 30, 59, 118));
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendDropShadowWithNull)
    {
        FilterOperation filter = FilterOperation::CreateDropShadowFilter(
            gfx::Point(4, 4), 4.f, SkColorSetARGB(255, 40, 0, 0));

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateDropShadowFilter(
            gfx::Point(3, 3), 3.f, SkColorSetARGB(191, 40, 0, 0));
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateDropShadowFilter(
            gfx::Point(1, 1), 1.f, SkColorSetARGB(64, 40, 0, 0));
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendZoomFilters)
    {
        FilterOperation from = FilterOperation::CreateZoomFilter(2.f, 3);
        FilterOperation to = FilterOperation::CreateZoomFilter(6.f, 0);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateZoomFilter(1.f, 5);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateZoomFilter(5.f, 1);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.5);
        expected = FilterOperation::CreateZoomFilter(8.f, 0);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendZoomWithNull)
    {
        FilterOperation filter = FilterOperation::CreateZoomFilter(2.f, 1);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateZoomFilter(1.75f, 1);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateZoomFilter(1.25f, 0);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendSaturatingBrightnessFilters)
    {
        FilterOperation from = FilterOperation::CreateSaturatingBrightnessFilter(3.f);
        FilterOperation to = FilterOperation::CreateSaturatingBrightnessFilter(7.f);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        FilterOperation expected = FilterOperation::CreateSaturatingBrightnessFilter(0.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 0.75);
        expected = FilterOperation::CreateSaturatingBrightnessFilter(6.f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(&from, &to, 1.5);
        expected = FilterOperation::CreateSaturatingBrightnessFilter(9.f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendSaturatingBrightnessWithNull)
    {
        FilterOperation filter = FilterOperation::CreateSaturatingBrightnessFilter(1.f);

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        FilterOperation expected = FilterOperation::CreateSaturatingBrightnessFilter(0.75f);
        EXPECT_EQ(expected, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        expected = FilterOperation::CreateSaturatingBrightnessFilter(0.25f);
        EXPECT_EQ(expected, blended);
    }

    TEST(FilterOperationsTest, BlendReferenceFilters)
    {
        skia::RefPtr<SkImageFilter> from_filter = skia::AdoptRef(SkBlurImageFilter::Create(1.f, 1.f));
        skia::RefPtr<SkImageFilter> to_filter = skia::AdoptRef(SkBlurImageFilter::Create(2.f, 2.f));
        FilterOperation from = FilterOperation::CreateReferenceFilter(from_filter);
        FilterOperation to = FilterOperation::CreateReferenceFilter(to_filter);

        FilterOperation blended = FilterOperation::Blend(&from, &to, -0.75);
        EXPECT_EQ(from, blended);

        blended = FilterOperation::Blend(&from, &to, 0.5);
        EXPECT_EQ(from, blended);

        blended = FilterOperation::Blend(&from, &to, 0.6);
        EXPECT_EQ(to, blended);

        blended = FilterOperation::Blend(&from, &to, 1.5);
        EXPECT_EQ(to, blended);
    }

    TEST(FilterOperationsTest, BlendReferenceWithNull)
    {
        skia::RefPtr<SkImageFilter> image_filter = skia::AdoptRef(SkBlurImageFilter::Create(1.f, 1.f));
        FilterOperation filter = FilterOperation::CreateReferenceFilter(image_filter);
        FilterOperation null_filter = FilterOperation::CreateReferenceFilter(skia::RefPtr<SkImageFilter>());

        FilterOperation blended = FilterOperation::Blend(&filter, NULL, 0.25);
        EXPECT_EQ(filter, blended);
        blended = FilterOperation::Blend(&filter, NULL, 0.75);
        EXPECT_EQ(null_filter, blended);

        blended = FilterOperation::Blend(NULL, &filter, 0.25);
        EXPECT_EQ(null_filter, blended);
        blended = FilterOperation::Blend(NULL, &filter, 0.75);
        EXPECT_EQ(filter, blended);
    }

    // Tests blending non-empty sequences that have the same length and matching
    // operations.
    TEST(FilterOperationsTest, BlendMatchingSequences)
    {
        FilterOperations from;
        FilterOperations to;

        from.Append(FilterOperation::CreateBlurFilter(0.f));
        to.Append(FilterOperation::CreateBlurFilter(2.f));

        from.Append(FilterOperation::CreateSaturateFilter(4.f));
        to.Append(FilterOperation::CreateSaturateFilter(0.f));

        from.Append(FilterOperation::CreateZoomFilter(2.0f, 1));
        to.Append(FilterOperation::CreateZoomFilter(10.f, 9));

        FilterOperations blended = to.Blend(from, -0.75);
        FilterOperations expected;
        expected.Append(FilterOperation::CreateBlurFilter(0.f));
        expected.Append(FilterOperation::CreateSaturateFilter(7.f));
        expected.Append(FilterOperation::CreateZoomFilter(1.f, 0));
        EXPECT_EQ(blended, expected);

        blended = to.Blend(from, 0.75);
        expected.Clear();
        expected.Append(FilterOperation::CreateBlurFilter(1.5f));
        expected.Append(FilterOperation::CreateSaturateFilter(1.f));
        expected.Append(FilterOperation::CreateZoomFilter(8.f, 7));
        EXPECT_EQ(blended, expected);

        blended = to.Blend(from, 1.5);
        expected.Clear();
        expected.Append(FilterOperation::CreateBlurFilter(3.f));
        expected.Append(FilterOperation::CreateSaturateFilter(0.f));
        expected.Append(FilterOperation::CreateZoomFilter(14.f, 13));
        EXPECT_EQ(blended, expected);
    }

    TEST(FilterOperationsTest, BlendEmptyAndNonEmptySequences)
    {
        FilterOperations empty;
        FilterOperations filters;

        filters.Append(FilterOperation::CreateGrayscaleFilter(0.75f));
        filters.Append(FilterOperation::CreateBrightnessFilter(2.f));
        filters.Append(FilterOperation::CreateHueRotateFilter(10.0f));

        FilterOperations blended = empty.Blend(filters, -0.75);
        FilterOperations expected;
        expected.Append(FilterOperation::CreateGrayscaleFilter(1.f));
        expected.Append(FilterOperation::CreateBrightnessFilter(2.75f));
        expected.Append(FilterOperation::CreateHueRotateFilter(17.5f));
        EXPECT_EQ(blended, expected);

        blended = empty.Blend(filters, 0.75);
        expected.Clear();
        expected.Append(FilterOperation::CreateGrayscaleFilter(0.1875f));
        expected.Append(FilterOperation::CreateBrightnessFilter(1.25f));
        expected.Append(FilterOperation::CreateHueRotateFilter(2.5f));
        EXPECT_EQ(blended, expected);

        blended = empty.Blend(filters, 1.5);
        expected.Clear();
        expected.Append(FilterOperation::CreateGrayscaleFilter(0.f));
        expected.Append(FilterOperation::CreateBrightnessFilter(0.5f));
        expected.Append(FilterOperation::CreateHueRotateFilter(-5.f));
        EXPECT_EQ(blended, expected);

        blended = filters.Blend(empty, -0.75);
        expected.Clear();
        expected.Append(FilterOperation::CreateGrayscaleFilter(0.f));
        expected.Append(FilterOperation::CreateBrightnessFilter(0.25f));
        expected.Append(FilterOperation::CreateHueRotateFilter(-7.5f));
        EXPECT_EQ(blended, expected);

        blended = filters.Blend(empty, 0.75);
        expected.Clear();
        expected.Append(FilterOperation::CreateGrayscaleFilter(0.5625f));
        expected.Append(FilterOperation::CreateBrightnessFilter(1.75f));
        expected.Append(FilterOperation::CreateHueRotateFilter(7.5f));
        EXPECT_EQ(blended, expected);

        blended = filters.Blend(empty, 1.5);
        expected.Clear();
        expected.Append(FilterOperation::CreateGrayscaleFilter(1.f));
        expected.Append(FilterOperation::CreateBrightnessFilter(2.5f));
        expected.Append(FilterOperation::CreateHueRotateFilter(15.f));
        EXPECT_EQ(blended, expected);
    }

    TEST(FilterOperationsTest, BlendEmptySequences)
    {
        FilterOperations empty;

        FilterOperations blended = empty.Blend(empty, -0.75);
        EXPECT_EQ(blended, empty);

        blended = empty.Blend(empty, 0.75);
        EXPECT_EQ(blended, empty);

        blended = empty.Blend(empty, 1.5);
        EXPECT_EQ(blended, empty);
    }

    // Tests blending non-empty sequences that have non-matching operations.
    TEST(FilterOperationsTest, BlendNonMatchingSequences)
    {
        FilterOperations from;
        FilterOperations to;

        from.Append(FilterOperation::CreateSaturateFilter(3.f));
        from.Append(FilterOperation::CreateBlurFilter(2.f));
        to.Append(FilterOperation::CreateSaturateFilter(4.f));
        to.Append(FilterOperation::CreateHueRotateFilter(0.5f));

        FilterOperations blended = to.Blend(from, -0.75);
        EXPECT_EQ(to, blended);
        blended = to.Blend(from, 0.75);
        EXPECT_EQ(to, blended);
        blended = to.Blend(from, 1.5);
        EXPECT_EQ(to, blended);
    }

    // Tests blending non-empty sequences of different sizes.
    TEST(FilterOperationsTest, BlendRaggedSequences)
    {
        FilterOperations from;
        FilterOperations to;

        from.Append(FilterOperation::CreateSaturateFilter(3.f));
        from.Append(FilterOperation::CreateBlurFilter(2.f));
        to.Append(FilterOperation::CreateSaturateFilter(4.f));

        FilterOperations blended = to.Blend(from, -0.75);
        FilterOperations expected;
        expected.Append(FilterOperation::CreateSaturateFilter(2.25f));
        expected.Append(FilterOperation::CreateBlurFilter(3.5f));
        EXPECT_EQ(expected, blended);

        blended = to.Blend(from, 0.75);
        expected.Clear();
        expected.Append(FilterOperation::CreateSaturateFilter(3.75f));
        expected.Append(FilterOperation::CreateBlurFilter(0.5f));
        EXPECT_EQ(expected, blended);

        blended = to.Blend(from, 1.5);
        expected.Clear();
        expected.Append(FilterOperation::CreateSaturateFilter(4.5f));
        expected.Append(FilterOperation::CreateBlurFilter(0.f));
        EXPECT_EQ(expected, blended);

        from.Append(FilterOperation::CreateOpacityFilter(1.f));
        to.Append(FilterOperation::CreateOpacityFilter(1.f));
        blended = to.Blend(from, -0.75);
        EXPECT_EQ(to, blended);
        blended = to.Blend(from, 0.75);
        EXPECT_EQ(to, blended);
        blended = to.Blend(from, 1.5);
        EXPECT_EQ(to, blended);
    }

} // namespace
} // namespace cc
