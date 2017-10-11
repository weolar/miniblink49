/*
 * Copyright (C) 2003, 2009, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
 *
 * Portions are Copyright (C) 1998 Netscape Communications Corporation.
 *
 * Other contributors:
 *   Robert O'Callahan <roc+@cs.cmu.edu>
 *   David Baron <dbaron@fas.harvard.edu>
 *   Christian Biesinger <cbiesinger@web.de>
 *   Randall Jesup <rjesup@wgate.com>
 *   Roland Mainz <roland.mainz@informatik.med.uni-giessen.de>
 *   Josh Soref <timeless@mac.com>
 *   Boris Zbarsky <bzbarsky@mit.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Alternatively, the contents of this file may be used under the terms
 * of either the Mozilla Public License Version 1.1, found at
 * http://www.mozilla.org/MPL/ (the "MPL") or the GNU General Public
 * License Version 2.0, found at http://www.fsf.org/copyleft/gpl.html
 * (the "GPL"), in which case the provisions of the MPL or the GPL are
 * applicable instead of those above.  If you wish to allow use of your
 * version of this file only under the terms of one of those two
 * licenses (the MPL or the GPL) and not to allow others to use your
 * version of this file under the LGPL, indicate your decision by
 * deletingthe provisions above and replace them with the notice and
 * other provisions required by the MPL or the GPL, as the case may be.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under any of the LGPL, the MPL or the GPL.
 */

#ifndef DeprecatedPaintLayer_h
#define DeprecatedPaintLayer_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBox.h"
#include "core/paint/DeprecatedPaintLayerClipper.h"
#include "core/paint/DeprecatedPaintLayerFilterInfo.h"
#include "core/paint/DeprecatedPaintLayerFragment.h"
#include "core/paint/DeprecatedPaintLayerReflectionInfo.h"
#include "core/paint/DeprecatedPaintLayerScrollableArea.h"
#include "core/paint/DeprecatedPaintLayerStackingNode.h"
#include "core/paint/DeprecatedPaintLayerStackingNodeIterator.h"
#include "platform/graphics/CompositingReasons.h"
#include "public/platform/WebBlendMode.h"
#include "wtf/OwnPtr.h"

namespace blink {

class FilterEffectBuilder;
class FilterOperations;
class HitTestRequest;
class HitTestResult;
class HitTestingTransformState;
class DeprecatedPaintLayerCompositor;
class CompositedDeprecatedPaintLayerMapping;
class ComputedStyle;
class TransformationMatrix;

enum IncludeSelfOrNot { IncludeSelf, ExcludeSelf };

enum CompositingQueryMode {
    CompositingQueriesAreAllowed,
    CompositingQueriesAreOnlyAllowedInCertainDocumentLifecyclePhases
};

// FIXME: remove this once the compositing query ASSERTS are no longer hit.
class CORE_EXPORT DisableCompositingQueryAsserts {
    WTF_MAKE_NONCOPYABLE(DisableCompositingQueryAsserts);
public:
    DisableCompositingQueryAsserts();
private:
    TemporaryChange<CompositingQueryMode> m_disabler;
};

// DeprecatedPaintLayer is an old object that handles lots of unrelated operations.
// We want it to die at some point and be replaced by more focused objects. Removing
// a lot of unneeded complexity.
// Complex painting operations (opacity, clipping, filters, reflections, ...),
// hardware acceleration (through DeprecatedPaintLayerCompositor),
// scrolling (through DeprecatedPaintLayerScrollableArea)
// along with some optimizations are all handled by DeprecatedPaintLayer.
class CORE_EXPORT DeprecatedPaintLayer {
    WTF_MAKE_NONCOPYABLE(DeprecatedPaintLayer);
public:
    DeprecatedPaintLayer(LayoutBoxModelObject*, DeprecatedPaintLayerType);
    ~DeprecatedPaintLayer();

    String debugName() const;

    LayoutBoxModelObject* layoutObject() const { return m_layoutObject; }
    LayoutBox* layoutBox() const { return m_layoutObject && m_layoutObject->isBox() ? toLayoutBox(m_layoutObject) : 0; }
    DeprecatedPaintLayer* parent() const { return m_parent; }
    DeprecatedPaintLayer* previousSibling() const { return m_previous; }
    DeprecatedPaintLayer* nextSibling() const { return m_next; }
    DeprecatedPaintLayer* firstChild() const { return m_first; }
    DeprecatedPaintLayer* lastChild() const { return m_last; }

    const DeprecatedPaintLayer* compositingContainer() const;

    void addChild(DeprecatedPaintLayer* newChild, DeprecatedPaintLayer* beforeChild = 0);
    DeprecatedPaintLayer* removeChild(DeprecatedPaintLayer*);

    void removeOnlyThisLayer();
    void insertOnlyThisLayer();

    void styleChanged(StyleDifference, const ComputedStyle* oldStyle);

    // FIXME: Many people call this function while it has out-of-date information.
    bool isSelfPaintingLayer() const { return m_isSelfPaintingLayer; }

    void setLayerType(DeprecatedPaintLayerType layerType) { m_layerType = layerType; }

    bool isTransparent() const { return layoutObject()->isTransparent() || layoutObject()->style()->hasBlendMode() || layoutObject()->hasMask(); }

    bool isReflection() const { return layoutObject()->isReplica(); }
    DeprecatedPaintLayerReflectionInfo* reflectionInfo() { return m_reflectionInfo.get(); }
    const DeprecatedPaintLayerReflectionInfo* reflectionInfo() const { return m_reflectionInfo.get(); }

    const DeprecatedPaintLayer* root() const
    {
        const DeprecatedPaintLayer* curr = this;
        while (curr->parent())
            curr = curr->parent();
        return curr;
    }

    const LayoutPoint& location() const { ASSERT(!m_needsPositionUpdate); return m_location; }
    // FIXME: size() should ASSERT(!m_needsPositionUpdate) as well, but that fails in some tests,
    // for example, fast/repaint/clipped-relative.html.
    const IntSize& size() const { return m_size; }
    void setSizeHackForLayoutTreeAsText(const IntSize& size) { m_size = size; }

    LayoutRect rect() const { return LayoutRect(location(), LayoutSize(size())); }

    bool isRootLayer() const { return m_isRootLayer; }

    DeprecatedPaintLayerCompositor* compositor() const;

    // Notification from the layoutObject that its content changed (e.g. current frame of image changed).
    // Allows updates of layer content without invalidating paint.
    void contentChanged(ContentChangeType);

    void updateLayerPositionsAfterLayout();
    void updateLayerPositionsAfterOverflowScroll(const DoubleSize& scrollDelta);

    DeprecatedPaintLayer* enclosingPaginationLayer() const { return m_enclosingPaginationLayer; }

    void updateTransformationMatrix();
    DeprecatedPaintLayer* renderingContextRoot();

    const LayoutSize& offsetForInFlowPosition() const { return m_offsetForInFlowPosition; }

    void blockSelectionGapsBoundsChanged();
    void addBlockSelectionGapsBounds(const LayoutRect&);
    void clearBlockSelectionGapsBounds();
    void invalidatePaintForBlockSelectionGaps();
    IntRect blockSelectionGapsBounds() const;
    bool hasBlockSelectionGapBounds() const;

    DeprecatedPaintLayerStackingNode* stackingNode() { return m_stackingNode.get(); }
    const DeprecatedPaintLayerStackingNode* stackingNode() const { return m_stackingNode.get(); }

    bool subtreeIsInvisible() const { return !hasVisibleContent() && !hasVisibleDescendant(); }

    // FIXME: hasVisibleContent() should call updateDescendantDependentFlags() if m_visibleContentStatusDirty.
    bool hasVisibleContent() const { ASSERT(!m_visibleContentStatusDirty); return m_hasVisibleContent; }

    // FIXME: hasVisibleDescendant() should call updateDescendantDependentFlags() if m_visibleDescendantStatusDirty.
    bool hasVisibleDescendant() const { ASSERT(!m_visibleDescendantStatusDirty); return m_hasVisibleDescendant; }

    void dirtyVisibleContentStatus();
    void potentiallyDirtyVisibleContentStatus(EVisibility);

    bool hasBoxDecorationsOrBackground() const;
    bool hasVisibleBoxDecorations() const;
    // True if this layer container layoutObjects that paint.
    bool hasNonEmptyChildLayoutObjects() const;

    // Will ensure that hasNonCompositiedChild are up to date.
    void updateScrollingStateAfterCompositingChange();
    bool hasVisibleNonLayerContent() const { return m_hasVisibleNonLayerContent; }
    bool hasNonCompositedChild() const { ASSERT(isAllowedToQueryCompositingState()); return m_hasNonCompositedChild; }

    // Gets the ancestor layer that serves as the containing block of this layer. It is assumed
    // that this layer is established by an out-of-flow positioned layout object (i.e. either
    // absolutely or fixed positioned).
    // If |ancestor| is specified, |*skippedAncestor| will be set to true if |ancestor| is found in
    // the ancestry chain between this layer and the containing block layer; if not found, it will
    // be set to false. Either both |ancestor| and |skippedAncestor| should be nullptr, or none of
    // them should.
    DeprecatedPaintLayer* enclosingPositionedAncestor(const DeprecatedPaintLayer* ancestor = nullptr, bool* skippedAncestor = nullptr) const;

    bool isPaintInvalidationContainer() const;

    // Do *not* call this method unless you know what you are dooing. You probably want to call enclosingCompositingLayerForPaintInvalidation() instead.
    // If includeSelf is true, may return this.
    DeprecatedPaintLayer* enclosingLayerWithCompositedDeprecatedPaintLayerMapping(IncludeSelfOrNot) const;

    // Returns the enclosing layer root into which this layer paints, inclusive of this one. Note that the enclosing layer may or may not have its own
    // GraphicsLayer backing, but is nevertheless the root for a call to the Layer::paint*() methods.
    DeprecatedPaintLayer* enclosingLayerForPaintInvalidation() const;

    DeprecatedPaintLayer* enclosingLayerForPaintInvalidationCrossingFrameBoundaries() const;

    bool hasAncestorWithFilterOutsets() const;

    bool canUseConvertToLayerCoords() const
    {
        // These LayoutObjects have an impact on their layers without the layoutObjects knowing about it.
        return !layoutObject()->hasTransformRelatedProperty() && !layoutObject()->isSVGRoot();
    }

    void convertToLayerCoords(const DeprecatedPaintLayer* ancestorLayer, LayoutPoint&) const;
    void convertToLayerCoords(const DeprecatedPaintLayer* ancestorLayer, LayoutRect&) const;

    // Does the same as convertToLayerCoords() when not in multicol. For multicol, however,
    // convertToLayerCoords() calculates the offset in flow-thread coordinates (what the layout
    // engine uses internally), while this method calculates the visual coordinates; i.e. it figures
    // out which column the layer starts in and adds in the offset. See
    // http://www.chromium.org/developers/design-documents/multi-column-layout for more info.
    LayoutPoint visualOffsetFromAncestor(const DeprecatedPaintLayer* ancestorLayer) const;

    // The hitTest() method looks for mouse events by walking layers that intersect the point from front to back.
    bool hitTest(HitTestResult&);
    bool hitTest(const HitTestRequest&, const HitTestLocation&, HitTestResult&);

    // Pass offsetFromRoot if known.
    bool intersectsDamageRect(const LayoutRect& layerBounds, const LayoutRect& damageRect, const DeprecatedPaintLayer* rootLayer, const LayoutPoint* offsetFromRoot = 0) const;

    // Bounding box relative to some ancestor layer. Pass offsetFromRoot if known.
    LayoutRect physicalBoundingBox(const DeprecatedPaintLayer* ancestorLayer, const LayoutPoint* offsetFromRoot = 0) const;
    LayoutRect physicalBoundingBoxIncludingReflectionAndStackingChildren(const DeprecatedPaintLayer* ancestorLayer, const LayoutPoint& offsetFromRoot) const;
    LayoutRect fragmentsBoundingBox(const DeprecatedPaintLayer* ancestorLayer) const;

    LayoutRect boundingBoxForCompositingOverlapTest() const;

    // If true, this layer's children are included in its bounds for overlap testing.
    // We can't rely on the children's positions if this layer has a filter that could have moved the children's pixels around.
    bool overlapBoundsIncludeChildren() const { return hasFilter() && layoutObject()->style()->filter().hasFilterThatMovesPixels(); }

    enum CalculateBoundsOptions {
        ApplyBoundsChickenEggHacks,
        DoNotApplyBoundsChickenEggHacks,
    };
    LayoutRect boundingBoxForCompositing(const DeprecatedPaintLayer* ancestorLayer = 0, CalculateBoundsOptions = DoNotApplyBoundsChickenEggHacks) const;

    LayoutUnit staticInlinePosition() const { return m_staticInlinePosition; }
    LayoutUnit staticBlockPosition() const { return m_staticBlockPosition; }

    void setStaticInlinePosition(LayoutUnit position) { m_staticInlinePosition = position; }
    void setStaticBlockPosition(LayoutUnit position) { m_staticBlockPosition = position; }

    LayoutSize subpixelAccumulation() const;
    void setSubpixelAccumulation(const LayoutSize&);

    bool hasTransformRelatedProperty() const { return layoutObject()->hasTransformRelatedProperty(); }
    // Note that this transform has the transform-origin baked in.
    TransformationMatrix* transform() const { return m_transform.get(); }
    void setTransform(PassOwnPtr<TransformationMatrix> transform) { m_transform = transform; }
    void clearTransform() { m_transform.clear(); }

    // currentTransform computes a transform which takes accelerated animations into account. The
    // resulting transform has transform-origin baked in. If the layer does not have a transform,
    // returns the identity matrix.
    TransformationMatrix currentTransform() const;
    TransformationMatrix renderableTransform(PaintBehavior) const;

    // Get the perspective transform, which is applied to transformed sublayers.
    // Returns true if the layer has a -webkit-perspective.
    // Note that this transform does not have the perspective-origin baked in.
    TransformationMatrix perspectiveTransform() const;
    FloatPoint perspectiveOrigin() const;
    bool preserves3D() const { return layoutObject()->style()->transformStyle3D() == TransformStyle3DPreserve3D; }
    bool has3DTransform() const { return m_transform && !m_transform->isAffine(); }

    // FIXME: reflections should force transform-style to be flat in the style: https://bugs.webkit.org/show_bug.cgi?id=106959
    bool shouldPreserve3D() const { return !layoutObject()->hasReflection() && layoutObject()->style()->transformStyle3D() == TransformStyle3DPreserve3D; }

    void filterNeedsPaintInvalidation();
    bool hasFilter() const { return layoutObject()->hasFilter(); }

    void* operator new(size_t);
    // Only safe to call from LayoutBoxModelObject::destroyLayer()
    void operator delete(void*);

    CompositingState compositingState() const;

    // This returns true if our document is in a phase of its lifestyle during which
    // compositing state may legally be read.
    bool isAllowedToQueryCompositingState() const;

    // Don't null check this.
    // FIXME: Rename.
    CompositedDeprecatedPaintLayerMapping* compositedDeprecatedPaintLayerMapping() const;
    GraphicsLayer* graphicsLayerBacking() const;
    GraphicsLayer* graphicsLayerBackingForScrolling() const;
    // NOTE: If you are using hasCompositedDeprecatedPaintLayerMapping to determine the state of compositing for this layer,
    // (and not just to do bookkeeping related to the mapping like, say, allocating or deallocating a mapping),
    // then you may have incorrect logic. Use compositingState() instead.
    // FIXME: This is identical to null checking compositedDeprecatedPaintLayerMapping(), why not just call that?
    bool hasCompositedDeprecatedPaintLayerMapping() const { return m_compositedDeprecatedPaintLayerMapping.get(); }
    void ensureCompositedDeprecatedPaintLayerMapping();
    void clearCompositedDeprecatedPaintLayerMapping(bool layerBeingDestroyed = false);
    CompositedDeprecatedPaintLayerMapping* groupedMapping() const { return m_groupedMapping; }
    enum SetGroupMappingOptions {
        InvalidateLayerAndRemoveFromMapping,
        DoNotInvalidateLayerAndRemoveFromMapping
    };
    void setGroupedMapping(CompositedDeprecatedPaintLayerMapping*, SetGroupMappingOptions);

    bool hasCompositedMask() const;
    bool hasCompositedClippingMask() const;
    bool needsCompositedScrolling() const { return m_scrollableArea && m_scrollableArea->needsCompositedScrolling(); }

    // Computes the position of the given layout object in the space of |paintInvalidationContainer|.
    // FIXME: invert the logic to have paint invalidation containers take care of painting objects into them, rather than the reverse.
    // This will allow us to clean up this static method messiness.
    static LayoutPoint positionFromPaintInvalidationBacking(const LayoutObject*, const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* = 0);

    static void mapPointToPaintBackingCoordinates(const LayoutBoxModelObject* paintInvalidationContainer, FloatPoint&);
    static void mapRectToPaintBackingCoordinates(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&);

    // Adjusts the given rect (in the coordinate space of the LayoutObject) to the coordinate space of |paintInvalidationContainer|'s GraphicsLayer backing.
    static void mapRectToPaintInvalidationBacking(const LayoutObject*, const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect&, const PaintInvalidationState* = 0);

    // Computes the bounding paint invalidation rect for |layoutObject|, in the coordinate space of |paintInvalidationContainer|'s GraphicsLayer backing.
    static LayoutRect computePaintInvalidationRect(const LayoutObject*, const DeprecatedPaintLayer* paintInvalidationContainer, const PaintInvalidationState* = 0);

    bool paintsWithTransparency(PaintBehavior paintBehavior) const
    {
        return isTransparent() && ((paintBehavior & PaintBehaviorFlattenCompositingLayers) || compositingState() != PaintsIntoOwnBacking);
    }

    bool paintsWithTransform(PaintBehavior) const;

    // Returns true if background phase is painted opaque in the given rect.
    // The query rect is given in local coordinates.
    bool backgroundIsKnownToBeOpaqueInRect(const LayoutRect&) const;

    bool containsDirtyOverlayScrollbars() const { return m_containsDirtyOverlayScrollbars; }
    void setContainsDirtyOverlayScrollbars(bool dirtyScrollbars) { m_containsDirtyOverlayScrollbars = dirtyScrollbars; }

    FilterOperations computeFilterOperations(const ComputedStyle&);
    bool paintsWithFilters() const;
    FilterEffectBuilder* filterEffectBuilder() const
    {
        DeprecatedPaintLayerFilterInfo* filterInfo = this->filterInfo();
        return filterInfo ? filterInfo->builder() : 0;
    }

    DeprecatedPaintLayerFilterInfo* filterInfo() const { return hasFilterInfo() ? DeprecatedPaintLayerFilterInfo::filterInfoForLayer(this) : 0; }
    DeprecatedPaintLayerFilterInfo* ensureFilterInfo() { return DeprecatedPaintLayerFilterInfo::createFilterInfoForLayerIfNeeded(this); }
    void removeFilterInfoIfNeeded()
    {
        if (hasFilterInfo())
            DeprecatedPaintLayerFilterInfo::removeFilterInfoForLayer(this);
    }

    bool hasFilterInfo() const { return m_hasFilterInfo; }
    void setHasFilterInfo(bool hasFilterInfo) { m_hasFilterInfo = hasFilterInfo; }

    void updateFilters(const ComputedStyle* oldStyle, const ComputedStyle& newStyle);

    Node* enclosingElement() const;

    bool isInTopLayer() const;

    bool scrollsWithViewport() const;
    bool scrollsWithRespectTo(const DeprecatedPaintLayer*) const;

    void addLayerHitTestRects(LayerHitTestRects&) const;

    // Compute rects only for this layer
    void computeSelfHitTestRects(LayerHitTestRects&) const;

    // FIXME: This should probably return a ScrollableArea but a lot of internal methods are mistakenly exposed.
    DeprecatedPaintLayerScrollableArea* scrollableArea() const { return m_scrollableArea.get(); }
    DeprecatedPaintLayerClipper& clipper() { return m_clipper; }
    const DeprecatedPaintLayerClipper& clipper() const { return m_clipper; }

    inline bool isPositionedContainer() const
    {
        // FIXME: This is not in sync with containingBlock.
        // LayoutObject::canContainFixedPositionObjects() should probably be used
        // instead.
        LayoutBoxModelObject* layerlayoutObject = layoutObject();
        return isRootLayer() || layerlayoutObject->isPositioned() || hasTransformRelatedProperty();
    }

    bool scrollsOverflow() const;

    CompositingReasons potentialCompositingReasonsFromStyle() const { return m_potentialCompositingReasonsFromStyle; }
    void setPotentialCompositingReasonsFromStyle(CompositingReasons reasons) { ASSERT(reasons == (reasons & CompositingReasonComboAllStyleDeterminedReasons)); m_potentialCompositingReasonsFromStyle = reasons; }

    bool hasStyleDeterminedDirectCompositingReasons() const { return m_potentialCompositingReasonsFromStyle & CompositingReasonComboAllDirectStyleDeterminedReasons; }

    class AncestorDependentCompositingInputs {
    public:
        AncestorDependentCompositingInputs()
            : opacityAncestor(0)
            , transformAncestor(0)
            , filterAncestor(0)
            , clippingContainer(0)
            , ancestorScrollingLayer(0)
            , scrollParent(0)
            , clipParent(0)
            , hasAncestorWithClipPath(false)
        { }

        IntRect clippedAbsoluteBoundingBox;
        const DeprecatedPaintLayer* opacityAncestor;
        const DeprecatedPaintLayer* transformAncestor;
        const DeprecatedPaintLayer* filterAncestor;
        const LayoutObject* clippingContainer;
        const DeprecatedPaintLayer* ancestorScrollingLayer;

        // A scroll parent is a compositor concept. It's only needed in blink
        // because we need to use it as a promotion trigger. A layer has a
        // scroll parent if neither its compositor scrolling ancestor, nor any
        // other layer scrolled by this ancestor, is a stacking ancestor of this
        // layer. Layers with scroll parents must be scrolled with the main
        // scrolling layer by the compositor.
        const DeprecatedPaintLayer* scrollParent;

        // A clip parent is another compositor concept that has leaked into
        // blink so that it may be used as a promotion trigger. Layers with clip
        // parents escape the clip of a stacking tree ancestor. The compositor
        // needs to know about clip parents in order to circumvent its normal
        // clipping logic.
        const DeprecatedPaintLayer* clipParent;

        unsigned hasAncestorWithClipPath : 1;
    };

    class DescendantDependentCompositingInputs {
    public:
        DescendantDependentCompositingInputs()
            : hasDescendantWithClipPath(false)
            , hasNonIsolatedDescendantWithBlendMode(false)
        { }

        unsigned hasDescendantWithClipPath : 1;
        unsigned hasNonIsolatedDescendantWithBlendMode : 1;
    };

    void setNeedsCompositingInputsUpdate();
    bool childNeedsCompositingInputsUpdate() const { return m_childNeedsCompositingInputsUpdate; }
    bool needsCompositingInputsUpdate() const
    {
        // While we're updating the compositing inputs, these values may differ.
        // We should never be asking for this value when that is the case.
        ASSERT(m_needsDescendantDependentCompositingInputsUpdate == m_needsAncestorDependentCompositingInputsUpdate);
        return m_needsDescendantDependentCompositingInputsUpdate;
    }

    void updateAncestorDependentCompositingInputs(const AncestorDependentCompositingInputs&);
    void updateDescendantDependentCompositingInputs(const DescendantDependentCompositingInputs&);
    void didUpdateCompositingInputs();

    const AncestorDependentCompositingInputs& ancestorDependentCompositingInputs() const { ASSERT(!m_needsAncestorDependentCompositingInputsUpdate); return m_ancestorDependentCompositingInputs; }
    const DescendantDependentCompositingInputs& descendantDependentCompositingInputs() const { ASSERT(!m_needsDescendantDependentCompositingInputsUpdate); return m_descendantDependentCompositingInputs; }

    IntRect clippedAbsoluteBoundingBox() const { return ancestorDependentCompositingInputs().clippedAbsoluteBoundingBox; }
    const DeprecatedPaintLayer* opacityAncestor() const { return ancestorDependentCompositingInputs().opacityAncestor; }
    const DeprecatedPaintLayer* transformAncestor() const { return ancestorDependentCompositingInputs().transformAncestor; }
    const DeprecatedPaintLayer* filterAncestor() const { return ancestorDependentCompositingInputs().filterAncestor; }
    const LayoutObject* clippingContainer() const { return ancestorDependentCompositingInputs().clippingContainer; }
    const DeprecatedPaintLayer* ancestorScrollingLayer() const { return ancestorDependentCompositingInputs().ancestorScrollingLayer; }
    DeprecatedPaintLayer* scrollParent() const { return const_cast<DeprecatedPaintLayer*>(ancestorDependentCompositingInputs().scrollParent); }
    DeprecatedPaintLayer* clipParent() const { return const_cast<DeprecatedPaintLayer*>(ancestorDependentCompositingInputs().clipParent); }
    bool hasAncestorWithClipPath() const { return ancestorDependentCompositingInputs().hasAncestorWithClipPath; }
    bool hasDescendantWithClipPath() const { return descendantDependentCompositingInputs().hasDescendantWithClipPath; }
    bool hasNonIsolatedDescendantWithBlendMode() const;

    bool lostGroupedMapping() const { ASSERT(isAllowedToQueryCompositingState()); return m_lostGroupedMapping; }
    void setLostGroupedMapping(bool b) { m_lostGroupedMapping = b; }

    CompositingReasons compositingReasons() const { ASSERT(isAllowedToQueryCompositingState()); return m_compositingReasons; }
    void setCompositingReasons(CompositingReasons, CompositingReasons mask = CompositingReasonAll);

    bool hasCompositingDescendant() const { ASSERT(isAllowedToQueryCompositingState()); return m_hasCompositingDescendant; }
    void setHasCompositingDescendant(bool);

    bool shouldIsolateCompositedDescendants() const { ASSERT(isAllowedToQueryCompositingState()); return m_shouldIsolateCompositedDescendants; }
    void setShouldIsolateCompositedDescendants(bool);

    void updateDescendantDependentFlags();

    void updateOrRemoveFilterEffectBuilder();

    void updateSelfPaintingLayer();

    DeprecatedPaintLayer* enclosingTransformedAncestor() const;
    LayoutPoint computeOffsetFromTransformedAncestor() const;

    void didUpdateNeedsCompositedScrolling();

    bool hasSelfPaintingLayerDescendant() const
    {
        if (m_hasSelfPaintingLayerDescendantDirty)
            updateHasSelfPaintingLayerDescendant();
        ASSERT(!m_hasSelfPaintingLayerDescendantDirty);
        return m_hasSelfPaintingLayerDescendant;
    }
    LayoutRect paintingExtent(const DeprecatedPaintLayer* rootLayer, const LayoutRect& paintDirtyRect, const LayoutSize& subPixelAccumulation, PaintBehavior);
    void appendSingleFragmentIgnoringPagination(DeprecatedPaintLayerFragments&, const DeprecatedPaintLayer* rootLayer, const LayoutRect& dirtyRect, ClipRectsCacheSlot, OverlayScrollbarSizeRelevancy = IgnoreOverlayScrollbarSize, ShouldRespectOverflowClip = RespectOverflowClip, const LayoutPoint* offsetFromRoot = 0, const LayoutSize& subPixelAccumulation = LayoutSize());
    void collectFragments(DeprecatedPaintLayerFragments&, const DeprecatedPaintLayer* rootLayer, const LayoutRect& dirtyRect,
        ClipRectsCacheSlot, OverlayScrollbarSizeRelevancy inOverlayScrollbarSizeRelevancy = IgnoreOverlayScrollbarSize,
        ShouldRespectOverflowClip = RespectOverflowClip, const LayoutPoint* offsetFromRoot = 0,
        const LayoutSize& subPixelAccumulation = LayoutSize(), const LayoutRect* layerBoundingBox = 0);

    LayoutPoint layoutBoxLocation() const { return layoutObject()->isBox() ? toLayoutBox(layoutObject())->location() : LayoutPoint(); }

    enum TransparencyClipBoxBehavior {
        PaintingTransparencyClipBox,
        HitTestingTransparencyClipBox
    };

    enum TransparencyClipBoxMode {
        DescendantsOfTransparencyClipBox,
        RootOfTransparencyClipBox
    };

    static LayoutRect transparencyClipBox(const DeprecatedPaintLayer*, const DeprecatedPaintLayer* rootLayer, TransparencyClipBoxBehavior transparencyBehavior,
        TransparencyClipBoxMode transparencyMode, const LayoutSize& subPixelAccumulation, PaintBehavior = 0);

private:
    // Bounding box in the coordinates of this layer.
    LayoutRect logicalBoundingBox() const;

    bool hasOverflowControls() const;

    void dirtyAncestorChainHasSelfPaintingLayerDescendantStatus();

    // Returns true if the position changed.
    bool updateLayerPosition();

    void updateLayerPositionRecursive();
    void updateLayerPositionsAfterScrollRecursive(const DoubleSize& scrollDelta, bool paintInvalidationContainerWasScrolled);

    void setNextSibling(DeprecatedPaintLayer* next) { m_next = next; }
    void setPreviousSibling(DeprecatedPaintLayer* prev) { m_previous = prev; }
    void setFirstChild(DeprecatedPaintLayer* first) { m_first = first; }
    void setLastChild(DeprecatedPaintLayer* last) { m_last = last; }

    void updateHasSelfPaintingLayerDescendant() const;
    DeprecatedPaintLayer* hitTestLayer(DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* containerLayer, HitTestResult&,
        const LayoutRect& hitTestRect, const HitTestLocation&, bool appliedTransform,
        const HitTestingTransformState* = 0, double* zOffset = 0);
    DeprecatedPaintLayer* hitTestLayerByApplyingTransform(DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* containerLayer, HitTestResult&,
        const LayoutRect& hitTestRect, const HitTestLocation&, const HitTestingTransformState* = 0, double* zOffset = 0,
        const LayoutPoint& translationOffset = LayoutPoint());
    DeprecatedPaintLayer* hitTestChildren(ChildrenIteration, DeprecatedPaintLayer* rootLayer, HitTestResult&,
        const LayoutRect& hitTestRect, const HitTestLocation&,
        const HitTestingTransformState*, double* zOffsetForDescendants, double* zOffset,
        const HitTestingTransformState* unflattenedTransformState, bool depthSortDescendants);

    PassRefPtr<HitTestingTransformState> createLocalTransformState(DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* containerLayer,
        const LayoutRect& hitTestRect, const HitTestLocation&,
        const HitTestingTransformState* containerTransformState,
        const LayoutPoint& translationOffset = LayoutPoint()) const;

    bool hitTestContents(HitTestResult&, const LayoutRect& layerBounds, const HitTestLocation&, HitTestFilter) const;
    bool hitTestContentsForFragments(const DeprecatedPaintLayerFragments&, HitTestResult&, const HitTestLocation&, HitTestFilter, bool& insideClipRect) const;
    DeprecatedPaintLayer* hitTestTransformedLayerInFragments(DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer* containerLayer, HitTestResult&,
        const LayoutRect& hitTestRect, const HitTestLocation&, const HitTestingTransformState*, double* zOffset, ClipRectsCacheSlot);

    bool childBackgroundIsKnownToBeOpaqueInRect(const LayoutRect&) const;

    bool shouldBeSelfPaintingLayer() const;

    // FIXME: We should only create the stacking node if needed.
    bool requiresStackingNode() const { return true; }
    void updateStackingNode();

    void updateReflectionInfo(const ComputedStyle*);

    // FIXME: We could lazily allocate our ScrollableArea based on style properties ('overflow', ...)
    // but for now, we are always allocating it for LayoutBox as it's safer. crbug.com/467721.
    bool requiresScrollableArea() const { return layoutBox(); }
    void updateScrollableArea();

    void dirtyAncestorChainVisibleDescendantStatus();

    bool attemptDirectCompositingUpdate(StyleDifference, const ComputedStyle* oldStyle);
    void updateTransform(const ComputedStyle* oldStyle, const ComputedStyle& newStyle);

    void dirty3DTransformedDescendantStatus();
    // Both updates the status, and returns true if descendants of this have 3d.
    bool update3DTransformedDescendantStatus();

    void updateOrRemoveFilterClients();

    void updatePaginationRecursive(bool needsPaginationUpdate = false);
    void clearPaginationRecursive();

    DeprecatedPaintLayerType m_layerType;

    // Self-painting layer is an optimization where we avoid the heavy Layer painting
    // machinery for a Layer allocated only to handle the overflow clip case.
    // FIXME(crbug.com/332791): Self-painting layer should be merged into the overflow-only concept.
    unsigned m_isSelfPaintingLayer : 1;

    // If have no self-painting descendants, we don't have to walk our children during painting. This can lead to
    // significant savings, especially if the tree has lots of non-self-painting layers grouped together (e.g. table cells).
    mutable unsigned m_hasSelfPaintingLayerDescendant : 1;
    mutable unsigned m_hasSelfPaintingLayerDescendantDirty : 1;

    const unsigned m_isRootLayer : 1;

    unsigned m_visibleContentStatusDirty : 1;
    unsigned m_hasVisibleContent : 1;
    unsigned m_visibleDescendantStatusDirty : 1;
    unsigned m_hasVisibleDescendant : 1;

    unsigned m_hasVisibleNonLayerContent : 1;

#if ENABLE(ASSERT)
    unsigned m_needsPositionUpdate : 1;
#endif

    unsigned m_3DTransformedDescendantStatusDirty : 1;
    // Set on a stacking context layer that has 3D descendants anywhere
    // in a preserves3D hierarchy. Hint to do 3D-aware hit testing.
    unsigned m_has3DTransformedDescendant : 1;

    unsigned m_containsDirtyOverlayScrollbars : 1;

    unsigned m_hasFilterInfo : 1;
    unsigned m_needsAncestorDependentCompositingInputsUpdate : 1;
    unsigned m_needsDescendantDependentCompositingInputsUpdate : 1;
    unsigned m_childNeedsCompositingInputsUpdate : 1;

    // Used only while determining what layers should be composited. Applies to the tree of z-order lists.
    unsigned m_hasCompositingDescendant : 1;

    // Applies to the real layout layer tree (i.e., the tree determined by the layer's parent and children and
    // as opposed to the tree formed by the z-order and normal flow lists).
    unsigned m_hasNonCompositedChild : 1;

    // Should be for stacking contexts having unisolated blending descendants.
    unsigned m_shouldIsolateCompositedDescendants : 1;

    // True if this layout layer just lost its grouped mapping due to the CompositedDeprecatedPaintLayerMapping being destroyed,
    // and we don't yet know to what graphics layer this Layer will be assigned.
    unsigned m_lostGroupedMapping : 1;

    LayoutBoxModelObject* m_layoutObject;

    DeprecatedPaintLayer* m_parent;
    DeprecatedPaintLayer* m_previous;
    DeprecatedPaintLayer* m_next;
    DeprecatedPaintLayer* m_first;
    DeprecatedPaintLayer* m_last;

    // Our current relative position offset.
    LayoutSize m_offsetForInFlowPosition;

    // Our (x,y) coordinates are in our parent layer's coordinate space.
    LayoutPoint m_location;

    // The layer's width/height
    IntSize m_size;

    // Cached normal flow values for absolute positioned elements with static left/top values.
    LayoutUnit m_staticInlinePosition;
    LayoutUnit m_staticBlockPosition;

    OwnPtr<TransformationMatrix> m_transform;

    // Pointer to the enclosing Layer that caused us to be paginated. It is 0 if we are not paginated.
    //
    // See LayoutMultiColumnFlowThread and
    // https://sites.google.com/a/chromium.org/dev/developers/design-documents/multi-column-layout
    // for more information about the multicol implementation. It's important to understand the
    // difference between flow thread coordinates and visual coordinates when working with multicol
    // in Layer, since Layer is one of the few places where we have to worry about the
    // visual ones. Internally we try to use flow-thread coordinates whenever possible.
    DeprecatedPaintLayer* m_enclosingPaginationLayer;

    // These compositing reasons are updated whenever style changes, not while updating compositing layers.
    // They should not be used to infer the compositing state of this layer.
    CompositingReasons m_potentialCompositingReasonsFromStyle;

    // Once computed, indicates all that a layer needs to become composited using the CompositingReasons enum bitfield.
    CompositingReasons m_compositingReasons;

    DescendantDependentCompositingInputs m_descendantDependentCompositingInputs;
    AncestorDependentCompositingInputs m_ancestorDependentCompositingInputs;

    IntRect m_blockSelectionGapsBounds;

    OwnPtr<CompositedDeprecatedPaintLayerMapping> m_compositedDeprecatedPaintLayerMapping;
    OwnPtrWillBePersistent<DeprecatedPaintLayerScrollableArea> m_scrollableArea;

    CompositedDeprecatedPaintLayerMapping* m_groupedMapping;

    DeprecatedPaintLayerClipper m_clipper; // FIXME: Lazily allocate?
    OwnPtr<DeprecatedPaintLayerStackingNode> m_stackingNode;
    OwnPtr<DeprecatedPaintLayerReflectionInfo> m_reflectionInfo;

    LayoutSize m_subpixelAccumulation; // The accumulated subpixel offset of a composited layer's composited bounds compared to absolute coordinates.
};

} // namespace blink

#ifndef NDEBUG
// Outside the WebCore namespace for ease of invocation from gdb.
void showLayerTree(const blink::DeprecatedPaintLayer*);
void showLayerTree(const blink::LayoutObject*);
#endif

#endif // Layer_h
