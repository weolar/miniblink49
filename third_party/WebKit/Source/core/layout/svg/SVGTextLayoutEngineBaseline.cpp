/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/layout/svg/SVGTextLayoutEngineBaseline.h"

#include "core/layout/LayoutObject.h"
#include "core/layout/svg/SVGTextMetrics.h"
#include "core/style/SVGComputedStyle.h"
#include "core/svg/SVGLengthContext.h"
#include "platform/fonts/Font.h"
#include "platform/text/UnicodeRange.h"

namespace blink {

SVGTextLayoutEngineBaseline::SVGTextLayoutEngineBaseline(const Font& font, float effectiveZoom)
    : m_font(font)
    , m_effectiveZoom(effectiveZoom)
{
    ASSERT(m_effectiveZoom);
}

float SVGTextLayoutEngineBaseline::calculateBaselineShift(const ComputedStyle& style) const
{
    const SVGComputedStyle& svgStyle = style.svgStyle();

    switch (svgStyle.baselineShift()) {
    case BS_LENGTH:
        return SVGLengthContext::valueForLength(svgStyle.baselineShiftValue(), style, m_font.fontDescription().computedPixelSize() / m_effectiveZoom);
    case BS_SUB:
        return -m_font.fontMetrics().floatHeight() / 2 / m_effectiveZoom;
    case BS_SUPER:
        return m_font.fontMetrics().floatHeight() / 2 / m_effectiveZoom;
    default:
        ASSERT_NOT_REACHED();
        return 0;
    }
}

EAlignmentBaseline SVGTextLayoutEngineBaseline::dominantBaselineToAlignmentBaseline(bool isVerticalText, const LayoutObject* textLayoutObject) const
{
    ASSERT(textLayoutObject);
    ASSERT(textLayoutObject->style());
    ASSERT(textLayoutObject->parent());
    ASSERT(textLayoutObject->parent()->style());

    const SVGComputedStyle& style = textLayoutObject->style()->svgStyle();

    EDominantBaseline baseline = style.dominantBaseline();
    if (baseline == DB_AUTO) {
        if (isVerticalText)
            baseline = DB_CENTRAL;
        else
            baseline = DB_ALPHABETIC;
    }

    switch (baseline) {
    case DB_USE_SCRIPT:
        // FIXME: The dominant-baseline and the baseline-table components are set by determining the predominant script of the character data content.
        return AB_ALPHABETIC;
    case DB_NO_CHANGE:
        return dominantBaselineToAlignmentBaseline(isVerticalText, textLayoutObject->parent());
    case DB_RESET_SIZE:
        return dominantBaselineToAlignmentBaseline(isVerticalText, textLayoutObject->parent());
    case DB_IDEOGRAPHIC:
        return AB_IDEOGRAPHIC;
    case DB_ALPHABETIC:
        return AB_ALPHABETIC;
    case DB_HANGING:
        return AB_HANGING;
    case DB_MATHEMATICAL:
        return AB_MATHEMATICAL;
    case DB_CENTRAL:
        return AB_CENTRAL;
    case DB_MIDDLE:
        return AB_MIDDLE;
    case DB_TEXT_AFTER_EDGE:
        return AB_TEXT_AFTER_EDGE;
    case DB_TEXT_BEFORE_EDGE:
        return AB_TEXT_BEFORE_EDGE;
    default:
        ASSERT_NOT_REACHED();
        return AB_AUTO;
    }
}

float SVGTextLayoutEngineBaseline::calculateAlignmentBaselineShift(bool isVerticalText, const LayoutObject* textLayoutObject) const
{
    ASSERT(textLayoutObject);
    ASSERT(textLayoutObject->style());
    ASSERT(textLayoutObject->parent());

    const LayoutObject* textLayoutObjectParent = textLayoutObject->parent();
    ASSERT(textLayoutObjectParent);

    EAlignmentBaseline baseline = textLayoutObject->style()->svgStyle().alignmentBaseline();
    if (baseline == AB_AUTO || baseline == AB_BASELINE) {
        baseline = dominantBaselineToAlignmentBaseline(isVerticalText, textLayoutObjectParent);
        ASSERT(baseline != AB_AUTO && baseline != AB_BASELINE);
    }

    const FontMetrics& fontMetrics = m_font.fontMetrics();
    float ascent = fontMetrics.floatAscent() / m_effectiveZoom;
    float descent = fontMetrics.floatDescent() / m_effectiveZoom;
    float xheight = fontMetrics.xHeight() / m_effectiveZoom;

    // Note: http://wiki.apache.org/xmlgraphics-fop/LineLayout/AlignmentHandling
    switch (baseline) {
    case AB_BEFORE_EDGE:
    case AB_TEXT_BEFORE_EDGE:
        return ascent;
    case AB_MIDDLE:
        return xheight / 2;
    case AB_CENTRAL:
        return (ascent - descent) / 2;
    case AB_AFTER_EDGE:
    case AB_TEXT_AFTER_EDGE:
    case AB_IDEOGRAPHIC:
        return -descent;
    case AB_ALPHABETIC:
        return 0;
    case AB_HANGING:
        return ascent * 8 / 10.f;
    case AB_MATHEMATICAL:
        return ascent / 2;
    case AB_BASELINE:
    default:
        ASSERT_NOT_REACHED();
        return 0;
    }
}

float SVGTextLayoutEngineBaseline::calculateGlyphOrientationAngle(bool isVerticalText, const SVGComputedStyle& style, const UChar& character) const
{
    switch (isVerticalText ? style.glyphOrientationVertical() : style.glyphOrientationHorizontal()) {
    case GO_AUTO: {
        // Spec: Fullwidth ideographic and fullwidth Latin text will be set with a glyph-orientation of 0-degrees.
        // Text which is not fullwidth will be set with a glyph-orientation of 90-degrees.
        unsigned unicodeRange = findCharUnicodeRange(character);
        if (unicodeRange == cRangeSetLatin || unicodeRange == cRangeArabic)
            return 90;

        return 0;
    }
    case GO_90DEG:
        return 90;
    case GO_180DEG:
        return 180;
    case GO_270DEG:
        return 270;
    case GO_0DEG:
    default:
        return 0;
    }
}

static inline bool glyphOrientationIsMultiplyOf180Degrees(float orientationAngle)
{
    return !fabsf(fmodf(orientationAngle, 180));
}

float SVGTextLayoutEngineBaseline::calculateGlyphAdvanceAndOrientation(bool isVerticalText, const SVGTextMetrics& metrics, float angle, float& xOrientationShift, float& yOrientationShift) const
{
    bool orientationIsMultiplyOf180Degrees = glyphOrientationIsMultiplyOf180Degrees(angle);

    // The function is based on spec requirements:
    //
    // Spec: If the 'glyph-orientation-horizontal' results in an orientation angle that is not a multiple of
    // of 180 degrees, then the current text position is incremented according to the vertical metrics of the glyph.
    //
    // Spec: If if the 'glyph-orientation-vertical' results in an orientation angle that is not a multiple of
    // 180 degrees, then the current text position is incremented according to the horizontal metrics of the glyph.

    const FontMetrics& fontMetrics = m_font.fontMetrics();

    float ascent = fontMetrics.floatAscent() / m_effectiveZoom;
    float descent = fontMetrics.floatDescent() / m_effectiveZoom;

    // Vertical orientation handling.
    if (isVerticalText) {
        float ascentMinusDescent = ascent - descent;

        if (!angle) {
            xOrientationShift = (ascentMinusDescent - metrics.width()) / 2;
            yOrientationShift = ascent;
        } else if (angle == 180) {
            xOrientationShift = (ascentMinusDescent + metrics.width()) / 2;
        } else if (angle == 270) {
            yOrientationShift = metrics.width();
            xOrientationShift = ascentMinusDescent;
        }

        // Vertical advance calculation.
        if (angle && !orientationIsMultiplyOf180Degrees)
            return metrics.width();

        return metrics.height();
    }

    // Horizontal orientation handling.
    if (angle == 90) {
        yOrientationShift = -metrics.width();
    } else if (angle == 180) {
        xOrientationShift = metrics.width();
        yOrientationShift = -ascent;
    } else if (angle == 270) {
        xOrientationShift = metrics.width();
    }

    // Horizontal advance calculation.
    if (angle && !orientationIsMultiplyOf180Degrees)
        return metrics.height();

    return metrics.width();
}

}
