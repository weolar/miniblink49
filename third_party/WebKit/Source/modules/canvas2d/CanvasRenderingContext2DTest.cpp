// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/canvas2d/CanvasRenderingContext2D.h"

#include "core/frame/FrameView.h"
#include "core/frame/ImageBitmap.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLDocument.h"
#include "core/html/ImageData.h"
#include "core/loader/EmptyClients.h"
#include "core/testing/DummyPageHolder.h"
#include "modules/canvas2d/CanvasGradient.h"
#include "modules/canvas2d/CanvasPattern.h"
#include "modules/webgl/WebGLRenderingContext.h"
#include "platform/graphics/ExpensiveCanvasHeuristicParameters.h"
#include "platform/graphics/RecordingImageBufferSurface.h"
#include "platform/graphics/StaticBitmapImage.h"
#include "platform/graphics/UnacceleratedImageBufferSurface.h"
#include "third_party/skia/include/core/SkSurface.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Mock;

namespace blink {

enum BitmapOpacity {
    OpaqueBitmap,
    TransparentBitmap
};

class FakeImageSource : public CanvasImageSource {
public:
    FakeImageSource(IntSize, BitmapOpacity);

    PassRefPtr<Image> getSourceImageForCanvas(SourceImageMode, SourceImageStatus*) const override;

    bool wouldTaintOrigin(SecurityOrigin* destinationSecurityOrigin) const override { return false; }
    FloatSize elementSize() const override { return FloatSize(m_size); }
    bool isOpaque() const override { return m_isOpaque; }

    ~FakeImageSource() override { }

private:
    IntSize m_size;
    RefPtr<Image> m_image;
    bool m_isOpaque;
};

FakeImageSource::FakeImageSource(IntSize size, BitmapOpacity opacity)
    : m_size(size)
    , m_isOpaque(opacity == OpaqueBitmap)
{
    SkAutoTUnref<SkSurface> surface(SkSurface::NewRasterN32Premul(m_size.width(), m_size.height()));
    surface->getCanvas()->clear(opacity == OpaqueBitmap ? SK_ColorWHITE : SK_ColorTRANSPARENT);
    RefPtr<SkImage> image = adoptRef(surface->newImageSnapshot());
    m_image = StaticBitmapImage::create(image);
}

PassRefPtr<Image> FakeImageSource::getSourceImageForCanvas(SourceImageMode, SourceImageStatus* status) const
{
    if (status)
        *status = NormalSourceImageStatus;
    return m_image;
}

//============================================================================

class CanvasRenderingContext2DTest : public ::testing::Test {
protected:
    CanvasRenderingContext2DTest();
    void SetUp() override;

    DummyPageHolder& page() const { return *m_dummyPageHolder; }
    HTMLDocument& document() const { return *m_document; }
    HTMLCanvasElement& canvasElement() const { return *m_canvasElement; }
    CanvasRenderingContext2D* context2d() const { return static_cast<CanvasRenderingContext2D*>(canvasElement().renderingContext()); }

    void createContext(OpacityMode);

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
    RefPtrWillBePersistent<HTMLDocument> m_document;
    RefPtrWillBePersistent<HTMLCanvasElement> m_canvasElement;

    class WrapGradients final : public NoBaseWillBeGarbageCollectedFinalized<WrapGradients> {
    public:
        static PassOwnPtrWillBeRawPtr<WrapGradients> create()
        {
            return adoptPtrWillBeNoop(new WrapGradients);
        }

        DEFINE_INLINE_TRACE()
        {
            visitor->trace(m_opaqueGradient);
            visitor->trace(m_alphaGradient);
        }

        StringOrCanvasGradientOrCanvasPattern m_opaqueGradient;
        StringOrCanvasGradientOrCanvasPattern m_alphaGradient;
    };

    // TODO(Oilpan): avoid tedious part-object wrapper by supporting on-heap ::testing::Tests.
    OwnPtrWillBePersistent<WrapGradients> m_wrapGradients;

protected:
    // Pre-canned objects for testing
    Persistent<ImageData> m_fullImageData;
    Persistent<ImageData> m_partialImageData;
    FakeImageSource m_opaqueBitmap;
    FakeImageSource m_alphaBitmap;

    StringOrCanvasGradientOrCanvasPattern& opaqueGradient() { return m_wrapGradients->m_opaqueGradient; }
    StringOrCanvasGradientOrCanvasPattern& alphaGradient() { return m_wrapGradients->m_alphaGradient; }
};

CanvasRenderingContext2DTest::CanvasRenderingContext2DTest()
    : m_wrapGradients(WrapGradients::create())
    , m_opaqueBitmap(IntSize(10, 10), OpaqueBitmap)
    , m_alphaBitmap(IntSize(10, 10), TransparentBitmap)
{ }

void CanvasRenderingContext2DTest::createContext(OpacityMode opacityMode)
{
    String canvasType("2d");
    CanvasContextCreationAttributes attributes;
    attributes.setAlpha(opacityMode == NonOpaque);
    m_canvasElement->getCanvasRenderingContext(canvasType, attributes);
}

void CanvasRenderingContext2DTest::SetUp()
{
    Page::PageClients pageClients;
    fillWithEmptyClients(pageClients);
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600), &pageClients);
    m_document = toHTMLDocument(&m_dummyPageHolder->document());
    m_document->documentElement()->setInnerHTML("<body><canvas id='c'></canvas></body>", ASSERT_NO_EXCEPTION);
    m_document->view()->updateAllLifecyclePhases();
    m_canvasElement = toHTMLCanvasElement(m_document->getElementById("c"));

    m_fullImageData = ImageData::create(IntSize(10, 10));
    m_partialImageData = ImageData::create(IntSize(2, 2));

    NonThrowableExceptionState exceptionState;
    CanvasGradient* opaqueGradient = CanvasGradient::create(FloatPoint(0, 0), FloatPoint(10, 0));
    opaqueGradient->addColorStop(0, String("green"), exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    opaqueGradient->addColorStop(1, String("blue"), exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    this->opaqueGradient().setCanvasGradient(opaqueGradient);

    CanvasGradient* alphaGradient = CanvasGradient::create(FloatPoint(0, 0), FloatPoint(10, 0));
    alphaGradient->addColorStop(0, String("green"), exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    alphaGradient->addColorStop(1, String("rgba(0, 0, 255, 0.5)"), exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    StringOrCanvasGradientOrCanvasPattern wrappedAlphaGradient;
    this->alphaGradient().setCanvasGradient(alphaGradient);
}

//============================================================================

class MockImageBufferSurfaceForOverwriteTesting : public UnacceleratedImageBufferSurface {
public:
    MockImageBufferSurfaceForOverwriteTesting(const IntSize& size, OpacityMode mode) : UnacceleratedImageBufferSurface(size, mode) { }
    ~MockImageBufferSurfaceForOverwriteTesting() override { }
    bool isRecording() const override { return true; } // otherwise overwrites are not tracked

    MOCK_METHOD0(willOverwriteCanvas, void());
};

//============================================================================

class MockCanvasObserver final : public NoBaseWillBeGarbageCollectedFinalized<MockCanvasObserver>, public CanvasObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(MockCanvasObserver);
public:
    static PassOwnPtrWillBeRawPtr<MockCanvasObserver> create()
    {
        return adoptPtrWillBeNoop(new MockCanvasObserver);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        CanvasObserver::trace(visitor);
    }


    virtual ~MockCanvasObserver() { }
    MOCK_METHOD2(canvasChanged, void(HTMLCanvasElement*, const FloatRect&));
    MOCK_METHOD1(canvasResized, void(HTMLCanvasElement*));
#if !ENABLE(OILPAN)
    void canvasDestroyed(HTMLCanvasElement*) override { }
#endif
};

//============================================================================

#define TEST_OVERDRAW_SETUP(EXPECTED_OVERDRAWS) \
        OwnPtr<MockImageBufferSurfaceForOverwriteTesting> mockSurface = adoptPtr(new MockImageBufferSurfaceForOverwriteTesting(IntSize(10, 10), NonOpaque)); \
        MockImageBufferSurfaceForOverwriteTesting* surfacePtr = mockSurface.get(); \
        canvasElement().createImageBufferUsingSurfaceForTesting(mockSurface.release()); \
        EXPECT_CALL(*surfacePtr, willOverwriteCanvas()).Times(EXPECTED_OVERDRAWS); \
        context2d()->save();

#define TEST_OVERDRAW_FINALIZE \
        context2d()->restore(); \
        Mock::VerifyAndClearExpectations(surfacePtr);

#define TEST_OVERDRAW_1(EXPECTED_OVERDRAWS, CALL1) \
    do { \
        TEST_OVERDRAW_SETUP(EXPECTED_OVERDRAWS) \
        context2d()->CALL1; \
        TEST_OVERDRAW_FINALIZE \
    } while (0)

#define TEST_OVERDRAW_2(EXPECTED_OVERDRAWS, CALL1, CALL2) \
    do { \
        TEST_OVERDRAW_SETUP(EXPECTED_OVERDRAWS) \
        context2d()->CALL1; \
        context2d()->CALL2; \
        TEST_OVERDRAW_FINALIZE \
    } while (0)

#define TEST_OVERDRAW_3(EXPECTED_OVERDRAWS, CALL1, CALL2, CALL3) \
    do { \
        TEST_OVERDRAW_SETUP(EXPECTED_OVERDRAWS) \
        context2d()->CALL1; \
        context2d()->CALL2; \
        context2d()->CALL3; \
        TEST_OVERDRAW_FINALIZE \
    } while (0)

#define TEST_OVERDRAW_4(EXPECTED_OVERDRAWS, CALL1, CALL2, CALL3, CALL4) \
    do { \
        TEST_OVERDRAW_SETUP(EXPECTED_OVERDRAWS) \
        context2d()->CALL1; \
        context2d()->CALL2; \
        context2d()->CALL3; \
        context2d()->CALL4; \
        TEST_OVERDRAW_FINALIZE \
    } while (0)

//============================================================================

class MockSurfaceFactory : public RecordingImageBufferFallbackSurfaceFactory {
public:
    enum FallbackExpectation {
        ExpectFallback,
        ExpectNoFallback
    };
    static PassOwnPtr<MockSurfaceFactory> create(FallbackExpectation expectation) { return adoptPtr(new MockSurfaceFactory(expectation)); }

    PassOwnPtr<ImageBufferSurface> createSurface(const IntSize& size, OpacityMode mode) override
    {
        EXPECT_EQ(ExpectFallback, m_expectation);
        m_didFallback = true;
        return adoptPtr(new UnacceleratedImageBufferSurface(size, mode));
    }

    ~MockSurfaceFactory() override
    {
        if (m_expectation == ExpectFallback) {
            EXPECT_TRUE(m_didFallback);
        }
    }

private:
    MockSurfaceFactory(FallbackExpectation expectation)
        : m_expectation(expectation)
        , m_didFallback(false) { }

    FallbackExpectation m_expectation;
    bool m_didFallback;
};

//============================================================================

TEST_F(CanvasRenderingContext2DTest, detectOverdrawWithFillRect)
{
    createContext(NonOpaque);

    TEST_OVERDRAW_1(1, fillRect(-1, -1, 12, 12));
    TEST_OVERDRAW_1(1, fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_1(0, strokeRect(0, 0, 10, 10)); // stroking instead of filling does not overwrite
    TEST_OVERDRAW_2(0, setGlobalAlpha(0.5f), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_1(0, fillRect(0, 0, 9, 9));
    TEST_OVERDRAW_2(0, translate(1, 1), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(1, translate(1, 1), fillRect(-1, -1, 10, 10));
    TEST_OVERDRAW_2(1, setFillStyle(opaqueGradient()), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(0, setFillStyle(alphaGradient()), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_3(0, setGlobalAlpha(0.5), setFillStyle(opaqueGradient()), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_3(1, setGlobalAlpha(0.5f), setGlobalCompositeOperation(String("copy")), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(1, setGlobalCompositeOperation(String("copy")), fillRect(0, 0, 9, 9));
    TEST_OVERDRAW_3(0, rect(0, 0, 5, 5), clip(), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_4(0, rect(0, 0, 5, 5), clip(), setGlobalCompositeOperation(String("copy")), fillRect(0, 0, 10, 10));
}

TEST_F(CanvasRenderingContext2DTest, detectOverdrawWithClearRect)
{
    createContext(NonOpaque);

    TEST_OVERDRAW_1(1, clearRect(0, 0, 10, 10));
    TEST_OVERDRAW_1(0, clearRect(0, 0, 9, 9));
    TEST_OVERDRAW_2(1, setGlobalAlpha(0.5f), clearRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(1, setFillStyle(alphaGradient()), clearRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(0, translate(1, 1), clearRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(1, translate(1, 1), clearRect(-1, -1, 10, 10));
    TEST_OVERDRAW_2(1, setGlobalCompositeOperation(String("destination-in")), clearRect(0, 0, 10, 10)); // composite op ignored
    TEST_OVERDRAW_3(0, rect(0, 0, 5, 5), clip(), clearRect(0, 0, 10, 10));
}

TEST_F(CanvasRenderingContext2DTest, detectOverdrawWithDrawImage)
{
    createContext(NonOpaque);
    NonThrowableExceptionState exceptionState;

    TEST_OVERDRAW_1(1, drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 0, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_1(1, drawImage(&m_opaqueBitmap, 0, 0, 1, 1, 0, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_2(0, setGlobalAlpha(0.5f), drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 0, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_1(0, drawImage(&m_alphaBitmap, 0, 0, 10, 10, 0, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_2(0, setGlobalAlpha(0.5f), drawImage(&m_alphaBitmap, 0, 0, 10, 10, 0, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_1(0, drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 1, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_1(0, drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 0, 0, 9, 9, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_1(1, drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 0, 0, 11, 11, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_2(1, translate(-1, 0), drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 1, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_2(0, translate(-1, 0), drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 0, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_2(0, setFillStyle(opaqueGradient()), drawImage(&m_alphaBitmap, 0, 0, 10, 10, 0, 0, 10, 10, exceptionState)); // fillStyle ignored by drawImage
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_2(1, setFillStyle(alphaGradient()), drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 0, 0, 10, 10, exceptionState)); // fillStyle ignored by drawImage
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_2(1, setGlobalCompositeOperation(String("copy")), drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 1, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
    TEST_OVERDRAW_3(0, rect(0, 0, 5, 5), clip(), drawImage(&m_opaqueBitmap, 0, 0, 10, 10, 0, 0, 10, 10, exceptionState));
    EXPECT_FALSE(exceptionState.hadException());
}

TEST_F(CanvasRenderingContext2DTest, detectOverdrawWithPutImageData)
{
    createContext(NonOpaque);

    // Test putImageData
    TEST_OVERDRAW_1(1, putImageData(m_fullImageData.get(), 0, 0));
    TEST_OVERDRAW_1(1, putImageData(m_fullImageData.get(), 0, 0, 0, 0, 10, 10));
    TEST_OVERDRAW_1(0, putImageData(m_fullImageData.get(), 0, 0, 1, 1, 8, 8));
    TEST_OVERDRAW_2(1, setGlobalAlpha(0.5f), putImageData(m_fullImageData.get(), 0, 0)); // alpha has no effect
    TEST_OVERDRAW_1(0, putImageData(m_partialImageData.get(), 0, 0));
    TEST_OVERDRAW_2(1, translate(1, 1), putImageData(m_fullImageData.get(), 0, 0)); // ignores tranforms
    TEST_OVERDRAW_1(0, putImageData(m_fullImageData.get(), 1, 0));
    TEST_OVERDRAW_3(1, rect(0, 0, 5, 5), clip(), putImageData(m_fullImageData.get(), 0, 0)); // ignores clip
}

TEST_F(CanvasRenderingContext2DTest, detectOverdrawWithCompositeOperations)
{
    createContext(NonOpaque);

    // Test composite operators with an opaque rect that covers the entire canvas
    // Note: all the untested composite operations take the same code path as source-in,
    // which assumes that the destination may not be overwritten
    TEST_OVERDRAW_2(1, setGlobalCompositeOperation(String("clear")), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(1, setGlobalCompositeOperation(String("copy")), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(1, setGlobalCompositeOperation(String("source-over")), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_2(0, setGlobalCompositeOperation(String("source-in")), fillRect(0, 0, 10, 10));
    // Test composite operators with a transparent rect that covers the entire canvas
    TEST_OVERDRAW_3(1, setGlobalAlpha(0.5f), setGlobalCompositeOperation(String("clear")), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_3(1, setGlobalAlpha(0.5f), setGlobalCompositeOperation(String("copy")), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_3(0, setGlobalAlpha(0.5f), setGlobalCompositeOperation(String("source-over")), fillRect(0, 0, 10, 10));
    TEST_OVERDRAW_3(0, setGlobalAlpha(0.5f), setGlobalCompositeOperation(String("source-in")), fillRect(0, 0, 10, 10));
    // Test composite operators with an opaque rect that does not cover the entire canvas
    TEST_OVERDRAW_2(0, setGlobalCompositeOperation(String("clear")), fillRect(0, 0, 5, 5));
    TEST_OVERDRAW_2(1, setGlobalCompositeOperation(String("copy")), fillRect(0, 0, 5, 5));
    TEST_OVERDRAW_2(0, setGlobalCompositeOperation(String("source-over")), fillRect(0, 0, 5, 5));
    TEST_OVERDRAW_2(0, setGlobalCompositeOperation(String("source-in")), fillRect(0, 0, 5, 5));
}

TEST_F(CanvasRenderingContext2DTest, NoLayerPromotionByDefault)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, NoLayerPromotionUnderOverdrawLimit)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->setGlobalAlpha(0.5f); // To prevent overdraw optimization
    for (int i = 0; i < ExpensiveCanvasHeuristicParameters::ExpensiveOverdrawThreshold - 1; i++) {
        context2d()->fillRect(0, 0, 10, 10);
    }

    EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, LayerPromotionOverOverdrawLimit)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->setGlobalAlpha(0.5f); // To prevent overdraw optimization
    for (int i = 0; i < ExpensiveCanvasHeuristicParameters::ExpensiveOverdrawThreshold; i++) {
        context2d()->fillRect(0, 0, 10, 10);
    }

    EXPECT_TRUE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, NoLayerPromotionUnderImageSizeRatioLimit)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    NonThrowableExceptionState exceptionState;
    RefPtrWillBeRawPtr<Element> sourceCanvasElement = document().createElement("canvas", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    HTMLCanvasElement* sourceCanvas = static_cast<HTMLCanvasElement*>(sourceCanvasElement.get());
    IntSize sourceSize(10, 10 * ExpensiveCanvasHeuristicParameters::ExpensiveImageSizeRatio);
    OwnPtr<UnacceleratedImageBufferSurface> sourceSurface = adoptPtr(new UnacceleratedImageBufferSurface(sourceSize, NonOpaque));
    sourceCanvas->createImageBufferUsingSurfaceForTesting(sourceSurface.release());

    // Go through an ImageBitmap to avoid triggering a display list fallback
    RefPtrWillBeRawPtr<ImageBitmap> sourceImageBitmap = ImageBitmap::create(sourceCanvas, IntRect(IntPoint(0, 0), sourceSize));

    context2d()->drawImage(sourceImageBitmap.get(), 0, 0, 1, 1, 0, 0, 1, 1, exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, LayerPromotionOverImageSizeRatioLimit)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    NonThrowableExceptionState exceptionState;
    RefPtrWillBeRawPtr<Element> sourceCanvasElement = document().createElement("canvas", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    HTMLCanvasElement* sourceCanvas = static_cast<HTMLCanvasElement*>(sourceCanvasElement.get());
    IntSize sourceSize(10, 10 * ExpensiveCanvasHeuristicParameters::ExpensiveImageSizeRatio + 1);
    OwnPtr<UnacceleratedImageBufferSurface> sourceSurface = adoptPtr(new UnacceleratedImageBufferSurface(sourceSize, NonOpaque));
    sourceCanvas->createImageBufferUsingSurfaceForTesting(sourceSurface.release());

    // Go through an ImageBitmap to avoid triggering a display list fallback
    RefPtrWillBeRawPtr<ImageBitmap> sourceImageBitmap = ImageBitmap::create(sourceCanvas, IntRect(IntPoint(0, 0), sourceSize));

    context2d()->drawImage(sourceImageBitmap.get(), 0, 0, 1, 1, 0, 0, 1, 1, exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    EXPECT_TRUE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, NoLayerPromotionUnderExpensivePathPointCount)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->beginPath();
    context2d()->moveTo(7, 5);
    for (int i = 1; i < ExpensiveCanvasHeuristicParameters::ExpensivePathPointCount-1; i++) {
        float angleRad = twoPiFloat * i / (ExpensiveCanvasHeuristicParameters::ExpensivePathPointCount - 1);
        context2d()->lineTo(5 + 2 * cos(angleRad), 5 + 2 * sin(angleRad));
    }
    context2d()->fill();

    EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, LayerPromotionOverExpensivePathPointCount)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->beginPath();
    context2d()->moveTo(7, 5);
    for (int i = 1; i < ExpensiveCanvasHeuristicParameters::ExpensivePathPointCount + 1; i++) {
        float angleRad = twoPiFloat * i / (ExpensiveCanvasHeuristicParameters::ExpensivePathPointCount + 1);
        context2d()->lineTo(5 + 2 * cos(angleRad), 5 + 2 * sin(angleRad));
    }
    context2d()->fill();

    EXPECT_TRUE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, LayerPromotionWhenPathIsConcave)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->beginPath();
    context2d()->moveTo(1, 1);
    context2d()->lineTo(5, 5);
    context2d()->lineTo(9, 1);
    context2d()->lineTo(5, 9);
    context2d()->fill();

    if (ExpensiveCanvasHeuristicParameters::ConcavePathsAreExpensive) {
        EXPECT_TRUE(canvasElement().shouldBeDirectComposited());
    } else {
        EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
    }
}

TEST_F(CanvasRenderingContext2DTest, NoLayerPromotionWithRectangleClip)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->beginPath();
    context2d()->rect(1, 1, 2, 2);
    context2d()->clip();
    context2d()->fillRect(0, 0, 4, 4);

    EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, LayerPromotionWithComplexClip)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->beginPath();
    context2d()->moveTo(1, 1);
    context2d()->lineTo(5, 5);
    context2d()->lineTo(9, 1);
    context2d()->lineTo(5, 9);
    context2d()->clip();
    context2d()->fillRect(0, 0, 4, 4);

    if (ExpensiveCanvasHeuristicParameters::ComplexClipsAreExpensive) {
        EXPECT_TRUE(canvasElement().shouldBeDirectComposited());
    } else {
        EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
    }
}

TEST_F(CanvasRenderingContext2DTest, LayerPromotionWithBlurredShadow)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->setShadowColor(String("red"));
    context2d()->setShadowBlur(1.0f);
    context2d()->fillRect(1, 1, 1, 1);

    if (ExpensiveCanvasHeuristicParameters::BlurredShadowsAreExpensive) {
        EXPECT_TRUE(canvasElement().shouldBeDirectComposited());
    } else {
        EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
    }
}

TEST_F(CanvasRenderingContext2DTest, NoLayerPromotionWithSharpShadow)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->setShadowColor(String("red"));
    context2d()->setShadowOffsetX(1.0f);
    context2d()->fillRect(1, 1, 1, 1);

    EXPECT_FALSE(canvasElement().shouldBeDirectComposited());
}

TEST_F(CanvasRenderingContext2DTest, NoFallbackWithSmallState)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectNoFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->fillRect(0, 0, 1, 1); // To have a non-empty dirty rect
    for (int i = 0; i < ExpensiveCanvasHeuristicParameters::ExpensiveRecordingStackDepth - 1; ++i) {
        context2d()->save();
        context2d()->translate(1.0f, 0.0f);
    }
    canvasElement().doDeferredPaintInvalidation(); // To close the current frame
}

TEST_F(CanvasRenderingContext2DTest, FallbackWithLargeState)
{
    createContext(NonOpaque);
    OwnPtr<RecordingImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(IntSize(10, 10), MockSurfaceFactory::create(MockSurfaceFactory::ExpectFallback), NonOpaque));
    canvasElement().createImageBufferUsingSurfaceForTesting(surface.release());

    context2d()->fillRect(0, 0, 1, 1); // To have a non-empty dirty rect
    for (int i = 0; i < ExpensiveCanvasHeuristicParameters::ExpensiveRecordingStackDepth; ++i) {
        context2d()->save();
        context2d()->translate(1.0f, 0.0f);
    }
    canvasElement().doDeferredPaintInvalidation(); // To close the current frame
}

TEST_F(CanvasRenderingContext2DTest, CanvasObserver)
{
    createContext(NonOpaque);
    OwnPtrWillBeRawPtr<MockCanvasObserver> observer = MockCanvasObserver::create();
    canvasElement().addObserver(observer.get());

    // The canvasChanged notification must be immediate, and not deferred until paint time
    // because offscreen canvases, which are not painted, also need to emit notifications.
    EXPECT_CALL(*observer, canvasChanged(&canvasElement(), FloatRect(0, 0, 1, 1))).Times(1);
    context2d()->fillRect(0, 0, 1, 1);
    Mock::VerifyAndClearExpectations(observer.get());

    canvasElement().removeObserver(observer.get());
}

} // namespace blink
