/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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
 *
 */

#ifndef LayoutInline_h
#define LayoutInline_h

#include "core/CoreExport.h"
#include "core/editing/PositionWithAffinity.h"
#include "core/layout/LayoutBoxModelObject.h"
#include "core/layout/line/InlineFlowBox.h"
#include "core/layout/line/LineBoxList.h"

namespace blink {

class CORE_EXPORT LayoutInline : public LayoutBoxModelObject {
public:
    explicit LayoutInline(Element*);

    static LayoutInline* createAnonymous(Document*);

    LayoutObject* firstChild() const { ASSERT(children() == virtualChildren()); return children()->firstChild(); }
    LayoutObject* lastChild() const { ASSERT(children() == virtualChildren()); return children()->lastChild(); }

    // If you have a LayoutInline, use firstChild or lastChild instead.
    void slowFirstChild() const = delete;
    void slowLastChild() const = delete;

    virtual void addChild(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) override;

    Element* node() const { return toElement(LayoutBoxModelObject::node()); }

    virtual LayoutRectOutsets marginBoxOutsets() const override final;
    virtual LayoutUnit marginLeft() const override final;
    virtual LayoutUnit marginRight() const override final;
    virtual LayoutUnit marginTop() const override final;
    virtual LayoutUnit marginBottom() const override final;
    virtual LayoutUnit marginBefore(const ComputedStyle* otherStyle = nullptr) const override final;
    virtual LayoutUnit marginAfter(const ComputedStyle* otherStyle = nullptr) const override final;
    virtual LayoutUnit marginStart(const ComputedStyle* otherStyle = nullptr) const override final;
    virtual LayoutUnit marginEnd(const ComputedStyle* otherStyle = nullptr) const override final;

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint& accumulatedOffset) const override final;
    virtual void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed) const override;

    virtual LayoutSize offsetFromContainer(const LayoutObject*, const LayoutPoint&, bool* offsetDependsOnPoint = nullptr) const override final;

    IntRect linesBoundingBox() const;
    LayoutRect linesVisualOverflowBoundingBox() const;

    InlineFlowBox* createAndAppendInlineFlowBox();

    void dirtyLineBoxes(bool fullLayout);

    LineBoxList* lineBoxes() { return &m_lineBoxes; }
    const LineBoxList* lineBoxes() const { return &m_lineBoxes; }

    InlineFlowBox* firstLineBox() const { return m_lineBoxes.firstLineBox(); }
    InlineFlowBox* lastLineBox() const { return m_lineBoxes.lastLineBox(); }
    InlineBox* firstLineBoxIncludingCulling() const { return alwaysCreateLineBoxes() ? firstLineBox() : culledInlineFirstLineBox(); }
    InlineBox* lastLineBoxIncludingCulling() const { return alwaysCreateLineBoxes() ? lastLineBox() : culledInlineLastLineBox(); }

    virtual LayoutBoxModelObject* virtualContinuation() const override final { return continuation(); }
    LayoutInline* inlineElementContinuation() const;

    virtual void updateDragState(bool dragOn) override final;

    LayoutSize offsetForInFlowPositionedInline(const LayoutBox& child) const;

    virtual void addFocusRingRects(Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const override final;

    using LayoutBoxModelObject::continuation;
    using LayoutBoxModelObject::setContinuation;

    bool alwaysCreateLineBoxes() const { return alwaysCreateLineBoxesForLayoutInline(); }
    void setAlwaysCreateLineBoxes(bool alwaysCreateLineBoxes = true) { setAlwaysCreateLineBoxesForLayoutInline(alwaysCreateLineBoxes); }
    void updateAlwaysCreateLineBoxes(bool fullLayout);

    virtual LayoutRect localCaretRect(InlineBox*, int, LayoutUnit* extraWidthToEndOfLine) override final;

    bool hitTestCulledInline(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset);

    virtual const char* name() const override { return "LayoutInline"; }

protected:
    virtual void willBeDestroyed() override;

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    virtual void computeSelfHitTestRects(Vector<LayoutRect>& rects, const LayoutPoint& layerOffset) const override;

    virtual void invalidateDisplayItemClients(const LayoutBoxModelObject& paintInvalidationContainer) const override;

private:
    virtual LayoutObjectChildList* virtualChildren() override final { return children(); }
    virtual const LayoutObjectChildList* virtualChildren() const override final { return children(); }
    const LayoutObjectChildList* children() const { return &m_children; }
    LayoutObjectChildList* children() { return &m_children; }

    virtual bool isLayoutInline() const override final { return true; }

    LayoutRect culledInlineVisualOverflowBoundingBox() const;
    InlineBox* culledInlineFirstLineBox() const;
    InlineBox* culledInlineLastLineBox() const;

    template<typename GeneratorContext>
    void generateLineBoxRects(GeneratorContext& yield) const;
    template<typename GeneratorContext>
    void generateCulledLineBoxRects(GeneratorContext& yield, const LayoutInline* container) const;

    void addChildToContinuation(LayoutObject* newChild, LayoutObject* beforeChild);
    virtual void addChildIgnoringContinuation(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) override final;

    void moveChildrenToIgnoringContinuation(LayoutInline* to, LayoutObject* startChild);

    void splitInlines(LayoutBlock* fromBlock, LayoutBlock* toBlock, LayoutBlock* middleBlock,
        LayoutObject* beforeChild, LayoutBoxModelObject* oldCont);
    void splitFlow(LayoutObject* beforeChild, LayoutBlock* newBlockBox,
        LayoutObject* newChild, LayoutBoxModelObject* oldCont);

    virtual void layout() override final { ASSERT_NOT_REACHED(); } // Do nothing for layout()

    virtual void paint(const PaintInfo&, const LayoutPoint&) override final;

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override final;

    virtual DeprecatedPaintLayerType layerTypeRequired() const override { return isRelPositioned() || createsGroup() || hasClipPath() || style()->shouldCompositeForCurrentAnimations() || style()->hasCompositorProxy() ? NormalDeprecatedPaintLayer : NoDeprecatedPaintLayer; }

    virtual LayoutUnit offsetLeft() const override final;
    virtual LayoutUnit offsetTop() const override final;
    virtual LayoutUnit offsetWidth() const override final { return linesBoundingBox().width(); }
    virtual LayoutUnit offsetHeight() const override final { return linesBoundingBox().height(); }

    virtual LayoutRect absoluteClippedOverflowRect() const override;
    virtual LayoutRect clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = nullptr) const override;
    virtual LayoutRect rectWithOutlineForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, LayoutUnit outlineWidth, const PaintInvalidationState* = nullptr) const override final;
    virtual void mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&, const PaintInvalidationState*) const override final;

    // This method differs from clippedOverflowRectForPaintInvalidation in that it includes
    // the rects for culled inline boxes, which aren't necessary for paint invalidation.
    LayoutRect clippedOverflowRect(const LayoutBoxModelObject*, const PaintInvalidationState* = nullptr) const;

    virtual void mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState&, MapCoordinatesFlags = ApplyContainerFlip, bool* wasFixed = 0, const PaintInvalidationState* = nullptr) const override;

    virtual PositionWithAffinity positionForPoint(const LayoutPoint&) override final;

    virtual IntRect borderBoundingBox() const override final
    {
        IntRect boundingBox = linesBoundingBox();
        return IntRect(0, 0, boundingBox.width(), boundingBox.height());
    }

    virtual InlineFlowBox* createInlineFlowBox(); // Subclassed by SVG and Ruby

    virtual void dirtyLinesFromChangedChild(LayoutObject* child) override final { m_lineBoxes.dirtyLinesFromChangedChild(this, child); }

    virtual LayoutUnit lineHeight(bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override final;
    virtual int baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const override final;

    virtual void childBecameNonInline(LayoutObject* child) override final;

    virtual void updateHitTestResult(HitTestResult&, const LayoutPoint&) override final;

    virtual void imageChanged(WrappedImagePtr, const IntRect* = nullptr) override final;

    virtual void addAnnotatedRegions(Vector<AnnotatedRegionValue>&) override final;

    virtual void updateFromStyle() override final;

    LayoutInline* clone() const;

    LayoutBoxModelObject* continuationBefore(LayoutObject* beforeChild);

    LayoutObjectChildList m_children;
    LineBoxList m_lineBoxes; // All of the line boxes created for this inline flow.  For example, <i>Hello<br>world.</i> will have two <i> line boxes.
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutInline, isLayoutInline());

} // namespace blink

#endif // LayoutInline_h
