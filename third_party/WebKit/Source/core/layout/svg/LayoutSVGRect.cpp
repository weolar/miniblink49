/*
 * Copyright (C) 2011 University of Szeged
 * Copyright (C) 2011 Renata Hodovan <reni@webkit.org>
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL UNIVERSITY OF SZEGED OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/svg/LayoutSVGRect.h"

#include "core/svg/SVGRectElement.h"
#include "wtf/MathExtras.h"

namespace blink {

LayoutSVGRect::LayoutSVGRect(SVGRectElement* node)
    : LayoutSVGShape(node)
    , m_usePathFallback(false)
{
}

LayoutSVGRect::~LayoutSVGRect()
{
}

void LayoutSVGRect::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    if (diff.needsFullLayout() && oldStyle) {
        const SVGComputedStyle& oldSvgStyle = oldStyle->svgStyle();
        const SVGComputedStyle& svgStyle = style()->svgStyle();
        if (oldSvgStyle.rx() != svgStyle.rx()
            || oldSvgStyle.ry() != svgStyle.ry()
            || oldSvgStyle.vectorEffect() != svgStyle.vectorEffect()
            || definitelyHasSimpleStroke(oldSvgStyle) != definitelyHasSimpleStroke(svgStyle))
            setNeedsShapeUpdate();
    }

    // Superclass will take care of calling clientStyleChanged.
    LayoutSVGShape::styleDidChange(diff, oldStyle);
}

void LayoutSVGRect::updateShapeFromElement()
{
    m_usePathFallback = false;

    // Fallback to LayoutSVGShape and path-based hit detection if the rect
    // has rounded corners or a non-scaling or non-simple stroke.
    SVGLengthContext lengthContext(toSVGRectElement(element()));
    if (lengthContext.valueForLength(styleRef().svgStyle().rx(), styleRef(), SVGLengthMode::Width) > 0
        || lengthContext.valueForLength(styleRef().svgStyle().ry(), styleRef(), SVGLengthMode::Height) > 0
        || hasNonScalingStroke()
        || !definitelyHasSimpleStroke(style()->svgStyle())) {
        LayoutSVGShape::updateShapeFromElement();
        m_usePathFallback = true;
        return;
    }

    clearPath();
}

void LayoutSVGRect::updateStrokeAndFillBoundingBoxes()
{
    SVGLengthContext lengthContext(toSVGRectElement(element()));
    FloatSize boundingBoxSize(
        lengthContext.valueForLength(styleRef().width(), styleRef(), SVGLengthMode::Width),
        lengthContext.valueForLength(styleRef().height(), styleRef(), SVGLengthMode::Height));

    // Spec: "A negative value is an error."
    if (boundingBoxSize.width() < 0 || boundingBoxSize.height() < 0) {
        m_fillBoundingBox = FloatRect();
        m_strokeBoundingBox = FloatRect();
        return;
    }

    if (m_usePathFallback) {
        // Spec: "A value of zero disables rendering of the element." so we can skip
        // the path fallback and rely on the existing bounding box calculation.
        if (!boundingBoxSize.isEmpty()) {
            LayoutSVGShape::updateStrokeAndFillBoundingBoxes();
            return;
        }
        m_usePathFallback = false;
        clearPath();
    }

    m_fillBoundingBox = FloatRect(
        FloatPoint(
            lengthContext.valueForLength(styleRef().svgStyle().x(), styleRef(), SVGLengthMode::Width),
            lengthContext.valueForLength(styleRef().svgStyle().y(), styleRef(), SVGLengthMode::Height)),
        boundingBoxSize);
    m_strokeBoundingBox = m_fillBoundingBox;
    if (style()->svgStyle().hasStroke())
        m_strokeBoundingBox.inflate(strokeWidth() / 2);
}

bool LayoutSVGRect::shapeDependentStrokeContains(const FloatPoint& point)
{
    // The optimized code below does not support non-simple strokes so we need
    // to fall back to LayoutSVGShape::shapeDependentStrokeContains in these cases.
    if (m_usePathFallback || !definitelyHasSimpleStroke(style()->svgStyle())) {
        if (!m_usePathFallback)
            LayoutSVGShape::updateShapeFromElement();
        return LayoutSVGShape::shapeDependentStrokeContains(point);
    }

    const float halfStrokeWidth = strokeWidth() / 2;
    const float halfWidth = m_fillBoundingBox.width() / 2;
    const float halfHeight = m_fillBoundingBox.height() / 2;

    const FloatPoint fillBoundingBoxCenter = FloatPoint(m_fillBoundingBox.x() + halfWidth, m_fillBoundingBox.y() + halfHeight);
    const float absDeltaX = std::abs(point.x() - fillBoundingBoxCenter.x());
    const float absDeltaY = std::abs(point.y() - fillBoundingBoxCenter.y());

    if (!(absDeltaX <= halfWidth + halfStrokeWidth && absDeltaY <= halfHeight + halfStrokeWidth))
        return false;

    return (halfWidth - halfStrokeWidth <= absDeltaX)
        || (halfHeight - halfStrokeWidth <= absDeltaY);
}

bool LayoutSVGRect::shapeDependentFillContains(const FloatPoint& point, const WindRule fillRule) const
{
    if (m_usePathFallback)
        return LayoutSVGShape::shapeDependentFillContains(point, fillRule);
    return m_fillBoundingBox.contains(point.x(), point.y());
}

// Returns true if the stroke is continuous and definitely uses miter joins.
bool LayoutSVGRect::definitelyHasSimpleStroke(const SVGComputedStyle& svgStyle) const
{
    // The four angles of a rect are 90 degrees. Using the formula at:
    // http://www.w3.org/TR/SVG/painting.html#StrokeMiterlimitProperty
    // when the join style of the rect is "miter", the ratio of the miterLength
    // to the stroke-width is found to be
    // miterLength / stroke-width = 1 / sin(45 degrees)
    //                            = 1 / (1 / sqrt(2))
    //                            = sqrt(2)
    //                            = 1.414213562373095...
    // When sqrt(2) exceeds the miterlimit, then the join style switches to
    // "bevel". When the miterlimit is greater than or equal to sqrt(2) then
    // the join style remains "miter".
    //
    // An approximation of sqrt(2) is used here because at certain precise
    // miterlimits, the join style used might not be correct (e.g. a miterlimit
    // of 1.4142135 should result in bevel joins, but may be drawn using miter
    // joins).
    return svgStyle.strokeDashArray()->isEmpty()
        && svgStyle.joinStyle() == MiterJoin
        && svgStyle.strokeMiterLimit() >= 1.5;
}

}
