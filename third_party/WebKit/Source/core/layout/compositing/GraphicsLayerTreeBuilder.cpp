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
#include "core/layout/compositing/GraphicsLayerTreeBuilder.h"

#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLVideoElement.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/paint/DeprecatedPaintLayer.h"

namespace blink {

GraphicsLayerTreeBuilder::GraphicsLayerTreeBuilder()
{
}

GraphicsLayerTreeBuilder::~GraphicsLayerTreeBuilder()
{
}

static bool shouldAppendLayer(const DeprecatedPaintLayer& layer)
{
    if (!RuntimeEnabledFeatures::overlayFullscreenVideoEnabled())
        return true;
    Node* node = layer.layoutObject()->node();
    if (node && isHTMLVideoElement(*node)) {
        HTMLVideoElement* element = toHTMLVideoElement(node);
        // For WebRTC, video frame contains all the data and no hardware surface is used.
        // We should always append the layer in this case.
        if (element->isFullscreen() && !HTMLMediaElement::isMediaStreamURL(element->sourceURL().string()))
            return false;
    }
    return true;
}

void GraphicsLayerTreeBuilder::rebuild(DeprecatedPaintLayer& layer, AncestorInfo info)
{
    // Make the layer compositing if necessary, and set up clipping and content layers.
    // Note that we can only do work here that is independent of whether the descendant layers
    // have been processed. computeCompositingRequirements() will already have done the paint invalidation if necessary.

    layer.stackingNode()->updateLayerListsIfNeeded();

    const bool hasCompositedDeprecatedPaintLayerMapping = layer.hasCompositedDeprecatedPaintLayerMapping();
    CompositedDeprecatedPaintLayerMapping* currentCompositedDeprecatedPaintLayerMapping = layer.compositedDeprecatedPaintLayerMapping();

    // If this layer has a compositedDeprecatedPaintLayerMapping, then that is where we place subsequent children GraphicsLayers.
    // Otherwise children continue to append to the child list of the enclosing layer.
    GraphicsLayerVector layerChildren;
    AncestorInfo infoForChildren(info);
    if (hasCompositedDeprecatedPaintLayerMapping) {
        infoForChildren.childLayersOfEnclosingCompositedLayer = &layerChildren;
        infoForChildren.enclosingCompositedLayer = &layer;
    }

#if ENABLE(ASSERT)
    LayerListMutationDetector mutationChecker(layer.stackingNode());
#endif

    if (layer.stackingNode()->isStackingContext()) {
        DeprecatedPaintLayerStackingNodeIterator iterator(*layer.stackingNode(), NegativeZOrderChildren);
        while (DeprecatedPaintLayerStackingNode* curNode = iterator.next())
            rebuild(*curNode->layer(), infoForChildren);

        // If a negative z-order child is compositing, we get a foreground layer which needs to get parented.
        if (hasCompositedDeprecatedPaintLayerMapping && currentCompositedDeprecatedPaintLayerMapping->foregroundLayer())
            infoForChildren.childLayersOfEnclosingCompositedLayer->append(currentCompositedDeprecatedPaintLayerMapping->foregroundLayer());
    }

    DeprecatedPaintLayerStackingNodeIterator iterator(*layer.stackingNode(), NormalFlowChildren | PositiveZOrderChildren);
    while (DeprecatedPaintLayerStackingNode* curNode = iterator.next())
        rebuild(*curNode->layer(), infoForChildren);

    if (hasCompositedDeprecatedPaintLayerMapping) {
        bool parented = false;
        if (layer.layoutObject()->isLayoutPart())
            parented = DeprecatedPaintLayerCompositor::parentFrameContentLayers(toLayoutPart(layer.layoutObject()));

        if (!parented)
            currentCompositedDeprecatedPaintLayerMapping->parentForSublayers()->setChildren(layerChildren);

        // If the layer has a clipping layer the overflow controls layers will be siblings of the clipping layer.
        // Otherwise, the overflow control layers are normal children.
        // FIXME: Why isn't this handled in CLM updateInternalHierarchy?
        if (!currentCompositedDeprecatedPaintLayerMapping->hasClippingLayer() && !currentCompositedDeprecatedPaintLayerMapping->hasScrollingLayer()) {
            if (GraphicsLayer* overflowControlLayer = currentCompositedDeprecatedPaintLayerMapping->layerForHorizontalScrollbar()) {
                overflowControlLayer->removeFromParent();
                currentCompositedDeprecatedPaintLayerMapping->parentForSublayers()->addChild(overflowControlLayer);
            }

            if (GraphicsLayer* overflowControlLayer = currentCompositedDeprecatedPaintLayerMapping->layerForVerticalScrollbar()) {
                overflowControlLayer->removeFromParent();
                currentCompositedDeprecatedPaintLayerMapping->parentForSublayers()->addChild(overflowControlLayer);
            }

            if (GraphicsLayer* overflowControlLayer = currentCompositedDeprecatedPaintLayerMapping->layerForScrollCorner()) {
                overflowControlLayer->removeFromParent();
                currentCompositedDeprecatedPaintLayerMapping->parentForSublayers()->addChild(overflowControlLayer);
            }
        }

        if (shouldAppendLayer(layer))
            info.childLayersOfEnclosingCompositedLayer->append(currentCompositedDeprecatedPaintLayerMapping->childForSuperlayers());
    }

    if (layer.scrollParent()
        && layer.scrollParent()->hasCompositedDeprecatedPaintLayerMapping()
        && layer.scrollParent()->compositedDeprecatedPaintLayerMapping()->needsToReparentOverflowControls()
        && layer.scrollParent()->scrollableArea()->topmostScrollChild() == &layer)
        info.childLayersOfEnclosingCompositedLayer->append(layer.scrollParent()->compositedDeprecatedPaintLayerMapping()->detachLayerForOverflowControls(*info.enclosingCompositedLayer));
}

}
