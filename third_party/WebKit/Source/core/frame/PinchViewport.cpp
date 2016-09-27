/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#include "config.h"
#include "core/frame/PinchViewport.h"

#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/layout/LayoutView.h"
#include "core/layout/TextAutosizer.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "platform/TraceEvent.h"
#include "platform/geometry/DoubleRect.h"
#include "platform/geometry/FloatSize.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/GraphicsLayerFactory.h"
#include "platform/scroll/Scrollbar.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCompositorSupport.h"
#include "public/platform/WebLayer.h"
#include "public/platform/WebLayerTreeView.h"
#include "public/platform/WebScrollbar.h"
#include "public/platform/WebScrollbarLayer.h"

using blink::WebLayer;
using blink::WebLayerTreeView;
using blink::WebScrollbar;
using blink::WebScrollbarLayer;
using blink::FrameHost;
using blink::GraphicsLayer;
using blink::GraphicsLayerFactory;

namespace blink {

PinchViewport::PinchViewport(FrameHost& owner)
    : m_frameHost(&owner)
    , m_scale(1)
    , m_topControlsAdjustment(0)
{
    reset();
}

PinchViewport::~PinchViewport()
{
}

DEFINE_TRACE(PinchViewport)
{
    visitor->trace(m_frameHost);
    ScrollableArea::trace(visitor);
}

void PinchViewport::setSize(const IntSize& size)
{
    // When the main frame is remote, we won't have an associated frame.
    if (!mainFrame())
        return;

    if (m_size == size)
        return;

    bool autosizerNeedsUpdating =
        (size.width() != m_size.width())
        && mainFrame()->settings()
        && mainFrame()->settings()->textAutosizingEnabled();

    TRACE_EVENT2("blink", "PinchViewport::setSize", "width", size.width(), "height", size.height());
    m_size = size;

    if (m_innerViewportContainerLayer) {
        m_innerViewportContainerLayer->setSize(m_size);

        // Need to re-compute sizes for the overlay scrollbars.
        setupScrollbar(WebScrollbar::Horizontal);
        setupScrollbar(WebScrollbar::Vertical);
    }

    if (autosizerNeedsUpdating) {
        // This needs to happen after setting the m_size member since it'll be read in the update call.
        if (TextAutosizer* textAutosizer = mainFrame()->document()->textAutosizer())
            textAutosizer->updatePageInfoInAllFrames();
    }
}

void PinchViewport::reset()
{
    setScaleAndLocation(1, FloatPoint());
}

void PinchViewport::mainFrameDidChangeSize()
{
    TRACE_EVENT0("blink", "PinchViewport::mainFrameDidChangeSize");

    // In unit tests we may not have initialized the layer tree.
    if (m_innerViewportScrollLayer)
        m_innerViewportScrollLayer->setSize(contentsSize());

    clampToBoundaries();
}

FloatSize PinchViewport::visibleSize() const
{
    FloatSize scaledSize(m_size);
    scaledSize.expand(0, m_topControlsAdjustment);
    scaledSize.scale(1 / m_scale);
    return scaledSize;
}

FloatRect PinchViewport::visibleRect() const
{
    return FloatRect(location(), visibleSize());
}

FloatRect PinchViewport::visibleRectInDocument() const
{
    if (!mainFrame() || !mainFrame()->view())
        return FloatRect();

    FloatPoint viewLocation = FloatPoint(mainFrame()->view()->scrollableArea()->scrollPositionDouble());
    return FloatRect(viewLocation, visibleSize());
}

FloatRect PinchViewport::mainViewToViewportCSSPixels(const FloatRect& rect) const
{
    // Note, this is in CSS Pixels so we don't apply scale.
    FloatRect rectInViewport = rect;
    rectInViewport.moveBy(-location());
    return rectInViewport;
}

FloatPoint PinchViewport::viewportCSSPixelsToRootFrame(const FloatPoint& point) const
{
    // Note, this is in CSS Pixels so we don't apply scale.
    FloatPoint pointInRootFrame = point;
    pointInRootFrame.moveBy(location());
    return pointInRootFrame;
}

void PinchViewport::setLocation(const FloatPoint& newLocation)
{
    setScaleAndLocation(m_scale, newLocation);
}

void PinchViewport::move(const FloatPoint& delta)
{
    setLocation(m_offset + delta);
}

void PinchViewport::move(const FloatSize& delta)
{
    setLocation(m_offset + delta);
}

void PinchViewport::setScale(float scale)
{
    setScaleAndLocation(scale, m_offset);
}

void PinchViewport::setScaleAndLocation(float scale, const FloatPoint& location)
{
    bool valuesChanged = false;

    if (scale != m_scale) {
        m_scale = scale;
        valuesChanged = true;
        frameHost().chromeClient().pageScaleFactorChanged();
    }

    FloatPoint clampedOffset(clampOffsetToBoundaries(location));

    if (clampedOffset != m_offset) {
        m_offset = clampedOffset;
        scrollAnimator()->setCurrentPosition(m_offset);

        // SVG runs with accelerated compositing disabled so no ScrollingCoordinator.
        if (ScrollingCoordinator* coordinator = frameHost().page().scrollingCoordinator())
            coordinator->scrollableAreaScrollLayerDidChange(this);

        Document* document = mainFrame()->document();
        document->enqueueScrollEventForNode(document);

        mainFrame()->loader().client()->didChangeScrollOffset();
        InspectorInstrumentation::didScroll(mainFrame());
        valuesChanged = true;
    }

    if (!valuesChanged)
        return;

    mainFrame()->loader().saveScrollState();

    clampToBoundaries();
}

bool PinchViewport::magnifyScaleAroundAnchor(float magnifyDelta, const FloatPoint& anchor)
{
    const float oldPageScale = scale();
    const float newPageScale = frameHost().chromeClient().clampPageScaleFactorToLimits(
        magnifyDelta * oldPageScale);
    if (newPageScale == oldPageScale)
        return false;
    if (!mainFrame() || !mainFrame()->view())
        return false;

    // Keep the center-of-pinch anchor in a stable position over the course
    // of the magnify.
    FloatPoint anchorAtOldScale = anchor.scaledBy(1.f / oldPageScale);
    FloatPoint anchorAtNewScale = anchor.scaledBy(1.f / newPageScale);
    FloatSize anchorDelta = anchorAtOldScale - anchorAtNewScale;

    // First try to use the anchor's delta to scroll the FrameView.
    FloatSize anchorDeltaUnusedByScroll = anchorDelta;

    if (!frameHost().settings().invertViewportScrollOrder()) {
        FrameView* view = mainFrame()->view();
        DoublePoint oldPosition = view->scrollPositionDouble();
        view->scrollBy(DoubleSize(anchorDelta.width(), anchorDelta.height()), UserScroll);
        DoublePoint newPosition = view->scrollPositionDouble();
        anchorDeltaUnusedByScroll -= toFloatSize(newPosition - oldPosition);
    }

    // Manually bubble any remaining anchor delta up to the pinch viewport.
    FloatPoint newLocation(location() + anchorDeltaUnusedByScroll);
    setScaleAndLocation(newPageScale, newLocation);
    return true;
}

// Modifies the top of the graphics layer tree to add layers needed to support
// the inner/outer viewport fixed-position model for pinch zoom. When finished,
// the tree will look like this (with * denoting added layers):
//
// *rootTransformLayer
//  +- *innerViewportContainerLayer (fixed pos container)
//  |   +- *overscrollElasticityLayer
//  |       +- *pageScaleLayer
//  |           +- *innerViewportScrollLayer
//  |               +-- overflowControlsHostLayer (root layer)
//  |                   +-- outerViewportContainerLayer (fixed pos container) [frame container layer in DeprecatedPaintLayerCompositor]
//  |                   |   +-- outerViewportScrollLayer [frame scroll layer in DeprecatedPaintLayerCompositor]
//  |                   |       +-- content layers ...
//  |                   +-- horizontal ScrollbarLayer (non-overlay)
//  |                   +-- verticalScrollbarLayer (non-overlay)
//  |                   +-- scroll corner (non-overlay)
//  +- *horizontalScrollbarLayer (overlay)
//  +- *verticalScrollbarLayer (overlay)
//
void PinchViewport::attachToLayerTree(GraphicsLayer* currentLayerTreeRoot, GraphicsLayerFactory* graphicsLayerFactory)
{
    TRACE_EVENT1("blink", "PinchViewport::attachToLayerTree", "currentLayerTreeRoot", (bool)currentLayerTreeRoot);
    if (!currentLayerTreeRoot) {
        if (m_innerViewportScrollLayer)
            m_innerViewportScrollLayer->removeAllChildren();
        return;
    }

    if (currentLayerTreeRoot->parent() && currentLayerTreeRoot->parent() == m_innerViewportScrollLayer)
        return;

    if (!m_innerViewportScrollLayer) {
        ASSERT(!m_overlayScrollbarHorizontal
            && !m_overlayScrollbarVertical
            && !m_overscrollElasticityLayer
            && !m_pageScaleLayer
            && !m_innerViewportContainerLayer);

        // FIXME: The root transform layer should only be created on demand.
        m_rootTransformLayer = GraphicsLayer::create(graphicsLayerFactory, this);
        m_innerViewportContainerLayer = GraphicsLayer::create(graphicsLayerFactory, this);
        m_overscrollElasticityLayer = GraphicsLayer::create(graphicsLayerFactory, this);
        m_pageScaleLayer = GraphicsLayer::create(graphicsLayerFactory, this);
        m_innerViewportScrollLayer = GraphicsLayer::create(graphicsLayerFactory, this);
        m_overlayScrollbarHorizontal = GraphicsLayer::create(graphicsLayerFactory, this);
        m_overlayScrollbarVertical = GraphicsLayer::create(graphicsLayerFactory, this);

        ScrollingCoordinator* coordinator = frameHost().page().scrollingCoordinator();
        ASSERT(coordinator);
        coordinator->setLayerIsContainerForFixedPositionLayers(m_innerViewportScrollLayer.get(), true);

        // Set masks to bounds so the compositor doesn't clobber a manually
        // set inner viewport container layer size.
        m_innerViewportContainerLayer->setMasksToBounds(frameHost().settings().mainFrameClipsContent());
        m_innerViewportContainerLayer->setSize(m_size);

        m_innerViewportScrollLayer->platformLayer()->setScrollClipLayer(
            m_innerViewportContainerLayer->platformLayer());
        m_innerViewportScrollLayer->platformLayer()->setUserScrollable(true, true);

        m_rootTransformLayer->addChild(m_innerViewportContainerLayer.get());
        m_innerViewportContainerLayer->addChild(m_overscrollElasticityLayer.get());
        m_overscrollElasticityLayer->addChild(m_pageScaleLayer.get());
        m_pageScaleLayer->addChild(m_innerViewportScrollLayer.get());
        m_innerViewportContainerLayer->addChild(m_overlayScrollbarHorizontal.get());
        m_innerViewportContainerLayer->addChild(m_overlayScrollbarVertical.get());

        // Ensure this class is set as the scroll layer's ScrollableArea.
        coordinator->scrollableAreaScrollLayerDidChange(this);

        // Setup the inner viewport overlay scrollbars.
        setupScrollbar(WebScrollbar::Horizontal);
        setupScrollbar(WebScrollbar::Vertical);
    }

    m_innerViewportScrollLayer->removeAllChildren();
    m_innerViewportScrollLayer->addChild(currentLayerTreeRoot);
}

void PinchViewport::setupScrollbar(WebScrollbar::Orientation orientation)
{
    bool isHorizontal = orientation == WebScrollbar::Horizontal;
    GraphicsLayer* scrollbarGraphicsLayer = isHorizontal ?
        m_overlayScrollbarHorizontal.get() : m_overlayScrollbarVertical.get();
    OwnPtr<WebScrollbarLayer>& webScrollbarLayer = isHorizontal ?
        m_webOverlayScrollbarHorizontal : m_webOverlayScrollbarVertical;

    int thumbThickness = frameHost().settings().pinchOverlayScrollbarThickness();
    int scrollbarThickness = thumbThickness;
    int scrollbarMargin = scrollbarThickness;

    // FIXME: Rather than manually creating scrollbar layers, we should create
    // real scrollbars so we can reuse all the machinery from ScrollbarTheme.
#if OS(ANDROID)
    thumbThickness = ScrollbarTheme::theme()->thumbThickness(0);
    scrollbarThickness = ScrollbarTheme::theme()->scrollbarThickness(RegularScrollbar);
    scrollbarMargin = ScrollbarTheme::theme()->scrollbarMargin();
#endif

    if (!webScrollbarLayer) {
        ScrollingCoordinator* coordinator = frameHost().page().scrollingCoordinator();
        ASSERT(coordinator);
        ScrollbarOrientation webcoreOrientation = isHorizontal ? HorizontalScrollbar : VerticalScrollbar;
        webScrollbarLayer = coordinator->createSolidColorScrollbarLayer(webcoreOrientation, thumbThickness, scrollbarMargin, false);

        webScrollbarLayer->setClipLayer(m_innerViewportContainerLayer->platformLayer());

        // The compositor will control the scrollbar's visibility. Set to invisible by defualt
        // so scrollbars don't show up in layout tests.
        webScrollbarLayer->layer()->setOpacity(0);

        scrollbarGraphicsLayer->setContentsToPlatformLayer(webScrollbarLayer->layer());
        scrollbarGraphicsLayer->setDrawsContent(false);
    }

    int xPosition = isHorizontal ? 0 : m_innerViewportContainerLayer->size().width() - scrollbarThickness;
    int yPosition = isHorizontal ? m_innerViewportContainerLayer->size().height() - scrollbarThickness : 0;
    int width = isHorizontal ? m_innerViewportContainerLayer->size().width() - scrollbarThickness : scrollbarThickness;
    int height = isHorizontal ? scrollbarThickness : m_innerViewportContainerLayer->size().height() - scrollbarThickness;

    // Use the GraphicsLayer to position the scrollbars.
    scrollbarGraphicsLayer->setPosition(IntPoint(xPosition, yPosition));
    scrollbarGraphicsLayer->setSize(IntSize(width, height));
    scrollbarGraphicsLayer->setContentsRect(IntRect(0, 0, width, height));
}

void PinchViewport::registerLayersWithTreeView(WebLayerTreeView* layerTreeView) const
{
    TRACE_EVENT0("blink", "PinchViewport::registerLayersWithTreeView");
    ASSERT(layerTreeView);

    if (!mainFrame())
        return;

    ASSERT(frameHost().page().deprecatedLocalMainFrame()->contentLayoutObject());

    DeprecatedPaintLayerCompositor* compositor = frameHost().page().deprecatedLocalMainFrame()->contentLayoutObject()->compositor();
    // Get the outer viewport scroll layer.
    WebLayer* scrollLayer = compositor->scrollLayer() ? compositor->scrollLayer()->platformLayer() : 0;

    m_webOverlayScrollbarHorizontal->setScrollLayer(scrollLayer);
    m_webOverlayScrollbarVertical->setScrollLayer(scrollLayer);

    ASSERT(compositor);
    layerTreeView->registerViewportLayers(
        m_overscrollElasticityLayer->platformLayer(),
        m_pageScaleLayer->platformLayer(),
        m_innerViewportScrollLayer->platformLayer(),
        scrollLayer);
}

void PinchViewport::clearLayersForTreeView(WebLayerTreeView* layerTreeView) const
{
    ASSERT(layerTreeView);

    layerTreeView->clearViewportLayers();
}

DoubleRect PinchViewport::visibleContentRectDouble(IncludeScrollbarsInRect) const
{
    return visibleRect();
}

IntRect PinchViewport::visibleContentRect(IncludeScrollbarsInRect scrollbarInclusion) const
{
    return enclosingIntRect(visibleContentRectDouble(scrollbarInclusion));
}

bool PinchViewport::shouldUseIntegerScrollOffset() const
{
    LocalFrame* frame = mainFrame();
    if (frame && frame->settings() && !frame->settings()->preferCompositingToLCDTextEnabled())
        return true;

    return ScrollableArea::shouldUseIntegerScrollOffset();
}

int PinchViewport::scrollSize(ScrollbarOrientation orientation) const
{
    IntSize scrollDimensions = maximumScrollPosition() - minimumScrollPosition();
    return (orientation == HorizontalScrollbar) ? scrollDimensions.width() : scrollDimensions.height();
}

IntPoint PinchViewport::minimumScrollPosition() const
{
    return IntPoint();
}

IntPoint PinchViewport::maximumScrollPosition() const
{
    return flooredIntPoint(maximumScrollPositionDouble());
}

DoublePoint PinchViewport::maximumScrollPositionDouble() const
{
    if (!mainFrame())
        return IntPoint();

    // FIXME: We probably shouldn't be storing the bounds in a float. crbug.com/422331.
    FloatSize frameViewSize(contentsSize());

    if (m_topControlsAdjustment) {
        float aspectRatio = visibleRect().width() / visibleRect().height();
        float adjustment = frameViewSize.width() / aspectRatio - frameViewSize.height();
        frameViewSize.expand(0, adjustment);
    }

    frameViewSize.scale(m_scale);
    frameViewSize = flooredIntSize(frameViewSize);

    FloatSize viewportSize(m_size);
    viewportSize.expand(0, m_topControlsAdjustment);

    FloatSize maxPosition = frameViewSize - viewportSize;
    maxPosition.scale(1 / m_scale);
    return DoublePoint(maxPosition);
}

IntPoint PinchViewport::clampDocumentOffsetAtScale(const IntPoint& offset, float scale)
{
    if (!mainFrame() || !mainFrame()->view())
        return IntPoint();

    FrameView* view = mainFrame()->view();

    FloatSize scaledSize(m_size);
    scaledSize.scale(1 / scale);

    IntPoint pinchViewportMax = flooredIntPoint(FloatSize(contentsSize()) - scaledSize);
    IntPoint max = view->maximumScrollPosition() + pinchViewportMax;
    IntPoint min = view->minimumScrollPosition(); // PinchViewportMin should be (0, 0)

    IntPoint clamped = offset;
    clamped = clamped.shrunkTo(max);
    clamped = clamped.expandedTo(min);
    return clamped;
}

void PinchViewport::setTopControlsAdjustment(float adjustment)
{
    m_topControlsAdjustment = adjustment;
}

IntRect PinchViewport::scrollableAreaBoundingBox() const
{
    // This method should return the bounding box in the parent view's coordinate
    // space; however, PinchViewport technically isn't a child of any Frames.
    // Nonetheless, the PinchViewport always occupies the entire main frame so just
    // return that.
    LocalFrame* frame = mainFrame();

    if (!frame || !frame->view())
        return IntRect();

    return frame->view()->frameRect();
}

IntSize PinchViewport::contentsSize() const
{
    LocalFrame* frame = mainFrame();

    if (!frame || !frame->view())
        return IntSize();

    return frame->view()->visibleContentRect(IncludeScrollbars).size();
}

void PinchViewport::invalidateScrollbarRect(Scrollbar*, const IntRect&)
{
    // Do nothing. Pinch scrollbars live on the compositor thread and will
    // be updated when the viewport is synced to the CC.
}

void PinchViewport::setScrollOffset(const IntPoint& offset, ScrollType scrollType)
{
    setScrollOffset(DoublePoint(offset), scrollType);
}

void PinchViewport::setScrollOffset(const DoublePoint& offset, ScrollType)
{
    setLocation(toFloatPoint(offset));
}

GraphicsLayer* PinchViewport::layerForContainer() const
{
    return m_innerViewportContainerLayer.get();
}

GraphicsLayer* PinchViewport::layerForScrolling() const
{
    return m_innerViewportScrollLayer.get();
}

GraphicsLayer* PinchViewport::layerForHorizontalScrollbar() const
{
    return m_overlayScrollbarHorizontal.get();
}

GraphicsLayer* PinchViewport::layerForVerticalScrollbar() const
{
    return m_overlayScrollbarVertical.get();
}

void PinchViewport::paintContents(const GraphicsLayer*, GraphicsContext&, GraphicsLayerPaintingPhase, const IntRect& inClip)
{
}

LocalFrame* PinchViewport::mainFrame() const
{
    return frameHost().page().mainFrame() && frameHost().page().mainFrame()->isLocalFrame() ? frameHost().page().deprecatedLocalMainFrame() : 0;
}

FloatPoint PinchViewport::clampOffsetToBoundaries(const FloatPoint& offset)
{
    FloatPoint clampedOffset(offset);
    clampedOffset = clampedOffset.shrunkTo(FloatPoint(maximumScrollPositionDouble()));
    clampedOffset = clampedOffset.expandedTo(FloatPoint(minimumScrollPositionDouble()));
    return clampedOffset;
}

void PinchViewport::clampToBoundaries()
{
    setLocation(m_offset);
}

FloatRect PinchViewport::viewportToRootFrame(const FloatRect& rectInViewport) const
{
    FloatRect rectInRootFrame = rectInViewport;
    rectInRootFrame.scale(1 / scale());
    rectInRootFrame.moveBy(location());
    return rectInRootFrame;
}

IntRect PinchViewport::viewportToRootFrame(const IntRect& rectInViewport) const
{
    // FIXME: How to snap to pixels?
    return enclosingIntRect(viewportToRootFrame(FloatRect(rectInViewport)));
}

FloatRect PinchViewport::rootFrameToViewport(const FloatRect& rectInRootFrame) const
{
    FloatRect rectInViewport = rectInRootFrame;
    rectInViewport.moveBy(-location());
    rectInViewport.scale(scale());
    return rectInViewport;
}

IntRect PinchViewport::rootFrameToViewport(const IntRect& rectInRootFrame) const
{
    // FIXME: How to snap to pixels?
    return enclosingIntRect(rootFrameToViewport(FloatRect(rectInRootFrame)));
}

FloatPoint PinchViewport::viewportToRootFrame(const FloatPoint& pointInViewport) const
{
    FloatPoint pointInRootFrame = pointInViewport;
    pointInRootFrame.scale(1 / scale(), 1 / scale());
    pointInRootFrame.moveBy(location());
    return pointInRootFrame;
}

FloatPoint PinchViewport::rootFrameToViewport(const FloatPoint& pointInRootFrame) const
{
    FloatPoint pointInViewport = pointInRootFrame;
    pointInViewport.moveBy(-location());
    pointInViewport.scale(scale(), scale());
    return pointInViewport;
}

IntPoint PinchViewport::viewportToRootFrame(const IntPoint& pointInViewport) const
{
    // FIXME: How to snap to pixels?
    return flooredIntPoint(FloatPoint(viewportToRootFrame(FloatPoint(pointInViewport))));
}

IntPoint PinchViewport::rootFrameToViewport(const IntPoint& pointInRootFrame) const
{
    // FIXME: How to snap to pixels?
    return flooredIntPoint(FloatPoint(rootFrameToViewport(FloatPoint(pointInRootFrame))));
}

String PinchViewport::debugName(const GraphicsLayer* graphicsLayer)
{
    String name;
    if (graphicsLayer == m_innerViewportContainerLayer.get()) {
        name = "Inner Viewport Container Layer";
    } else if (graphicsLayer == m_overscrollElasticityLayer.get()) {
        name =  "Overscroll Elasticity Layer";
    } else if (graphicsLayer == m_pageScaleLayer.get()) {
        name =  "Page Scale Layer";
    } else if (graphicsLayer == m_innerViewportScrollLayer.get()) {
        name =  "Inner Viewport Scroll Layer";
    } else if (graphicsLayer == m_overlayScrollbarHorizontal.get()) {
        name =  "Overlay Scrollbar Horizontal Layer";
    } else if (graphicsLayer == m_overlayScrollbarVertical.get()) {
        name =  "Overlay Scrollbar Vertical Layer";
    } else if (graphicsLayer == m_rootTransformLayer) {
        name =  "Root Transform Layer";
    } else {
        ASSERT_NOT_REACHED();
    }

    return name;
}

} // namespace blink
