/*
 * Copyright (C) 2008 Apple Inc.  All rights reserved.
 * Copyright (C) 2015 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/css/CSSGradientValue.h"

#include "core/CSSValueKeywords.h"
#include "core/css/CSSCalculationValue.h"
#include "core/css/CSSToLengthConversionData.h"
#include "core/css/Pair.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/TextLinkColors.h"
#include "core/layout/LayoutObject.h"
#include "platform/geometry/IntSize.h"
#include "platform/graphics/Gradient.h"
#include "platform/graphics/GradientGeneratedImage.h"
#include "platform/graphics/Image.h"
#include "platform/graphics/skia/SkiaUtils.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"
#include <utility>

namespace blink {

DEFINE_TRACE(CSSGradientColorStop)
{
    visitor->trace(m_position);
    visitor->trace(m_color);
}

PassRefPtr<Image> CSSGradientValue::image(LayoutObject* layoutObject, const IntSize& size)
{
    if (size.isEmpty())
        return nullptr;

    bool cacheable = isCacheable();
    if (cacheable) {
        if (!clients().contains(layoutObject))
            return nullptr;

        // Need to look up our size.  Create a string of width*height to use as a hash key.
        Image* result = getImage(layoutObject, size);
        if (result)
            return result;
    }

    // We need to create an image.
    RefPtr<Gradient> gradient;

    const ComputedStyle* rootStyle = layoutObject->document().documentElement()->computedStyle();
    CSSToLengthConversionData conversionData(layoutObject->style(), rootStyle, layoutObject->view(), layoutObject->style()->effectiveZoom());
    if (isLinearGradientValue())
        gradient = toCSSLinearGradientValue(this)->createGradient(conversionData, size, *layoutObject);
    else
        gradient = toCSSRadialGradientValue(this)->createGradient(conversionData, size, *layoutObject);

    RefPtr<Image> newImage = GradientGeneratedImage::create(gradient, size);
    if (cacheable)
        putImage(size, newImage);

    return newImage.release();
}

// Should only ever be called for deprecated gradients.
static inline bool compareStops(const CSSGradientColorStop& a, const CSSGradientColorStop& b)
{
    double aVal = a.m_position->getDoubleValue();
    double bVal = b.m_position->getDoubleValue();

    return aVal < bVal;
}

struct GradientStop {
    Color color;
    float offset;
    bool specified;

    GradientStop()
        : offset(0)
        , specified(false)
    { }
};

static void replaceColorHintsWithColorStops(Vector<GradientStop>& stops, const WillBeHeapVector<CSSGradientColorStop, 2>& cssGradientStops)
{
    // This algorithm will replace each color interpolation hint with 9 regular
    // color stops. The color values for the new color stops will be calculated
    // using the color weighting formula defined in the spec. The new color
    // stops will be positioned in such a way that all the pixels between the two
    // user defined color stops have color values close to the interpolation curve.
    // If the hint is closer to the left color stop, add 2 stops to the left and
    // 6 to the right, else add 6 stops to the left and 2 to the right.
    // The color stops on the side with more space start midway because
    // the curve approximates a line in that region.
    // Using this aproximation, it is possible to discern the color steps when
    // the gradient is large. If this becomes an issue, we can consider improving
    // the algorithm, or adding support for color interpolation hints to skia shaders.

    int indexOffset = 0;

    // The first and the last color stops cannot be color hints.
    for (size_t i = 1; i < cssGradientStops.size() - 1; ++i) {
        if (!cssGradientStops[i].isHint())
            continue;

        // The current index of the stops vector.
        size_t x = i + indexOffset;
        ASSERT(x >= 1);

        // offsetLeft          offset                            offsetRight
        //   |-------------------|---------------------------------|
        //          leftDist                 rightDist

        float offsetLeft = stops[x - 1].offset;
        float offsetRight = stops[x + 1].offset;
        float offset = stops[x].offset;
        float leftDist = offset - offsetLeft;
        float rightDist = offsetRight - offset;
        float totalDist = offsetRight - offsetLeft;

        Color leftColor = stops[x - 1].color;
        Color rightColor = stops[x + 1].color;

        ASSERT(offsetLeft <= offset && offset <= offsetRight);

        if (WebCoreFloatNearlyEqual(leftDist, rightDist)) {
            stops.remove(x);
            --indexOffset;
            continue;
        }

        if (WebCoreFloatNearlyEqual(leftDist, .0f)) {
            stops[x].color = rightColor;
            continue;
        }

        if (WebCoreFloatNearlyEqual(rightDist, .0f)) {
            stops[x].color = leftColor;
            continue;
        }

        GradientStop newStops[9];
        // Position the new color stops.
        if (leftDist > rightDist) {
            for (size_t y = 0; y < 7; ++y)
                newStops[y].offset = offsetLeft + leftDist * (7 + y) / 13;
            newStops[7].offset = offset + rightDist / 3;
            newStops[8].offset = offset + rightDist * 2 / 3;
        } else {
            newStops[0].offset = offsetLeft + leftDist / 3;
            newStops[1].offset = offsetLeft + leftDist * 2 / 3;
            for (size_t y = 0; y < 7; ++y)
                newStops[y + 2].offset = offset + rightDist * y / 13;
        }

        // calculate colors for the new color hints.
        // The color weighting for the new color stops will be pointRelativeOffset^(ln(0.5)/ln(hintRelativeOffset)).
        float hintRelativeOffset = leftDist / totalDist;
        for (size_t y = 0; y < 9; ++y) {
            float pointRelativeOffset = (newStops[y].offset - offsetLeft) / totalDist;
            float weighting = powf(pointRelativeOffset, logf(.5f) / logf(hintRelativeOffset));
            newStops[y].color = blend(leftColor, rightColor, weighting);
        }

        // Replace the color hint with the new color stops.
        stops.remove(x);
        stops.insert(x, newStops, 9);
        indexOffset += 8;
    }
}

static Color resolveStopColor(CSSPrimitiveValue* stopColor, const LayoutObject& object)
{
    return object.document().textLinkColors().colorFromPrimitiveValue(stopColor, object.resolveColor(CSSPropertyColor));
}

void CSSGradientValue::addDeprecatedStops(Gradient* gradient, const LayoutObject& object)
{
    ASSERT(m_gradientType == CSSDeprecatedLinearGradient || m_gradientType == CSSDeprecatedRadialGradient);

    if (!m_stopsSorted) {
        if (m_stops.size())
            std::stable_sort(m_stops.begin(), m_stops.end(), compareStops);
        m_stopsSorted = true;
    }

    for (const auto& stop : m_stops) {
        float offset;
        if (stop.m_position->isPercentage())
            offset = stop.m_position->getFloatValue() / 100;
        else
            offset = stop.m_position->getFloatValue();

        gradient->addColorStop(offset, resolveStopColor(stop.m_color.get(), object));
    }
}

static bool requiresStopsNormalization(const Vector<GradientStop>& stops, const Gradient* gradient)
{
    // We need at least two stops to normalize
    if (stops.size() < 2)
        return false;

    // Repeating gradients are implemented using a normalized stop offset range
    // with the point/radius pairs aligned on the interval endpoints.
    if (gradient->spreadMethod() == SpreadMethodRepeat)
        return true;

    // Degenerate stops
    if (stops.first().offset < 0 || stops.last().offset > 1)
        return true;

    return false;
}

// Redistribute the stops such that they fully cover [0 , 1] and add them to the gradient.
static bool normalizeAndAddStops(const Vector<GradientStop>& stops, Gradient* gradient)
{
    ASSERT(stops.size() > 1);

    const float firstOffset = stops.first().offset;
    const float lastOffset = stops.last().offset;
    const float span = lastOffset - firstOffset;

    if (fabs(span) < std::numeric_limits<float>::epsilon()) {
        // All stops are coincident -> use a single clamped offset value.
        const float clampedOffset = std::min(std::max(firstOffset, 0.f), 1.f);

        // For repeating gradients, a coincident stop set defines a solid-color image with the color
        // of the last color-stop in the rule.
        // For non-repeating gradients, both the first color and the last color can be significant
        // (padding on both sides of the offset).
        if (gradient->spreadMethod() != SpreadMethodRepeat)
            gradient->addColorStop(clampedOffset, stops.first().color);
        gradient->addColorStop(clampedOffset, stops.last().color);

        return false;
    }

    ASSERT(span > 0);

    for (size_t i = 0; i < stops.size(); ++i) {
        const float normalizedOffset = (stops[i].offset - firstOffset) / span;

        // stop offsets should be monotonically increasing in [0 , 1]
        ASSERT(normalizedOffset >= 0 && normalizedOffset <= 1);
        ASSERT(i == 0 || normalizedOffset >= (stops[i - 1].offset - firstOffset) / span);

        gradient->addColorStop(normalizedOffset, stops[i].color);
    }

    return true;
}

// Collapse all negative-offset stops to 0 and compute an interpolated color value for that point.
static void clampNegativeOffsets(Vector<GradientStop>& stops)
{
    float lastNegativeOffset = 0;

    for (size_t i = 0; i < stops.size(); ++i) {
        const float currentOffset = stops[i].offset;
        if (currentOffset >= 0) {
            if (i > 0) {
                // We found the negative -> positive offset transition: compute an interpolated
                // color value for 0 and use it with the last clamped stop.
                ASSERT(lastNegativeOffset < 0);
                float lerpRatio = -lastNegativeOffset / (currentOffset - lastNegativeOffset);
                stops[i - 1].color = blend(stops[i - 1].color, stops[i].color, lerpRatio);
            }

            break;
        }

        // Clamp all negative stops to 0.
        stops[i].offset = 0;
        lastNegativeOffset = currentOffset;
    }
}

// Update the linear gradient points to align with the given offset range.
static void adjustGradientPointsForOffsetRange(Gradient* gradient, float firstOffset, float lastOffset)
{
    ASSERT(!gradient->isRadial());
    ASSERT(firstOffset <= lastOffset);

    const FloatPoint p0 = gradient->p0();
    const FloatPoint p1 = gradient->p1();
    const FloatSize d(p1 - p0);

    // Linear offsets are relative to the [p0 , p1] segment.
    gradient->setP0(p0 + d * firstOffset);
    gradient->setP1(p0 + d * lastOffset);
}

// Update the radial gradient radii to align with the given offset range.
static void adjustGradientRadiiForOffsetRange(Gradient* gradient, float firstOffset, float lastOffset)
{
    ASSERT(gradient->isRadial());
    ASSERT(firstOffset <= lastOffset);

    // Radial offsets are relative to the [0 , endRadius] segment.
    float adjustedR0 = gradient->endRadius() * firstOffset;
    float adjustedR1 = gradient->endRadius() * lastOffset;
    ASSERT(adjustedR0 <= adjustedR1);

    // Unlike linear gradients (where we can adjust the points arbitrarily),
    // we cannot let our radii turn negative here.
    if (adjustedR0 < 0) {
        // For the non-repeat case, this can never happen: clampNegativeOffsets() ensures we don't
        // have to deal with negative offsets at this point.
        ASSERT(gradient->spreadMethod() == SpreadMethodRepeat);

        // When in repeat mode, we deal with it by repositioning both radii in the positive domain -
        // shifting them by a multiple of the radius span (which is the period of our repeating
        // gradient -> hence no visible side effects).
        const float radiusSpan = adjustedR1 - adjustedR0;
        const float shiftToPositive = radiusSpan * ceilf(-adjustedR0 / radiusSpan);
        adjustedR0 += shiftToPositive;
        adjustedR1 += shiftToPositive;
    }
    ASSERT(adjustedR0 >= 0);
    ASSERT(adjustedR1 >= adjustedR0);

    gradient->setStartRadius(adjustedR0);
    gradient->setEndRadius(adjustedR1);
}

void CSSGradientValue::addStops(Gradient* gradient, const CSSToLengthConversionData& conversionData,
    const LayoutObject& object)
{
    if (m_gradientType == CSSDeprecatedLinearGradient || m_gradientType == CSSDeprecatedRadialGradient) {
        addDeprecatedStops(gradient, object);
        return;
    }

    size_t numStops = m_stops.size();

    Vector<GradientStop> stops(numStops);

    bool hasHints = false;

    FloatPoint gradientStart = gradient->p0();
    FloatPoint gradientEnd;
    if (isLinearGradientValue())
        gradientEnd = gradient->p1();
    else if (isRadialGradientValue())
        gradientEnd = gradientStart + FloatSize(gradient->endRadius(), 0);
    float gradientLength = FloatSize(gradientStart - gradientEnd).diagonalLength();

    for (size_t i = 0; i < numStops; ++i) {
        const CSSGradientColorStop& stop = m_stops[i];

        if (stop.isHint())
            hasHints = true;
        else
            stops[i].color = resolveStopColor(stop.m_color.get(), object);

        if (stop.m_position) {
            if (stop.m_position->isPercentage())
                stops[i].offset = stop.m_position->getFloatValue() / 100;
            else if (stop.m_position->isLength() || stop.m_position->isCalculatedPercentageWithLength()) {
                float length;
                if (stop.m_position->isLength())
                    length = stop.m_position->computeLength<float>(conversionData);
                else
                    length = stop.m_position->cssCalcValue()->toCalcValue(conversionData)->evaluate(gradientLength);
                stops[i].offset = (gradientLength > 0) ? length / gradientLength : 0;
            } else {
                ASSERT_NOT_REACHED();
                stops[i].offset = 0;
            }
            stops[i].specified = true;
        } else {
            // If the first color-stop does not have a position, its position defaults to 0%.
            // If the last color-stop does not have a position, its position defaults to 100%.
            if (!i) {
                stops[i].offset = 0;
                stops[i].specified = true;
            } else if (numStops > 1 && i == numStops - 1) {
                stops[i].offset = 1;
                stops[i].specified = true;
            }
        }

        // If a color-stop has a position that is less than the specified position of any
        // color-stop before it in the list, its position is changed to be equal to the
        // largest specified position of any color-stop before it.
        if (stops[i].specified && i > 0) {
            size_t prevSpecifiedIndex;
            for (prevSpecifiedIndex = i - 1; prevSpecifiedIndex; --prevSpecifiedIndex) {
                if (stops[prevSpecifiedIndex].specified)
                    break;
            }

            if (stops[i].offset < stops[prevSpecifiedIndex].offset)
                stops[i].offset = stops[prevSpecifiedIndex].offset;
        }
    }

    ASSERT(stops.first().specified && stops.last().specified);

    // If any color-stop still does not have a position, then, for each run of adjacent
    // color-stops without positions, set their positions so that they are evenly spaced
    // between the preceding and following color-stops with positions.
    if (numStops > 2) {
        size_t unspecifiedRunStart = 0;
        bool inUnspecifiedRun = false;

        for (size_t i = 0; i < numStops; ++i) {
            if (!stops[i].specified && !inUnspecifiedRun) {
                unspecifiedRunStart = i;
                inUnspecifiedRun = true;
            } else if (stops[i].specified && inUnspecifiedRun) {
                size_t unspecifiedRunEnd = i;

                if (unspecifiedRunStart < unspecifiedRunEnd) {
                    float lastSpecifiedOffset = stops[unspecifiedRunStart - 1].offset;
                    float nextSpecifiedOffset = stops[unspecifiedRunEnd].offset;
                    float delta = (nextSpecifiedOffset - lastSpecifiedOffset) / (unspecifiedRunEnd - unspecifiedRunStart + 1);

                    for (size_t j = unspecifiedRunStart; j < unspecifiedRunEnd; ++j)
                        stops[j].offset = lastSpecifiedOffset + (j - unspecifiedRunStart + 1) * delta;
                }

                inUnspecifiedRun = false;
            }
        }
    }

    ASSERT(stops.size() == m_stops.size());
    if (hasHints) {
        replaceColorHintsWithColorStops(stops, m_stops);
    }

    // At this point we have a fully resolved set of stops. Time to perform adjustments for
    // repeat gradients and degenerate values if needed.
    if (requiresStopsNormalization(stops, gradient)) {
        // Negative offsets are only an issue for non-repeating radial gradients: linear gradient
        // points can be repositioned arbitrarily, and for repeating radial gradients we shift
        // the radii into equivalent positive values.
        if (isRadialGradientValue() && !m_repeating)
            clampNegativeOffsets(stops);

        if (normalizeAndAddStops(stops, gradient)) {
            if (isLinearGradientValue()) {
                adjustGradientPointsForOffsetRange(gradient, stops.first().offset, stops.last().offset);
            } else {
                adjustGradientRadiiForOffsetRange(gradient, stops.first().offset, stops.last().offset);
            }
        } else {
            // Normalization failed because the stop set is coincident.
        }
    } else {
        // No normalization required, just add the current stops.
        for (const auto& stop : stops)
            gradient->addColorStop(stop.offset, stop.color);
    }
}

static float positionFromValue(CSSPrimitiveValue* value, const CSSToLengthConversionData& conversionData, const IntSize& size, bool isHorizontal)
{
    int origin = 0;
    int sign = 1;
    int edgeDistance = isHorizontal ? size.width() : size.height();

    // In this case the center of the gradient is given relative to an edge in the form of:
    // [ top | bottom | right | left ] [ <percentage> | <length> ].
    if (Pair* pair = value->getPairValue()) {
        CSSValueID originID = pair->first()->getValueID();
        value = pair->second();

        if (originID == CSSValueRight || originID == CSSValueBottom) {
            // For right/bottom, the offset is relative to the far edge.
            origin = edgeDistance;
            sign = -1;
        }
    }

    if (value->isNumber())
        return origin + sign * value->getFloatValue() * conversionData.zoom();

    if (value->isPercentage())
        return origin + sign * value->getFloatValue() / 100.f * edgeDistance;

    if (value->isCalculatedPercentageWithLength())
        return origin + sign * value->cssCalcValue()->toCalcValue(conversionData)->evaluate(edgeDistance);

    switch (value->getValueID()) {
    case CSSValueTop:
        ASSERT(!isHorizontal);
        return 0;
    case CSSValueLeft:
        ASSERT(isHorizontal);
        return 0;
    case CSSValueBottom:
        ASSERT(!isHorizontal);
        return size.height();
    case CSSValueRight:
        ASSERT(isHorizontal);
        return size.width();
    default:
        break;
    }

    return origin + sign * value->computeLength<float>(conversionData);
}

FloatPoint CSSGradientValue::computeEndPoint(CSSPrimitiveValue* horizontal, CSSPrimitiveValue* vertical, const CSSToLengthConversionData& conversionData, const IntSize& size)
{
    FloatPoint result;

    if (horizontal)
        result.setX(positionFromValue(horizontal, conversionData, size, true));

    if (vertical)
        result.setY(positionFromValue(vertical, conversionData, size, false));

    return result;
}

bool CSSGradientValue::isCacheable() const
{
    for (size_t i = 0; i < m_stops.size(); ++i) {
        const CSSGradientColorStop& stop = m_stops[i];

        if (!stop.isHint() && stop.m_color->colorIsDerivedFromElement())
            return false;

        if (!stop.m_position)
            continue;

        if (stop.m_position->isFontRelativeLength())
            return false;
    }

    return true;
}

bool CSSGradientValue::knownToBeOpaque(const LayoutObject* object) const
{
    ASSERT(object);
    for (auto& stop : m_stops) {
        if (!stop.isHint() && resolveStopColor(stop.m_color.get(), *object).hasAlpha())
            return false;
    }
    return true;
}

DEFINE_TRACE_AFTER_DISPATCH(CSSGradientValue)
{
#if ENABLE(OILPAN)
    visitor->trace(m_firstX);
    visitor->trace(m_firstY);
    visitor->trace(m_secondX);
    visitor->trace(m_secondY);
    visitor->trace(m_stops);
#endif
    CSSImageGeneratorValue::traceAfterDispatch(visitor);
}

String CSSLinearGradientValue::customCSSText() const
{
    StringBuilder result;
    if (m_gradientType == CSSDeprecatedLinearGradient) {
        result.appendLiteral("-webkit-gradient(linear, ");
        result.append(m_firstX->cssText());
        result.append(' ');
        result.append(m_firstY->cssText());
        result.appendLiteral(", ");
        result.append(m_secondX->cssText());
        result.append(' ');
        result.append(m_secondY->cssText());
        appendCSSTextForDeprecatedColorStops(result);
    } else if (m_gradientType == CSSPrefixedLinearGradient) {
        if (m_repeating)
            result.appendLiteral("-webkit-repeating-linear-gradient(");
        else
            result.appendLiteral("-webkit-linear-gradient(");

        if (m_angle)
            result.append(m_angle->cssText());
        else {
            if (m_firstX && m_firstY) {
                result.append(m_firstX->cssText());
                result.append(' ');
                result.append(m_firstY->cssText());
            } else if (m_firstX || m_firstY) {
                if (m_firstX)
                    result.append(m_firstX->cssText());

                if (m_firstY)
                    result.append(m_firstY->cssText());
            }
        }

        for (unsigned i = 0; i < m_stops.size(); i++) {
            const CSSGradientColorStop& stop = m_stops[i];
            result.appendLiteral(", ");
            result.append(stop.m_color->cssText());
            if (stop.m_position) {
                result.append(' ');
                result.append(stop.m_position->cssText());
            }
        }
    } else {
        if (m_repeating)
            result.appendLiteral("repeating-linear-gradient(");
        else
            result.appendLiteral("linear-gradient(");

        bool wroteSomething = false;

        if (m_angle && m_angle->computeDegrees() != 180) {
            result.append(m_angle->cssText());
            wroteSomething = true;
        } else if ((m_firstX || m_firstY) && !(!m_firstX && m_firstY && m_firstY->getValueID() == CSSValueBottom)) {
            result.appendLiteral("to ");
            if (m_firstX && m_firstY) {
                result.append(m_firstX->cssText());
                result.append(' ');
                result.append(m_firstY->cssText());
            } else if (m_firstX)
                result.append(m_firstX->cssText());
            else
                result.append(m_firstY->cssText());
            wroteSomething = true;
        }

        if (wroteSomething)
            result.appendLiteral(", ");

        for (unsigned i = 0; i < m_stops.size(); i++) {
            const CSSGradientColorStop& stop = m_stops[i];
            if (i)
                result.appendLiteral(", ");
            if (stop.m_color)
                result.append(stop.m_color->cssText());
            if (stop.m_color && stop.m_position)
                result.append(' ');
            if (stop.m_position)
                result.append(stop.m_position->cssText());
        }

    }

    result.append(')');
    return result.toString();
}

// Compute the endpoints so that a gradient of the given angle covers a box of the given size.
static void endPointsFromAngle(float angleDeg, const IntSize& size, FloatPoint& firstPoint, FloatPoint& secondPoint, CSSGradientType type)
{
    // Prefixed gradients use "polar coordinate" angles, rather than "bearing" angles.
    if (type == CSSPrefixedLinearGradient)
        angleDeg = 90 - angleDeg;

    angleDeg = fmodf(angleDeg, 360);
    if (angleDeg < 0)
        angleDeg += 360;

    if (!angleDeg) {
        firstPoint.set(0, size.height());
        secondPoint.set(0, 0);
        return;
    }

    if (angleDeg == 90) {
        firstPoint.set(0, 0);
        secondPoint.set(size.width(), 0);
        return;
    }

    if (angleDeg == 180) {
        firstPoint.set(0, 0);
        secondPoint.set(0, size.height());
        return;
    }

    if (angleDeg == 270) {
        firstPoint.set(size.width(), 0);
        secondPoint.set(0, 0);
        return;
    }

    // angleDeg is a "bearing angle" (0deg = N, 90deg = E),
    // but tan expects 0deg = E, 90deg = N.
    float slope = tan(deg2rad(90 - angleDeg));

    // We find the endpoint by computing the intersection of the line formed by the slope,
    // and a line perpendicular to it that intersects the corner.
    float perpendicularSlope = -1 / slope;

    // Compute start corner relative to center, in Cartesian space (+y = up).
    float halfHeight = size.height() / 2;
    float halfWidth = size.width() / 2;
    FloatPoint endCorner;
    if (angleDeg < 90)
        endCorner.set(halfWidth, halfHeight);
    else if (angleDeg < 180)
        endCorner.set(halfWidth, -halfHeight);
    else if (angleDeg < 270)
        endCorner.set(-halfWidth, -halfHeight);
    else
        endCorner.set(-halfWidth, halfHeight);

    // Compute c (of y = mx + c) using the corner point.
    float c = endCorner.y() - perpendicularSlope * endCorner.x();
    float endX = c / (slope - perpendicularSlope);
    float endY = perpendicularSlope * endX + c;

    // We computed the end point, so set the second point,
    // taking into account the moved origin and the fact that we're in drawing space (+y = down).
    secondPoint.set(halfWidth + endX, halfHeight - endY);
    // Reflect around the center for the start point.
    firstPoint.set(halfWidth - endX, halfHeight + endY);
}

PassRefPtr<Gradient> CSSLinearGradientValue::createGradient(const CSSToLengthConversionData& conversionData, const IntSize& size, const LayoutObject& object)
{
    ASSERT(!size.isEmpty());

    FloatPoint firstPoint;
    FloatPoint secondPoint;
    if (m_angle) {
        float angle = m_angle->computeDegrees();
        endPointsFromAngle(angle, size, firstPoint, secondPoint, m_gradientType);
    } else {
        switch (m_gradientType) {
        case CSSDeprecatedLinearGradient:
            firstPoint = computeEndPoint(m_firstX.get(), m_firstY.get(), conversionData, size);
            if (m_secondX || m_secondY)
                secondPoint = computeEndPoint(m_secondX.get(), m_secondY.get(), conversionData, size);
            else {
                if (m_firstX)
                    secondPoint.setX(size.width() - firstPoint.x());
                if (m_firstY)
                    secondPoint.setY(size.height() - firstPoint.y());
            }
            break;
        case CSSPrefixedLinearGradient:
            firstPoint = computeEndPoint(m_firstX.get(), m_firstY.get(), conversionData, size);
            if (m_firstX)
                secondPoint.setX(size.width() - firstPoint.x());
            if (m_firstY)
                secondPoint.setY(size.height() - firstPoint.y());
            break;
        case CSSLinearGradient:
            if (m_firstX && m_firstY) {
                // "Magic" corners, so the 50% line touches two corners.
                float rise = size.width();
                float run = size.height();
                if (m_firstX && m_firstX->getValueID() == CSSValueLeft)
                    run *= -1;
                if (m_firstY && m_firstY->getValueID() == CSSValueBottom)
                    rise *= -1;
                // Compute angle, and flip it back to "bearing angle" degrees.
                float angle = 90 - rad2deg(atan2(rise, run));
                endPointsFromAngle(angle, size, firstPoint, secondPoint, m_gradientType);
            } else if (m_firstX || m_firstY) {
                secondPoint = computeEndPoint(m_firstX.get(), m_firstY.get(), conversionData, size);
                if (m_firstX)
                    firstPoint.setX(size.width() - secondPoint.x());
                if (m_firstY)
                    firstPoint.setY(size.height() - secondPoint.y());
            } else
                secondPoint.setY(size.height());
            break;
        default:
            ASSERT_NOT_REACHED();
        }

    }

    RefPtr<Gradient> gradient = Gradient::create(firstPoint, secondPoint);

    gradient->setSpreadMethod(m_repeating ? SpreadMethodRepeat : SpreadMethodPad);
    gradient->setDrawsInPMColorSpace(true);

    // Now add the stops.
    addStops(gradient.get(), conversionData, object);

    return gradient.release();
}

bool CSSLinearGradientValue::equals(const CSSLinearGradientValue& other) const
{
    if (m_gradientType == CSSDeprecatedLinearGradient)
        return other.m_gradientType == m_gradientType
            && compareCSSValuePtr(m_firstX, other.m_firstX)
            && compareCSSValuePtr(m_firstY, other.m_firstY)
            && compareCSSValuePtr(m_secondX, other.m_secondX)
            && compareCSSValuePtr(m_secondY, other.m_secondY)
            && m_stops == other.m_stops;

    if (m_repeating != other.m_repeating)
        return false;

    if (m_angle)
        return compareCSSValuePtr(m_angle, other.m_angle) && m_stops == other.m_stops;

    if (other.m_angle)
        return false;

    bool equalXandY = false;
    if (m_firstX && m_firstY)
        equalXandY = compareCSSValuePtr(m_firstX, other.m_firstX) && compareCSSValuePtr(m_firstY, other.m_firstY);
    else if (m_firstX)
        equalXandY = compareCSSValuePtr(m_firstX, other.m_firstX) && !other.m_firstY;
    else if (m_firstY)
        equalXandY = compareCSSValuePtr(m_firstY, other.m_firstY) && !other.m_firstX;
    else
        equalXandY = !other.m_firstX && !other.m_firstY;

    return equalXandY && m_stops == other.m_stops;
}

DEFINE_TRACE_AFTER_DISPATCH(CSSLinearGradientValue)
{
    visitor->trace(m_angle);
    CSSGradientValue::traceAfterDispatch(visitor);
}

inline void CSSGradientValue::appendCSSTextForDeprecatedColorStops(StringBuilder& result) const
{
    for (unsigned i = 0; i < m_stops.size(); i++) {
        const CSSGradientColorStop& stop = m_stops[i];
        result.appendLiteral(", ");
        if (stop.m_position->getDoubleValue() == 0) {
            result.appendLiteral("from(");
            result.append(stop.m_color->cssText());
            result.append(')');
        } else if (stop.m_position->getDoubleValue() == 1) {
            result.appendLiteral("to(");
            result.append(stop.m_color->cssText());
            result.append(')');
        } else {
            result.appendLiteral("color-stop(");
            result.appendNumber(stop.m_position->getDoubleValue());
            result.appendLiteral(", ");
            result.append(stop.m_color->cssText());
            result.append(')');
        }
    }
}

String CSSRadialGradientValue::customCSSText() const
{
    StringBuilder result;

    if (m_gradientType == CSSDeprecatedRadialGradient) {
        result.appendLiteral("-webkit-gradient(radial, ");
        result.append(m_firstX->cssText());
        result.append(' ');
        result.append(m_firstY->cssText());
        result.appendLiteral(", ");
        result.append(m_firstRadius->cssText());
        result.appendLiteral(", ");
        result.append(m_secondX->cssText());
        result.append(' ');
        result.append(m_secondY->cssText());
        result.appendLiteral(", ");
        result.append(m_secondRadius->cssText());
        appendCSSTextForDeprecatedColorStops(result);
    } else if (m_gradientType == CSSPrefixedRadialGradient) {
        if (m_repeating)
            result.appendLiteral("-webkit-repeating-radial-gradient(");
        else
            result.appendLiteral("-webkit-radial-gradient(");

        if (m_firstX && m_firstY) {
            result.append(m_firstX->cssText());
            result.append(' ');
            result.append(m_firstY->cssText());
        } else if (m_firstX)
            result.append(m_firstX->cssText());
         else if (m_firstY)
            result.append(m_firstY->cssText());
        else
            result.appendLiteral("center");

        if (m_shape || m_sizingBehavior) {
            result.appendLiteral(", ");
            if (m_shape) {
                result.append(m_shape->cssText());
                result.append(' ');
            } else
                result.appendLiteral("ellipse ");

            if (m_sizingBehavior)
                result.append(m_sizingBehavior->cssText());
            else
                result.appendLiteral("cover");

        } else if (m_endHorizontalSize && m_endVerticalSize) {
            result.appendLiteral(", ");
            result.append(m_endHorizontalSize->cssText());
            result.append(' ');
            result.append(m_endVerticalSize->cssText());
        }

        for (unsigned i = 0; i < m_stops.size(); i++) {
            const CSSGradientColorStop& stop = m_stops[i];
            result.appendLiteral(", ");
            result.append(stop.m_color->cssText());
            if (stop.m_position) {
                result.append(' ');
                result.append(stop.m_position->cssText());
            }
        }
    } else {
        if (m_repeating)
            result.appendLiteral("repeating-radial-gradient(");
        else
            result.appendLiteral("radial-gradient(");

        bool wroteSomething = false;

        // The only ambiguous case that needs an explicit shape to be provided
        // is when a sizing keyword is used (or all sizing is omitted).
        if (m_shape && m_shape->getValueID() != CSSValueEllipse && (m_sizingBehavior || (!m_sizingBehavior && !m_endHorizontalSize))) {
            result.appendLiteral("circle");
            wroteSomething = true;
        }

        if (m_sizingBehavior && m_sizingBehavior->getValueID() != CSSValueFarthestCorner) {
            if (wroteSomething)
                result.append(' ');
            result.append(m_sizingBehavior->cssText());
            wroteSomething = true;
        } else if (m_endHorizontalSize) {
            if (wroteSomething)
                result.append(' ');
            result.append(m_endHorizontalSize->cssText());
            if (m_endVerticalSize) {
                result.append(' ');
                result.append(m_endVerticalSize->cssText());
            }
            wroteSomething = true;
        }

        if (m_firstX || m_firstY) {
            if (wroteSomething)
                result.append(' ');
            result.appendLiteral("at ");
            if (m_firstX && m_firstY) {
                result.append(m_firstX->cssText());
                result.append(' ');
                result.append(m_firstY->cssText());
            } else if (m_firstX)
                result.append(m_firstX->cssText());
            else
                result.append(m_firstY->cssText());
            wroteSomething = true;
        }

        if (wroteSomething)
            result.appendLiteral(", ");

        for (unsigned i = 0; i < m_stops.size(); i++) {
            const CSSGradientColorStop& stop = m_stops[i];
            if (i)
                result.appendLiteral(", ");
            if (stop.m_color)
                result.append(stop.m_color->cssText());
            if (stop.m_color && stop.m_position)
                result.append(' ');
            if (stop.m_position)
                result.append(stop.m_position->cssText());
        }

    }

    result.append(')');
    return result.toString();
}

float CSSRadialGradientValue::resolveRadius(CSSPrimitiveValue* radius, const CSSToLengthConversionData& conversionData, float* widthOrHeight)
{
    float result = 0;
    if (radius->isNumber()) // Can the radius be a percentage?
        result = radius->getFloatValue() * conversionData.zoom();
    else if (widthOrHeight && radius->isPercentage())
        result = *widthOrHeight * radius->getFloatValue() / 100;
    else
        result = radius->computeLength<float>(conversionData);

    return std::max(result, 0.0f);
}

namespace {

enum EndShapeType {
    CircleEndShape,
    EllipseEndShape
};

// Compute the radius to the closest/farthest side (depending on the compare functor).
FloatSize radiusToSide(const FloatPoint& point, const FloatSize& size, EndShapeType shape,
    bool (*compare)(float, float))
{
    float dx1 = fabs(point.x());
    float dy1 = fabs(point.y());
    float dx2 = fabs(point.x() - size.width());
    float dy2 = fabs(point.y() - size.height());

    float dx = compare(dx1, dx2) ? dx1 : dx2;
    float dy = compare(dy1, dy2) ? dy1 : dy2;

    if (shape == CircleEndShape)
        return compare(dx, dy) ? FloatSize(dx, dx) : FloatSize(dy, dy);

    ASSERT(shape == EllipseEndShape);
    return FloatSize(dx, dy);
}

// Compute the radius of an ellipse with center at 0,0 which passes through p, and has
// width/height given by aspectRatio.
inline FloatSize ellipseRadius(const FloatPoint& p, float aspectRatio)
{
    // x^2/a^2 + y^2/b^2 = 1
    // a/b = aspectRatio, b = a/aspectRatio
    // a = sqrt(x^2 + y^2/(1/r^2))
    float a = sqrtf(p.x() * p.x() + p.y() * p.y() * aspectRatio * aspectRatio);
    return FloatSize(a, a / aspectRatio);
}

// Compute the radius to the closest/farthest corner (depending on the compare functor).
FloatSize radiusToCorner(const FloatPoint& point, const FloatSize& size, EndShapeType shape,
    bool (*compare)(float, float))
{
    const FloatRect rect(FloatPoint(), size);
    const FloatPoint corners[] = {
        rect.minXMinYCorner(),
        rect.maxXMinYCorner(),
        rect.maxXMaxYCorner(),
        rect.minXMaxYCorner()
    };

    unsigned cornerIndex = 0;
    float distance = (point - corners[cornerIndex]).diagonalLength();
    for (unsigned i = 1; i < WTF_ARRAY_LENGTH(corners); ++i) {
        float newDistance = (point - corners[i]).diagonalLength();
        if (compare(newDistance, distance)) {
            cornerIndex = i;
            distance = newDistance;
        }
    }

    if (shape == CircleEndShape)
        return FloatSize(distance, distance);

    ASSERT(shape == EllipseEndShape);
    // If the end shape is an ellipse, the gradient-shape has the same ratio of width to height
    // that it would if closest-side or farthest-side were specified, as appropriate.
    const FloatSize sideRadius = radiusToSide(point, size, EllipseEndShape, compare);

    return ellipseRadius(FloatPoint(corners[cornerIndex] - point), sideRadius.aspectRatio());
}

} // anonymous namespace

PassRefPtr<Gradient> CSSRadialGradientValue::createGradient(const CSSToLengthConversionData& conversionData, const IntSize& size, const LayoutObject& object)
{
    ASSERT(!size.isEmpty());

    FloatPoint firstPoint = computeEndPoint(m_firstX.get(), m_firstY.get(), conversionData, size);
    if (!m_firstX)
        firstPoint.setX(size.width() / 2);
    if (!m_firstY)
        firstPoint.setY(size.height() / 2);

    FloatPoint secondPoint = computeEndPoint(m_secondX.get(), m_secondY.get(), conversionData, size);
    if (!m_secondX)
        secondPoint.setX(size.width() / 2);
    if (!m_secondY)
        secondPoint.setY(size.height() / 2);

    float firstRadius = 0;
    if (m_firstRadius)
        firstRadius = resolveRadius(m_firstRadius.get(), conversionData);

    FloatSize secondRadius(0, 0);
    if (m_secondRadius) {
        secondRadius.setWidth(resolveRadius(m_secondRadius.get(), conversionData));
        secondRadius.setHeight(secondRadius.width());
    } else if (m_endHorizontalSize) {
        float width = size.width();
        float height = size.height();
        secondRadius.setWidth(resolveRadius(m_endHorizontalSize.get(), conversionData, &width));
        secondRadius.setHeight(m_endVerticalSize
            ? resolveRadius(m_endVerticalSize.get(), conversionData, &height)
            : secondRadius.width());
    } else {
        EndShapeType shape = (m_shape && m_shape->getValueID() == CSSValueCircle) ||
            (!m_shape && !m_sizingBehavior && m_endHorizontalSize && !m_endVerticalSize)
            ? CircleEndShape
            : EllipseEndShape;

        switch (m_sizingBehavior ? m_sizingBehavior->getValueID() : 0) {
        case CSSValueContain:
        case CSSValueClosestSide:
            secondRadius = radiusToSide(secondPoint, size, shape,
                [] (float a, float b) { return a < b; });
            break;
        case CSSValueFarthestSide:
            secondRadius = radiusToSide(secondPoint, size, shape,
                [] (float a, float b) { return a > b; });
            break;
        case CSSValueClosestCorner:
            secondRadius = radiusToCorner(secondPoint, size, shape,
                [] (float a, float b) { return a < b; });
            break;
        default:
            secondRadius = radiusToCorner(secondPoint, size, shape,
                [] (float a, float b) { return a > b; });
            break;
        }
    }

    bool isDegenerate = !secondRadius.width() || !secondRadius.height();
    RefPtr<Gradient> gradient = Gradient::create(firstPoint, firstRadius, secondPoint,
        isDegenerate ? 0 : secondRadius.width(), isDegenerate ? 1 : secondRadius.aspectRatio());

    gradient->setSpreadMethod(m_repeating ? SpreadMethodRepeat : SpreadMethodPad);
    gradient->setDrawsInPMColorSpace(true);

    // Now add the stops.
    addStops(gradient.get(), conversionData, object);

    return gradient.release();
}

bool CSSRadialGradientValue::equals(const CSSRadialGradientValue& other) const
{
    if (m_gradientType == CSSDeprecatedRadialGradient)
        return other.m_gradientType == m_gradientType
            && compareCSSValuePtr(m_firstX, other.m_firstX)
            && compareCSSValuePtr(m_firstY, other.m_firstY)
            && compareCSSValuePtr(m_secondX, other.m_secondX)
            && compareCSSValuePtr(m_secondY, other.m_secondY)
            && compareCSSValuePtr(m_firstRadius, other.m_firstRadius)
            && compareCSSValuePtr(m_secondRadius, other.m_secondRadius)
            && m_stops == other.m_stops;

    if (m_repeating != other.m_repeating)
        return false;

    bool equalXandY = false;
    if (m_firstX && m_firstY)
        equalXandY = compareCSSValuePtr(m_firstX, other.m_firstX) && compareCSSValuePtr(m_firstY, other.m_firstY);
    else if (m_firstX)
        equalXandY = compareCSSValuePtr(m_firstX, other.m_firstX) && !other.m_firstY;
    else if (m_firstY)
        equalXandY = compareCSSValuePtr(m_firstY, other.m_firstY) && !other.m_firstX;
    else
        equalXandY = !other.m_firstX && !other.m_firstY;

    if (!equalXandY)
        return false;

    bool equalShape = true;
    bool equalSizingBehavior = true;
    bool equalHorizontalAndVerticalSize = true;

    if (m_shape)
        equalShape = compareCSSValuePtr(m_shape, other.m_shape);
    else if (m_sizingBehavior)
        equalSizingBehavior = compareCSSValuePtr(m_sizingBehavior, other.m_sizingBehavior);
    else if (m_endHorizontalSize && m_endVerticalSize)
        equalHorizontalAndVerticalSize = compareCSSValuePtr(m_endHorizontalSize, other.m_endHorizontalSize) && compareCSSValuePtr(m_endVerticalSize, other.m_endVerticalSize);
    else {
        equalShape = !other.m_shape;
        equalSizingBehavior = !other.m_sizingBehavior;
        equalHorizontalAndVerticalSize = !other.m_endHorizontalSize && !other.m_endVerticalSize;
    }
    return equalShape && equalSizingBehavior && equalHorizontalAndVerticalSize && m_stops == other.m_stops;
}

DEFINE_TRACE_AFTER_DISPATCH(CSSRadialGradientValue)
{
    visitor->trace(m_firstRadius);
    visitor->trace(m_secondRadius);
    visitor->trace(m_shape);
    visitor->trace(m_sizingBehavior);
    visitor->trace(m_endHorizontalSize);
    visitor->trace(m_endVerticalSize);
    CSSGradientValue::traceAfterDispatch(visitor);
}

} // namespace blink
