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

#ifndef LayoutSVGResourceMasker_h
#define LayoutSVGResourceMasker_h

#include "core/layout/svg/LayoutSVGResourceContainer.h"
#include "core/svg/SVGMaskElement.h"
#include "core/svg/SVGUnitTypes.h"
#include "platform/geometry/FloatRect.h"

class SkPicture;

namespace blink {

class AffineTransform;
class GraphicsContext;

class LayoutSVGResourceMasker final : public LayoutSVGResourceContainer {
public:
    explicit LayoutSVGResourceMasker(SVGMaskElement*);
    virtual ~LayoutSVGResourceMasker();

    virtual const char* name() const override { return "LayoutSVGResourceMasker"; }

    virtual void removeAllClientsFromCache(bool markForInvalidation = true) override;
    virtual void removeClientFromCache(LayoutObject*, bool markForInvalidation = true) override;

    FloatRect resourceBoundingBox(const LayoutObject*);

    SVGUnitTypes::SVGUnitType maskUnits() const { return toSVGMaskElement(element())->maskUnits()->currentValue()->enumValue(); }
    SVGUnitTypes::SVGUnitType maskContentUnits() const { return toSVGMaskElement(element())->maskContentUnits()->currentValue()->enumValue(); }

    static const LayoutSVGResourceType s_resourceType = MaskerResourceType;
    virtual LayoutSVGResourceType resourceType() const override { return s_resourceType; }

    PassRefPtr<const SkPicture> createContentPicture(AffineTransform&, const FloatRect&, GraphicsContext*);

private:
    void calculateMaskContentPaintInvalidationRect();

    RefPtr<const SkPicture> m_maskContentPicture;
    FloatRect m_maskContentBoundaries;
};

DEFINE_LAYOUT_SVG_RESOURCE_TYPE_CASTS(LayoutSVGResourceMasker, MaskerResourceType);

}

#endif
