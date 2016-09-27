// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/FramePainter.h"

#include "core/dom/DocumentMarkerController.h"
#include "core/fetch/MemoryCache.h"
#include "core/frame/FrameView.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/layout/LayoutView.h"
#include "core/page/Page.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/DeprecatedPaintLayerPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/ScrollbarPainter.h"
#include "core/paint/TransformRecorder.h"
#include "platform/fonts/FontCache.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/ClipRecorder.h"
#include "platform/scroll/ScrollbarTheme.h"

namespace blink {

bool FramePainter::s_inPaintContents = false;

void FramePainter::paint(GraphicsContext* context, const IntRect& rect)
{
    m_frameView.notifyPageThatContentAreaWillPaint();

    IntRect documentDirtyRect = rect;
    IntRect visibleAreaWithoutScrollbars(m_frameView.location(), m_frameView.visibleContentRect().size());
    documentDirtyRect.intersect(visibleAreaWithoutScrollbars);

    if (!documentDirtyRect.isEmpty()) {
        TransformRecorder transformRecorder(*context, *m_frameView.layoutView(),
            AffineTransform::translation(m_frameView.x() - m_frameView.scrollX(), m_frameView.y() - m_frameView.scrollY()));

        ClipRecorder recorder(*context, *m_frameView.layoutView(), DisplayItem::ClipFrameToVisibleContentRect, LayoutRect(m_frameView.visibleContentRect()));

        documentDirtyRect.moveBy(-m_frameView.location() + m_frameView.scrollPosition());
        paintContents(context, documentDirtyRect);
    }

    // Now paint the scrollbars.
    if (!m_frameView.scrollbarsSuppressed() && (m_frameView.horizontalScrollbar() || m_frameView.verticalScrollbar())) {
        IntRect scrollViewDirtyRect = rect;
        IntRect visibleAreaWithScrollbars(m_frameView.location(), m_frameView.visibleContentRect(IncludeScrollbars).size());
        scrollViewDirtyRect.intersect(visibleAreaWithScrollbars);
        scrollViewDirtyRect.moveBy(-m_frameView.location());

        TransformRecorder transformRecorder(*context, *m_frameView.layoutView(),
            AffineTransform::translation(m_frameView.x(), m_frameView.y()));

        ClipRecorder recorder(*context, *m_frameView.layoutView(), DisplayItem::ClipFrameScrollbars, LayoutRect(IntPoint(), visibleAreaWithScrollbars.size()));

        paintScrollbars(context, scrollViewDirtyRect);
    }
}

void FramePainter::paintContents(GraphicsContext* context, const IntRect& rect)
{
    Document* document = m_frameView.frame().document();

#ifndef NDEBUG
    bool fillWithRed;
    if (document->printing())
        fillWithRed = false; // Printing, don't fill with red (can't remember why).
    else if (m_frameView.frame().owner())
        fillWithRed = false; // Subframe, don't fill with red.
    else if (m_frameView.isTransparent())
        fillWithRed = false; // Transparent, don't fill with red.
    else if (m_frameView.paintBehavior() & PaintBehaviorSelectionOnly)
        fillWithRed = false; // Selections are transparent, don't fill with red.
    else if (m_frameView.nodeToDraw())
        fillWithRed = false; // Element images are transparent, don't fill with red.
    else
        fillWithRed = true;

    if (fillWithRed && !LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, *m_frameView.layoutView(), DisplayItem::DebugRedFill)) {
        IntRect contentRect(IntPoint(), m_frameView.contentsSize());
        LayoutObjectDrawingRecorder drawingRecorder(*context, *m_frameView.layoutView(), DisplayItem::DebugRedFill, contentRect);
    }
#endif

    LayoutView* layoutView = m_frameView.layoutView();
    if (!layoutView) {
        WTF_LOG_ERROR("called FramePainter::paint with nil layoutObject");
        return;
    }

    RELEASE_ASSERT(!m_frameView.needsLayout());
    ASSERT(document->lifecycle().state() >= DocumentLifecycle::CompositingClean);

    TRACE_EVENT1("devtools.timeline", "Paint", "data", InspectorPaintEvent::data(layoutView, LayoutRect(rect), 0));

    bool isTopLevelPainter = !s_inPaintContents;
    s_inPaintContents = true;

    FontCachePurgePreventer fontCachePurgePreventer;

    PaintBehavior oldPaintBehavior = m_frameView.paintBehavior();

    if (FrameView* parentView = m_frameView.parentFrameView()) {
        if (parentView->paintBehavior() & PaintBehaviorFlattenCompositingLayers)
            m_frameView.setPaintBehavior(m_frameView.paintBehavior() | PaintBehaviorFlattenCompositingLayers);
    }

    if (document->printing())
        m_frameView.setPaintBehavior(m_frameView.paintBehavior() | PaintBehaviorFlattenCompositingLayers);

    ASSERT(!m_frameView.isPainting());
    m_frameView.setIsPainting(true);

    // m_frameView.nodeToDraw() is used to draw only one element (and its descendants)
    LayoutObject* layoutObject = m_frameView.nodeToDraw() ? m_frameView.nodeToDraw()->layoutObject() : 0;
    DeprecatedPaintLayer* rootLayer = layoutView->layer();

#if ENABLE(ASSERT)
    layoutView->assertSubtreeIsLaidOut();
    LayoutObject::SetLayoutNeededForbiddenScope forbidSetNeedsLayout(*rootLayer->layoutObject());
#endif

    DeprecatedPaintLayerPainter layerPainter(*rootLayer);

    float deviceScaleFactor = blink::deviceScaleFactor(rootLayer->layoutObject()->frame());
    context->setDeviceScaleFactor(deviceScaleFactor);

    layerPainter.paint(context, LayoutRect(rect), m_frameView.paintBehavior(), layoutObject);

    if (rootLayer->containsDirtyOverlayScrollbars())
        layerPainter.paintOverlayScrollbars(context, LayoutRect(rect), m_frameView.paintBehavior(), layoutObject);

    m_frameView.setIsPainting(false);

    m_frameView.setPaintBehavior(oldPaintBehavior);
    m_frameView.setLastPaintTime(currentTime());

    // Regions may have changed as a result of the visibility/z-index of element changing.
    if (document->annotatedRegionsDirty())
        m_frameView.updateAnnotatedRegions();

    if (isTopLevelPainter) {
        // Everything that happens after paintContents completions is considered
        // to be part of the next frame.
        memoryCache()->updateFramePaintTimestamp();
        s_inPaintContents = false;
    }

    InspectorInstrumentation::didPaint(layoutView, 0, context, LayoutRect(rect));
}

void FramePainter::paintScrollbars(GraphicsContext* context, const IntRect& rect)
{
    if (m_frameView.horizontalScrollbar() && !m_frameView.layerForHorizontalScrollbar())
        paintScrollbar(context, m_frameView.horizontalScrollbar(), rect);
    if (m_frameView.verticalScrollbar() && !m_frameView.layerForVerticalScrollbar())
        paintScrollbar(context, m_frameView.verticalScrollbar(), rect);

    if (m_frameView.layerForScrollCorner())
        return;

    paintScrollCorner(context, m_frameView.scrollCornerRect());
}

void FramePainter::paintScrollCorner(GraphicsContext* context, const IntRect& cornerRect)
{
    if (m_frameView.scrollCorner()) {
        bool needsBackground = m_frameView.frame().isMainFrame();
        if (needsBackground && !LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, *m_frameView.layoutView(), DisplayItem::ScrollbarCorner)) {
            LayoutObjectDrawingRecorder drawingRecorder(*context, *m_frameView.layoutView(), DisplayItem::ScrollbarCorner, cornerRect);
            context->fillRect(cornerRect, m_frameView.baseBackgroundColor());

        }
        ScrollbarPainter::paintIntoRect(m_frameView.scrollCorner(), context, cornerRect.location(), LayoutRect(cornerRect));
        return;
    }

    ScrollbarTheme::theme()->paintScrollCorner(context, *m_frameView.layoutView(), cornerRect);
}

void FramePainter::paintScrollbar(GraphicsContext* context, Scrollbar* bar, const IntRect& rect)
{
    bool needsBackground = bar->isCustomScrollbar() && m_frameView.frame().isMainFrame();
    if (needsBackground) {
        IntRect toFill = bar->frameRect();
        toFill.intersect(rect);
        context->fillRect(toFill, m_frameView.baseBackgroundColor());
    }

    bar->paint(context, rect);
}

} // namespace blink
