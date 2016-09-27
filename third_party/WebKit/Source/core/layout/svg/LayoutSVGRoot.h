/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Google, Inc.  All rights reserved.
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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

#ifndef LayoutSVGRoot_h
#define LayoutSVGRoot_h

#include "core/layout/LayoutReplaced.h"

namespace blink {

class SVGElement;

class LayoutSVGRoot final : public LayoutReplaced {
public:
    explicit LayoutSVGRoot(SVGElement*);
    virtual ~LayoutSVGRoot();

    bool isEmbeddedThroughSVGImage() const;
    bool isEmbeddedThroughFrameContainingSVGDocument() const;

    virtual void computeIntrinsicRatioInformation(FloatSize& intrinsicSize, double& intrinsicRatio) const override;
    virtual void mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&, const PaintInvalidationState*) const override;

    // If you have a LayoutSVGRoot, use firstChild or lastChild instead.
    void slowFirstChild() const = delete;
    void slowLastChild() const = delete;

    LayoutObject* firstChild() const { ASSERT(children() == virtualChildren()); return children()->firstChild(); }
    LayoutObject* lastChild() const { ASSERT(children() == virtualChildren()); return children()->lastChild(); }

    bool isLayoutSizeChanged() const { return m_isLayoutSizeChanged; }
    virtual void setNeedsBoundariesUpdate() override { m_needsBoundariesOrTransformUpdate = true; }
    virtual void setNeedsTransformUpdate() override { m_needsBoundariesOrTransformUpdate = true; }

    IntSize containerSize() const { return m_containerSize; }
    void setContainerSize(const IntSize& containerSize)
    {
        // SVGImage::draw() does a view layout prior to painting,
        // and we need that layout to know of the new size otherwise
        // the layout may be incorrectly using the old size.
        if (m_containerSize != containerSize)
            setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::SizeChanged);
        m_containerSize = containerSize;
    }

    // localToBorderBoxTransform maps local SVG viewport coordinates to local CSS box coordinates.
    const AffineTransform& localToBorderBoxTransform() const { return m_localToBorderBoxTransform; }
    bool shouldApplyViewportClip() const;

    virtual bool hasNonIsolatedBlendingDescendants() const override final;

    virtual const char* name() const override { return "LayoutSVGRoot"; }

private:
    const LayoutObjectChildList* children() const { return &m_children; }
    LayoutObjectChildList* children() { return &m_children; }

    virtual LayoutObjectChildList* virtualChildren() override { return children(); }
    virtual const LayoutObjectChildList* virtualChildren() const override { return children(); }

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVG || type == LayoutObjectSVGRoot || LayoutReplaced::isOfType(type); }

    virtual LayoutUnit computeReplacedLogicalWidth(ShouldComputePreferred  = ComputeActual) const override;
    virtual LayoutUnit computeReplacedLogicalHeight() const override;
    virtual void layout() override;
    virtual void paintReplaced(const PaintInfo&, const LayoutPoint&) override;

    virtual void willBeDestroyed() override;
    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const override;
    virtual void addChild(LayoutObject* child, LayoutObject* beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject*) override;

    virtual void insertedIntoTree() override;
    virtual void willBeRemovedFromTree() override;

    virtual const AffineTransform& localToParentTransform() const override;

    virtual FloatRect objectBoundingBox() const override { return m_objectBoundingBox; }
    virtual FloatRect strokeBoundingBox() const override { return m_strokeBoundingBox; }
    virtual FloatRect paintInvalidationRectInLocalCoordinates() const override { return m_paintInvalidationBoundingBox; }

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override;

    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const override;

    virtual void mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState&, MapCoordinatesFlags = ApplyContainerFlip, bool* wasFixed = nullptr, const PaintInvalidationState* = nullptr) const override;
    virtual const LayoutObject* pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap&) const override;

    virtual bool canBeSelectionLeaf() const override { return false; }
    virtual bool canHaveChildren() const override { return true; }

    virtual void descendantIsolationRequirementsChanged(DescendantIsolationState) override final;

    void updateCachedBoundaries();
    void buildLocalToBorderBoxTransform();

    LayoutObjectChildList m_children;
    IntSize m_containerSize;
    FloatRect m_objectBoundingBox;
    bool m_objectBoundingBoxValid;
    FloatRect m_strokeBoundingBox;
    FloatRect m_paintInvalidationBoundingBox;
    mutable AffineTransform m_localToParentTransform;
    AffineTransform m_localToBorderBoxTransform;
    bool m_isLayoutSizeChanged : 1;
    bool m_needsBoundariesOrTransformUpdate : 1;
    bool m_hasBoxDecorationBackground : 1;
    mutable bool m_hasNonIsolatedBlendingDescendants : 1;
    mutable bool m_hasNonIsolatedBlendingDescendantsDirty : 1;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSVGRoot, isSVGRoot());

} // namespace blink

#endif // LayoutSVGRoot_h
