// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/canvas2d/CanvasRenderingContext2D.h"

#include "core/frame/FrameView.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLDocument.h"
#include "core/html/ImageData.h"
#include "core/loader/EmptyClients.h"
#include "core/testing/DummyPageHolder.h"
#include "modules/canvas2d/CanvasGradient.h"
#include "modules/canvas2d/CanvasPattern.h"
#include "modules/webgl/WebGLRenderingContext.h"
#include "platform/graphics/UnacceleratedImageBufferSurface.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Mock;

namespace blink {

class CanvasRenderingContext2DAPITest : public ::testing::Test {
protected:
    CanvasRenderingContext2DAPITest();
    void SetUp() override;

    DummyPageHolder& page() const { return *m_dummyPageHolder; }
    HTMLDocument& document() const { return *m_document; }
    HTMLCanvasElement& canvasElement() const { return *m_canvasElement; }
    CanvasRenderingContext2D* context2d() const;

    void createContext(OpacityMode);

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
    RefPtrWillBePersistent<HTMLDocument> m_document;
    RefPtrWillBePersistent<HTMLCanvasElement> m_canvasElement;

};

CanvasRenderingContext2DAPITest::CanvasRenderingContext2DAPITest()
{ }

CanvasRenderingContext2D* CanvasRenderingContext2DAPITest::context2d() const
{
    // If the following check fails, perhaps you forgot to call createContext
    // in your test?
    EXPECT_NE(nullptr, canvasElement().renderingContext());
    EXPECT_TRUE(canvasElement().renderingContext()->is2d());
    return static_cast<CanvasRenderingContext2D*>(canvasElement().renderingContext());
}

void CanvasRenderingContext2DAPITest::createContext(OpacityMode opacityMode)
{
    String canvasType("2d");
    CanvasContextCreationAttributes attributes;
    attributes.setAlpha(opacityMode == NonOpaque);
    m_canvasElement->getCanvasRenderingContext(canvasType, attributes);
    context2d(); // Calling this for the checks
}

void CanvasRenderingContext2DAPITest::SetUp()
{
    Page::PageClients pageClients;
    fillWithEmptyClients(pageClients);
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600), &pageClients);
    m_document = toHTMLDocument(&m_dummyPageHolder->document());
    m_document->documentElement()->setInnerHTML("<body><canvas id='c'></canvas></body>", ASSERT_NO_EXCEPTION);
    m_document->view()->updateAllLifecyclePhases();
    m_canvasElement = toHTMLCanvasElement(m_document->getElementById("c"));
}

TEST_F(CanvasRenderingContext2DAPITest, SetShadowColor_Clamping)
{
    createContext(NonOpaque);

    context2d()->setShadowColor("rgba(0,0,0,0)");
    EXPECT_EQ(String("rgba(0, 0, 0, 0)"), context2d()->shadowColor());
    context2d()->setShadowColor("rgb(0,0,0)");
    EXPECT_EQ(String("#000000"), context2d()->shadowColor());
    context2d()->setShadowColor("rgb(0,999,0)");
    EXPECT_EQ(String("#00ff00"), context2d()->shadowColor());
    context2d()->setShadowColor("rgb(0,999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999,0)");
    EXPECT_EQ(String("#00ff00"), context2d()->shadowColor());
    context2d()->setShadowColor("rgb(0,0,256)");
    EXPECT_EQ(String("#0000ff"), context2d()->shadowColor());
    context2d()->setShadowColor("rgb(999999999999999999999999,0,-9999999999999999999999999999)");
    EXPECT_EQ(String("#ff0000"), context2d()->shadowColor());
    context2d()->setShadowColor("rgba(9999999999999999999999999999999999999999999999999999999999999999999999999999999999,9,0,1)");
    EXPECT_EQ(String("#ff0900"), context2d()->shadowColor());
    context2d()->setShadowColor("rgba(9999999999999999999999999999999999999999999999999999999999999999999999999999999999,9,0,-99999999999999999999999999999999999999)");
    EXPECT_EQ(String("rgba(255, 9, 0, 0)"), context2d()->shadowColor());
    context2d()->setShadowColor("rgba(7,9999999999999999999999999999999999999999999999999999999999999999999999999999999999,0,99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999)");
    EXPECT_EQ(String("#07ff00"), context2d()->shadowColor());
    context2d()->setShadowColor("rgba(-7,9999999999999999999999999999999999999999999999999999999999999999999999999999999999,0,99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999)");
    EXPECT_EQ(String("#00ff00"), context2d()->shadowColor());
    context2d()->setShadowColor("rgba(0%,100%,0%,0.4)");
    EXPECT_EQ(String("rgba(0, 255, 0, 0.4)"), context2d()->shadowColor());
}

String trySettingStrokeStyle(CanvasRenderingContext2D* ctx, const String& value)
{
    StringOrCanvasGradientOrCanvasPattern arg1, arg2, arg3;
    arg1.setString("#666");
    ctx->setStrokeStyle(arg1);
    arg2.setString(value);
    ctx->setStrokeStyle(arg2);
    ctx->strokeStyle(arg3);
    EXPECT_TRUE(arg3.isString());
    return arg3.getAsString();
}

String trySettingFillStyle(CanvasRenderingContext2D* ctx, const String& value)
{
    StringOrCanvasGradientOrCanvasPattern arg1, arg2, arg3;
    arg1.setString("#666");
    ctx->setFillStyle(arg1);
    arg2.setString(value);
    ctx->setFillStyle(arg2);
    ctx->fillStyle(arg3);
    EXPECT_TRUE(arg3.isString());
    return arg3.getAsString();
}

String trySettingShadowColor(CanvasRenderingContext2D* ctx, const String& value)
{
    ctx->setShadowColor("#666");
    ctx->setShadowColor(value);
    return ctx->shadowColor();
}

void trySettingColor(CanvasRenderingContext2D* ctx, const String& value, const String& expected)
{
    EXPECT_EQ(expected, trySettingStrokeStyle(ctx, value));
    EXPECT_EQ(expected, trySettingFillStyle(ctx, value));
    EXPECT_EQ(expected, trySettingShadowColor(ctx, value));
}

TEST_F(CanvasRenderingContext2DAPITest, ColorSerialization)
{
    createContext(NonOpaque);
    // Check round trips
    trySettingColor(context2d(), "transparent", "rgba(0, 0, 0, 0)");
    trySettingColor(context2d(), "red", "#ff0000");
    trySettingColor(context2d(), "white", "#ffffff");
    trySettingColor(context2d(), "", "#666666");
    trySettingColor(context2d(), "RGBA(0, 0, 0, 0)", "rgba(0, 0, 0, 0)");
    trySettingColor(context2d(), "rgba(0,255,0,1.0)", "#00ff00");
    trySettingColor(context2d(), "rgba(1,2,3,0.4)", "rgba(1, 2, 3, 0.4)");
    trySettingColor(context2d(), "RgB(1,2,3)", "#010203");
    trySettingColor(context2d(), "rGbA(1,2,3,0)", "rgba(1, 2, 3, 0)");
}

TEST_F(CanvasRenderingContext2DAPITest, DefaultAttributeValues)
{
    createContext(NonOpaque);

    {
        StringOrCanvasGradientOrCanvasPattern value;
        context2d()->strokeStyle(value);
        EXPECT_TRUE(value.isString());
        EXPECT_EQ(String("#000000"), value.getAsString());
    }

    {
        StringOrCanvasGradientOrCanvasPattern value;
        context2d()->fillStyle(value);
        EXPECT_TRUE(value.isString());
        EXPECT_EQ(String("#000000"), value.getAsString());
    }

    EXPECT_EQ(String("rgba(0, 0, 0, 0)"), context2d()->shadowColor());
}

TEST_F(CanvasRenderingContext2DAPITest, LineDashStateSave)
{
    createContext(NonOpaque);

    Vector<float> simpleDash;
    simpleDash.append(4);
    simpleDash.append(2);

    context2d()->setLineDash(simpleDash);
    EXPECT_EQ(simpleDash, context2d()->getLineDash());
    context2d()->save();
    // Realize the save.
    context2d()->scale(2, 2);
    EXPECT_EQ(simpleDash, context2d()->getLineDash());
    context2d()->restore();
    EXPECT_EQ(simpleDash, context2d()->getLineDash());
}

TEST_F(CanvasRenderingContext2DAPITest, CreateImageData)
{
    createContext(NonOpaque);

    NonThrowableExceptionState exceptionState;

    // create a 100x50 imagedata and fill it with white pixels
    ImageData* imageData = context2d()->createImageData(100, 50, exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    EXPECT_EQ(100, imageData->width());
    EXPECT_EQ(50, imageData->height());

    for (unsigned i = 0; i < imageData->data()->length(); ++i)
        imageData->data()->data()[i] = 255;

    EXPECT_EQ(255, imageData->data()->data()[32]);

    // createImageData(imageData) should create a new ImageData of the same size as 'imageData'
    // but filled with transparent black

    ImageData* sameSizeImageData = context2d()->createImageData(imageData);
    EXPECT_EQ(100, sameSizeImageData->width());
    EXPECT_EQ(50, sameSizeImageData->height());
    EXPECT_EQ(0, sameSizeImageData->data()->data()[32]);

    // createImageData(width, height) takes the absolute magnitude of the size arguments

    ImageData* imgdata1 = context2d()->createImageData(10, 20, exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    ImageData* imgdata2 = context2d()->createImageData(-10, 20, exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    ImageData* imgdata3 = context2d()->createImageData(10, -20, exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    ImageData* imgdata4 = context2d()->createImageData(-10, -20, exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    EXPECT_EQ((unsigned)800, imgdata1->data()->length());
    EXPECT_EQ((unsigned)800, imgdata2->data()->length());
    EXPECT_EQ((unsigned)800, imgdata3->data()->length());
    EXPECT_EQ((unsigned)800, imgdata4->data()->length());
}

} // namespace blink
