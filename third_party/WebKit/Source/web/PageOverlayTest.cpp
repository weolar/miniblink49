// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/PageOverlay.h"

#include "core/frame/FrameView.h"
#include "core/layout/LayoutView.h"
#include "platform/graphics/Color.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCanvas.h"
#include "public/platform/WebThread.h"
#include "public/web/WebPageOverlay.h"
#include "public/web/WebSettings.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "web/WebGraphicsContextImpl.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::AtLeast;
using testing::Property;

namespace blink {
namespace {

static const int viewportWidth = 800;
static const int viewportHeight = 600;

// These unit tests cover both PageOverlay and PageOverlayList.

void enableAcceleratedCompositing(WebSettings* settings)
{
    settings->setAcceleratedCompositingEnabled(true);
}

void disableAcceleratedCompositing(WebSettings* settings)
{
    settings->setAcceleratedCompositingEnabled(false);
}

class PageOverlayTest : public ::testing::Test {
protected:
    enum CompositingMode { AcceleratedCompositing, UnacceleratedCompositing };

    void initialize(CompositingMode compositingMode)
    {
        m_helper.initialize(
            false /* enableJavascript */, nullptr /* webFrameClient */, nullptr /* webViewClient */,
            compositingMode == AcceleratedCompositing ? enableAcceleratedCompositing : disableAcceleratedCompositing);
        webViewImpl()->resize(WebSize(viewportWidth, viewportHeight));
        webViewImpl()->layout();
        ASSERT_EQ(compositingMode == AcceleratedCompositing, webViewImpl()->isAcceleratedCompositingActive());
        ASSERT_TRUE(!webViewImpl()->pageOverlays() || webViewImpl()->pageOverlays()->empty());
    }

    WebViewImpl* webViewImpl() const { return m_helper.webViewImpl(); }

    template <typename OverlayType>
    void runPageOverlayTestWithUnacceleratedCompositing();

    template <typename OverlayType>
    void runPageOverlayTestWithAcceleratedCompositing();

private:
    FrameTestHelpers::WebViewHelper m_helper;
};

// WebPageOverlay that uses a WebCanvas to draw a solid color.
class SimpleCanvasOverlay : public WebPageOverlay {
public:
    SimpleCanvasOverlay(SkColor color) : m_color(color) { }

    void paintPageOverlay(WebGraphicsContext* context, const WebSize& size) override
    {
        WebFloatRect rect(0, 0, size.width, size.height);
        WebCanvas* canvas = context->beginDrawing(rect);
        SkPaint paint;
        paint.setColor(m_color);
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawRectCoords(0, 0, size.width, size.height, paint);
        context->endDrawing();
    }

private:
    SkColor m_color;
};

// WebPageOverlay that uses the underlying blink::GraphicsContext to paint a
// solid color.
class PrivateGraphicsContextOverlay : public WebPageOverlay {
public:
    PrivateGraphicsContextOverlay(Color color) : m_color(color) { }

    void paintPageOverlay(WebGraphicsContext* context, const WebSize& size) override
    {
        GraphicsContext& graphicsContext = toWebGraphicsContextImpl(context)->graphicsContext();
        if (DrawingRecorder::useCachedDrawingIfPossible(graphicsContext, *this, DisplayItem::PageOverlay))
            return;
        FloatRect rect(0, 0, size.width, size.height);
        DrawingRecorder drawingRecorder(graphicsContext, *this, DisplayItem::PageOverlay, rect);
        graphicsContext.fillRect(rect, m_color);
    }

    DisplayItemClient displayItemClient() const { return toDisplayItemClient(this); }
    String debugName() const { return "PrivateGraphicsContextOverlay"; }

private:
    Color m_color;
};

template <bool(*getter)(), void(*setter)(bool)>
class RuntimeFeatureChange {
public:
    RuntimeFeatureChange(bool newValue) : m_oldValue(getter()) { setter(newValue); }
    ~RuntimeFeatureChange() { setter(m_oldValue); }
private:
    bool m_oldValue;
};
using SlimmingPaintScope = RuntimeFeatureChange<&RuntimeEnabledFeatures::slimmingPaintEnabled, RuntimeEnabledFeatures::setSlimmingPaintEnabled>;

class MockCanvas : public SkCanvas {
public:
    MockCanvas(int width, int height) : SkCanvas(width, height) { }
    MOCK_METHOD2(onDrawRect, void(const SkRect&, const SkPaint&));
};

template <typename OverlayType>
void PageOverlayTest::runPageOverlayTestWithUnacceleratedCompositing()
{
    initialize(UnacceleratedCompositing);

    OverlayType overlay(SK_ColorYELLOW);
    webViewImpl()->addPageOverlay(&overlay, 0 /* zOrder */);
    EXPECT_TRUE(webViewImpl()->pageOverlays() && !webViewImpl()->pageOverlays()->empty());
    webViewImpl()->layout();

    MockCanvas canvas(viewportWidth, viewportHeight);
    EXPECT_CALL(canvas, onDrawRect(_, _)).Times(AtLeast(1));
    EXPECT_CALL(canvas, onDrawRect(SkRect::MakeWH(viewportWidth, viewportHeight), Property(&SkPaint::getColor, SK_ColorYELLOW)));
    webViewImpl()->paint(&canvas, WebRect(0, 0, viewportWidth, viewportHeight));
}

TEST_F(PageOverlayTest, SimpleCanvasOverlay_UnacceleratedCompositing_NoSlimmingPaint)
{
    SlimmingPaintScope slimmingPaintEnabled(false);
    runPageOverlayTestWithUnacceleratedCompositing<SimpleCanvasOverlay>();
}

TEST_F(PageOverlayTest, SimpleCanvasOverlay_UnacceleratedCompositing_SlimmingPaint)
{
    SlimmingPaintScope slimmingPaintEnabled(true);
    runPageOverlayTestWithUnacceleratedCompositing<SimpleCanvasOverlay>();
}

TEST_F(PageOverlayTest, PrivateGraphicsContextOverlay_UnacceleratedCompositing_NoSlimmingPaint)
{
    SlimmingPaintScope slimmingPaintEnabled(false);
    runPageOverlayTestWithUnacceleratedCompositing<PrivateGraphicsContextOverlay>();
}

TEST_F(PageOverlayTest, PrivateGraphicsContextOverlay_UnacceleratedCompositing_SlimmingPaint)
{
    SlimmingPaintScope slimmingPaintEnabled(true);
    runPageOverlayTestWithUnacceleratedCompositing<PrivateGraphicsContextOverlay>();
}

template <typename OverlayType>
void PageOverlayTest::runPageOverlayTestWithAcceleratedCompositing()
{
    initialize(AcceleratedCompositing);
    webViewImpl()->layerTreeView()->setViewportSize(WebSize(viewportWidth, viewportHeight));

    OverlayType overlay(SK_ColorYELLOW);
    webViewImpl()->addPageOverlay(&overlay, 0 /* zOrder */);
    EXPECT_TRUE(webViewImpl()->pageOverlays() && !webViewImpl()->pageOverlays()->empty());
    webViewImpl()->layout();

    // Ideally, we would get results from the compositor that showed that this
    // page overlay actually winds up getting drawn on top of the rest.
    // For now, we just check that the GraphicsLayer will draw the right thing.

    MockCanvas canvas(viewportWidth, viewportHeight);
    EXPECT_CALL(canvas, onDrawRect(_, _)).Times(AtLeast(0));
    EXPECT_CALL(canvas, onDrawRect(SkRect::MakeWH(viewportWidth, viewportHeight), Property(&SkPaint::getColor, SK_ColorYELLOW)));

    GraphicsLayer* graphicsLayer = webViewImpl()->pageOverlays()->graphicsLayerForTesting();
    WebRect rect(0, 0, viewportWidth, viewportHeight);
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        // If slimming paint is on, we paint the layer with a null canvas to get
        // a display list, and then replay that onto the mock canvas for
        // examination. This is about as close to the real path as we can easily
        // get.
        GraphicsContext graphicsContext(graphicsLayer->displayItemList());
        graphicsLayer->paint(graphicsContext, rect);

        graphicsContext.beginRecording(IntRect(rect));
        graphicsLayer->displayItemList()->commitNewDisplayItemsAndReplay(graphicsContext);
        graphicsContext.endRecording()->playback(&canvas);
    } else {
        OwnPtr<GraphicsContext> graphicsContext = GraphicsContext::deprecatedCreateWithCanvas(&canvas);
        graphicsLayer->paint(*graphicsContext, rect);
    }
}

TEST_F(PageOverlayTest, SimpleCanvasOverlay_AcceleratedCompositing_NoSlimmingPaint)
{
    SlimmingPaintScope slimmingPaintEnabled(false);
    runPageOverlayTestWithAcceleratedCompositing<SimpleCanvasOverlay>();
}

TEST_F(PageOverlayTest, SimpleCanvasOverlay_AcceleratedCompositing_SlimmingPaint)
{
    SlimmingPaintScope slimmingPaintEnabled(true);
    runPageOverlayTestWithAcceleratedCompositing<SimpleCanvasOverlay>();
}

TEST_F(PageOverlayTest, PrivateGraphicsContextOverlay_AcceleratedCompositing_NoSlimmingPaint)
{
    SlimmingPaintScope slimmingPaintEnabled(false);
    runPageOverlayTestWithAcceleratedCompositing<PrivateGraphicsContextOverlay>();
}

TEST_F(PageOverlayTest, PrivateGraphicsContextOverlay_AcceleratedCompositing_SlimmingPaint)
{
    SlimmingPaintScope slimmingPaintEnabled(true);
    runPageOverlayTestWithAcceleratedCompositing<PrivateGraphicsContextOverlay>();
}

} // namespace
} // namespace blink
