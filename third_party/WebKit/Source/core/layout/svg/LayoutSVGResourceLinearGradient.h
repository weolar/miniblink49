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

#ifndef LayoutSVGResourceLinearGradient_h
#define LayoutSVGResourceLinearGradient_h

#include "core/layout/svg/LayoutSVGResourceGradient.h"
#include "core/svg/LinearGradientAttributes.h"

namespace blink {

class SVGLinearGradientElement;

class LayoutSVGResourceLinearGradient final : public LayoutSVGResourceGradient {
public:
    explicit LayoutSVGResourceLinearGradient(SVGLinearGradientElement*);
    virtual ~LayoutSVGResourceLinearGradient();

    virtual const char* name() const override { return "LayoutSVGResourceLinearGradient"; }

    static const LayoutSVGResourceType s_resourceType = LinearGradientResourceType;
    virtual LayoutSVGResourceType resourceType() const override { return s_resourceType; }

    virtual SVGUnitTypes::SVGUnitType gradientUnits() const override { return attributes().gradientUnits(); }
    virtual void calculateGradientTransform(AffineTransform& transform) override { transform = attributes().gradientTransform(); }
    virtual bool collectGradientAttributes(SVGGradientElement*) override;
    virtual void buildGradient(GradientData*) const override;

    FloatPoint startPoint(const LinearGradientAttributes&) const;
    FloatPoint endPoint(const LinearGradientAttributes&) const;

private:
#if ENABLE(OILPAN)
    // FIXME: Oilpan: the LayoutObject hierarchy isn't on the heap, but
    // the SVG property hierarchy is. And the two meet here, so keep a
    // persistent reference to the SVG part object by way of a heap-allocated
    // wrapper object.
    Persistent<LinearGradientAttributesWrapper> m_attributesWrapper;

    LinearGradientAttributes& mutableAttributes() { return m_attributesWrapper->attributes(); }
    const LinearGradientAttributes& attributes() const { return m_attributesWrapper->attributes(); }
#else
    LinearGradientAttributes m_attributes;

    LinearGradientAttributes& mutableAttributes() { return m_attributes; }
    const LinearGradientAttributes& attributes() const { return m_attributes; }
#endif
};

} // namespace blink

#endif // LayoutSVGResourceLinearGradient_h
