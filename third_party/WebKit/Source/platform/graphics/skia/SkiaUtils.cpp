/*
 * Copyright (c) 2006,2007,2008, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "platform/graphics/skia/SkiaUtils.h"

#include "SkColorPriv.h"
#include "SkRegion.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/ImageBuffer.h"

namespace blink {

static const struct CompositOpToXfermodeMode {
    CompositeOperator mCompositOp;
    SkXfermode::Mode m_xfermodeMode;
} gMapCompositOpsToXfermodeModes[] = {
    { CompositeClear,           SkXfermode::kClear_Mode },
    { CompositeCopy,            SkXfermode::kSrc_Mode },
    { CompositeSourceOver,      SkXfermode::kSrcOver_Mode },
    { CompositeSourceIn,        SkXfermode::kSrcIn_Mode },
    { CompositeSourceOut,       SkXfermode::kSrcOut_Mode },
    { CompositeSourceAtop,      SkXfermode::kSrcATop_Mode },
    { CompositeDestinationOver, SkXfermode::kDstOver_Mode },
    { CompositeDestinationIn,   SkXfermode::kDstIn_Mode },
    { CompositeDestinationOut,  SkXfermode::kDstOut_Mode },
    { CompositeDestinationAtop, SkXfermode::kDstATop_Mode },
    { CompositeXOR,             SkXfermode::kXor_Mode },
    { CompositePlusLighter,     SkXfermode::kPlus_Mode }
};

// keep this array in sync with WebBlendMode enum in public/platform/WebBlendMode.h
static const SkXfermode::Mode gMapBlendOpsToXfermodeModes[] = {
    SkXfermode::kClear_Mode, // WebBlendModeNormal
    SkXfermode::kMultiply_Mode, // WebBlendModeMultiply
    SkXfermode::kScreen_Mode, // WebBlendModeScreen
    SkXfermode::kOverlay_Mode, // WebBlendModeOverlay
    SkXfermode::kDarken_Mode, // WebBlendModeDarken
    SkXfermode::kLighten_Mode, // WebBlendModeLighten
    SkXfermode::kColorDodge_Mode, // WebBlendModeColorDodge
    SkXfermode::kColorBurn_Mode, // WebBlendModeColorBurn
    SkXfermode::kHardLight_Mode, // WebBlendModeHardLight
    SkXfermode::kSoftLight_Mode, // WebBlendModeSoftLight
    SkXfermode::kDifference_Mode, // WebBlendModeDifference
    SkXfermode::kExclusion_Mode, // WebBlendModeExclusion
    SkXfermode::kHue_Mode, // WebBlendModeHue
    SkXfermode::kSaturation_Mode, // WebBlendModeSaturation
    SkXfermode::kColor_Mode, // WebBlendModeColor
    SkXfermode::kLuminosity_Mode // WebBlendModeLuminosity
};

SkXfermode::Mode WebCoreCompositeToSkiaComposite(CompositeOperator op, WebBlendMode blendMode)
{
    ASSERT(op == CompositeSourceOver || blendMode == WebBlendModeNormal);
    if (blendMode != WebBlendModeNormal) {
        if (static_cast<uint8_t>(blendMode) >= SK_ARRAY_COUNT(gMapBlendOpsToXfermodeModes)) {
            SkDEBUGF(("GraphicsContext::setPlatformCompositeOperation unknown WebBlendMode %d\n", blendMode));
            return SkXfermode::kSrcOver_Mode;
        }
        return gMapBlendOpsToXfermodeModes[static_cast<uint8_t>(blendMode)];
    }

    const CompositOpToXfermodeMode* table = gMapCompositOpsToXfermodeModes;
    if (static_cast<uint8_t>(op) >= SK_ARRAY_COUNT(gMapCompositOpsToXfermodeModes)) {
        SkDEBUGF(("GraphicsContext::setPlatformCompositeOperation unknown CompositeOperator %d\n", op));
        return SkXfermode::kSrcOver_Mode;
    }
    SkASSERT(table[static_cast<uint8_t>(op)].mCompositOp == op);
    return table[static_cast<uint8_t>(op)].m_xfermodeMode;
}

CompositeOperator compositeOperatorFromSkia(SkXfermode::Mode xferMode)
{
    switch (xferMode) {
    case SkXfermode::kClear_Mode:
        return CompositeClear;
    case SkXfermode::kSrc_Mode:
        return CompositeCopy;
    case SkXfermode::kSrcOver_Mode:
        return CompositeSourceOver;
    case SkXfermode::kSrcIn_Mode:
        return CompositeSourceIn;
    case SkXfermode::kSrcOut_Mode:
        return CompositeSourceOut;
    case SkXfermode::kSrcATop_Mode:
        return CompositeSourceAtop;
    case SkXfermode::kDstOver_Mode:
        return CompositeDestinationOver;
    case SkXfermode::kDstIn_Mode:
        return CompositeDestinationIn;
    case SkXfermode::kDstOut_Mode:
        return CompositeDestinationOut;
    case SkXfermode::kDstATop_Mode:
        return CompositeDestinationAtop;
    case SkXfermode::kXor_Mode:
        return CompositeXOR;
    case SkXfermode::kPlus_Mode:
        return CompositePlusLighter;
    default:
        break;
    }
    return CompositeSourceOver;
}

WebBlendMode blendModeFromSkia(SkXfermode::Mode xferMode)
{
    switch (xferMode) {
    case SkXfermode::kSrcOver_Mode:
        return WebBlendModeNormal;
    case SkXfermode::kMultiply_Mode:
        return WebBlendModeMultiply;
    case SkXfermode::kScreen_Mode:
        return WebBlendModeScreen;
    case SkXfermode::kOverlay_Mode:
        return WebBlendModeOverlay;
    case SkXfermode::kDarken_Mode:
        return WebBlendModeDarken;
    case SkXfermode::kLighten_Mode:
        return WebBlendModeLighten;
    case SkXfermode::kColorDodge_Mode:
        return WebBlendModeColorDodge;
    case SkXfermode::kColorBurn_Mode:
        return WebBlendModeColorBurn;
    case SkXfermode::kHardLight_Mode:
        return WebBlendModeHardLight;
    case SkXfermode::kSoftLight_Mode:
        return WebBlendModeSoftLight;
    case SkXfermode::kDifference_Mode:
        return WebBlendModeDifference;
    case SkXfermode::kExclusion_Mode:
        return WebBlendModeExclusion;
    case SkXfermode::kHue_Mode:
        return WebBlendModeHue;
    case SkXfermode::kSaturation_Mode:
        return WebBlendModeSaturation;
    case SkXfermode::kColor_Mode:
        return WebBlendModeColor;
    case SkXfermode::kLuminosity_Mode:
        return WebBlendModeLuminosity;
    default:
        break;
    }
    return WebBlendModeNormal;
}

bool SkPathContainsPoint(const SkPath& originalPath, const FloatPoint& point, SkPath::FillType ft)
{
    SkRect bounds = originalPath.getBounds();

    // We can immediately return false if the point is outside the bounding
    // rect.  We don't use bounds.contains() here, since it would exclude
    // points on the right and bottom edges of the bounding rect, and we want
    // to include them.
    SkScalar fX = SkFloatToScalar(point.x());
    SkScalar fY = SkFloatToScalar(point.y());
    if (fX < bounds.fLeft || fX > bounds.fRight || fY < bounds.fTop || fY > bounds.fBottom)
        return false;

    // Scale the path to a large size before hit testing for two reasons:
    // 1) Skia has trouble with coordinates close to the max signed 16-bit values, so we scale larger paths down.
    //    TODO: when Skia is patched to work properly with large values, this will not be necessary.
    // 2) Skia does not support analytic hit testing, so we scale paths up to do raster hit testing with subpixel accuracy.
    SkScalar biggestCoord = std::max(std::max(std::max(bounds.fRight, bounds.fBottom), -bounds.fLeft), -bounds.fTop);
    if (SkScalarNearlyZero(biggestCoord))
        return false;
    biggestCoord = std::max(std::max(biggestCoord, fX + 1), fY + 1);

    const SkScalar kMaxCoordinate = SkIntToScalar(1 << 15);
    SkScalar scale = kMaxCoordinate / biggestCoord;

    SkRegion rgn;
    SkRegion clip;
    SkMatrix m;
    SkPath scaledPath(originalPath);

    scaledPath.setFillType(ft);
    m.setScale(scale, scale);
    scaledPath.transform(m, 0);

    int x = static_cast<int>(floorf(0.5f + point.x() * scale));
    int y = static_cast<int>(floorf(0.5f + point.y() * scale));
    clip.setRect(x - 1, y - 1, x + 1, y + 1);

    return rgn.setPath(scaledPath, clip);
}

SkMatrix affineTransformToSkMatrix(const AffineTransform& source)
{
    SkMatrix result;

    result.setScaleX(WebCoreDoubleToSkScalar(source.a()));
    result.setSkewX(WebCoreDoubleToSkScalar(source.c()));
    result.setTranslateX(WebCoreDoubleToSkScalar(source.e()));

    result.setScaleY(WebCoreDoubleToSkScalar(source.d()));
    result.setSkewY(WebCoreDoubleToSkScalar(source.b()));
    result.setTranslateY(WebCoreDoubleToSkScalar(source.f()));

    // FIXME: Set perspective properly.
    result.setPerspX(0);
    result.setPerspY(0);
    result.set(SkMatrix::kMPersp2, SK_Scalar1);

    return result;
}

bool nearlyIntegral(float value)
{
    return fabs(value - floorf(value)) < std::numeric_limits<float>::epsilon();
}

InterpolationQuality limitInterpolationQuality(const GraphicsContext* context, InterpolationQuality resampling)
{
    return std::min(resampling, context->imageInterpolationQuality());
}

InterpolationQuality computeInterpolationQuality(
    float srcWidth,
    float srcHeight,
    float destWidth,
    float destHeight,
    bool isDataComplete)
{
    // The percent change below which we will not resample. This usually means
    // an off-by-one error on the web page, and just doing nearest neighbor
    // sampling is usually good enough.
    const float kFractionalChangeThreshold = 0.025f;

    // Images smaller than this in either direction are considered "small" and
    // are not resampled ever (see below).
    const int kSmallImageSizeThreshold = 8;

    // The amount an image can be stretched in a single direction before we
    // say that it is being stretched so much that it must be a line or
    // background that doesn't need resampling.
    const float kLargeStretch = 3.0f;

    // Figure out if we should resample this image. We try to prune out some
    // common cases where resampling won't give us anything, since it is much
    // slower than drawing stretched.
    float diffWidth = fabs(destWidth - srcWidth);
    float diffHeight = fabs(destHeight - srcHeight);
    bool widthNearlyEqual = diffWidth < std::numeric_limits<float>::epsilon();
    bool heightNearlyEqual = diffHeight < std::numeric_limits<float>::epsilon();
    // We don't need to resample if the source and destination are the same.
    if (widthNearlyEqual && heightNearlyEqual)
        return InterpolationNone;

    if (srcWidth <= kSmallImageSizeThreshold
        || srcHeight <= kSmallImageSizeThreshold
        || destWidth <= kSmallImageSizeThreshold
        || destHeight <= kSmallImageSizeThreshold) {
        // Small image detected.

        // Resample in the case where the new size would be non-integral.
        // This can cause noticeable breaks in repeating patterns, except
        // when the source image is only one pixel wide in that dimension.
        if ((!nearlyIntegral(destWidth) && srcWidth > 1 + std::numeric_limits<float>::epsilon())
            || (!nearlyIntegral(destHeight) && srcHeight > 1 + std::numeric_limits<float>::epsilon()))
            return InterpolationLow;

        // Otherwise, don't resample small images. These are often used for
        // borders and rules (think 1x1 images used to make lines).
        return InterpolationNone;
    }

    if (srcHeight * kLargeStretch <= destHeight || srcWidth * kLargeStretch <= destWidth) {
        // Large image detected.

        // Don't resample if it is being stretched a lot in only one direction.
        // This is trying to catch cases where somebody has created a border
        // (which might be large) and then is stretching it to fill some part
        // of the page.
        if (widthNearlyEqual || heightNearlyEqual)
            return InterpolationNone;

        // The image is growing a lot and in more than one direction. Resampling
        // is slow and doesn't give us very much when growing a lot.
        return InterpolationLow;
    }

    if ((diffWidth / srcWidth < kFractionalChangeThreshold)
        && (diffHeight / srcHeight < kFractionalChangeThreshold)) {
        // It is disappointingly common on the web for image sizes to be off by
        // one or two pixels. We don't bother resampling if the size difference
        // is a small fraction of the original size.
        return InterpolationNone;
    }

    // When the image is not yet done loading, use linear. We don't cache the
    // partially resampled images, and as they come in incrementally, it causes
    // us to have to resample the whole thing every time.
    if (!isDataComplete)
        return InterpolationLow;

    // Everything else gets resampled at high quality.
    return InterpolationHigh;
}

int clampedAlphaForBlending(float alpha)
{
    if (alpha < 0)
        return 0;
    int roundedAlpha = roundf(alpha * 256);
    if (roundedAlpha > 256)
        roundedAlpha = 256;
    return roundedAlpha;
}

SkColor scaleAlpha(SkColor color, float alpha)
{
    return scaleAlpha(color, clampedAlphaForBlending(alpha));
}

SkColor scaleAlpha(SkColor color, int alpha)
{
    int a = (SkColorGetA(color) * alpha) >> 8;
    return (color & 0x00FFFFFF) | (a << 24);
}

template<typename PrimitiveType>
void drawFocusRingPrimitive(const PrimitiveType&, SkCanvas*, const SkPaint&, float cornerRadius)
{
    ASSERT_NOT_REACHED(); // Missing an explicit specialization?
}

template<>
void drawFocusRingPrimitive<SkRect>(const SkRect& rect, SkCanvas* canvas, const SkPaint& paint, float cornerRadius)
{
    SkRRect rrect;
    rrect.setRectXY(rect, SkFloatToScalar(cornerRadius), SkFloatToScalar(cornerRadius));
    canvas->drawRRect(rrect, paint);
}

template<>
void drawFocusRingPrimitive<SkPath>(const SkPath& path, SkCanvas* canvas, const SkPaint& paint, float cornerRadius)
{
    SkPaint pathPaint = paint;
    pathPaint.setPathEffect(SkCornerPathEffect::Create(SkFloatToScalar(cornerRadius)))->unref();
    canvas->drawPath(path, pathPaint);
}

template<typename PrimitiveType>
void drawPlatformFocusRing(const PrimitiveType& primitive, SkCanvas* canvas, SkColor color, int width)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(color);

#if OS(MACOSX)
    paint.setStrokeWidth(width);
    paint.setAlpha(64);
    float cornerRadius = (width - 1) * 0.5f;
#else
    paint.setStrokeWidth(1);
    const float cornerRadius = 1;
#endif

    drawFocusRingPrimitive(primitive, canvas, paint, cornerRadius);

#if OS(MACOSX)
    // Inner part
    paint.setAlpha(128);
    paint.setStrokeWidth(paint.getStrokeWidth() * 0.5f);
    drawFocusRingPrimitive(primitive, canvas, paint, cornerRadius);
#endif
}

template void PLATFORM_EXPORT drawPlatformFocusRing<SkRect>(const SkRect&, SkCanvas*, SkColor, int width);
template void PLATFORM_EXPORT drawPlatformFocusRing<SkPath>(const SkPath&, SkCanvas*, SkColor, int width);

}  // namespace blink
