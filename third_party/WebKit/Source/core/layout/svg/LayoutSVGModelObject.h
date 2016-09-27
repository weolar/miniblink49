/*
 * Copyright (c) 2009, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutSVGModelObject_h
#define LayoutSVGModelObject_h

#include "core/layout/LayoutObject.h"
#include "core/svg/SVGElement.h"

namespace blink {

// Most layoutObjects in the SVG layout tree will inherit from this class
// but not all. (e.g. LayoutSVGForeignObject, LayoutSVGBlock) thus methods
// required by SVG layoutObjects need to be declared on LayoutObject, but shared
// logic can go in this class or in SVGLayoutSupport.

class LayoutSVGModelObject : public LayoutObject {
public:
    explicit LayoutSVGModelObject(SVGElement*);

    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const override;

    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const override;

    virtual FloatRect paintInvalidationRectInLocalCoordinates() const override final { return m_paintInvalidationBoundingBox; }

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint& accumulatedOffset) const override final;
    virtual void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed) const override;

    virtual void mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState&, MapCoordinatesFlags = ApplyContainerFlip, bool* wasFixed = nullptr, const PaintInvalidationState* = nullptr) const override final;
    virtual const LayoutObject* pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap&) const override final;
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    virtual void computeLayerHitTestRects(LayerHitTestRects&) const override final;

    SVGElement* element() const { return toSVGElement(LayoutObject::node()); }

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVG || LayoutObject::isOfType(type); }

protected:
    virtual void addLayerHitTestRects(LayerHitTestRects&, const DeprecatedPaintLayer* currentCompositedLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const override final;
    virtual void willBeDestroyed() override;

private:
    // LayoutSVGModelObject subclasses should use element() instead.
    void node() const = delete;

    // This method should never be called, SVG uses a different nodeAtPoint method
    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override final;
    virtual IntRect absoluteFocusRingBoundingBoxRect() const override final;

    virtual void invalidateTreeIfNeeded(PaintInvalidationState&) override final;

protected:
    FloatRect m_paintInvalidationBoundingBox;
};

}

#endif
