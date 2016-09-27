/*
 * Copyright (C) Research In Motion Limited 2009-2010. All rights reserved.
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

#ifndef LayoutSVGResourceMarker_h
#define LayoutSVGResourceMarker_h

#include "core/layout/svg/LayoutSVGResourceContainer.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGMarkerElement.h"
#include "platform/geometry/FloatRect.h"
#include "wtf/HashSet.h"

namespace blink {

class LayoutObject;

class LayoutSVGResourceMarker final : public LayoutSVGResourceContainer {
public:
    explicit LayoutSVGResourceMarker(SVGMarkerElement*);
    virtual ~LayoutSVGResourceMarker();

    virtual const char* name() const override { return "LayoutSVGResourceMarker"; }

    virtual void removeAllClientsFromCache(bool markForInvalidation = true) override;
    virtual void removeClientFromCache(LayoutObject*, bool markForInvalidation = true) override;

    // Calculates marker boundaries, mapped to the target element's coordinate space
    FloatRect markerBoundaries(const AffineTransform& markerTransformation) const;

    virtual void layout() override;
    virtual void calcViewport() override;

    virtual const AffineTransform& localToParentTransform() const override;
    AffineTransform markerTransformation(const FloatPoint& origin, float angle, float strokeWidth) const;

    FloatPoint referencePoint() const;
    float angle() const;
    SVGMarkerUnitsType markerUnits() const { return toSVGMarkerElement(element())->markerUnits()->currentValue()->enumValue(); }
    SVGMarkerOrientType orientType() const { return toSVGMarkerElement(element())->orientType()->currentValue()->enumValue(); }

    const FloatRect& viewport() const { return m_viewport; }

    static const LayoutSVGResourceType s_resourceType = MarkerResourceType;
    virtual LayoutSVGResourceType resourceType() const override { return s_resourceType; }

private:
    // Generates a transformation matrix usable to layout marker content. Handles scaling the marker content
    // acording to SVGs markerUnits="strokeWidth" concept, when a strokeWidth value != -1 is passed in.
    AffineTransform markerContentTransformation(const AffineTransform& contentTransformation, const FloatPoint& origin, float strokeWidth = -1) const;

    AffineTransform viewportTransform() const;

    mutable AffineTransform m_localToParentTransform;
    FloatRect m_viewport;
};

DEFINE_LAYOUT_SVG_RESOURCE_TYPE_CASTS(LayoutSVGResourceMarker, MarkerResourceType);

}

#endif
