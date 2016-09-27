// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RecordingImageBufferSurface_h
#define RecordingImageBufferSurface_h

#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/ImageBufferSurface.h"
#include "public/platform/WebThread.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "wtf/LinkedStack.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefPtr.h"

class SkPicture;
class SkPictureRecorder;

namespace blink {

class ImageBuffer;
class RecordingImageBufferSurfaceTest;

class RecordingImageBufferFallbackSurfaceFactory {
public:
    virtual PassOwnPtr<ImageBufferSurface> createSurface(const IntSize&, OpacityMode) = 0;
    virtual ~RecordingImageBufferFallbackSurfaceFactory() { }
};

class PLATFORM_EXPORT RecordingImageBufferSurface : public ImageBufferSurface {
    WTF_MAKE_NONCOPYABLE(RecordingImageBufferSurface); WTF_MAKE_FAST_ALLOCATED(RecordingImageBufferSurface);
public:
    RecordingImageBufferSurface(const IntSize&, PassOwnPtr<RecordingImageBufferFallbackSurfaceFactory> fallbackFactory, OpacityMode = NonOpaque);
    ~RecordingImageBufferSurface() override;

    // Implementation of ImageBufferSurface interfaces
    SkCanvas* canvas() const override;
    PassRefPtr<SkPicture> getPicture() override;
    void willDrawVideo() override;
    void didDraw(const FloatRect&) override;
    bool isValid() const override { return true; }
    bool isRecording() const override { return !m_fallbackSurface; }
    void willAccessPixels() override;
    void willOverwriteCanvas() override;
    virtual void finalizeFrame(const FloatRect&);
    void setImageBuffer(ImageBuffer*) override;
    PassRefPtr<SkImage> newImageSnapshot() const override;
    void draw(GraphicsContext*, const FloatRect& destRect, const FloatRect& srcRect, SkXfermode::Mode) override;
    bool isExpensiveToPaint() override;
    void setHasExpensiveOp() override { m_currentFrameHasExpensiveOp = true; }

    // Passthroughs to fallback surface
    const SkBitmap& bitmap() override;
    bool restore() override;
    WebLayer* layer() const override;
    bool isAccelerated() const override;
    void setIsHidden(bool) override;

private:
    friend class RecordingImageBufferSurfaceTest; // for unit testing
    void fallBackToRasterCanvas();
    bool initializeCurrentFrame();
    bool finalizeFrameInternal();
    int approximateOpCount();

    OwnPtr<SkPictureRecorder> m_currentFrame;
    RefPtr<SkPicture> m_previousFrame;
    OwnPtr<ImageBufferSurface> m_fallbackSurface;
    ImageBuffer* m_imageBuffer;
    int m_initialSaveCount;
    int m_currentFramePixelCount;
    int m_previousFramePixelCount;
    bool m_frameWasCleared;
    bool m_didRecordDrawCommandsInCurrentFrame;
    bool m_currentFrameHasExpensiveOp;
    bool m_previousFrameHasExpensiveOp;
    OwnPtr<RecordingImageBufferFallbackSurfaceFactory> m_fallbackFactory;
};

} // namespace blink

#endif
