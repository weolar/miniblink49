// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "platform/graphics/RecordingImageBufferSurface.h"

#include "platform/graphics/ExpensiveCanvasHeuristicParameters.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/ImageBuffer.h"
#include "public/platform/Platform.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"

namespace blink {

RecordingImageBufferSurface::RecordingImageBufferSurface(const IntSize& size, PassOwnPtr<RecordingImageBufferFallbackSurfaceFactory> fallbackFactory, OpacityMode opacityMode)
    : ImageBufferSurface(size, opacityMode)
    , m_imageBuffer(0)
    , m_currentFramePixelCount(0)
    , m_previousFramePixelCount(0)
    , m_frameWasCleared(true)
    , m_didRecordDrawCommandsInCurrentFrame(false)
    , m_currentFrameHasExpensiveOp(false)
    , m_previousFrameHasExpensiveOp(false)
    , m_fallbackFactory(fallbackFactory)
{
    initializeCurrentFrame();
}

RecordingImageBufferSurface::~RecordingImageBufferSurface()
{ }

bool RecordingImageBufferSurface::initializeCurrentFrame()
{
    static SkRTreeFactory rTreeFactory;
    m_currentFrame = adoptPtr(new SkPictureRecorder);
    m_currentFrame->beginRecording(size().width(), size().height(), &rTreeFactory);
    if (m_imageBuffer) {
        m_imageBuffer->resetCanvas(m_currentFrame->getRecordingCanvas());
    }
    m_didRecordDrawCommandsInCurrentFrame = false;
    m_currentFrameHasExpensiveOp = false;
    m_currentFramePixelCount = 0;
    return true;
}

void RecordingImageBufferSurface::setImageBuffer(ImageBuffer* imageBuffer)
{
    m_imageBuffer = imageBuffer;
    if (m_currentFrame && m_imageBuffer) {
        m_imageBuffer->resetCanvas(m_currentFrame->getRecordingCanvas());
    }
    if (m_fallbackSurface) {
        m_fallbackSurface->setImageBuffer(imageBuffer);
    }
}

void RecordingImageBufferSurface::willAccessPixels()
{
    if (m_fallbackSurface)
        m_fallbackSurface->willAccessPixels();
    else
        fallBackToRasterCanvas();
}

void RecordingImageBufferSurface::fallBackToRasterCanvas()
{
    if (m_fallbackSurface) {
        ASSERT(!m_currentFrame);
        return;
    }

    m_fallbackSurface = m_fallbackFactory->createSurface(size(), opacityMode());
    m_fallbackSurface->setImageBuffer(m_imageBuffer);

    if (m_previousFrame) {
        m_previousFrame->playback(m_fallbackSurface->canvas());
        m_previousFrame.clear();
    }

    if (m_currentFrame) {
        RefPtr<SkPicture> currentPicture = adoptRef(m_currentFrame->endRecording());
        currentPicture->playback(m_fallbackSurface->canvas());
        m_currentFrame.clear();
    }

    if (m_imageBuffer) {
        m_imageBuffer->resetCanvas(m_fallbackSurface->canvas());
    }

}

PassRefPtr<SkImage> RecordingImageBufferSurface::newImageSnapshot() const
{
    if (!m_fallbackSurface)
        const_cast<RecordingImageBufferSurface*>(this)->fallBackToRasterCanvas();
    return m_fallbackSurface->newImageSnapshot();
}

SkCanvas* RecordingImageBufferSurface::canvas() const
{
    if (m_fallbackSurface)
        return m_fallbackSurface->canvas();

    ASSERT(m_currentFrame->getRecordingCanvas());
    return m_currentFrame->getRecordingCanvas();
}

PassRefPtr<SkPicture> RecordingImageBufferSurface::getPicture()
{
    if (m_fallbackSurface)
        return nullptr;

    bool canUsePicture = finalizeFrameInternal();
    m_imageBuffer->didFinalizeFrame();

    if (canUsePicture) {
        return m_previousFrame;
    }

    if (!m_fallbackSurface)
        fallBackToRasterCanvas();
    return nullptr;
}

void RecordingImageBufferSurface::finalizeFrame(const FloatRect &dirtyRect)
{
    if (m_fallbackSurface) {
        m_fallbackSurface->finalizeFrame(dirtyRect);
        return;
    }

    if (!finalizeFrameInternal())
        fallBackToRasterCanvas();
}

void RecordingImageBufferSurface::willOverwriteCanvas()
{
    m_frameWasCleared = true;
    m_previousFrame.clear();
    m_previousFrameHasExpensiveOp = false;
    m_previousFramePixelCount = 0;
    if (m_didRecordDrawCommandsInCurrentFrame) {
        // Discard previous draw commands
        adoptRef(m_currentFrame->endRecording());
        initializeCurrentFrame();
    }
}

void RecordingImageBufferSurface::didDraw(const FloatRect& rect)
{
    m_didRecordDrawCommandsInCurrentFrame = true;
    IntRect pixelBounds = enclosingIntRect(rect);
    m_currentFramePixelCount += pixelBounds.width() * pixelBounds.height();
}

bool RecordingImageBufferSurface::finalizeFrameInternal()
{
    ASSERT(!m_fallbackSurface);
    ASSERT(m_currentFrame);
    ASSERT(m_currentFrame->getRecordingCanvas());

    if (!m_imageBuffer->isDirty()) {
        if (!m_previousFrame) {
            // Create an initial blank frame
            m_previousFrame = adoptRef(m_currentFrame->endRecording());
            initializeCurrentFrame();
        }
        return m_currentFrame;
    }

    if (!m_frameWasCleared || m_currentFrame->getRecordingCanvas()->getSaveCount() > ExpensiveCanvasHeuristicParameters::ExpensiveRecordingStackDepth) {
        return false;
    }

    m_previousFrame = adoptRef(m_currentFrame->endRecording());
    m_previousFrameHasExpensiveOp = m_currentFrameHasExpensiveOp;
    m_previousFramePixelCount = m_currentFramePixelCount;
    if (!initializeCurrentFrame())
        return false;

    m_frameWasCleared = false;
    return true;
}

void RecordingImageBufferSurface::willDrawVideo()
{
    // Video draws need to be synchronous
    if (!m_fallbackSurface)
        fallBackToRasterCanvas();
}

void RecordingImageBufferSurface::draw(GraphicsContext* context, const FloatRect& destRect, const FloatRect& srcRect, SkXfermode::Mode op)
{
    if (m_fallbackSurface) {
        m_fallbackSurface->draw(context, destRect, srcRect, op);
        return;
    }

    RefPtr<SkPicture> picture = getPicture();
    if (picture) {
        context->compositePicture(picture.get(), destRect, srcRect, op);
    } else {
        ImageBufferSurface::draw(context, destRect, srcRect, op);
    }
}

bool RecordingImageBufferSurface::isExpensiveToPaint()
{
    if (m_fallbackSurface)
        return m_fallbackSurface->isExpensiveToPaint();

    if (m_didRecordDrawCommandsInCurrentFrame) {
        if (m_currentFrameHasExpensiveOp)
            return true;

        if (m_currentFramePixelCount >= (size().width() * size().height() * ExpensiveCanvasHeuristicParameters::ExpensiveOverdrawThreshold))
            return true;

        if (m_frameWasCleared)
            return false; // early exit because previous frame is overdrawn
    }

    if (m_previousFrame) {
        if (m_previousFrameHasExpensiveOp)
            return true;

        if (m_previousFramePixelCount >= (size().width() * size().height() * ExpensiveCanvasHeuristicParameters::ExpensiveOverdrawThreshold))
            return true;
    }

    return false;
}

// Fallback passthroughs

const SkBitmap& RecordingImageBufferSurface::bitmap()
{
    if (m_fallbackSurface)
        return m_fallbackSurface->bitmap();
    return ImageBufferSurface::bitmap();
}

bool RecordingImageBufferSurface::restore()
{
    if (m_fallbackSurface)
        return m_fallbackSurface->restore();
    return ImageBufferSurface::restore();
}

WebLayer* RecordingImageBufferSurface::layer() const
{
    if (m_fallbackSurface)
        return m_fallbackSurface->layer();
    return ImageBufferSurface::layer();
}

bool RecordingImageBufferSurface::isAccelerated() const
{
    if (m_fallbackSurface)
        return m_fallbackSurface->isAccelerated();
    return ImageBufferSurface::isAccelerated();
}

void RecordingImageBufferSurface::setIsHidden(bool hidden)
{
    if (m_fallbackSurface)
        m_fallbackSurface->setIsHidden(hidden);
    else
        ImageBufferSurface::setIsHidden(hidden);
}

} // namespace blink
