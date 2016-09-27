/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef LayoutSVGResourceRadialGradient_h
#define LayoutSVGResourceRadialGradient_h

#include "core/layout/svg/LayoutSVGResourceGradient.h"
#include "core/svg/RadialGradientAttributes.h"

namespace blink {

class SVGRadialGradientElement;

class LayoutSVGResourceRadialGradient final : public LayoutSVGResourceGradient {
public:
    explicit LayoutSVGResourceRadialGradient(SVGRadialGradientElement*);
    virtual ~LayoutSVGResourceRadialGradient();

    virtual const char* name() const override { return "LayoutSVGResourceRadialGradient"; }

    static const LayoutSVGResourceType s_resourceType = RadialGradientResourceType;
    virtual LayoutSVGResourceType resourceType() const override { return s_resourceType; }

    virtual SVGUnitTypes::SVGUnitType gradientUnits() const override { return attributes().gradientUnits(); }
    virtual void calculateGradientTransform(AffineTransform& transform) override { transform = attributes().gradientTransform(); }
    virtual bool collectGradientAttributes(SVGGradientElement*) override;
    virtual void buildGradient(GradientData*) const override;

    FloatPoint centerPoint(const RadialGradientAttributes&) const;
    FloatPoint focalPoint(const RadialGradientAttributes&) const;
    float radius(const RadialGradientAttributes&) const;
    float focalRadius(const RadialGradientAttributes&) const;

private:
#if ENABLE(OILPAN)
    Persistent<RadialGradientAttributesWrapper> m_attributesWrapper;

    RadialGradientAttributes& mutableAttributes() { return m_attributesWrapper->attributes(); }
    const RadialGradientAttributes& attributes() const { return m_attributesWrapper->attributes(); }
#else
    RadialGradientAttributes m_attributes;

    RadialGradientAttributes& mutableAttributes() { return m_attributes; }
    const RadialGradientAttributes& attributes() const { return m_attributes; }
#endif
};

DEFINE_LAYOUT_SVG_RESOURCE_TYPE_CASTS(LayoutSVGResourceRadialGradient, RadialGradientResourceType);

}

#endif
