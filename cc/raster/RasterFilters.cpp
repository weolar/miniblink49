// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/RasterFilters.h"

#include <algorithm>

#include "cc/blink/WebFilterOperationsImpl.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/effects/SkAlphaThresholdFilter.h"
#include "third_party/skia/include/effects/SkBlurImageFilter.h"
#include "third_party/skia/include/effects/SkColorFilterImageFilter.h"
#include "third_party/skia/include/effects/SkColorMatrixFilter.h"
#include "third_party/skia/include/effects/SkComposeImageFilter.h"
#include "third_party/skia/include/effects/SkDropShadowImageFilter.h"
#include "third_party/skia/include/effects/SkMagnifierImageFilter.h"
#include "third_party/skia/include/effects/SkRectShaderImageFilter.h"
#include "third_party/WebKit/Source/platform/graphics/filters/FilterOperation.h"

namespace cc {

namespace {

void getBrightnessMatrix(float amount, SkScalar matrix[20])
{
    // Spec implementation
    // (http://dvcs.w3.org/hg/FXTF/raw-file/tip/filters/index.html#brightnessEquivalent)
    // <feFunc[R|G|B] type="linear" slope="[amount]">
    memset(matrix, 0, 20 * sizeof(SkScalar));
    matrix[0] = matrix[6] = matrix[12] = amount;
    matrix[18] = 1.f;
}

void getSaturatingBrightnessMatrix(float amount, SkScalar matrix[20])
{
    // Legacy implementation used by internal clients.
    // <feFunc[R|G|B] type="linear" intercept="[amount]"/>
    memset(matrix, 0, 20 * sizeof(SkScalar));
    matrix[0] = matrix[6] = matrix[12] = matrix[18] = 1.f;
    matrix[4] = matrix[9] = matrix[14] = amount * 255.f;
}

void getContrastMatrix(float amount, SkScalar matrix[20])
{
    memset(matrix, 0, 20 * sizeof(SkScalar));
    matrix[0] = matrix[6] = matrix[12] = amount;
    matrix[4] = matrix[9] = matrix[14] = (-0.5f * amount + 0.5f) * 255.f;
    matrix[18] = 1.f;
}

void getSaturateMatrix(float amount, SkScalar matrix[20])
{
    // Note, these values are computed to ensure MatrixNeedsClamping is false
    // for amount in [0..1]
    matrix[0] = 0.213f + 0.787f * amount;
    matrix[1] = 0.715f - 0.715f * amount;
    matrix[2] = 1.f - (matrix[0] + matrix[1]);
    matrix[3] = matrix[4] = 0.f;
    matrix[5] = 0.213f - 0.213f * amount;
    matrix[6] = 0.715f + 0.285f * amount;
    matrix[7] = 1.f - (matrix[5] + matrix[6]);
    matrix[8] = matrix[9] = 0.f;
    matrix[10] = 0.213f - 0.213f * amount;
    matrix[11] = 0.715f - 0.715f * amount;
    matrix[12] = 1.f - (matrix[10] + matrix[11]);
    matrix[13] = matrix[14] = 0.f;
    matrix[15] = matrix[16] = matrix[17] = matrix[19] = 0.f;
    matrix[18] = 1.f;
}

void GetHueRotateMatrix(float hue, SkScalar matrix[20])
{
    const float kPi = 3.1415926535897932384626433832795f;

    float cos_hue = cosf(hue * kPi / 180.f);
    float sin_hue = sinf(hue * kPi / 180.f);
    matrix[0] = 0.213f + cos_hue * 0.787f - sin_hue * 0.213f;
    matrix[1] = 0.715f - cos_hue * 0.715f - sin_hue * 0.715f;
    matrix[2] = 0.072f - cos_hue * 0.072f + sin_hue * 0.928f;
    matrix[3] = matrix[4] = 0.f;
    matrix[5] = 0.213f - cos_hue * 0.213f + sin_hue * 0.143f;
    matrix[6] = 0.715f + cos_hue * 0.285f + sin_hue * 0.140f;
    matrix[7] = 0.072f - cos_hue * 0.072f - sin_hue * 0.283f;
    matrix[8] = matrix[9] = 0.f;
    matrix[10] = 0.213f - cos_hue * 0.213f - sin_hue * 0.787f;
    matrix[11] = 0.715f - cos_hue * 0.715f + sin_hue * 0.715f;
    matrix[12] = 0.072f + cos_hue * 0.928f + sin_hue * 0.072f;
    matrix[13] = matrix[14] = 0.f;
    matrix[15] = matrix[16] = matrix[17] = 0.f;
    matrix[18] = 1.f;
    matrix[19] = 0.f;
}

void getInvertMatrix(float amount, SkScalar matrix[20])
{
    memset(matrix, 0, 20 * sizeof(SkScalar));
    matrix[0] = matrix[6] = matrix[12] = 1.f - 2.f * amount;
    matrix[4] = matrix[9] = matrix[14] = amount * 255.f;
    matrix[18] = 1.f;
}

void getOpacityMatrix(float amount, SkScalar matrix[20])
{
    memset(matrix, 0, 20 * sizeof(SkScalar));
    matrix[0] = matrix[6] = matrix[12] = 1.f;
    matrix[18] = amount;
}

void getGrayscaleMatrix(float amount, SkScalar matrix[20])
{
    // Note, these values are computed to ensure MatrixNeedsClamping is false
    // for amount in [0..1]
    matrix[0] = 0.2126f + 0.7874f * amount;
    matrix[1] = 0.7152f - 0.7152f * amount;
    matrix[2] = 1.f - (matrix[0] + matrix[1]);
    matrix[3] = matrix[4] = 0.f;

    matrix[5] = 0.2126f - 0.2126f * amount;
    matrix[6] = 0.7152f + 0.2848f * amount;
    matrix[7] = 1.f - (matrix[5] + matrix[6]);
    matrix[8] = matrix[9] = 0.f;

    matrix[10] = 0.2126f - 0.2126f * amount;
    matrix[11] = 0.7152f - 0.7152f * amount;
    matrix[12] = 1.f - (matrix[10] + matrix[11]);
    matrix[13] = matrix[14] = 0.f;

    matrix[15] = matrix[16] = matrix[17] = matrix[19] = 0.f;
    matrix[18] = 1.f;
}

void getSepiaMatrix(float amount, SkScalar matrix[20])
{
    matrix[0] = 0.393f + 0.607f * amount;
    matrix[1] = 0.769f - 0.769f * amount;
    matrix[2] = 0.189f - 0.189f * amount;
    matrix[3] = matrix[4] = 0.f;

    matrix[5] = 0.349f - 0.349f * amount;
    matrix[6] = 0.686f + 0.314f * amount;
    matrix[7] = 0.168f - 0.168f * amount;
    matrix[8] = matrix[9] = 0.f;

    matrix[10] = 0.272f - 0.272f * amount;
    matrix[11] = 0.534f - 0.534f * amount;
    matrix[12] = 0.131f + 0.869f * amount;
    matrix[13] = matrix[14] = 0.f;

    matrix[15] = matrix[16] = matrix[17] = matrix[19] = 0.f;
    matrix[18] = 1.f;
}

skia::RefPtr<SkImageFilter> createMatrixImageFilter(
    const SkScalar matrix[20],
    const skia::RefPtr<SkImageFilter>& input)
{
    skia::RefPtr<SkColorFilter> color_filter =
        skia::AdoptRef(SkColorMatrixFilter::Create(matrix));
    return skia::AdoptRef(
        SkColorFilterImageFilter::Create(color_filter.get(), input.get()));
}

}  // namespace

skia::RefPtr<SkImageFilter> RasterFilter::buildImageFilter(const cc_blink::WebFilterOperationsImpl* filters, const blink::FloatSize& size)
{
    skia::RefPtr<SkImageFilter> imageFilter;
    SkScalar matrix[20];
    if (!filters)
        return imageFilter;

    for (size_t i = 0; i < filters->getSize(); ++i) {
        const blink::FilterOperation& op = filters->at(i);
        switch (op.type()) {
        case blink::FilterOperation::GRAYSCALE:
            getGrayscaleMatrix(1.f - blink::toBasicColorMatrixFilterOperation(op).amount(), matrix);
            imageFilter = createMatrixImageFilter(matrix, imageFilter);
            break;
        case blink::FilterOperation::SEPIA:
            getSepiaMatrix(1.f - blink::toBasicColorMatrixFilterOperation(op).amount(), matrix);
            imageFilter = createMatrixImageFilter(matrix, imageFilter);
            break;
        case blink::FilterOperation::SATURATE:
            getSaturateMatrix(blink::toBasicColorMatrixFilterOperation(op).amount(), matrix);
            imageFilter = createMatrixImageFilter(matrix, imageFilter);
            break;
        case blink::FilterOperation::HUE_ROTATE:
            GetHueRotateMatrix(blink::toBasicColorMatrixFilterOperation(op).amount(), matrix);
            imageFilter = createMatrixImageFilter(matrix, imageFilter);
            break;
        case blink::FilterOperation::INVERT:
            getInvertMatrix(blink::toBasicComponentTransferFilterOperation(op).amount(), matrix);
            imageFilter = createMatrixImageFilter(matrix, imageFilter);
            break;
        case blink::FilterOperation::OPACITY:
            getOpacityMatrix(blink::toBasicComponentTransferFilterOperation(op).amount(), matrix);
            imageFilter = createMatrixImageFilter(matrix, imageFilter);
            break;
        case blink::FilterOperation::BRIGHTNESS:
            getBrightnessMatrix(blink::toBasicComponentTransferFilterOperation(op).amount(), matrix);
            imageFilter = createMatrixImageFilter(matrix, imageFilter);
            break;
        case blink::FilterOperation::CONTRAST:
            getContrastMatrix(blink::toBasicComponentTransferFilterOperation(op).amount(), matrix);
            imageFilter = createMatrixImageFilter(matrix, imageFilter);
            break;
        case blink::FilterOperation::BLUR:
            imageFilter = skia::AdoptRef(SkBlurImageFilter::Create(
                blink::toBlurFilterOperation(op).stdDeviation().getFloatValue(), blink::toBlurFilterOperation(op).stdDeviation().getFloatValue(), imageFilter.get()));
            break;
        case blink::FilterOperation::DROP_SHADOW:
            const blink::DropShadowFilterOperation& dropShadow = blink::toDropShadowFilterOperation(op);
            imageFilter = skia::AdoptRef(SkDropShadowImageFilter::Create(
                SkIntToScalar(dropShadow.location().x()),
                SkIntToScalar(dropShadow.location().y()),
                SkIntToScalar(dropShadow.stdDeviation()),
                SkIntToScalar(dropShadow.stdDeviation()),
                dropShadow.color().rgb(),
                SkDropShadowImageFilter::kDrawShadowAndForeground_ShadowMode,
                imageFilter.get()));
            break;
//         case blink::FilterOperation::COLOR_MATRIX:
//             imageFilter = createMatrixImageFilter(op.matrix(), imageFilter);
//             break;
//         case blink::FilterOperation::ZOOM:
//         {
//             skia::RefPtr<SkImageFilter> zoom_filter =
//                 skia::AdoptRef(SkMagnifierImageFilter::Create(
//                     SkRect::MakeXYWH(
//                         (size.width() - (size.width() / op.amount())) / 2.f,
//                         (size.height() - (size.height() / op.amount())) / 2.f,
//                         size.width() / op.amount(),
//                         size.height() / op.amount()),
//                     op.zoom_inset()));
//             if (imageFilter.get()) {
//                 // TODO(ajuma): When there's a 1-input version of
//                 // SkMagnifierImageFilter, use that to handle the input filter
//                 // instead of using an SkComposeImageFilter.
//                 imageFilter = skia::AdoptRef(SkComposeImageFilter::Create(
//                     zoom_filter.get(), imageFilter.get()));
//             } else {
//                 imageFilter = zoom_filter;
//             }
//             break;
//         }
//         case blink::FilterOperation::SATURATING_BRIGHTNESS:
//             getSaturatingBrightnessMatrix(op.amount(), matrix);
//             imageFilter = createMatrixImageFilter(matrix, imageFilter);
//             break;
//         case blink::FilterOperation::REFERENCE:
//         {
//             if (!op.imageFilter())
//                 break;
// 
//             skia::RefPtr<SkColorFilter> cf;
// 
//             {
//                 SkColorFilter* colorfilter_rawptr = NULL;
//                 op.imageFilter()->asColorFilter(&colorfilter_rawptr);
//                 cf = skia::AdoptRef(colorfilter_rawptr);
//             }
// 
//             if (cf && cf->asColorMatrix(matrix) &&
//                 !op.imageFilter()->getInput(0)) {
//                 imageFilter = createMatrixImageFilter(matrix, imageFilter);
//             } else if (imageFilter) {
//                 imageFilter = skia::AdoptRef(SkComposeImageFilter::Create(
//                     op.imageFilter().get(), imageFilter.get()));
//             } else {
//                 imageFilter = op.imageFilter();
//             }
//             break;
//         }
//         case blink::FilterOperation::ALPHA_THRESHOLD:
//         {
//             skia::RefPtr<SkImageFilter> alpha_filter = skia::AdoptRef(
//                 SkAlphaThresholdFilter::Create(
//                     op.region(), op.amount(), op.outer_threshold()));
//             if (imageFilter.get()) {
//                 imageFilter = skia::AdoptRef(SkComposeImageFilter::Create(
//                     alpha_filter.get(), imageFilter.get()));
//             } else {
//                 imageFilter = alpha_filter;
//             }
//             break;
//         }
        }
    }
    return imageFilter;
}

}  // namespace cc
