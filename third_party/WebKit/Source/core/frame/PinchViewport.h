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

#ifndef PinchViewport_h
#define PinchViewport_h

#include "core/CoreExport.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/IntSize.h"
#include "platform/graphics/GraphicsLayerClient.h"
#include "platform/scroll/ScrollableArea.h"
#include "public/platform/WebScrollbar.h"
#include "public/platform/WebSize.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"

namespace blink {
class WebLayerTreeView;
class WebScrollbarLayer;
}

namespace blink {

class FrameHost;
class GraphicsContext;
class GraphicsLayer;
class GraphicsLayerFactory;
class IntRect;
class IntSize;
class LocalFrame;

// Represents the pinch-to-zoom viewport the user is currently seeing the page through. This
// class corresponds to the InnerViewport on the compositor. It is a ScrollableArea; it's
// offset is set through the GraphicsLayer <-> CC sync mechanisms. Its contents is the page's
// main FrameView, which corresponds to the outer viewport. The inner viewport is always contained
// in the outer viewport and can pan within it.
class CORE_EXPORT PinchViewport final : public NoBaseWillBeGarbageCollectedFinalized<PinchViewport>, public GraphicsLayerClient, public ScrollableArea {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(PinchViewport);
public:
    static PassOwnPtrWillBeRawPtr<PinchViewport> create(FrameHost& host)
    {
        return adoptPtrWillBeNoop(new PinchViewport(host));
    }
    virtual ~PinchViewport();

    DECLARE_VIRTUAL_TRACE();

    void attachToLayerTree(GraphicsLayer*, GraphicsLayerFactory*);
    GraphicsLayer* rootGraphicsLayer()
    {
        return m_rootTransformLayer.get();
    }
    GraphicsLayer* containerLayer()
    {
        return m_innerViewportContainerLayer.get();
    }
    GraphicsLayer* scrollLayer()
    {
        return m_innerViewportScrollLayer.get();
    }

    // Sets the location of the pinch viewport relative to the outer viewport. The
    // coordinates are in partial CSS pixels.
    void setLocation(const FloatPoint&);
    // FIXME: This should be called moveBy
    void move(const FloatPoint&);
    void move(const FloatSize&);
    FloatPoint location() const { return m_offset; }

    // Sets the size of the inner viewport when unscaled in CSS pixels.
    void setSize(const IntSize&);
    IntSize size() const { return m_size; }

    // Gets the scaled size, i.e. the viewport in root view space.
    FloatSize visibleSize() const;

    // Resets the viewport to initial state.
    void reset();

    // Let the viewport know that the main frame changed size (either through screen
    // rotation on Android or window resize elsewhere).
    void mainFrameDidChangeSize();

    // Sets scale and location in one operation, preventing intermediate clamping.
    void setScaleAndLocation(float scale, const FloatPoint& location);
    void setScale(float);
    float scale() const { return m_scale; }

    // Update scale factor, magnifying or minifying by magnifyDelta, centered around
    // the point specified by anchor in window coordinates. Returns false if page
    // scale factor is left unchanged.
    bool magnifyScaleAroundAnchor(float magnifyDelta, const FloatPoint& anchor);

    void registerLayersWithTreeView(WebLayerTreeView*) const;
    void clearLayersForTreeView(WebLayerTreeView*) const;

    // The portion of the unzoomed frame visible in the inner "pinch" viewport,
    // in partial CSS pixels. Relative to the main frame.
    FloatRect visibleRect() const;

    // The viewport rect relative to the document origin, in partial CSS pixels.
    // FIXME: This should be a DoubleRect since scroll offsets are now doubles.
    FloatRect visibleRectInDocument() const;

    // Convert the given rect in the main FrameView's coordinates into a rect
    // in the viewport. The given and returned rects are in CSS pixels, meaning
    // scale isn't applied.
    FloatRect mainViewToViewportCSSPixels(const FloatRect&) const;
    FloatPoint viewportCSSPixelsToRootFrame(const FloatPoint&) const;

    // Clamp the given point, in document coordinates, to the maximum/minimum
    // scroll extents of the viewport within the document.
    IntPoint clampDocumentOffsetAtScale(const IntPoint& offset, float scale);

    // FIXME: This is kind of a hack. Ideally, we would just resize the
    // viewports to account for top controls. However, FrameView includes much
    // more than just scrolling so we can't simply resize it without incurring
    // all sorts of side-effects. Until we can seperate out the scrollability
    // aspect from FrameView, we use this method to let PinchViewport make the
    // necessary adjustments so that we don't incorrectly clamp scroll offsets
    // coming from the compositor. crbug.com/422328
    void setTopControlsAdjustment(float);

    // Adjust the viewport's offset so that it remains bounded by the outer
    // viepwort.
    void clampToBoundaries();

    FloatRect viewportToRootFrame(const FloatRect&) const;
    IntRect viewportToRootFrame(const IntRect&) const;
    FloatRect rootFrameToViewport(const FloatRect&) const;
    IntRect rootFrameToViewport(const IntRect&) const;

    FloatPoint viewportToRootFrame(const FloatPoint&) const;
    FloatPoint rootFrameToViewport(const FloatPoint&) const;
    IntPoint viewportToRootFrame(const IntPoint&) const;
    IntPoint rootFrameToViewport(const IntPoint&) const;

    // ScrollableArea implementation
    virtual DoubleRect visibleContentRectDouble(IncludeScrollbarsInRect = ExcludeScrollbars) const override;
    virtual IntRect visibleContentRect(IncludeScrollbarsInRect = ExcludeScrollbars) const override;
    virtual bool shouldUseIntegerScrollOffset() const override;
    virtual bool isActive() const override { return false; }
    virtual int scrollSize(ScrollbarOrientation) const override;
    virtual bool isScrollCornerVisible() const override { return false; }
    virtual IntRect scrollCornerRect() const override { return IntRect(); }
    virtual IntPoint scrollPosition() const override { return flooredIntPoint(m_offset); }
    virtual DoublePoint scrollPositionDouble() const override { return m_offset; }
    virtual IntPoint minimumScrollPosition() const override;
    virtual IntPoint maximumScrollPosition() const override;
    virtual DoublePoint maximumScrollPositionDouble() const override;
    virtual int visibleHeight() const override { return visibleRect().height(); }
    virtual int visibleWidth() const override { return visibleRect().width(); }
    virtual IntSize contentsSize() const override;
    virtual bool scrollbarsCanBeActive() const override { return false; }
    virtual IntRect scrollableAreaBoundingBox() const override;
    virtual bool userInputScrollable(ScrollbarOrientation) const override { return true; }
    virtual bool shouldPlaceVerticalScrollbarOnLeft() const override { return false; }
    virtual void invalidateScrollbarRect(Scrollbar*, const IntRect&) override;
    virtual void invalidateScrollCornerRect(const IntRect&) override { }
    virtual void setScrollOffset(const IntPoint&, ScrollType) override;
    virtual void setScrollOffset(const DoublePoint&, ScrollType) override;
    virtual GraphicsLayer* layerForContainer() const override;
    virtual GraphicsLayer* layerForScrolling() const override;
    virtual GraphicsLayer* layerForHorizontalScrollbar() const override;
    virtual GraphicsLayer* layerForVerticalScrollbar() const override;

private:
    explicit PinchViewport(FrameHost&);

    // GraphicsLayerClient implementation.
    virtual void paintContents(const GraphicsLayer*, GraphicsContext&, GraphicsLayerPaintingPhase, const IntRect& inClip) override;
    virtual String debugName(const GraphicsLayer*) override;

    void setupScrollbar(WebScrollbar::Orientation);
    FloatPoint clampOffsetToBoundaries(const FloatPoint&);

    LocalFrame* mainFrame() const;

    FrameHost& frameHost() const
    {
        ASSERT(m_frameHost);
        return *m_frameHost;
    }

    RawPtrWillBeMember<FrameHost> m_frameHost;
    OwnPtr<GraphicsLayer> m_rootTransformLayer;
    OwnPtr<GraphicsLayer> m_innerViewportContainerLayer;
    OwnPtr<GraphicsLayer> m_overscrollElasticityLayer;
    OwnPtr<GraphicsLayer> m_pageScaleLayer;
    OwnPtr<GraphicsLayer> m_innerViewportScrollLayer;
    OwnPtr<GraphicsLayer> m_overlayScrollbarHorizontal;
    OwnPtr<GraphicsLayer> m_overlayScrollbarVertical;
    OwnPtr<WebScrollbarLayer> m_webOverlayScrollbarHorizontal;
    OwnPtr<WebScrollbarLayer> m_webOverlayScrollbarVertical;

    // Offset of the pinch viewport from the main frame's origin, in CSS pixels.
    FloatPoint m_offset;
    float m_scale;
    IntSize m_size;
    float m_topControlsAdjustment;
};

} // namespace blink

#endif // PinchViewport_h
