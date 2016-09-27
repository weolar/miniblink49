// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/ImageQualityController.h"

#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutTestHelper.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"

#include <gtest/gtest.h>

namespace blink {

class ImageQualityControllerTest : public RenderingTest {
protected:
    ImageQualityController* controller() { return m_controller; }

private:
    virtual void SetUp() override
    {
        m_controller = ImageQualityController::imageQualityController();
        RenderingTest::SetUp();
    }
    virtual void TearDown() override
    {
    }
    ImageQualityController* m_controller;
};

TEST_F(ImageQualityControllerTest, RegularImage)
{
    setBodyInnerHTML("<img src='myimage'></img>");
    LayoutObject* obj = document().body()->firstChild()->layoutObject();

    EXPECT_EQ(InterpolationDefault, controller()->chooseInterpolationQuality(nullptr, obj, nullptr, nullptr, LayoutSize()));
}

TEST_F(ImageQualityControllerTest, ImageRenderingPixelated)
{
    setBodyInnerHTML("<img src='myimage' style='image-rendering: pixelated'></img>");
    LayoutObject* obj = document().body()->firstChild()->layoutObject();

    EXPECT_EQ(InterpolationNone, controller()->chooseInterpolationQuality(nullptr, obj, nullptr, nullptr, LayoutSize()));
}

#if !USE(LOW_QUALITY_IMAGE_INTERPOLATION)

class TestImageAnimated : public Image {
public:
    bool maybeAnimated() override { return true; }
    bool currentFrameKnownToBeOpaque() override { return false; }
    IntSize size() const override { return IntSize(); }
    void destroyDecodedData(bool) override { }
    void draw(SkCanvas*, const SkPaint&, const FloatRect& dstRect, const FloatRect& srcRect, RespectImageOrientationEnum, ImageClampingMode) override { }
};

TEST_F(ImageQualityControllerTest, ImageMaybeAnimated)
{
    setBodyInnerHTML("<img src='myimage'></img>");
    LayoutImage* img = toLayoutImage(document().body()->firstChild()->layoutObject());

    RefPtr<TestImageAnimated> testImage = adoptRef(new TestImageAnimated);
    EXPECT_EQ(InterpolationMedium, controller()->chooseInterpolationQuality(nullptr, img, testImage.get(), nullptr, LayoutSize()));
}

class TestImageWithContrast : public Image {
public:
    bool maybeAnimated() override { return true; }
    bool currentFrameKnownToBeOpaque() override { return false; }
    IntSize size() const override { return IntSize(); }
    void destroyDecodedData(bool) override { }
    void draw(SkCanvas*, const SkPaint&, const FloatRect& dstRect, const FloatRect& srcRect, RespectImageOrientationEnum, ImageClampingMode) override { }

    bool isBitmapImage() const override { return true; }
};

TEST_F(ImageQualityControllerTest, LowQualityFilterForContrast)
{
    setBodyInnerHTML("<img src='myimage' style='image-rendering: -webkit-optimize-contrast'></img>");
    LayoutImage* img = toLayoutImage(document().body()->firstChild()->layoutObject());

    RefPtr<TestImageWithContrast> testImage = adoptRef(new TestImageWithContrast);
    EXPECT_EQ(InterpolationLow, controller()->chooseInterpolationQuality(nullptr, img, testImage.get(), testImage.get(), LayoutSize()));
}

class TestImageLowQuality : public Image {
public:
    bool maybeAnimated() override { return true; }
    bool currentFrameKnownToBeOpaque() override { return false; }
    IntSize size() const override { return IntSize(1, 1); }
    void destroyDecodedData(bool) override { }
    void draw(SkCanvas*, const SkPaint&, const FloatRect& dstRect, const FloatRect& srcRect, RespectImageOrientationEnum, ImageClampingMode) override { }

    bool isBitmapImage() const override { return true; }
};

TEST_F(ImageQualityControllerTest, MediumQualityFilterForUnscaledImage)
{
    setBodyInnerHTML("<img src='myimage'></img>");
    LayoutImage* img = toLayoutImage(document().body()->firstChild()->layoutObject());

    RefPtr<TestImageLowQuality> testImage = adoptRef(new TestImageLowQuality);
    OwnPtr<DisplayItemList> displayItemList = DisplayItemList::create();
    GraphicsContext context(displayItemList.get());
    EXPECT_EQ(InterpolationMedium, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(1, 1)));
}

class MockTimer : public Timer<ImageQualityController> {
    typedef void (ImageQualityController::*TimerFiredFunction)(Timer*);
public:
    MockTimer(ImageQualityController* o, TimerFiredFunction f)
    : Timer<ImageQualityController>(o, f)
    {
    }

    void fire()
    {
        this->Timer<ImageQualityController>::fired();
        stop();
    }
};

TEST_F(ImageQualityControllerTest, LowQualityFilterForLiveResize)
{
    MockTimer* mockTimer = new MockTimer(controller(), &ImageQualityController::highQualityRepaintTimerFired);
    controller()->setTimer(mockTimer);
    setBodyInnerHTML("<img src='myimage'></img>");
    LayoutImage* img = toLayoutImage(document().body()->firstChild()->layoutObject());

    RefPtr<TestImageLowQuality> testImage = adoptRef(new TestImageLowQuality);
    OwnPtr<DisplayItemList> displayItemList = DisplayItemList::create();
    GraphicsContext context(displayItemList.get());

    // Start a resize
    document().frame()->view()->willStartLiveResize();
    EXPECT_EQ(InterpolationLow, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(2, 2)));

    document().frame()->view()->willEndLiveResize();

    // End of live resize, but timer has not fired. Therefore paint at non-low quality.
    EXPECT_EQ(InterpolationMedium, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(3, 3)));

    // Start another resize
    document().frame()->view()->willStartLiveResize();
    EXPECT_EQ(InterpolationLow, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(3, 3)));

    // While still in resize, expire the timer.
    document().frame()->view()->willEndLiveResize();

    mockTimer->fire();
    // End of live resize, and timer has fired. Therefore paint at non-low quality, even though the size has changed.
    EXPECT_EQ(InterpolationMedium, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(4, 4)));
}

TEST_F(ImageQualityControllerTest, LowQualityFilterForResizingImage)
{
    MockTimer* mockTimer = new MockTimer(controller(), &ImageQualityController::highQualityRepaintTimerFired);
    controller()->setTimer(mockTimer);
    setBodyInnerHTML("<img src='myimage'></img>");
    LayoutImage* img = toLayoutImage(document().body()->firstChild()->layoutObject());

    RefPtr<TestImageLowQuality> testImage = adoptRef(new TestImageLowQuality);
    OwnPtr<DisplayItemList> displayItemList = DisplayItemList::create();
    GraphicsContext context(displayItemList.get());

    // Paint once. This will kick off a timer to see if we resize it during that timer's execution.
    EXPECT_EQ(InterpolationMedium, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(2, 2)));

    // Go into low-quality mode now that the size changed.
    EXPECT_EQ(InterpolationLow, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(3, 3)));

    // Stay in low-quality mode since the size changed again.
    EXPECT_EQ(InterpolationLow, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(4, 4)));

    mockTimer->fire();
    // The timer fired before painting at another size, so this doesn't count as animation. Therefore not painting at low quality.
    EXPECT_EQ(InterpolationMedium, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(4, 4)));
}

TEST_F(ImageQualityControllerTest, DontKickTheAnimationTimerWhenPaintingAtTheSameSize)
{
    MockTimer* mockTimer = new MockTimer(controller(), &ImageQualityController::highQualityRepaintTimerFired);
    controller()->setTimer(mockTimer);
    setBodyInnerHTML("<img src='myimage'></img>");
    LayoutImage* img = toLayoutImage(document().body()->firstChild()->layoutObject());

    RefPtr<TestImageLowQuality> testImage = adoptRef(new TestImageLowQuality);
    OwnPtr<DisplayItemList> displayItemList = DisplayItemList::create();
    GraphicsContext context(displayItemList.get());

    // Paint once. This will kick off a timer to see if we resize it during that timer's execution.
    EXPECT_EQ(InterpolationMedium, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(2, 2)));

    // Go into low-quality mode now that the size changed.
    EXPECT_EQ(InterpolationLow, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(3, 3)));

    // Stay in low-quality mode since the size changed again.
    EXPECT_EQ(InterpolationLow, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(4, 4)));

    mockTimer->stop();
    EXPECT_FALSE(mockTimer->isActive());
    // Painted at the same size, so even though timer is still executing, don't go to low quality.
    EXPECT_EQ(InterpolationLow, controller()->chooseInterpolationQuality(&context, img, testImage.get(), testImage.get(), LayoutSize(4, 4)));
    // Check that the timer was not kicked. It should not have been, since the image was painted at the same size as last time.
    EXPECT_FALSE(mockTimer->isActive());
}

#endif

} // namespace blink
