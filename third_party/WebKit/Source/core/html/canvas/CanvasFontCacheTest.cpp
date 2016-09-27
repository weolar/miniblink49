// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/canvas/CanvasFontCache.h"


#include "core/frame/FrameView.h"
#include "core/html/HTMLDocument.h"
#include "core/html/canvas/CanvasContextCreationAttributes.h"
#include "core/html/canvas/CanvasRenderingContext.h"
#include "core/loader/EmptyClients.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/graphics/UnacceleratedImageBufferSurface.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Mock;

namespace blink {

class CanvasFontCacheTest : public ::testing::Test {
protected:
    CanvasFontCacheTest();
    void SetUp() override;

    DummyPageHolder& page() const { return *m_dummyPageHolder; }
    HTMLDocument& document() const { return *m_document; }
    HTMLCanvasElement& canvasElement() const { return *m_canvasElement; }
    CanvasRenderingContext* context2d() const;
    CanvasFontCache* cache() { return m_document->canvasFontCache(); }

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
    RefPtrWillBePersistent<HTMLDocument> m_document;
    RefPtrWillBePersistent<HTMLCanvasElement> m_canvasElement;
};

CanvasFontCacheTest::CanvasFontCacheTest()
{ }

CanvasRenderingContext* CanvasFontCacheTest::context2d() const
{
    // If the following check fails, perhaps you forgot to call createContext
    // in your test?
    EXPECT_NE(nullptr, canvasElement().renderingContext());
    EXPECT_TRUE(canvasElement().renderingContext()->is2d());
    return canvasElement().renderingContext();
}

void CanvasFontCacheTest::SetUp()
{
    Page::PageClients pageClients;
    fillWithEmptyClients(pageClients);
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600), &pageClients);
    m_document = toHTMLDocument(&m_dummyPageHolder->document());
    m_document->documentElement()->setInnerHTML("<body><canvas id='c'></canvas></body>", ASSERT_NO_EXCEPTION);
    m_document->view()->updateAllLifecyclePhases();
    m_canvasElement = toHTMLCanvasElement(m_document->getElementById("c"));
    String canvasType("2d");
    CanvasContextCreationAttributes attributes;
    attributes.setAlpha(true);
    m_canvasElement->getCanvasRenderingContext(canvasType, attributes);
    context2d(); // Calling this for the checks
}

TEST_F(CanvasFontCacheTest, CacheHardLimit)
{
    String fontString;
    unsigned i;
    for (i = 0; i < cache()->hardMaxFonts() + 1; i++) {
        fontString = String::number(i + 1) + "px sans-serif";
        context2d()->setFont(fontString);
        if (i < cache()->hardMaxFonts()) {
            EXPECT_TRUE(cache()->isInCache("1px sans-serif"));
        } else {
            EXPECT_FALSE(cache()->isInCache("1px sans-serif"));
        }
        EXPECT_TRUE(cache()->isInCache(fontString));
    }
}

TEST_F(CanvasFontCacheTest, PageVisibilityChange)
{
    context2d()->setFont("10px sans-serif");
    EXPECT_TRUE(cache()->isInCache("10px sans-serif"));
    page().page().setVisibilityState(PageVisibilityStateHidden, false);
    EXPECT_FALSE(cache()->isInCache("10px sans-serif"));

    context2d()->setFont("15px sans-serif");
    EXPECT_FALSE(cache()->isInCache("10px sans-serif"));
    EXPECT_TRUE(cache()->isInCache("15px sans-serif"));

    context2d()->setFont("10px sans-serif");
    EXPECT_TRUE(cache()->isInCache("10px sans-serif"));
    EXPECT_FALSE(cache()->isInCache("15px sans-serif"));

    page().page().setVisibilityState(PageVisibilityStateVisible, false);
    context2d()->setFont("15px sans-serif");
    context2d()->setFont("10px sans-serif");
    EXPECT_TRUE(cache()->isInCache("10px sans-serif"));
    EXPECT_TRUE(cache()->isInCache("15px sans-serif"));
}

} // namespace blink
