/*
 * Copyright (C) 2012 Google, Inc.
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

#include "core/layout/svg/LayoutSVGEllipse.h"

#include "core/svg/SVGCircleElement.h"
#include "core/svg/SVGEllipseElement.h"

#include <cmath>

namespace blink {

LayoutSVGEllipse::LayoutSVGEllipse(SVGGeometryElement* node)
    : LayoutSVGShape(node)
    , m_usePathFallback(false)
{
}

LayoutSVGEllipse::~LayoutSVGEllipse()
{
}

void LayoutSVGEllipse::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    if (diff.needsFullLayout() && oldStyle) {
        const SVGComputedStyle& oldSvgStyle = oldStyle->svgStyle();
        const SVGComputedStyle& svgStyle = style()->svgStyle();

        bool radiusChanged;
        if (isSVGCircleElement(*element()))
            radiusChanged = oldSvgStyle.r() != svgStyle.r();
        else
            radiusChanged = (oldSvgStyle.rx() != svgStyle.rx()) || (oldSvgStyle.ry() != svgStyle.ry());

        if (oldSvgStyle.cx() != svgStyle.cx()
            || oldSvgStyle.cy() != svgStyle.cy()
            || radiusChanged
            || oldSvgStyle.vectorEffect() != svgStyle.vectorEffect()
            || hasContinuousStroke(oldSvgStyle) != hasContinuousStroke(svgStyle))
            setNeedsShapeUpdate();
    }

    // Superclass will take care of calling clientStyleChanged.
    LayoutSVGShape::styleDidChange(diff, oldStyle);
}

void LayoutSVGEllipse::updateShapeFromElement()
{
    m_usePathFallback = false;
    calculateRadiiAndCenter();

    // Fall back to LayoutSVGShape and path-based hit detection if the ellipse
    // has a non-scaling or discontinuous stroke.
    if (hasNonScalingStroke() || !hasContinuousStroke(style()->svgStyle())) {
        LayoutSVGShape::updateShapeFromElement();
        m_usePathFallback = true;
        return;
    }

    clearPath();
}

void LayoutSVGEllipse::calculateRadiiAndCenter()
{
    ASSERT(element());
    SVGLengthContext lengthContext(element());
    m_center = FloatPoint(
        lengthContext.valueForLength(style()->svgStyle().cx(), styleRef(), SVGLengthMode::Width),
        lengthContext.valueForLength(style()->svgStyle().cy(), styleRef(), SVGLengthMode::Height));

    if (isSVGCircleElement(*element())) {
        float radius = lengthContext.valueForLength(style()->svgStyle().r(), styleRef(), SVGLengthMode::Other);
        m_radii = FloatSize(radius, radius);
    } else {
        m_radii = FloatSize(
            lengthContext.valueForLength(style()->svgStyle().rx(), styleRef(), SVGLengthMode::Width),
            lengthContext.valueForLength(style()->svgStyle().ry(), styleRef(), SVGLengthMode::Height));
    }
}

void LayoutSVGEllipse::updateStrokeAndFillBoundingBoxes()
{
    // Spec: "A negative value is an error."
    if (m_radii.width() < 0 || m_radii.height() < 0) {
        m_fillBoundingBox = FloatRect();
        m_strokeBoundingBox = FloatRect();
        return;
    }

    if (m_usePathFallback) {
        // Spec: "A value of zero disables rendering of the element." so we can skip
        // the path fallback and rely on the existing bounding box calculation.
        if (!m_radii.isEmpty()) {
            LayoutSVGShape::updateStrokeAndFillBoundingBoxes();
            return;
        }
        m_usePathFallback = false;
        clearPath();
    }

    m_fillBoundingBox = FloatRect(m_center.x() - m_radii.width(), m_center.y() - m_radii.height(), 2 * m_radii.width(), 2 * m_radii.height());
    m_strokeBoundingBox = m_fillBoundingBox;
    if (style()->svgStyle().hasStroke())
        m_strokeBoundingBox.inflate(strokeWidth() / 2);
}

bool LayoutSVGEllipse::shapeDependentStrokeContains(const FloatPoint& point)
{
    // The optimized check below for circles does not support non-scaling or
    // discontinuous strokes.
    if (m_usePathFallback
        || !hasContinuousStroke(style()->svgStyle())
        || m_radii.width() != m_radii.height()) {
        if (!m_usePathFallback)
            LayoutSVGShape::updateShapeFromElement();
        return LayoutSVGShape::shapeDependentStrokeContains(point);
    }

    const FloatPoint center = FloatPoint(m_center.x() - point.x(), m_center.y() - point.y());
    const float halfStrokeWidth = strokeWidth() / 2;
    const float r = m_radii.width();
    return std::abs(center.length() - r) <= halfStrokeWidth;
}

bool LayoutSVGEllipse::shapeDependentFillContains(const FloatPoint& point, const WindRule fillRule) const
{
    const FloatPoint center = FloatPoint(m_center.x() - point.x(), m_center.y() - point.y());

    // This works by checking if the point satisfies the ellipse equation.
    // (x/rX)^2 + (y/rY)^2 <= 1
    const float xrX = center.x() / m_radii.width();
    const float yrY = center.y() / m_radii.height();
    return xrX * xrX + yrY * yrY <= 1.0;
}

bool LayoutSVGEllipse::hasContinuousStroke(const SVGComputedStyle& svgStyle) const
{
    return svgStyle.strokeDashArray()->isEmpty();
}

}
