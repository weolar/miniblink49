/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Torch Mobile (Beijing) Co. Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ImageBuffer_h
#define ImageBuffer_h

#include "platform/PlatformExport.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/IntSize.h"
//#include "platform/graphics/Canvas2DLayerBridge.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "platform/graphics/GraphicsTypes.h"
#include "platform/graphics/GraphicsTypes3D.h"
#include "platform/graphics/ImageBufferSurface.h"
#include "platform/graphics/paint/DisplayItemClient.h"
#include "platform/transforms/AffineTransform.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/Uint8ClampedArray.h"

namespace WTF {

class ArrayBufferContents;

} // namespace WTF

namespace blink {

class DrawingBuffer;
class GraphicsContext;
class Image;
class ImageBufferClient;
class IntPoint;
class IntRect;
class WebGraphicsContext3D;

enum Multiply {
    Premultiplied,
    Unmultiplied
};

enum BackingStoreCopy {
    CopyBackingStore, // Guarantee subsequent draws don't affect the copy.
    DontCopyBackingStore // Subsequent draws may affect the copy.
};

enum ScaleBehavior {
    Scaled,
    Unscaled
};

class PLATFORM_EXPORT ImageBuffer {
    WTF_MAKE_NONCOPYABLE(ImageBuffer); WTF_MAKE_FAST_ALLOCATED(ImageBuffer);
public:
    static PassOwnPtr<ImageBuffer> create(const IntSize&, OpacityMode = NonOpaque);
    static PassOwnPtr<ImageBuffer> create(PassOwnPtr<ImageBufferSurface>);

    ~ImageBuffer();

    void setClient(ImageBufferClient* client) { m_client = client; }

    const IntSize& size() const { return m_surface->size(); }
    bool isAccelerated() const { return m_surface->isAccelerated(); }
    bool isRecording() const { return m_surface->isRecording(); }
    void setHasExpensiveOp() { m_surface->setHasExpensiveOp(); }
    bool isExpensiveToPaint() const { return m_surface->isExpensiveToPaint(); }
    bool isSurfaceValid() const;
    bool restoreSurface() const;
    void didDraw(const FloatRect& rect) const { m_surface->didDraw(rect); }

    void setFilterQuality(SkFilterQuality filterQuality) { m_surface->setFilterQuality(filterQuality); }
    void setIsHidden(bool hidden) { m_surface->setIsHidden(hidden); }

    // Called by subclasses of ImageBufferSurface to install a new canvas object
    void resetCanvas(SkCanvas*);

    void willDrawVideo() { m_surface->willDrawVideo(); }

    SkCanvas* canvas() const;

    // Called at the end of a task that rendered a whole frame
    void finalizeFrame(const FloatRect &dirtyRect);
    void didFinalizeFrame();

    bool isDirty();

    const SkBitmap& bitmap() const;

    void willAccessPixels() { m_surface->willAccessPixels(); }
    void willOverwriteCanvas() { m_surface->willOverwriteCanvas(); }

    PassRefPtr<Image> copyImage(BackingStoreCopy = CopyBackingStore, ScaleBehavior = Scaled) const;
    // Give hints on the faster copyImage Mode, return DontCopyBackingStore if it supports the DontCopyBackingStore behavior
    // or return CopyBackingStore if it doesn't.
    static BackingStoreCopy fastCopyImageMode();

    bool getImageData(Multiply, const IntRect&, WTF::ArrayBufferContents&) const;

    void putByteArray(Multiply, const unsigned char* source, const IntSize& sourceSize, const IntRect& sourceRect, const IntPoint& destPoint);

    String toDataURL(const String& mimeType, const double* quality = 0) const;
    AffineTransform baseTransform() const { return AffineTransform(); }
    WebLayer* platformLayer() const;

    // FIXME: current implementations of this method have the restriction that they only work
    // with textures that are RGB or RGBA format, UNSIGNED_BYTE type and level 0, as specified in
    // Extensions3D::canUseCopyTextureCHROMIUM().
    // Destroys the TEXTURE_2D binding for the active texture unit of the passed context
    bool copyToPlatformTexture(WebGraphicsContext3D*, Platform3DObject, GLenum, GLenum, GLint, bool, bool);

    bool copyRenderingResultsFromDrawingBuffer(DrawingBuffer*, SourceDrawingBuffer);

    void flush();

    void notifySurfaceInvalid();

    PassRefPtr<SkImage> newImageSnapshot() const;

    DisplayItemClient displayItemClient() const { return toDisplayItemClient(this); }
    String debugName() const { return "ImageBuffer"; }

    void draw(GraphicsContext*, const FloatRect&, const FloatRect*, SkXfermode::Mode);

private:
    ImageBuffer(PassOwnPtr<ImageBufferSurface>);

    OwnPtr<ImageBufferSurface> m_surface;
    ImageBufferClient* m_client;
};

struct ImageDataBuffer {
    ImageDataBuffer(const IntSize& size, unsigned char* data) : m_data(data), m_size(size) { }
    String PLATFORM_EXPORT toDataURL(const String& mimeType, const double* quality) const;
    unsigned char* pixels() const { return m_data; }
    int height() const { return m_size.height(); }
    int width() const { return m_size.width(); }
    unsigned char* m_data;
    IntSize m_size;
};

} // namespace blink

#endif // ImageBuffer_h
