/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2006, 2007, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef LayoutBoxModelObject_h
#define LayoutBoxModelObject_h

#include "core/CoreExport.h"
#include "core/layout/LayoutObject.h"
#include "core/style/ShadowData.h"
#include "platform/geometry/LayoutRect.h"

namespace blink {

class DeprecatedPaintLayer;
class DeprecatedPaintLayerScrollableArea;

enum DeprecatedPaintLayerType {
    NoDeprecatedPaintLayer,
    NormalDeprecatedPaintLayer,
    // A forced or overflow clip layer is required for bookkeeping purposes,
    // but does not force a layer to be self painting.
    OverflowClipDeprecatedPaintLayer,
    ForcedDeprecatedPaintLayer
};

// Modes for some of the line-related functions.
enum LinePositionMode { PositionOnContainingLine, PositionOfInteriorLineBoxes };
enum LineDirectionMode { HorizontalLine, VerticalLine };
typedef unsigned BorderEdgeFlags;

enum BackgroundBleedAvoidance {
    BackgroundBleedNone,
    BackgroundBleedShrinkBackground,
    BackgroundBleedClipOnly,
    BackgroundBleedClipLayer,
};

enum ContentChangeType {
    ImageChanged,
    CanvasChanged,
    CanvasContextChanged
};

class InlineFlowBox;

// This class is the base for all objects that adhere to the CSS box model as described
// at http://www.w3.org/TR/CSS21/box.html

class CORE_EXPORT LayoutBoxModelObject : public LayoutObject {
public:
    LayoutBoxModelObject(ContainerNode*);
    virtual ~LayoutBoxModelObject();

    // This is the only way layers should ever be destroyed.
    void destroyLayer();

    LayoutSize relativePositionOffset() const;
    LayoutSize relativePositionLogicalOffset() const { return style()->isHorizontalWritingMode() ? relativePositionOffset() : relativePositionOffset().transposedSize(); }

    LayoutSize offsetForInFlowPosition() const;

    // IE extensions. Used to calculate offsetWidth/Height.  Overridden by inlines (LayoutFlow)
    // to return the remaining width on a given line (and the height of a single line).
    virtual LayoutUnit offsetLeft() const;
    virtual LayoutUnit offsetTop() const;
    virtual LayoutUnit offsetWidth() const = 0;
    virtual LayoutUnit offsetHeight() const = 0;

    int pixelSnappedOffsetLeft() const { return roundToInt(offsetLeft()); }
    int pixelSnappedOffsetTop() const { return roundToInt(offsetTop()); }
    virtual int pixelSnappedOffsetWidth() const;
    virtual int pixelSnappedOffsetHeight() const;

    bool hasSelfPaintingLayer() const;
    DeprecatedPaintLayer* layer() const { return m_layer.get(); }
    DeprecatedPaintLayerScrollableArea* scrollableArea() const;

    virtual void updateFromStyle();

    virtual DeprecatedPaintLayerType layerTypeRequired() const = 0;

    // This will work on inlines to return the bounding box of all of the lines' border boxes.
    virtual IntRect borderBoundingBox() const = 0;

    // Checks if this box, or any of it's descendants, or any of it's continuations,
    // will take up space in the layout of the page.
    bool hasNonEmptyLayoutSize() const;

    // These return the CSS computed padding values.
    LayoutUnit computedCSSPaddingTop() const { return computedCSSPadding(style()->paddingTop()); }
    LayoutUnit computedCSSPaddingBottom() const { return computedCSSPadding(style()->paddingBottom()); }
    LayoutUnit computedCSSPaddingLeft() const { return computedCSSPadding(style()->paddingLeft()); }
    LayoutUnit computedCSSPaddingRight() const { return computedCSSPadding(style()->paddingRight()); }
    LayoutUnit computedCSSPaddingBefore() const { return computedCSSPadding(style()->paddingBefore()); }
    LayoutUnit computedCSSPaddingAfter() const { return computedCSSPadding(style()->paddingAfter()); }
    LayoutUnit computedCSSPaddingStart() const { return computedCSSPadding(style()->paddingStart()); }
    LayoutUnit computedCSSPaddingEnd() const { return computedCSSPadding(style()->paddingEnd()); }

    // These functions are used during layout. Table cells
    // override them to include some extra intrinsic padding.
    virtual LayoutUnit paddingTop() const { return computedCSSPaddingTop(); }
    virtual LayoutUnit paddingBottom() const { return computedCSSPaddingBottom(); }
    virtual LayoutUnit paddingLeft() const { return computedCSSPaddingLeft(); }
    virtual LayoutUnit paddingRight() const { return computedCSSPaddingRight(); }
    virtual LayoutUnit paddingBefore() const { return computedCSSPaddingBefore(); }
    virtual LayoutUnit paddingAfter() const { return computedCSSPaddingAfter(); }
    virtual LayoutUnit paddingStart() const { return computedCSSPaddingStart(); }
    virtual LayoutUnit paddingEnd() const { return computedCSSPaddingEnd(); }

    virtual int borderTop() const { return style()->borderTopWidth(); }
    virtual int borderBottom() const { return style()->borderBottomWidth(); }
    virtual int borderLeft() const { return style()->borderLeftWidth(); }
    virtual int borderRight() const { return style()->borderRightWidth(); }
    virtual int borderBefore() const { return style()->borderBeforeWidth(); }
    virtual int borderAfter() const { return style()->borderAfterWidth(); }
    virtual int borderStart() const { return style()->borderStartWidth(); }
    virtual int borderEnd() const { return style()->borderEndWidth(); }

    int borderWidth() const { return borderLeft() + borderRight(); }
    int borderHeight() const { return borderTop() + borderBottom(); }

    // Insets from the border box to the inside of the border.
    LayoutRectOutsets borderInsets() const { return LayoutRectOutsets(-borderTop(), -borderRight(), -borderBottom(), -borderLeft()); }

    bool hasBorderOrPadding() const { return style()->hasBorder() || style()->hasPadding(); }

    LayoutUnit borderAndPaddingStart() const { return borderStart() + paddingStart(); }
    LayoutUnit borderAndPaddingBefore() const { return borderBefore() + paddingBefore(); }
    LayoutUnit borderAndPaddingAfter() const { return borderAfter() + paddingAfter(); }

    LayoutUnit borderAndPaddingHeight() const { return borderTop() + borderBottom() + paddingTop() + paddingBottom(); }
    LayoutUnit borderAndPaddingWidth() const { return borderLeft() + borderRight() + paddingLeft() + paddingRight(); }
    LayoutUnit borderAndPaddingLogicalHeight() const { return hasBorderOrPadding() ? borderAndPaddingBefore() + borderAndPaddingAfter() : LayoutUnit(); }
    LayoutUnit borderAndPaddingLogicalWidth() const { return borderStart() + borderEnd() + paddingStart() + paddingEnd(); }
    LayoutUnit borderAndPaddingLogicalLeft() const { return style()->isHorizontalWritingMode() ? borderLeft() + paddingLeft() : borderTop() + paddingTop(); }

    LayoutUnit borderLogicalLeft() const { return style()->isHorizontalWritingMode() ? borderLeft() : borderTop(); }

    LayoutUnit paddingLogicalWidth() const { return paddingStart() + paddingEnd(); }
    LayoutUnit paddingLogicalHeight() const { return paddingBefore() + paddingAfter(); }

    virtual LayoutRectOutsets marginBoxOutsets() const = 0;
    virtual LayoutUnit marginTop() const = 0;
    virtual LayoutUnit marginBottom() const = 0;
    virtual LayoutUnit marginLeft() const = 0;
    virtual LayoutUnit marginRight() const = 0;
    virtual LayoutUnit marginBefore(const ComputedStyle* otherStyle = nullptr) const = 0;
    virtual LayoutUnit marginAfter(const ComputedStyle* otherStyle = nullptr) const = 0;
    virtual LayoutUnit marginStart(const ComputedStyle* otherStyle = nullptr) const = 0;
    virtual LayoutUnit marginEnd(const ComputedStyle* otherStyle = nullptr) const = 0;
    LayoutUnit marginHeight() const { return marginTop() + marginBottom(); }
    LayoutUnit marginWidth() const { return marginLeft() + marginRight(); }
    LayoutUnit marginLogicalHeight() const { return marginBefore() + marginAfter(); }
    LayoutUnit marginLogicalWidth() const { return marginStart() + marginEnd(); }

    bool hasInlineDirectionBordersPaddingOrMargin() const { return hasInlineDirectionBordersOrPadding() || marginStart() || marginEnd(); }
    bool hasInlineDirectionBordersOrPadding() const { return borderStart() || borderEnd() || paddingStart() || paddingEnd(); }

    virtual LayoutUnit containingBlockLogicalWidthForContent() const;

    virtual void childBecameNonInline(LayoutObject* /*child*/) { }

    virtual bool boxShadowShouldBeAppliedToBackground(BackgroundBleedAvoidance, InlineFlowBox* = nullptr) const;

    // Overridden by subclasses to determine line height and baseline position.
    virtual LayoutUnit lineHeight(bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const = 0;
    virtual int baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode = PositionOnContainingLine) const = 0;

    virtual void mapAbsoluteToLocalPoint(MapCoordinatesFlags, TransformState&) const override;
    virtual const LayoutObject* pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap&) const override;

    virtual void setSelectionState(SelectionState) override;

    void contentChanged(ContentChangeType);
    bool hasAcceleratedCompositing() const;

    virtual void computeLayerHitTestRects(LayerHitTestRects&) const override;

    // Returns true if the background is painted opaque in the given rect.
    // The query rect is given in local coordinate system.
    virtual bool backgroundIsKnownToBeOpaqueInRect(const LayoutRect&) const { return false; }

    virtual void invalidateTreeIfNeeded(PaintInvalidationState&) override;

    // Indicate that the contents of this layoutObject need to be repainted. Only has an effect if compositing is being used,
    void setBackingNeedsPaintInvalidationInRect(const LayoutRect&, PaintInvalidationReason) const; // r is in the coordinate space of this layout object

    void invalidateDisplayItemClientOnBacking(const DisplayItemClientWrapper&) const;

    // http://www.w3.org/TR/css3-background/#body-background
    // <html> root element with no background steals background from its first <body> child.
    // The used background for such body element should be the initial value. (i.e. transparent)
    bool backgroundStolenForBeingBody(const ComputedStyle* rootElementStyle = nullptr) const;

protected:
    virtual void willBeDestroyed() override;

    LayoutPoint adjustedPositionRelativeToOffsetParent(const LayoutPoint&) const;

    bool calculateHasBoxDecorations() const;

    LayoutBoxModelObject* continuation() const;
    void setContinuation(LayoutBoxModelObject*);

    LayoutRect localCaretRectForEmptyElement(LayoutUnit width, LayoutUnit textIndentOffset);

    bool hasAutoHeightOrContainingBlockWithAutoHeight() const;
    LayoutBlock* containingBlockForAutoHeightDetection(Length logicalHeight) const;

    void addFocusRingRectsForNormalChildren(Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const;
    void addFocusRingRectsForDescendant(const LayoutObject& descendant, Vector<LayoutRect>&, const LayoutPoint& additionalOffset) const;

    virtual void addLayerHitTestRects(LayerHitTestRects&, const DeprecatedPaintLayer*, const LayoutPoint&, const LayoutRect&) const override;

    void styleWillChange(StyleDifference, const ComputedStyle& newStyle) override;
    void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

public:
    // These functions are only used internally to manipulate the layout tree structure via remove/insert/appendChildNode.
    // Since they are typically called only to move objects around within anonymous blocks (which only have layers in
    // the case of column spans), the default for fullRemoveInsert is false rather than true.
    void moveChildTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* child, LayoutObject* beforeChild, bool fullRemoveInsert = false);
    void moveChildTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* child, bool fullRemoveInsert = false)
    {
        moveChildTo(toBoxModelObject, child, 0, fullRemoveInsert);
    }
    void moveAllChildrenTo(LayoutBoxModelObject* toBoxModelObject, bool fullRemoveInsert = false)
    {
        moveAllChildrenTo(toBoxModelObject, 0, fullRemoveInsert);
    }
    void moveAllChildrenTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* beforeChild, bool fullRemoveInsert = false)
    {
        moveChildrenTo(toBoxModelObject, slowFirstChild(), 0, beforeChild, fullRemoveInsert);
    }
    // Move all of the kids from |startChild| up to but excluding |endChild|. 0 can be passed as the |endChild| to denote
    // that all the kids from |startChild| onwards should be moved.
    void moveChildrenTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* startChild, LayoutObject* endChild, bool fullRemoveInsert = false)
    {
        moveChildrenTo(toBoxModelObject, startChild, endChild, 0, fullRemoveInsert);
    }
    virtual void moveChildrenTo(LayoutBoxModelObject* toBoxModelObject, LayoutObject* startChild, LayoutObject* endChild, LayoutObject* beforeChild, bool fullRemoveInsert = false);

    enum ScaleByEffectiveZoomOrNot { ScaleByEffectiveZoom, DoNotScaleByEffectiveZoom };
    IntSize calculateImageIntrinsicDimensions(StyleImage*, const IntSize& scaledPositioningAreaSize, ScaleByEffectiveZoomOrNot) const;

private:
    void createLayer(DeprecatedPaintLayerType);

    LayoutUnit computedCSSPadding(const Length&) const;
    virtual bool isBoxModelObject() const override final { return true; }

    OwnPtr<DeprecatedPaintLayer> m_layer;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutBoxModelObject, isBoxModelObject());

} // namespace blink

#endif // LayoutBoxModelObject_h
