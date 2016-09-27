/*
    Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005, 2007 Rob Buis <buis@kde.org>
    Copyright (C) Research In Motion Limited 2010. All rights reserved.

    Based on khtml code by:
    Copyright (C) 1999 Antti Koivisto (koivisto@kde.org)
    Copyright (C) 1999-2003 Lars Knoll (knoll@kde.org)
    Copyright (C) 2002-2003 Dirk Mueller (mueller@kde.org)
    Copyright (C) 2002 Apple Computer, Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "core/style/SVGComputedStyleDefs.h"

#include "core/style/SVGComputedStyle.h"

namespace blink {

StyleFillData::StyleFillData()
    : opacity(SVGComputedStyle::initialFillOpacity())
    , paintType(SVGComputedStyle::initialFillPaintType())
    , paintColor(SVGComputedStyle::initialFillPaintColor())
    , paintUri(SVGComputedStyle::initialFillPaintUri())
    , visitedLinkPaintType(SVGComputedStyle::initialStrokePaintType())
    , visitedLinkPaintColor(SVGComputedStyle::initialFillPaintColor())
    , visitedLinkPaintUri(SVGComputedStyle::initialFillPaintUri())
{
}

StyleFillData::StyleFillData(const StyleFillData& other)
    : RefCounted<StyleFillData>()
    , opacity(other.opacity)
    , paintType(other.paintType)
    , paintColor(other.paintColor)
    , paintUri(other.paintUri)
    , visitedLinkPaintType(other.visitedLinkPaintType)
    , visitedLinkPaintColor(other.visitedLinkPaintColor)
    , visitedLinkPaintUri(other.visitedLinkPaintUri)
{
}

bool StyleFillData::operator==(const StyleFillData& other) const
{
    return opacity == other.opacity
        && paintType == other.paintType
        && paintColor == other.paintColor
        && paintUri == other.paintUri
        && visitedLinkPaintType == other.visitedLinkPaintType
        && visitedLinkPaintColor == other.visitedLinkPaintColor
        && visitedLinkPaintUri == other.visitedLinkPaintUri;
}

StyleStrokeData::StyleStrokeData()
    : opacity(SVGComputedStyle::initialStrokeOpacity())
    , miterLimit(SVGComputedStyle::initialStrokeMiterLimit())
    , width(SVGComputedStyle::initialStrokeWidth())
    , dashOffset(SVGComputedStyle::initialStrokeDashOffset())
    , dashArray(SVGComputedStyle::initialStrokeDashArray())
    , paintType(SVGComputedStyle::initialStrokePaintType())
    , paintColor(SVGComputedStyle::initialStrokePaintColor())
    , paintUri(SVGComputedStyle::initialStrokePaintUri())
    , visitedLinkPaintType(SVGComputedStyle::initialStrokePaintType())
    , visitedLinkPaintColor(SVGComputedStyle::initialStrokePaintColor())
    , visitedLinkPaintUri(SVGComputedStyle::initialStrokePaintUri())
{
}

StyleStrokeData::StyleStrokeData(const StyleStrokeData& other)
    : RefCounted<StyleStrokeData>()
    , opacity(other.opacity)
    , miterLimit(other.miterLimit)
    , width(other.width)
    , dashOffset(other.dashOffset)
    , dashArray(other.dashArray)
    , paintType(other.paintType)
    , paintColor(other.paintColor)
    , paintUri(other.paintUri)
    , visitedLinkPaintType(other.visitedLinkPaintType)
    , visitedLinkPaintColor(other.visitedLinkPaintColor)
    , visitedLinkPaintUri(other.visitedLinkPaintUri)
{
}

bool StyleStrokeData::operator==(const StyleStrokeData& other) const
{
    return width == other.width
        && opacity == other.opacity
        && miterLimit == other.miterLimit
        && dashOffset == other.dashOffset
        && *dashArray == *other.dashArray
        && paintType == other.paintType
        && paintColor == other.paintColor
        && paintUri == other.paintUri
        && visitedLinkPaintType == other.visitedLinkPaintType
        && visitedLinkPaintColor == other.visitedLinkPaintColor
        && visitedLinkPaintUri == other.visitedLinkPaintUri;
}

StyleStopData::StyleStopData()
    : opacity(SVGComputedStyle::initialStopOpacity())
    , color(SVGComputedStyle::initialStopColor())
{
}

StyleStopData::StyleStopData(const StyleStopData& other)
    : RefCounted<StyleStopData>()
    , opacity(other.opacity)
    , color(other.color)
{
}

bool StyleStopData::operator==(const StyleStopData& other) const
{
    return color == other.color
        && opacity == other.opacity;
}

StyleMiscData::StyleMiscData()
    : floodColor(SVGComputedStyle::initialFloodColor())
    , floodOpacity(SVGComputedStyle::initialFloodOpacity())
    , lightingColor(SVGComputedStyle::initialLightingColor())
    , baselineShiftValue(SVGComputedStyle::initialBaselineShiftValue())
{
}

StyleMiscData::StyleMiscData(const StyleMiscData& other)
    : RefCounted<StyleMiscData>()
    , floodColor(other.floodColor)
    , floodOpacity(other.floodOpacity)
    , lightingColor(other.lightingColor)
    , baselineShiftValue(other.baselineShiftValue)
{
}

bool StyleMiscData::operator==(const StyleMiscData& other) const
{
    return floodOpacity == other.floodOpacity
        && floodColor == other.floodColor
        && lightingColor == other.lightingColor
        && baselineShiftValue == other.baselineShiftValue;
}

StyleResourceData::StyleResourceData()
    : clipper(SVGComputedStyle::initialClipperResource())
    , filter(SVGComputedStyle::initialFilterResource())
    , masker(SVGComputedStyle::initialMaskerResource())
{
}

StyleResourceData::StyleResourceData(const StyleResourceData& other)
    : RefCounted<StyleResourceData>()
    , clipper(other.clipper)
    , filter(other.filter)
    , masker(other.masker)
{
}

bool StyleResourceData::operator==(const StyleResourceData& other) const
{
    return clipper == other.clipper
        && filter == other.filter
        && masker == other.masker;
}

StyleInheritedResourceData::StyleInheritedResourceData()
    : markerStart(SVGComputedStyle::initialMarkerStartResource())
    , markerMid(SVGComputedStyle::initialMarkerMidResource())
    , markerEnd(SVGComputedStyle::initialMarkerEndResource())
{
}

StyleInheritedResourceData::StyleInheritedResourceData(const StyleInheritedResourceData& other)
    : RefCounted<StyleInheritedResourceData>()
    , markerStart(other.markerStart)
    , markerMid(other.markerMid)
    , markerEnd(other.markerEnd)
{
}

bool StyleInheritedResourceData::operator==(const StyleInheritedResourceData& other) const
{
    return markerStart == other.markerStart
        && markerMid == other.markerMid
        && markerEnd == other.markerEnd;
}

StyleLayoutData::StyleLayoutData()
    : cx(SVGComputedStyle::initialCx())
    , cy(SVGComputedStyle::initialCy())
    , x(SVGComputedStyle::initialX())
    , y(SVGComputedStyle::initialY())
    , r(SVGComputedStyle::initialR())
    , rx(SVGComputedStyle::initialRx())
    , ry(SVGComputedStyle::initialRy())
{
}

inline StyleLayoutData::StyleLayoutData(const StyleLayoutData& other)
    : RefCounted<StyleLayoutData>()
    , cx(other.cx)
    , cy(other.cy)
    , x(other.x)
    , y(other.y)
    , r(other.r)
    , rx(other.rx)
    , ry(other.ry)
{
}

PassRefPtr<StyleLayoutData> StyleLayoutData::copy() const
{
    return adoptRef(new StyleLayoutData(*this));
}

bool StyleLayoutData::operator==(const StyleLayoutData& other) const
{
    return x == other.x
        && y == other.y
        && r == other.r
        && rx == other.rx
        && ry == other.ry
        && cx == other.cx
        && cy == other.cy;
}

}
