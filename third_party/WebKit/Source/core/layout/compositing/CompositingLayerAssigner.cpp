/*
 * Copyright (C) 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/compositing/CompositingLayerAssigner.h"

#include "core/inspector/InspectorTraceEvents.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/page/Page.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "platform/TraceEvent.h"

namespace blink {

// We will only allow squashing if the bbox-area:squashed-area doesn't exceed
// the ratio |gSquashingSparsityTolerance|:1.
static uint64_t gSquashingSparsityTolerance = 6;

CompositingLayerAssigner::CompositingLayerAssigner(DeprecatedPaintLayerCompositor* compositor)
    : m_compositor(compositor)
    , m_layersChanged(false)
{
}

CompositingLayerAssigner::~CompositingLayerAssigner()
{
}

void CompositingLayerAssigner::assign(DeprecatedPaintLayer* updateRoot, Vector<DeprecatedPaintLayer*>& layersNeedingPaintInvalidation)
{
    TRACE_EVENT0("blink", "CompositingLayerAssigner::assign");

    SquashingState squashingState;
    assignLayersToBackingsInternal(updateRoot, squashingState, layersNeedingPaintInvalidation);
    if (squashingState.hasMostRecentMapping)
        squashingState.mostRecentMapping->finishAccumulatingSquashingLayers(squashingState.nextSquashedLayerIndex);
}

void CompositingLayerAssigner::SquashingState::updateSquashingStateForNewMapping(CompositedDeprecatedPaintLayerMapping* newCompositedDeprecatedPaintLayerMapping, bool hasNewCompositedDeprecatedPaintLayerMapping)
{
    // The most recent backing is done accumulating any more squashing layers.
    if (hasMostRecentMapping)
        mostRecentMapping->finishAccumulatingSquashingLayers(nextSquashedLayerIndex);

    nextSquashedLayerIndex = 0;
    boundingRect = IntRect();
    mostRecentMapping = newCompositedDeprecatedPaintLayerMapping;
    hasMostRecentMapping = hasNewCompositedDeprecatedPaintLayerMapping;
    haveAssignedBackingsToEntireSquashingLayerSubtree = false;
}

bool CompositingLayerAssigner::squashingWouldExceedSparsityTolerance(const DeprecatedPaintLayer* candidate, const CompositingLayerAssigner::SquashingState& squashingState)
{
    IntRect bounds = candidate->clippedAbsoluteBoundingBox();
    IntRect newBoundingRect = squashingState.boundingRect;
    newBoundingRect.unite(bounds);
    const uint64_t newBoundingRectArea = newBoundingRect.size().area();
    const uint64_t newSquashedArea = squashingState.totalAreaOfSquashedRects + bounds.size().area();
    return newBoundingRectArea > gSquashingSparsityTolerance * newSquashedArea;
}

bool CompositingLayerAssigner::needsOwnBacking(const DeprecatedPaintLayer* layer) const
{
    if (!m_compositor->canBeComposited(layer))
        return false;

    return requiresCompositing(layer->compositingReasons()) || (m_compositor->staleInCompositingMode() && layer->isRootLayer());
}

CompositingStateTransitionType CompositingLayerAssigner::computeCompositedLayerUpdate(DeprecatedPaintLayer* layer)
{
    CompositingStateTransitionType update = NoCompositingStateChange;
    if (needsOwnBacking(layer)) {
        if (!layer->hasCompositedDeprecatedPaintLayerMapping()) {
            update = AllocateOwnCompositedDeprecatedPaintLayerMapping;
        }
    } else {
        if (layer->hasCompositedDeprecatedPaintLayerMapping())
            update = RemoveOwnCompositedDeprecatedPaintLayerMapping;

        if (!layer->subtreeIsInvisible() && m_compositor->canBeComposited(layer) && requiresSquashing(layer->compositingReasons())) {
            // We can't compute at this time whether the squashing layer update is a no-op,
            // since that requires walking the paint layer tree.
            update = PutInSquashingLayer;
        } else if (layer->groupedMapping() || layer->lostGroupedMapping()) {
            update = RemoveFromSquashingLayer;
        }
    }
    return update;
}

CompositingReasons CompositingLayerAssigner::getReasonsPreventingSquashing(const DeprecatedPaintLayer* layer, const CompositingLayerAssigner::SquashingState& squashingState)
{
    if (!squashingState.haveAssignedBackingsToEntireSquashingLayerSubtree)
        return CompositingReasonSquashingWouldBreakPaintOrder;

    ASSERT(squashingState.hasMostRecentMapping);
    const DeprecatedPaintLayer& squashingLayer = squashingState.mostRecentMapping->owningLayer();

    // FIXME: this special case for video exists only to deal with corner cases
    // where a LayoutVideo does not report that it needs to be directly composited.
    // Video does not currently support sharing a backing, but this could be
    // generalized in the future. The following layout tests fail if we permit the
    // video to share a backing with other layers.
    //
    // compositing/video/video-controls-layer-creation.html
    if (layer->layoutObject()->isVideo() || squashingLayer.layoutObject()->isVideo())
        return CompositingReasonSquashingVideoIsDisallowed;

    // Don't squash iframes, frames or plugins.
    // FIXME: this is only necessary because there is frame code that assumes that composited frames are not squashed.
    if (layer->layoutObject()->isLayoutPart() || squashingLayer.layoutObject()->isLayoutPart())
        return CompositingReasonSquashingLayoutPartIsDisallowed;

    if (layer->reflectionInfo())
        return CompositingReasonSquashingReflectionIsDisallowed;

    if (squashingWouldExceedSparsityTolerance(layer, squashingState))
        return CompositingReasonSquashingSparsityExceeded;

    if (layer->layoutObject()->style()->hasBlendMode())
        return CompositingReasonSquashingBlendingIsDisallowed;

    // FIXME: this is not efficient, since it walks up the tree. We should store these values on the CompositingInputsCache.
    if (layer->clippingContainer() != squashingLayer.clippingContainer() && !squashingLayer.compositedDeprecatedPaintLayerMapping()->containingSquashedLayer(layer->clippingContainer(), squashingState.nextSquashedLayerIndex))
        return CompositingReasonSquashingClippingContainerMismatch;

    // Composited descendants need to be clipped by a child containment graphics layer, which would not be available if the layer is
    // squashed (and therefore has no CLM nor a child containment graphics layer).
    if (m_compositor->clipsCompositingDescendants(layer))
        return CompositingReasonSquashedLayerClipsCompositingDescendants;

    if (layer->scrollsWithRespectTo(&squashingLayer))
        return CompositingReasonScrollsWithRespectToSquashingLayer;

    if (layer->scrollParent() && layer->hasCompositingDescendant())
        return CompositingReasonScrollChildWithCompositedDescendants;

    const DeprecatedPaintLayer::AncestorDependentCompositingInputs& compositingInputs = layer->ancestorDependentCompositingInputs();
    const DeprecatedPaintLayer::AncestorDependentCompositingInputs& squashingLayerCompositingInputs = squashingLayer.ancestorDependentCompositingInputs();

    if (compositingInputs.opacityAncestor != squashingLayerCompositingInputs.opacityAncestor)
        return CompositingReasonSquashingOpacityAncestorMismatch;

    if (compositingInputs.transformAncestor != squashingLayerCompositingInputs.transformAncestor)
        return CompositingReasonSquashingTransformAncestorMismatch;

    if (layer->hasFilter() || compositingInputs.filterAncestor != squashingLayerCompositingInputs.filterAncestor)
        return CompositingReasonSquashingFilterMismatch;

    if (layer->layoutObject()->style()->position() == FixedPosition)
        return CompositingReasonSquashingPositionFixedIsDisallowed;

    return CompositingReasonNone;
}

void CompositingLayerAssigner::updateSquashingAssignment(DeprecatedPaintLayer* layer, SquashingState& squashingState, const CompositingStateTransitionType compositedLayerUpdate,
    Vector<DeprecatedPaintLayer*>& layersNeedingPaintInvalidation)
{
    // NOTE: In the future as we generalize this, the background of this layer may need to be assigned to a different backing than
    // the squashed DeprecatedPaintLayer's own primary contents. This would happen when we have a composited negative z-index element that needs
    // to paint on top of the background, but below the layer's main contents. For now, because we always composite layers
    // when they have a composited negative z-index child, such layers will never need squashing so it is not yet an issue.
    if (compositedLayerUpdate == PutInSquashingLayer) {
        // A layer that is squashed with other layers cannot have its own CompositedDeprecatedPaintLayerMapping.
        ASSERT(!layer->hasCompositedDeprecatedPaintLayerMapping());
        ASSERT(squashingState.hasMostRecentMapping);

        bool changedSquashingLayer =
            squashingState.mostRecentMapping->updateSquashingLayerAssignment(layer, squashingState.nextSquashedLayerIndex);
        if (!changedSquashingLayer)
            return;

        // If we've modified the collection of squashed layers, we must update
        // the graphics layer geometry.
        squashingState.mostRecentMapping->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateSubtree);

        layer->clipper().clearClipRectsIncludingDescendants();

        // Issue a paint invalidation, since |layer| may have been added to an already-existing squashing layer.
        TRACE_LAYER_INVALIDATION(layer, InspectorLayerInvalidationTrackingEvent::AddedToSquashingLayer);
        layersNeedingPaintInvalidation.append(layer);
        m_layersChanged = true;
    } else if (compositedLayerUpdate == RemoveFromSquashingLayer) {
        if (layer->groupedMapping()) {
            // Before removing |layer| from an already-existing squashing layer that may have other content, issue a paint invalidation.
            m_compositor->paintInvalidationOnCompositingChange(layer);
            layer->groupedMapping()->setNeedsGraphicsLayerUpdate(GraphicsLayerUpdateSubtree);
            layer->setGroupedMapping(nullptr, DeprecatedPaintLayer::InvalidateLayerAndRemoveFromMapping);
        }

        // If we need to issue paint invalidations, do so now that we've removed it from a squashed layer.
        TRACE_LAYER_INVALIDATION(layer, InspectorLayerInvalidationTrackingEvent::RemovedFromSquashingLayer);
        layersNeedingPaintInvalidation.append(layer);
        m_layersChanged = true;

        layer->setLostGroupedMapping(false);
    }
}

void CompositingLayerAssigner::assignLayersToBackingsForReflectionLayer(DeprecatedPaintLayer* reflectionLayer, Vector<DeprecatedPaintLayer*>& layersNeedingPaintInvalidation)
{
    CompositingStateTransitionType compositedLayerUpdate = computeCompositedLayerUpdate(reflectionLayer);
    if (compositedLayerUpdate != NoCompositingStateChange) {
        TRACE_LAYER_INVALIDATION(reflectionLayer, InspectorLayerInvalidationTrackingEvent::ReflectionLayerChanged);
        layersNeedingPaintInvalidation.append(reflectionLayer);
        m_layersChanged = true;
        m_compositor->allocateOrClearCompositedDeprecatedPaintLayerMapping(reflectionLayer, compositedLayerUpdate);
    }
    m_compositor->updateDirectCompositingReasons(reflectionLayer);

    // FIXME: Why do we updateGraphicsLayerConfiguration here instead of in the GraphicsLayerUpdater?
    if (reflectionLayer->hasCompositedDeprecatedPaintLayerMapping())
        reflectionLayer->compositedDeprecatedPaintLayerMapping()->updateGraphicsLayerConfiguration();
}

static ScrollingCoordinator* scrollingCoordinatorFromLayer(DeprecatedPaintLayer& layer)
{
    Page* page = layer.layoutObject()->frame()->page();
    return (!page) ? nullptr : page->scrollingCoordinator();
}

void CompositingLayerAssigner::assignLayersToBackingsInternal(DeprecatedPaintLayer* layer, SquashingState& squashingState, Vector<DeprecatedPaintLayer*>& layersNeedingPaintInvalidation)
{
    if (requiresSquashing(layer->compositingReasons())) {
        CompositingReasons reasonsPreventingSquashing = getReasonsPreventingSquashing(layer, squashingState);
        if (reasonsPreventingSquashing)
            layer->setCompositingReasons(layer->compositingReasons() | reasonsPreventingSquashing);
    }

    CompositingStateTransitionType compositedLayerUpdate = computeCompositedLayerUpdate(layer);

    if (m_compositor->allocateOrClearCompositedDeprecatedPaintLayerMapping(layer, compositedLayerUpdate)) {
        TRACE_LAYER_INVALIDATION(layer, InspectorLayerInvalidationTrackingEvent::NewCompositedLayer);
        layersNeedingPaintInvalidation.append(layer);
        m_layersChanged = true;
        if (ScrollingCoordinator* scrollingCoordinator = scrollingCoordinatorFromLayer(*layer)) {
            if (layer->layoutObject()->style()->hasViewportConstrainedPosition())
                scrollingCoordinator->frameViewFixedObjectsDidChange(layer->layoutObject()->view()->frameView());
        }
    }

    // FIXME: special-casing reflection layers here is not right.
    if (layer->reflectionInfo())
        assignLayersToBackingsForReflectionLayer(layer->reflectionInfo()->reflectionLayer(), layersNeedingPaintInvalidation);

    // Add this layer to a squashing backing if needed.
    updateSquashingAssignment(layer, squashingState, compositedLayerUpdate, layersNeedingPaintInvalidation);

    const bool layerIsSquashed = compositedLayerUpdate == PutInSquashingLayer || (compositedLayerUpdate == NoCompositingStateChange && layer->groupedMapping());
    if (layerIsSquashed) {
        squashingState.nextSquashedLayerIndex++;
        IntRect layerBounds = layer->clippedAbsoluteBoundingBox();
        squashingState.totalAreaOfSquashedRects += layerBounds.size().area();
        squashingState.boundingRect.unite(layerBounds);
    }

    if (layer->stackingNode()->isStackingContext()) {
        DeprecatedPaintLayerStackingNodeIterator iterator(*layer->stackingNode(), NegativeZOrderChildren);
        while (DeprecatedPaintLayerStackingNode* curNode = iterator.next())
            assignLayersToBackingsInternal(curNode->layer(), squashingState, layersNeedingPaintInvalidation);
    }

    // At this point, if the layer is to be separately composited, then its backing becomes the most recent in paint-order.
    if (layer->compositingState() == PaintsIntoOwnBacking) {
        ASSERT(!requiresSquashing(layer->compositingReasons()));
        squashingState.updateSquashingStateForNewMapping(layer->compositedDeprecatedPaintLayerMapping(), layer->hasCompositedDeprecatedPaintLayerMapping());
    }

    if (layer->scrollParent())
        layer->scrollParent()->scrollableArea()->setTopmostScrollChild(layer);

    if (layer->needsCompositedScrolling())
        layer->scrollableArea()->setTopmostScrollChild(nullptr);

    DeprecatedPaintLayerStackingNodeIterator iterator(*layer->stackingNode(), NormalFlowChildren | PositiveZOrderChildren);
    while (DeprecatedPaintLayerStackingNode* curNode = iterator.next())
        assignLayersToBackingsInternal(curNode->layer(), squashingState, layersNeedingPaintInvalidation);

    if (squashingState.hasMostRecentMapping && &squashingState.mostRecentMapping->owningLayer() == layer)
        squashingState.haveAssignedBackingsToEntireSquashingLayerSubtree = true;
}

}
