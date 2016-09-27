/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2008 Dirk Schulze <krit@webkit.org>
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
#include "core/layout/svg/LayoutSVGResourcePaintServer.h"

#include "core/layout/svg/SVGResources.h"
#include "core/layout/svg/SVGResourcesCache.h"
#include "core/style/ComputedStyle.h"
#include "platform/graphics/skia/SkiaUtils.h"
#include "third_party/skia/include/core/SkPaint.h"

namespace blink {

SVGPaintServer::SVGPaintServer(Color color)
    : m_color(color)
{
}

SVGPaintServer::SVGPaintServer(PassRefPtr<Gradient> gradient)
    : m_gradient(gradient)
    , m_color(Color::black)
{
}

SVGPaintServer::SVGPaintServer(PassRefPtr<Pattern> pattern)
    : m_pattern(pattern)
    , m_color(Color::black)
{
}

void SVGPaintServer::applyToSkPaint(SkPaint& paint, float paintAlpha)
{
    SkColor baseColor = m_gradient || m_pattern ? SK_ColorBLACK : m_color.rgb();
    paint.setColor(scaleAlpha(baseColor, paintAlpha));
    if (m_pattern)
        paint.setShader(m_pattern->shader());
    else if (m_gradient)
        paint.setShader(m_gradient->shader());
    else
        paint.setShader(nullptr);
}

void SVGPaintServer::prependTransform(const AffineTransform& transform)
{
    ASSERT(m_gradient || m_pattern);
    if (m_pattern)
        m_pattern->setPatternSpaceTransform(transform * m_pattern->patternSpaceTransform());
    else
        m_gradient->setGradientSpaceTransform(transform * m_gradient->gradientSpaceTransform());
}

static SVGPaintDescription requestPaint(const LayoutObject& object, const ComputedStyle& style, LayoutSVGResourceMode mode)
{
    // If we have no style at all, ignore it.
    const SVGComputedStyle& svgStyle = style.svgStyle();

    // If we have no fill/stroke, return 0.
    if (mode == ApplyToFillMode) {
        if (!svgStyle.hasFill())
            return SVGPaintDescription();
    } else {
        if (!svgStyle.hasStroke())
            return SVGPaintDescription();
    }

    bool applyToFill = mode == ApplyToFillMode;
    SVGPaintType paintType = applyToFill ? svgStyle.fillPaintType() : svgStyle.strokePaintType();
    ASSERT(paintType != SVG_PAINTTYPE_NONE);

    Color color;
    bool hasColor = false;
    switch (paintType) {
    case SVG_PAINTTYPE_CURRENTCOLOR:
    case SVG_PAINTTYPE_RGBCOLOR:
    case SVG_PAINTTYPE_URI_CURRENTCOLOR:
    case SVG_PAINTTYPE_URI_RGBCOLOR:
        color = applyToFill ? svgStyle.fillPaintColor() : svgStyle.strokePaintColor();
        hasColor = true;
    default:
        break;
    }

    if (style.insideLink() == InsideVisitedLink) {
        // FIXME: This code doesn't support the uri component of the visited link paint, https://bugs.webkit.org/show_bug.cgi?id=70006
        SVGPaintType visitedPaintType = applyToFill ? svgStyle.visitedLinkFillPaintType() : svgStyle.visitedLinkStrokePaintType();

        // For SVG_PAINTTYPE_CURRENTCOLOR, 'color' already contains the 'visitedColor'.
        if (visitedPaintType < SVG_PAINTTYPE_URI_NONE && visitedPaintType != SVG_PAINTTYPE_CURRENTCOLOR) {
            const Color& visitedColor = applyToFill ? svgStyle.visitedLinkFillPaintColor() : svgStyle.visitedLinkStrokePaintColor();
            color = Color(visitedColor.red(), visitedColor.green(), visitedColor.blue(), color.alpha());
            hasColor = true;
        }
    }

    // If the primary resource is just a color, return immediately.
    if (paintType < SVG_PAINTTYPE_URI_NONE) {
        // |paintType| will be either <current-color> or <rgb-color> here - both of which will have a color.
        ASSERT(hasColor);
        return SVGPaintDescription(color);
    }

    LayoutSVGResourcePaintServer* uriResource = nullptr;
    if (SVGResources* resources = SVGResourcesCache::cachedResourcesForLayoutObject(&object))
        uriResource = applyToFill ? resources->fill() : resources->stroke();

    // If the requested resource is not available, return the color resource or 'none'.
    if (!uriResource) {
        // The fallback is 'none'. (SVG2 say 'none' is implied when no fallback is specified.)
        if (paintType == SVG_PAINTTYPE_URI_NONE || !hasColor)
            return SVGPaintDescription();

        return SVGPaintDescription(color);
    }

    // The paint server resource exists, though it may be invalid (pattern with width/height=0).
    // Return the fallback color to our caller so it can use it, if
    // preparePaintServer() on the resource container failed.
    if (hasColor)
        return SVGPaintDescription(uriResource, color);

    return SVGPaintDescription(uriResource);
}

SVGPaintServer SVGPaintServer::requestForLayoutObject(const LayoutObject& layoutObject, const ComputedStyle& style, LayoutSVGResourceMode resourceMode)
{
    ASSERT(resourceMode == ApplyToFillMode || resourceMode == ApplyToStrokeMode);

    SVGPaintDescription paintDescription = requestPaint(layoutObject, style, resourceMode);
    if (!paintDescription.isValid)
        return invalid();
    if (!paintDescription.resource)
        return SVGPaintServer(paintDescription.color);
    SVGPaintServer paintServer = paintDescription.resource->preparePaintServer(layoutObject);
    if (paintServer.isValid())
        return paintServer;
    if (paintDescription.hasFallback)
        return SVGPaintServer(paintDescription.color);
    return invalid();
}

bool SVGPaintServer::existsForLayoutObject(const LayoutObject& layoutObject, const ComputedStyle& style, LayoutSVGResourceMode resourceMode)
{
    return requestPaint(layoutObject, style, resourceMode).isValid;
}

LayoutSVGResourcePaintServer::LayoutSVGResourcePaintServer(SVGElement* element)
    : LayoutSVGResourceContainer(element)
{
}

LayoutSVGResourcePaintServer::~LayoutSVGResourcePaintServer()
{
}

SVGPaintDescription LayoutSVGResourcePaintServer::requestPaintDescription(const LayoutObject& layoutObject, const ComputedStyle& style, LayoutSVGResourceMode resourceMode)
{
    return requestPaint(layoutObject, style, resourceMode);
}

}
