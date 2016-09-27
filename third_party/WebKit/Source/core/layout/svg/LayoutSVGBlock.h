/*
 * Copyright (C) 2006 Apple Computer, Inc.
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

#ifndef LayoutSVGBlock_h
#define LayoutSVGBlock_h

#include "core/layout/LayoutBlockFlow.h"

namespace blink {

class SVGElement;

class LayoutSVGBlock : public LayoutBlockFlow {
public:
    explicit LayoutSVGBlock(SVGElement*);

    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const override final;

    virtual void mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState&, MapCoordinatesFlags = ApplyContainerFlip, bool* wasFixed = nullptr, const PaintInvalidationState* = nullptr) const override final;
    virtual const LayoutObject* pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap&) const override final;

    virtual AffineTransform localTransform() const override final { return m_localTransform; }

    virtual DeprecatedPaintLayerType layerTypeRequired() const override final { return NoDeprecatedPaintLayer; }

    virtual void invalidateTreeIfNeeded(PaintInvalidationState&) override;

protected:
    virtual void willBeDestroyed() override;
    virtual void mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&, const PaintInvalidationState*) const override final;

    AffineTransform m_localTransform;

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVG || LayoutBlockFlow::isOfType(type); }
private:
    virtual void updateFromStyle() override final;

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint& accumulatedOffset) const override final;

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override final;

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override;
};

}
#endif // LayoutSVGBlock_h
