/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "core/page/scrolling/ScrollingCoordinator.h"

#include "core/dom/Document.h"
#include "core/dom/Fullscreen.h"
#include "core/dom/Node.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLElement.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/CompositedDeprecatedPaintLayerMapping.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "core/plugins/PluginView.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/exported/WebScrollbarImpl.h"
#include "platform/exported/WebScrollbarThemeGeometryNative.h"
#include "platform/geometry/Region.h"
#include "platform/geometry/TransformState.h"
#include "platform/graphics/GraphicsLayer.h"
#if OS(MACOSX)
#include "platform/mac/ScrollAnimatorMac.h"
#endif
#include "platform/scroll/ScrollAnimator.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCompositorSupport.h"
#include "public/platform/WebLayerPositionConstraint.h"
#include "public/platform/WebScrollbarLayer.h"
#include "public/platform/WebScrollbarThemeGeometry.h"
#include "public/platform/WebScrollbarThemePainter.h"
#include "wtf/text/StringBuilder.h"

using blink::WebLayer;
using blink::WebLayerPositionConstraint;
using blink::WebRect;
using blink::WebScrollbarLayer;
using blink::WebVector;

namespace {

WebLayer* toWebLayer(blink::GraphicsLayer* layer)
{
    return layer ? layer->platformLayer() : nullptr;
}

} // namespace

namespace blink {

PassOwnPtrWillBeRawPtr<ScrollingCoordinator> ScrollingCoordinator::create(Page* page)
{
    return adoptPtrWillBeNoop(new ScrollingCoordinator(page));
}

ScrollingCoordinator::ScrollingCoordinator(Page* page)
    : m_page(page)
    , m_scrollGestureRegionIsDirty(false)
    , m_touchEventTargetRectsAreDirty(false)
    , m_shouldScrollOnMainThreadDirty(false)
    , m_wasFrameScrollable(false)
    , m_lastMainThreadScrollingReasons(0)
{
}

ScrollingCoordinator::~ScrollingCoordinator()
{
    ASSERT(!m_page);
}

DEFINE_TRACE(ScrollingCoordinator)
{
    visitor->trace(m_page);
#if ENABLE(OILPAN)
    visitor->trace(m_horizontalScrollbars);
    visitor->trace(m_verticalScrollbars);
#endif
}

void ScrollingCoordinator::setShouldHandleScrollGestureOnMainThreadRegion(const Region& region)
{
    if (!m_page->mainFrame()->isLocalFrame() || !m_page->deprecatedLocalMainFrame()->view())
        return;
    if (WebLayer* scrollLayer = toWebLayer(m_page->deprecatedLocalMainFrame()->view()->layerForScrolling())) {
        Vector<IntRect> rects = region.rects();
        WebVector<WebRect> webRects(rects.size());
        for (size_t i = 0; i < rects.size(); ++i)
            webRects[i] = rects[i];
        scrollLayer->setNonFastScrollableRegion(webRects);
    }
}

void ScrollingCoordinator::notifyLayoutUpdated()
{
    m_scrollGestureRegionIsDirty = true;
    m_touchEventTargetRectsAreDirty = true;
    m_shouldScrollOnMainThreadDirty = true;
}

void ScrollingCoordinator::scrollableAreasDidChange()
{
    ASSERT(m_page);
    if (!m_page->mainFrame()->isLocalFrame() || !m_page->deprecatedLocalMainFrame()->view())
        return;

    // Layout may update scrollable area bounding boxes. It also sets the same dirty
    // flag making this one redundant (See |ScrollingCoordinator::notifyLayoutUpdated|).
    // So if layout is expected, ignore this call allowing scrolling coordinator
    // to be notified post-layout to recompute gesture regions.
    if (m_page->deprecatedLocalMainFrame()->view()->needsLayout())
        return;

    m_scrollGestureRegionIsDirty = true;
}

void ScrollingCoordinator::updateAfterCompositingChangeIfNeeded()
{
    if (!m_page->mainFrame()->isLocalFrame())
        return;

    if (!shouldUpdateAfterCompositingChange())
        return;

    TRACE_EVENT0("input", "ScrollingCoordinator::updateAfterCompositingChangeIfNeeded");

    if (m_scrollGestureRegionIsDirty) {
        // Compute the region of the page where we can't handle scroll gestures and mousewheel events
        // on the impl thread. This currently includes:
        // 1. All scrollable areas, such as subframes, overflow divs and list boxes, whose composited
        // scrolling are not enabled. We need to do this even if the frame view whose layout was updated
        // is not the main frame.
        // 2. Resize control areas, e.g. the small rect at the right bottom of div/textarea/iframe when
        // CSS property "resize" is enabled.
        // 3. Plugin areas.
        Region shouldHandleScrollGestureOnMainThreadRegion = computeShouldHandleScrollGestureOnMainThreadRegion(m_page->deprecatedLocalMainFrame(), IntPoint());
        setShouldHandleScrollGestureOnMainThreadRegion(shouldHandleScrollGestureOnMainThreadRegion);
        m_scrollGestureRegionIsDirty = false;
    }

    if (m_touchEventTargetRectsAreDirty) {
        updateTouchEventTargetRectsIfNeeded();
        m_touchEventTargetRectsAreDirty = false;
    }

    FrameView* frameView = m_page->deprecatedLocalMainFrame()->view();
    bool frameIsScrollable = frameView && frameView->isScrollable();
    if (m_shouldScrollOnMainThreadDirty || m_wasFrameScrollable != frameIsScrollable) {
        setShouldUpdateScrollLayerPositionOnMainThread(mainThreadScrollingReasons());
        m_shouldScrollOnMainThreadDirty = false;
    }
    m_wasFrameScrollable = frameIsScrollable;

    if (WebLayer* scrollingWebLayer = frameView ? toWebLayer(frameView->layerForScrolling()) : nullptr) {
        scrollingWebLayer->setBounds(frameView->contentsSize());

        // If there is a non-root fullscreen element, prevent the viewport from
        // scrolling.
        Document* mainFrameDocument = m_page->deprecatedLocalMainFrame()->document();
        Element* fullscreenElement = Fullscreen::fullscreenElementFrom(*mainFrameDocument);
        WebLayer* pinchViewportScrollLayer = toWebLayer(m_page->frameHost().pinchViewport().scrollLayer());

        if (pinchViewportScrollLayer) {
            if (fullscreenElement && fullscreenElement != mainFrameDocument->documentElement())
                toWebLayer(m_page->frameHost().pinchViewport().scrollLayer())->setUserScrollable(false, false);
            else
                toWebLayer(m_page->frameHost().pinchViewport().scrollLayer())->setUserScrollable(true, true);
        }

        scrollingWebLayer->setUserScrollable(frameView->userInputScrollable(HorizontalScrollbar), frameView->userInputScrollable(VerticalScrollbar));
    }

    const FrameTree& tree = m_page->mainFrame()->tree();
    for (const Frame* child = tree.firstChild(); child; child = child->tree().nextSibling()) {
        if (!child->isLocalFrame())
            continue;
        if (WebLayer* scrollLayer = toWebLayer(toLocalFrame(child)->view()->layerForScrolling()))
            scrollLayer->setBounds(toLocalFrame(child)->view()->contentsSize());
    }
}

void ScrollingCoordinator::setLayerIsContainerForFixedPositionLayers(GraphicsLayer* layer, bool enable)
{
    if (WebLayer* scrollableLayer = toWebLayer(layer))
        scrollableLayer->setIsContainerForFixedPositionLayers(enable);
}

static void clearPositionConstraintExceptForLayer(GraphicsLayer* layer, GraphicsLayer* except)
{
    if (layer && layer != except && toWebLayer(layer))
        toWebLayer(layer)->setPositionConstraint(WebLayerPositionConstraint());
}

static WebLayerPositionConstraint computePositionConstraint(const DeprecatedPaintLayer* layer)
{
    ASSERT(layer->hasCompositedDeprecatedPaintLayerMapping());
    do {
        if (layer->layoutObject()->style()->position() == FixedPosition) {
            const LayoutObject* fixedPositionObject = layer->layoutObject();
            bool fixedToRight = !fixedPositionObject->style()->right().isAuto();
            bool fixedToBottom = !fixedPositionObject->style()->bottom().isAuto();
            return WebLayerPositionConstraint::fixedPosition(fixedToRight, fixedToBottom);
        }

        layer = layer->parent();

        // Composited layers that inherit a fixed position state will be positioned with respect to the nearest compositedDeprecatedPaintLayerMapping's GraphicsLayer.
        // So, once we find a layer that has its own compositedDeprecatedPaintLayerMapping, we can stop searching for a fixed position LayoutObject.
    } while (layer && !layer->hasCompositedDeprecatedPaintLayerMapping());
    return WebLayerPositionConstraint();
}

void ScrollingCoordinator::updateLayerPositionConstraint(DeprecatedPaintLayer* layer)
{
    ASSERT(layer->hasCompositedDeprecatedPaintLayerMapping());
    CompositedDeprecatedPaintLayerMapping* compositedDeprecatedPaintLayerMapping = layer->compositedDeprecatedPaintLayerMapping();
    GraphicsLayer* mainLayer = compositedDeprecatedPaintLayerMapping->childForSuperlayers();

    // Avoid unnecessary commits
    clearPositionConstraintExceptForLayer(compositedDeprecatedPaintLayerMapping->squashingContainmentLayer(), mainLayer);
    clearPositionConstraintExceptForLayer(compositedDeprecatedPaintLayerMapping->ancestorClippingLayer(), mainLayer);
    clearPositionConstraintExceptForLayer(compositedDeprecatedPaintLayerMapping->mainGraphicsLayer(), mainLayer);

    if (WebLayer* scrollableLayer = toWebLayer(mainLayer))
        scrollableLayer->setPositionConstraint(computePositionConstraint(layer));
}

void ScrollingCoordinator::willDestroyScrollableArea(ScrollableArea* scrollableArea)
{
    removeWebScrollbarLayer(scrollableArea, HorizontalScrollbar);
    removeWebScrollbarLayer(scrollableArea, VerticalScrollbar);
}

void ScrollingCoordinator::removeWebScrollbarLayer(ScrollableArea* scrollableArea, ScrollbarOrientation orientation)
{
    ScrollbarMap& scrollbars = orientation == HorizontalScrollbar ? m_horizontalScrollbars : m_verticalScrollbars;
    if (OwnPtr<WebScrollbarLayer> scrollbarLayer = scrollbars.take(scrollableArea))
        GraphicsLayer::unregisterContentsLayer(scrollbarLayer->layer());
}

static PassOwnPtr<WebScrollbarLayer> createScrollbarLayer(Scrollbar* scrollbar)
{
    ScrollbarTheme* theme = scrollbar->theme();
    WebScrollbarThemePainter painter(theme, scrollbar);
    OwnPtr<WebScrollbarThemeGeometry> geometry(WebScrollbarThemeGeometryNative::create(theme));

    OwnPtr<WebScrollbarLayer> scrollbarLayer = adoptPtr(Platform::current()->compositorSupport()->createScrollbarLayer(WebScrollbarImpl::create(scrollbar), painter, geometry.leakPtr()));
    GraphicsLayer::registerContentsLayer(scrollbarLayer->layer());
    return scrollbarLayer.release();
}

PassOwnPtr<WebScrollbarLayer> ScrollingCoordinator::createSolidColorScrollbarLayer(ScrollbarOrientation orientation, int thumbThickness, int trackStart, bool isLeftSideVerticalScrollbar)
{
    WebScrollbar::Orientation webOrientation = (orientation == HorizontalScrollbar) ? WebScrollbar::Horizontal : WebScrollbar::Vertical;
    OwnPtr<WebScrollbarLayer> scrollbarLayer = adoptPtr(Platform::current()->compositorSupport()->createSolidColorScrollbarLayer(webOrientation, thumbThickness, trackStart, isLeftSideVerticalScrollbar));
    GraphicsLayer::registerContentsLayer(scrollbarLayer->layer());
    return scrollbarLayer.release();
}

static void detachScrollbarLayer(GraphicsLayer* scrollbarGraphicsLayer)
{
    ASSERT(scrollbarGraphicsLayer);

    scrollbarGraphicsLayer->setContentsToPlatformLayer(nullptr);
    scrollbarGraphicsLayer->setDrawsContent(true);
}

static void setupScrollbarLayer(GraphicsLayer* scrollbarGraphicsLayer, WebScrollbarLayer* scrollbarLayer, WebLayer* scrollLayer, WebLayer* containerLayer)
{
    ASSERT(scrollbarGraphicsLayer);
    ASSERT(scrollbarLayer);

    if (!scrollLayer) {
        detachScrollbarLayer(scrollbarGraphicsLayer);
        return;
    }
    scrollbarLayer->setScrollLayer(scrollLayer);
    scrollbarLayer->setClipLayer(containerLayer);
    scrollbarGraphicsLayer->setContentsToPlatformLayer(scrollbarLayer->layer());
    scrollbarGraphicsLayer->setDrawsContent(false);
}

WebScrollbarLayer* ScrollingCoordinator::addWebScrollbarLayer(ScrollableArea* scrollableArea, ScrollbarOrientation orientation, PassOwnPtr<WebScrollbarLayer> scrollbarLayer)
{
    ScrollbarMap& scrollbars = orientation == HorizontalScrollbar ? m_horizontalScrollbars : m_verticalScrollbars;
    return scrollbars.add(scrollableArea, scrollbarLayer).storedValue->value.get();
}

WebScrollbarLayer* ScrollingCoordinator::getWebScrollbarLayer(ScrollableArea* scrollableArea, ScrollbarOrientation orientation)
{
    ScrollbarMap& scrollbars = orientation == HorizontalScrollbar ? m_horizontalScrollbars : m_verticalScrollbars;
    return scrollbars.get(scrollableArea);
}

void ScrollingCoordinator::scrollableAreaScrollbarLayerDidChange(ScrollableArea* scrollableArea, ScrollbarOrientation orientation)
{
// FIXME: Instead of hardcode here, we should make a setting flag.
#if OS(MACOSX)
    static const bool platformSupportsCoordinatedScrollbar = ScrollAnimatorMac::canUseCoordinatedScrollbar();
    static const bool platformSupportsMainFrameOnly = false; // Don't care.
#elif OS(ANDROID)
    static const bool platformSupportsCoordinatedScrollbar = true;
    static const bool platformSupportsMainFrameOnly = false;
#else
    static const bool platformSupportsCoordinatedScrollbar = true;
    static const bool platformSupportsMainFrameOnly = true;
#endif

    bool isMainFrame = isForMainFrame(scrollableArea);
    GraphicsLayer* scrollbarGraphicsLayer = orientation == HorizontalScrollbar
        ? scrollableArea->layerForHorizontalScrollbar()
        : scrollableArea->layerForVerticalScrollbar();

    bool shouldCreateCoordinatedScrollbar = platformSupportsCoordinatedScrollbar && !(platformSupportsMainFrameOnly && !isMainFrame);
    if (!shouldCreateCoordinatedScrollbar) {
        if (scrollbarGraphicsLayer) {
            WebLayer* scrollbarLayer = toWebLayer(scrollbarGraphicsLayer);
            scrollbarLayer->setShouldScrollOnMainThread(true);
        }
        return;
    }

    if (scrollbarGraphicsLayer) {
        Scrollbar* scrollbar = orientation == HorizontalScrollbar ? scrollableArea->horizontalScrollbar() : scrollableArea->verticalScrollbar();
        if (scrollbar->isCustomScrollbar()) {
            detachScrollbarLayer(scrollbarGraphicsLayer);
            return;
        }

        WebScrollbarLayer* scrollbarLayer = getWebScrollbarLayer(scrollableArea, orientation);
        if (!scrollbarLayer) {
            Settings* settings = m_page->mainFrame()->settings();

            OwnPtr<WebScrollbarLayer> webScrollbarLayer;
            if (settings->useSolidColorScrollbars()) {
                ASSERT(RuntimeEnabledFeatures::overlayScrollbarsEnabled());
                webScrollbarLayer = createSolidColorScrollbarLayer(orientation, scrollbar->theme()->thumbThickness(scrollbar), scrollbar->theme()->trackPosition(scrollbar), scrollableArea->shouldPlaceVerticalScrollbarOnLeft());
            } else {
                webScrollbarLayer = createScrollbarLayer(scrollbar);
            }
            scrollbarLayer = addWebScrollbarLayer(scrollableArea, orientation, webScrollbarLayer.release());
        }

        WebLayer* scrollLayer = toWebLayer(scrollableArea->layerForScrolling());
        WebLayer* containerLayer = toWebLayer(scrollableArea->layerForContainer());
        setupScrollbarLayer(scrollbarGraphicsLayer, scrollbarLayer, scrollLayer, containerLayer);

        // Root layer non-overlay scrollbars should be marked opaque to disable
        // blending.
        bool isOpaqueScrollbar = !scrollbar->isOverlayScrollbar();
        scrollbarGraphicsLayer->setContentsOpaque(isMainFrame && isOpaqueScrollbar);
    } else
        removeWebScrollbarLayer(scrollableArea, orientation);
}

bool ScrollingCoordinator::scrollableAreaScrollLayerDidChange(ScrollableArea* scrollableArea)
{
    GraphicsLayer* scrollLayer = scrollableArea->layerForScrolling();

    if (scrollLayer) {
        ASSERT(m_page);
        scrollLayer->setScrollableArea(scrollableArea, isForViewport(scrollableArea));
    }

    WebLayer* webLayer = toWebLayer(scrollableArea->layerForScrolling());
    WebLayer* containerLayer = toWebLayer(scrollableArea->layerForContainer());
    if (webLayer) {
        webLayer->setScrollClipLayer(containerLayer);
        // Non-layered Viewport constrained objects, e.g. fixed position elements, are
        // positioned in Blink using integer coordinates. In that case, we don't want
        // to set the WebLayer's scroll position at fractional precision otherwise the
        // WebLayer's position after snapping to device pixel can be off with regard to
        // fixed position elements.
        if (m_lastMainThreadScrollingReasons & ScrollingCoordinator::HasNonLayerViewportConstrainedObjects) {
            webLayer->setScrollPositionDouble(DoublePoint(scrollableArea->scrollPosition() - scrollableArea->minimumScrollPosition()));
        } else {
            DoublePoint scrollPosition(scrollableArea->scrollPositionDouble() - scrollableArea->minimumScrollPositionDouble());
            IntPoint flooredScrollPosition(flooredIntPoint(scrollPosition));
            DoublePoint fractionalPart(scrollPosition.x() - flooredScrollPosition.x(), scrollPosition.y() - flooredScrollPosition.y());
            webLayer->setScrollPositionDouble(scrollPosition);
            // Blink can only use the integer part of the scroll offset to position elements.
            // Sends the fractional part of the scroll offset to CC as scroll adjustment for
            // fixed-position layer.
            webLayer->setScrollCompensationAdjustment(fractionalPart);
        }

        webLayer->setBounds(scrollableArea->contentsSize());
        bool canScrollX = scrollableArea->userInputScrollable(HorizontalScrollbar);
        bool canScrollY = scrollableArea->userInputScrollable(VerticalScrollbar);
        webLayer->setUserScrollable(canScrollX, canScrollY);
    }
    if (WebScrollbarLayer* scrollbarLayer = getWebScrollbarLayer(scrollableArea, HorizontalScrollbar)) {
        GraphicsLayer* horizontalScrollbarLayer = scrollableArea->layerForHorizontalScrollbar();
        if (horizontalScrollbarLayer)
            setupScrollbarLayer(horizontalScrollbarLayer, scrollbarLayer, webLayer, containerLayer);
    }
    if (WebScrollbarLayer* scrollbarLayer = getWebScrollbarLayer(scrollableArea, VerticalScrollbar)) {
        GraphicsLayer* verticalScrollbarLayer = scrollableArea->layerForVerticalScrollbar();
        if (verticalScrollbarLayer)
            setupScrollbarLayer(verticalScrollbarLayer, scrollbarLayer, webLayer, containerLayer);
    }

    // Update the viewport layer registration if the outer viewport may have changed.
    if (m_page->settings().rootLayerScrolls() && isForRootLayer(scrollableArea))
        m_page->chromeClient().registerViewportLayers();

    scrollableArea->layerForScrollingDidChange();

    return !!webLayer;
}

using GraphicsLayerHitTestRects = WTF::HashMap<const GraphicsLayer*, Vector<LayoutRect>>;

// In order to do a DFS cross-frame walk of the Layer tree, we need to know which
// Layers have child frames inside of them. This computes a mapping for the
// current frame which we can consult while walking the layers of that frame.
// Whenever we descend into a new frame, a new map will be created.
using LayerFrameMap = HashMap<const DeprecatedPaintLayer*, Vector<const LocalFrame*>>;
static void makeLayerChildFrameMap(const LocalFrame* currentFrame, LayerFrameMap* map)
{
    map->clear();
    const FrameTree& tree = currentFrame->tree();
    for (const Frame* child = tree.firstChild(); child; child = child->tree().nextSibling()) {
        if (!child->isLocalFrame())
            continue;
        const LayoutObject* ownerLayoutObject = toLocalFrame(child)->ownerLayoutObject();
        if (!ownerLayoutObject)
            continue;
        const DeprecatedPaintLayer* containingLayer = ownerLayoutObject->enclosingLayer();
        LayerFrameMap::iterator iter = map->find(containingLayer);
        if (iter == map->end())
            map->add(containingLayer, Vector<const LocalFrame*>()).storedValue->value.append(toLocalFrame(child));
        else
            iter->value.append(toLocalFrame(child));
    }
}

static void projectRectsToGraphicsLayerSpaceRecursive(
    const DeprecatedPaintLayer* curLayer,
    const LayerHitTestRects& layerRects,
    GraphicsLayerHitTestRects& graphicsRects,
    LayoutGeometryMap& geometryMap,
    HashSet<const DeprecatedPaintLayer*>& layersWithRects,
    LayerFrameMap& layerChildFrameMap)
{
    // Project any rects for the current layer
    LayerHitTestRects::const_iterator layerIter = layerRects.find(curLayer);
    if (layerIter != layerRects.end()) {
        // Find the enclosing composited layer when it's in another document (for non-composited iframes).
        const DeprecatedPaintLayer* compositedLayer = layerIter->key->enclosingLayerForPaintInvalidationCrossingFrameBoundaries();
        ASSERT(compositedLayer);

        // Find the appropriate GraphicsLayer for the composited Layer.
        GraphicsLayer* graphicsLayer = compositedLayer->graphicsLayerBackingForScrolling();

        GraphicsLayerHitTestRects::iterator glIter = graphicsRects.find(graphicsLayer);
        Vector<LayoutRect>* glRects;
        if (glIter == graphicsRects.end())
            glRects = &graphicsRects.add(graphicsLayer, Vector<LayoutRect>()).storedValue->value;
        else
            glRects = &glIter->value;

        // Transform each rect to the co-ordinate space of the graphicsLayer.
        for (size_t i = 0; i < layerIter->value.size(); ++i) {
            LayoutRect rect = layerIter->value[i];
            if (compositedLayer != curLayer) {
                FloatQuad compositorQuad = geometryMap.mapToContainer(rect, compositedLayer->layoutObject());
                rect = LayoutRect(compositorQuad.boundingBox());
                // If the enclosing composited layer itself is scrolled, we have to undo the subtraction
                // of its scroll offset since we want the offset relative to the scrolling content, not
                // the element itself.
                if (compositedLayer->layoutObject()->hasOverflowClip())
                    rect.move(compositedLayer->layoutBox()->scrolledContentOffset());
            }
            DeprecatedPaintLayer::mapRectToPaintBackingCoordinates(compositedLayer->layoutObject(), rect);
            glRects->append(rect);
        }
    }

    // Walk child layers of interest
    for (const DeprecatedPaintLayer* childLayer = curLayer->firstChild(); childLayer; childLayer = childLayer->nextSibling()) {
        if (layersWithRects.contains(childLayer)) {
            geometryMap.pushMappingsToAncestor(childLayer, curLayer);
            projectRectsToGraphicsLayerSpaceRecursive(childLayer, layerRects, graphicsRects, geometryMap, layersWithRects, layerChildFrameMap);
            geometryMap.popMappingsToAncestor(curLayer);
        }
    }

    // If this layer has any frames of interest as a child of it, walk those (with an updated frame map).
    LayerFrameMap::iterator mapIter = layerChildFrameMap.find(curLayer);
    if (mapIter != layerChildFrameMap.end()) {
        for (size_t i = 0; i < mapIter->value.size(); i++) {
            const LocalFrame* childFrame = mapIter->value[i];
            const DeprecatedPaintLayer* childLayer = childFrame->view()->layoutView()->layer();
            if (layersWithRects.contains(childLayer)) {
                LayerFrameMap newLayerChildFrameMap;
                makeLayerChildFrameMap(childFrame, &newLayerChildFrameMap);
                geometryMap.pushMappingsToAncestor(childLayer, curLayer);
                projectRectsToGraphicsLayerSpaceRecursive(childLayer, layerRects, graphicsRects, geometryMap, layersWithRects, newLayerChildFrameMap);
                geometryMap.popMappingsToAncestor(curLayer);
            }
        }
    }
}

static void projectRectsToGraphicsLayerSpace(LocalFrame* mainFrame, const LayerHitTestRects& layerRects, GraphicsLayerHitTestRects& graphicsRects)
{
    TRACE_EVENT0("input", "ScrollingCoordinator::projectRectsToGraphicsLayerSpace");
    bool touchHandlerInChildFrame = false;

    // We have a set of rects per Layer, we need to map them to their bounding boxes in their
    // enclosing composited layer. To do this most efficiently we'll walk the Layer tree using
    // LayoutGeometryMap. First record all the branches we should traverse in the tree (including
    // all documents on the page).
    HashSet<const DeprecatedPaintLayer*> layersWithRects;
    for (const auto& layerRect : layerRects) {
        const DeprecatedPaintLayer* layer = layerRect.key;
        do {
            if (!layersWithRects.add(layer).isNewEntry)
                break;

            if (layer->parent()) {
                layer = layer->parent();
            } else if (LayoutObject* parentDocLayoutObject = layer->layoutObject()->frame()->ownerLayoutObject()) {
                layer = parentDocLayoutObject->enclosingLayer();
                touchHandlerInChildFrame = true;
            }
        } while (layer);
    }

    // Now walk the layer projecting rects while maintaining a LayoutGeometryMap
    MapCoordinatesFlags flags = UseTransforms;
    if (touchHandlerInChildFrame)
        flags |= TraverseDocumentBoundaries;
    DeprecatedPaintLayer* rootLayer = mainFrame->contentLayoutObject()->layer();
    LayoutGeometryMap geometryMap(flags);
    geometryMap.pushMappingsToAncestor(rootLayer, 0);
    LayerFrameMap layerChildFrameMap;
    makeLayerChildFrameMap(mainFrame, &layerChildFrameMap);
    projectRectsToGraphicsLayerSpaceRecursive(rootLayer, layerRects, graphicsRects, geometryMap, layersWithRects, layerChildFrameMap);
}

void ScrollingCoordinator::updateTouchEventTargetRectsIfNeeded()
{
    TRACE_EVENT0("input", "ScrollingCoordinator::updateTouchEventTargetRectsIfNeeded");

    if (!RuntimeEnabledFeatures::touchEnabled())
        return;

    LayerHitTestRects touchEventTargetRects;
    computeTouchEventTargetRects(touchEventTargetRects);
    setTouchEventTargetRects(touchEventTargetRects);
}

void ScrollingCoordinator::reset()
{
    for (const auto& scrollbar : m_horizontalScrollbars)
        GraphicsLayer::unregisterContentsLayer(scrollbar.value->layer());
    for (const auto& scrollbar : m_verticalScrollbars)
        GraphicsLayer::unregisterContentsLayer(scrollbar.value->layer());

    m_horizontalScrollbars.clear();
    m_verticalScrollbars.clear();
    m_layersWithTouchRects.clear();
    m_wasFrameScrollable = false;

    m_lastMainThreadScrollingReasons = 0;
    setShouldUpdateScrollLayerPositionOnMainThread(m_lastMainThreadScrollingReasons);
}

// Note that in principle this could be called more often than computeTouchEventTargetRects, for
// example during a non-composited scroll (although that's not yet implemented - crbug.com/261307).
void ScrollingCoordinator::setTouchEventTargetRects(LayerHitTestRects& layerRects)
{
    TRACE_EVENT0("input", "ScrollingCoordinator::setTouchEventTargetRects");

    // Update the list of layers with touch hit rects.
    HashSet<const DeprecatedPaintLayer*> oldLayersWithTouchRects;
    m_layersWithTouchRects.swap(oldLayersWithTouchRects);
    for (const auto& layerRect : layerRects) {
        if (!layerRect.value.isEmpty()) {
            const DeprecatedPaintLayer* compositedLayer = layerRect.key->enclosingLayerForPaintInvalidationCrossingFrameBoundaries();
            if (!compositedLayer) // weolar: blinkµÄbug
                continue;
            
            ASSERT(compositedLayer);
            m_layersWithTouchRects.add(compositedLayer);
        }
    }

    // Ensure we have an entry for each composited layer that previously had rects (so that old
    // ones will get cleared out). Note that ideally we'd track this on GraphicsLayer instead of
    // Layer, but we have no good hook into the lifetime of a GraphicsLayer.
    for (const DeprecatedPaintLayer* layer : oldLayersWithTouchRects) {
        if (!layerRects.contains(layer))
            layerRects.add(layer, Vector<LayoutRect>());
    }

    GraphicsLayerHitTestRects graphicsLayerRects;
    projectRectsToGraphicsLayerSpace(m_page->deprecatedLocalMainFrame(), layerRects, graphicsLayerRects);

    for (const auto& layerRect : graphicsLayerRects) {
        const GraphicsLayer* graphicsLayer = layerRect.key;
        WebVector<WebRect> webRects(layerRect.value.size());
        for (size_t i = 0; i < layerRect.value.size(); ++i)
            webRects[i] = enclosingIntRect(layerRect.value[i]);
        graphicsLayer->platformLayer()->setTouchEventHandlerRegion(webRects);
    }
}

void ScrollingCoordinator::touchEventTargetRectsDidChange()
{
    if (!RuntimeEnabledFeatures::touchEnabled())
        return;

    ASSERT(m_page);
    if (!m_page->mainFrame()->isLocalFrame() || !m_page->deprecatedLocalMainFrame()->view())
        return;

    // Wait until after layout to update.
    if (m_page->deprecatedLocalMainFrame()->view()->needsLayout())
        return;

    // FIXME: scheduleAnimation() is just a method of forcing the compositor to realize that it
    // needs to commit here. We should expose a cleaner API for this.
    LayoutView* layoutView = m_page->deprecatedLocalMainFrame()->contentLayoutObject();
    if (layoutView && layoutView->compositor() && layoutView->compositor()->staleInCompositingMode())
        m_page->deprecatedLocalMainFrame()->view()->scheduleAnimation();

    m_touchEventTargetRectsAreDirty = true;
}

void ScrollingCoordinator::updateScrollParentForGraphicsLayer(GraphicsLayer* child, DeprecatedPaintLayer* parent)
{
    WebLayer* scrollParentWebLayer = nullptr;
    if (parent && parent->hasCompositedDeprecatedPaintLayerMapping())
        scrollParentWebLayer = toWebLayer(parent->compositedDeprecatedPaintLayerMapping()->scrollingContentsLayer());

    child->setScrollParent(scrollParentWebLayer);
}

void ScrollingCoordinator::updateClipParentForGraphicsLayer(GraphicsLayer* child, DeprecatedPaintLayer* parent)
{
    WebLayer* clipParentWebLayer = nullptr;
    if (parent && parent->hasCompositedDeprecatedPaintLayerMapping())
        clipParentWebLayer = toWebLayer(parent->compositedDeprecatedPaintLayerMapping()->parentForSublayers());

    child->setClipParent(clipParentWebLayer);
}

void ScrollingCoordinator::willDestroyLayer(DeprecatedPaintLayer* layer)
{
    m_layersWithTouchRects.remove(layer);
}

void ScrollingCoordinator::updateHaveWheelEventHandlers()
{
    ASSERT(isMainThread());
    ASSERT(m_page);
    if (!m_page->mainFrame()->isLocalFrame() || !m_page->deprecatedLocalMainFrame()->view())
        return;

    if (WebLayer* scrollLayer = toWebLayer(m_page->deprecatedLocalMainFrame()->view()->layerForScrolling())) {
        bool haveHandlers = m_page->frameHost().eventHandlerRegistry().hasEventHandlers(EventHandlerRegistry::WheelEvent);
        scrollLayer->setHaveWheelEventHandlers(haveHandlers);
    }
}

void ScrollingCoordinator::updateHaveScrollEventHandlers()
{
    ASSERT(isMainThread());
    ASSERT(m_page);
    if (!m_page->mainFrame()->isLocalFrame() || !m_page->deprecatedLocalMainFrame()->view())
        return;

    // Currently the compositor only cares whether there are scroll handlers anywhere on the page
    // instead on a per-layer basis. We therefore only update this information for the root
    // scrolling layer.
    if (WebLayer* scrollLayer = toWebLayer(m_page->deprecatedLocalMainFrame()->view()->layerForScrolling())) {
        bool haveHandlers = m_page->frameHost().eventHandlerRegistry().hasEventHandlers(EventHandlerRegistry::ScrollEvent);
        scrollLayer->setHaveScrollEventHandlers(haveHandlers);
    }
}

void ScrollingCoordinator::setShouldUpdateScrollLayerPositionOnMainThread(MainThreadScrollingReasons reasons)
{
    if (!m_page->mainFrame()->isLocalFrame() || !m_page->deprecatedLocalMainFrame()->view())
        return;
    if (WebLayer* scrollLayer = toWebLayer(m_page->deprecatedLocalMainFrame()->view()->layerForScrolling())) {
        m_lastMainThreadScrollingReasons = reasons;
        scrollLayer->setShouldScrollOnMainThread(reasons);
    }
}

void ScrollingCoordinator::willBeDestroyed()
{
    ASSERT(m_page);
    m_page = nullptr;
    for (const auto& scrollbar : m_horizontalScrollbars)
        GraphicsLayer::unregisterContentsLayer(scrollbar.value->layer());
    for (const auto& scrollbar : m_verticalScrollbars)
        GraphicsLayer::unregisterContentsLayer(scrollbar.value->layer());
}

bool ScrollingCoordinator::coordinatesScrollingForFrameView(FrameView* frameView) const
{
    ASSERT(isMainThread());
    ASSERT(m_page);

    // We currently only handle the main frame.
    if (&frameView->frame() != m_page->mainFrame())
        return false;

    if (!m_page->mainFrame()->isLocalFrame())
        return false;

    // We currently only support composited mode.
    LayoutView* layoutView = m_page->deprecatedLocalMainFrame()->contentLayoutObject();
    if (!layoutView)
        return false;
    return layoutView->usesCompositing();
}

Region ScrollingCoordinator::computeShouldHandleScrollGestureOnMainThreadRegion(const LocalFrame* frame, const IntPoint& frameLocation) const
{
    Region shouldHandleScrollGestureOnMainThreadRegion;
    FrameView* frameView = frame->view();
    if (!frameView)
        return shouldHandleScrollGestureOnMainThreadRegion;

    IntPoint offset = frameLocation;
    offset.moveBy(frameView->frameRect().location());

    if (const FrameView::ScrollableAreaSet* scrollableAreas = frameView->scrollableAreas()) {
        for (const ScrollableArea* scrollableArea : *scrollableAreas) {
            // Composited scrollable areas can be scrolled off the main thread.
            if (scrollableArea->usesCompositedScrolling())
                continue;
            IntRect box = scrollableArea->scrollableAreaBoundingBox();
            box.moveBy(offset);
            shouldHandleScrollGestureOnMainThreadRegion.unite(box);
        }
    }

    // We use GestureScrollBegin/Update/End for moving the resizer handle. So we mark these
    // small resizer areas as non-fast-scrollable to allow the scroll gestures to be passed to
    // main thread if they are targeting the resizer area. (Resizing is done in EventHandler.cpp
    // on main thread).
    if (const FrameView::ResizerAreaSet* resizerAreas = frameView->resizerAreas()) {
        for (const LayoutBox* box : *resizerAreas) {
            IntRect bounds = box->absoluteBoundingBoxRect();
            IntRect corner = box->layer()->scrollableArea()->touchResizerCornerRect(bounds);
            corner.moveBy(offset);
            shouldHandleScrollGestureOnMainThreadRegion.unite(corner);
        }
    }

    if (const FrameView::ChildrenWidgetSet* children = frameView->children()) {
        for (const RefPtrWillBeMember<Widget>& child : *children) {
            if (!(*child).isPluginView())
                continue;

            PluginView* pluginView = toPluginView(child.get());
            if (pluginView->wantsWheelEvents()) {
                IntRect box = pluginView->frameRect();
                box.moveBy(offset);
                shouldHandleScrollGestureOnMainThreadRegion.unite(box);
            }
        }
    }

    const FrameTree& tree = frame->tree();
    for (Frame* subFrame = tree.firstChild(); subFrame; subFrame = subFrame->tree().nextSibling()) {
        if (subFrame->isLocalFrame())
            shouldHandleScrollGestureOnMainThreadRegion.unite(computeShouldHandleScrollGestureOnMainThreadRegion(toLocalFrame(subFrame), offset));
    }

    return shouldHandleScrollGestureOnMainThreadRegion;
}

static void accumulateDocumentTouchEventTargetRects(LayerHitTestRects& rects, const Document* document)
{
    ASSERT(document);
    const EventTargetSet* targets = document->frameHost()->eventHandlerRegistry().eventHandlerTargets(EventHandlerRegistry::TouchEvent);
    if (!targets)
        return;

    // If there's a handler on the window, document, html or body element (fairly common in practice),
    // then we can quickly mark the entire document and skip looking at any other handlers.
    // Note that technically a handler on the body doesn't cover the whole document, but it's
    // reasonable to be conservative and report the whole document anyway.
    //
    // Fullscreen HTML5 video when OverlayFullscreenVideo is enabled is implemented by replacing the
    // root cc::layer with the video layer so doing this optimization causes the compositor to think
    // that there are no handlers, therefore skip it.
    if (!document->layoutView()->compositor()->inOverlayFullscreenVideo()) {
        for (const auto& eventTarget : *targets) {
            EventTarget* target = eventTarget.key;
            Node* node = target->toNode();
            if (target->toDOMWindow() || node == document || node == document->documentElement() || node == document->body()) {
                if (LayoutView* layoutView = document->layoutView()) {
                    layoutView->computeLayerHitTestRects(rects);
                }
                return;
            }
        }
    }

    for (const auto& eventTarget : *targets) {
        EventTarget* target = eventTarget.key;
        Node* node = target->toNode();
        if (!node || !node->inDocument())
            continue;

        // If the document belongs to an invisible subframe it does not have a composited layer
        // and should be skipped.
        if (node->document().isInInvisibleSubframe())
            continue;

        if (node->isDocumentNode() && node != document) {
            accumulateDocumentTouchEventTargetRects(rects, toDocument(node));
        } else if (LayoutObject* layoutObject = node->layoutObject()) {
            // If the set also contains one of our ancestor nodes then processing
            // this node would be redundant.
            bool hasTouchEventTargetAncestor = false;
            for (Node* ancestor = node->parentNode(); ancestor && !hasTouchEventTargetAncestor; ancestor = ancestor->parentNode()) {
                if (targets->contains(ancestor))
                    hasTouchEventTargetAncestor = true;
            }
            if (!hasTouchEventTargetAncestor) {
                // Walk up the tree to the outermost non-composited scrollable layer.
                DeprecatedPaintLayer* enclosingNonCompositedScrollLayer = nullptr;
                for (DeprecatedPaintLayer* parent = layoutObject->enclosingLayer(); parent && parent->compositingState() == NotComposited; parent = parent->parent()) {
                    if (parent->scrollsOverflow())
                        enclosingNonCompositedScrollLayer = parent;
                }

                // Report the whole non-composited scroll layer as a touch hit rect because any
                // rects inside of it may move around relative to their enclosing composited layer
                // without causing the rects to be recomputed. Non-composited scrolling occurs on
                // the main thread, so we're not getting much benefit from compositor touch hit
                // testing in this case anyway.
                if (enclosingNonCompositedScrollLayer)
                    enclosingNonCompositedScrollLayer->computeSelfHitTestRects(rects);

                layoutObject->computeLayerHitTestRects(rects);
            }
        }
    }
}

void ScrollingCoordinator::computeTouchEventTargetRects(LayerHitTestRects& rects)
{
    TRACE_EVENT0("input", "ScrollingCoordinator::computeTouchEventTargetRects");
    ASSERT(RuntimeEnabledFeatures::touchEnabled());

    Document* document = m_page->deprecatedLocalMainFrame()->document();
    if (!document || !document->view())
        return;

    accumulateDocumentTouchEventTargetRects(rects, document);
}

void ScrollingCoordinator::frameViewHasSlowRepaintObjectsDidChange(FrameView* frameView)
{
    ASSERT(isMainThread());
    ASSERT(m_page);

    if (!coordinatesScrollingForFrameView(frameView))
        return;

    m_shouldScrollOnMainThreadDirty = true;
}

void ScrollingCoordinator::frameViewFixedObjectsDidChange(FrameView* frameView)
{
    ASSERT(isMainThread());
    ASSERT(m_page);

    if (!coordinatesScrollingForFrameView(frameView))
        return;

    m_shouldScrollOnMainThreadDirty = true;
}

bool ScrollingCoordinator::isForRootLayer(ScrollableArea* scrollableArea) const
{
    if (!m_page->mainFrame()->isLocalFrame())
        return false;

    // FIXME(305811): Refactor for OOPI.
    LayoutView* layoutView = m_page->deprecatedLocalMainFrame()->view()->layoutView();
    return layoutView ? scrollableArea == layoutView->layer()->scrollableArea() : false;
}

bool ScrollingCoordinator::isForMainFrame(ScrollableArea* scrollableArea) const
{
    if (!m_page->mainFrame()->isLocalFrame())
        return false;

    // FIXME(305811): Refactor for OOPI.
    return scrollableArea == m_page->deprecatedLocalMainFrame()->view();
}

bool ScrollingCoordinator::isForViewport(ScrollableArea* scrollableArea) const
{
    bool isForOuterViewport = m_page->settings().rootLayerScrolls() ?
        isForRootLayer(scrollableArea) :
        isForMainFrame(scrollableArea);

    return isForOuterViewport || scrollableArea == &m_page->frameHost().pinchViewport();
}

void ScrollingCoordinator::frameViewRootLayerDidChange(FrameView* frameView)
{
    ASSERT(isMainThread());
    ASSERT(m_page);

    if (!coordinatesScrollingForFrameView(frameView))
        return;

    notifyLayoutUpdated();
    updateHaveWheelEventHandlers();
    updateHaveScrollEventHandlers();
}

#if OS(MACOSX)
void ScrollingCoordinator::handleWheelEventPhase(PlatformWheelEventPhase phase)
{
    ASSERT(isMainThread());

    if (!m_page)
        return;

    FrameView* frameView = m_page->deprecatedLocalMainFrame()->view();
    if (!frameView)
        return;

    frameView->scrollAnimator()->handleWheelEventPhase(phase);
}
#endif

bool ScrollingCoordinator::hasVisibleSlowRepaintViewportConstrainedObjects(FrameView* frameView) const
{
    const FrameView::ViewportConstrainedObjectSet* viewportConstrainedObjects = frameView->viewportConstrainedObjects();
    if (!viewportConstrainedObjects)
        return false;

    for (const LayoutObject* layoutObject : *viewportConstrainedObjects) {
        ASSERT(layoutObject->isBoxModelObject() && layoutObject->hasLayer());
        ASSERT(layoutObject->style()->position() == FixedPosition);
        DeprecatedPaintLayer* layer = toLayoutBoxModelObject(layoutObject)->layer();

        // Whether the Layer scrolls with the viewport is a tree-depenent
        // property and our viewportConstrainedObjects collection is maintained
        // with only LayoutObject-level information.
        if (!layer->scrollsWithViewport())
            continue;

        // If the whole subtree is invisible, there's no reason to scroll on
        // the main thread because we don't need to generate invalidations
        // for invisible content.
        if (layer->subtreeIsInvisible())
            continue;

        // We're only smart enough to scroll viewport-constrainted objects
        // in the compositor if they have their own backing or they paint
        // into a grouped back (which necessarily all have the same viewport
        // constraints).
        CompositingState compositingState = layer->compositingState();
        if (compositingState != PaintsIntoOwnBacking && compositingState != PaintsIntoGroupedBacking)
            return true;
    }
    return false;
}

MainThreadScrollingReasons ScrollingCoordinator::mainThreadScrollingReasons() const
{
    MainThreadScrollingReasons reasons = static_cast<MainThreadScrollingReasons>(0);

    if (!m_page->settings().threadedScrollingEnabled())
        reasons |= ThreadedScrollingDisabled;

    if (!m_page->mainFrame()->isLocalFrame())
        return reasons;
    FrameView* frameView = m_page->deprecatedLocalMainFrame()->view();
    if (!frameView)
        return reasons;

    if (frameView->hasSlowRepaintObjects())
        reasons |= HasSlowRepaintObjects;
    FrameView::ScrollingReasons scrollingReasons = frameView->scrollingReasons();
    const bool mayBeScrolledByInput = (scrollingReasons == FrameView::Scrollable);
    const bool mayBeScrolledByScript = mayBeScrolledByInput || (scrollingReasons ==
        FrameView::NotScrollableExplicitlyDisabled);

    // TODO(awoloszyn) Currently crbug.com/304810 will let certain
    // overflow:hidden elements scroll on the compositor thread, so we should
    // not let this move there path as an optimization, when we have slow-repaint
    // elements.
    if (mayBeScrolledByScript && hasVisibleSlowRepaintViewportConstrainedObjects(frameView)) {
        reasons |= HasNonLayerViewportConstrainedObjects;
    }

    return reasons;
}

String ScrollingCoordinator::mainThreadScrollingReasonsAsText(MainThreadScrollingReasons reasons)
{
    StringBuilder stringBuilder;

    if (reasons & ScrollingCoordinator::HasSlowRepaintObjects)
        stringBuilder.appendLiteral("Has slow repaint objects, ");
    if (reasons & ScrollingCoordinator::HasNonLayerViewportConstrainedObjects)
        stringBuilder.appendLiteral("Has non-layer viewport-constrained objects, ");
    if (reasons & ScrollingCoordinator::ThreadedScrollingDisabled)
        stringBuilder.appendLiteral("Threaded scrolling is disabled, ");

    if (stringBuilder.length())
        stringBuilder.resize(stringBuilder.length() - 2);
    return stringBuilder.toString();
}

String ScrollingCoordinator::mainThreadScrollingReasonsAsText() const
{
    ASSERT(m_page->deprecatedLocalMainFrame()->document()->lifecycle().state() >= DocumentLifecycle::CompositingClean);
    return mainThreadScrollingReasonsAsText(m_lastMainThreadScrollingReasons);
}

bool ScrollingCoordinator::frameViewIsDirty() const
{
    FrameView* frameView = m_page->mainFrame()->isLocalFrame() ? m_page->deprecatedLocalMainFrame()->view() : nullptr;
    bool frameIsScrollable = frameView && frameView->isScrollable();
    if (frameIsScrollable != m_wasFrameScrollable)
        return true;

    if (WebLayer* scrollLayer = frameView ? toWebLayer(frameView->layerForScrolling()) : nullptr)
        return WebSize(frameView->contentsSize()) != scrollLayer->bounds();
    return false;
}

} // namespace blink
