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

// All of the functions in this file should move to new homes and this file should be deleted.

#ifndef SkiaUtils_h
#define SkiaUtils_h

#include "platform/PlatformExport.h"
#include "platform/geometry/FloatRect.h"
#include "platform/graphics/Color.h"
#include "platform/graphics/GraphicsTypes.h"
#include "platform/transforms/AffineTransform.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/effects/SkCornerPathEffect.h"
#include "wtf/MathExtras.h"

namespace blink {

class GraphicsContext;

SkXfermode::Mode PLATFORM_EXPORT WebCoreCompositeToSkiaComposite(CompositeOperator, WebBlendMode = WebBlendModeNormal);
CompositeOperator PLATFORM_EXPORT compositeOperatorFromSkia(SkXfermode::Mode);
WebBlendMode PLATFORM_EXPORT blendModeFromSkia(SkXfermode::Mode);

// Map alpha values from [0, 1] to [0, 256] for alpha blending.
int PLATFORM_EXPORT clampedAlphaForBlending(float);

// Multiply a color's alpha channel by an additional alpha factor where
// alpha is in the range [0, 1].
SkColor PLATFORM_EXPORT scaleAlpha(SkColor, float);

// Multiply a color's alpha channel by an additional alpha factor where
// alpha is in the range [0, 256].
SkColor PLATFORM_EXPORT scaleAlpha(SkColor, int);

inline SkFilterQuality WebCoreInterpolationQualityToSkFilterQuality(InterpolationQuality quality)
{
    // FIXME: this reflects existing client mappings, but should probably
    // be expanded to map higher level interpolations more accurately.
    return quality != InterpolationNone ? kLow_SkFilterQuality : kNone_SkFilterQuality;
}

// Skia has problems when passed infinite, etc floats, filter them to 0.
inline SkScalar WebCoreFloatToSkScalar(float f)
{
    return SkFloatToScalar(std::isfinite(f) ? f : 0);
}

inline SkScalar WebCoreDoubleToSkScalar(double d)
{
    return SkDoubleToScalar(std::isfinite(d) ? d : 0);
}

inline SkRect WebCoreFloatRectToSKRect(const FloatRect& rect)
{
    return SkRect::MakeLTRB(SkFloatToScalar(rect.x()), SkFloatToScalar(rect.y()),
        SkFloatToScalar(rect.maxX()), SkFloatToScalar(rect.maxY()));
}

inline bool WebCoreFloatNearlyEqual(float a, float b)
{
    return SkScalarNearlyEqual(WebCoreFloatToSkScalar(a), WebCoreFloatToSkScalar(b));
}

inline SkPath::FillType WebCoreWindRuleToSkFillType(WindRule rule)
{
    return static_cast<SkPath::FillType>(rule);
}

inline WindRule SkFillTypeToWindRule(SkPath::FillType fillType)
{
    switch (fillType) {
    case SkPath::kWinding_FillType:
    case SkPath::kEvenOdd_FillType:
        return static_cast<WindRule>(fillType);
    default:
        ASSERT_NOT_REACHED();
        break;
    }
    return RULE_NONZERO;
}

// Determine if a given WebKit point is contained in a path
bool SkPathContainsPoint(const SkPath&, const FloatPoint&, SkPath::FillType);

SkMatrix PLATFORM_EXPORT affineTransformToSkMatrix(const AffineTransform&);

bool nearlyIntegral(float value);

InterpolationQuality limitInterpolationQuality(const GraphicsContext*, InterpolationQuality resampling);

InterpolationQuality computeInterpolationQuality(
    float srcWidth,
    float srcHeight,
    float destWidth,
    float destHeight,
    bool isDataComplete = true);

// This replicates the old skia behavior when it used to take radius for blur. Now it takes sigma.
inline SkScalar skBlurRadiusToSigma(SkScalar radius)
{
    SkASSERT(radius >= 0);
    return 0.288675f * radius + 0.5f;
}

template<typename PrimitiveType>
void drawPlatformFocusRing(const PrimitiveType&, SkCanvas*, SkColor, int width);

} // namespace blink

#endif  // SkiaUtils_h
