/*
 * Copyright (c) 2008, Google Inc. All rights reserved.
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2010 Torch Mobile (Beijing) Co. Ltd. All rights reserved.
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

#include "config.h"
#include "platform/graphics/ImageBuffer.h"

//#include "GrContext.h"
#include "platform/MIMETypeRegistry.h"
#include "platform/geometry/IntRect.h"
#include "platform/graphics/BitmapImage.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsTypes3D.h"
#include "platform/graphics/ImageBufferClient.h"
#include "platform/graphics/UnacceleratedImageBufferSurface.h"
#include "platform/graphics/gpu/DrawingBuffer.h"
#include "platform/graphics/gpu/Extensions3DUtil.h"
#include "platform/graphics/skia/SkiaUtils.h"
#include "platform/image-encoders/skia/JPEGImageEncoder.h"
#include "platform/image-encoders/skia/PNGImageEncoder.h"
#include "platform/image-encoders/gdiplus/GDIPlusImageEncoder.h"
#include "public/platform/Platform.h"
#include "public/platform/WebExternalTextureMailbox.h"
#include "public/platform/WebGraphicsContext3D.h"
#include "public/platform/WebGraphicsContext3DProvider.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "wtf/ArrayBufferContents.h"
#include "wtf/MathExtras.h"
#include "wtf/Vector.h"
#include "wtf/text/Base64.h"
#include "wtf/text/WTFString.h"

namespace blink {

PassOwnPtr<ImageBuffer> ImageBuffer::create(PassOwnPtr<ImageBufferSurface> surface)
{
    if (!surface->isValid())
        return nullptr;
    return adoptPtr(new ImageBuffer(surface));
}

PassOwnPtr<ImageBuffer> ImageBuffer::create(const IntSize& size, OpacityMode opacityMode)
{
    OwnPtr<ImageBufferSurface> surface(adoptPtr(new UnacceleratedImageBufferSurface(size, opacityMode)));
    if (!surface->isValid())
        return nullptr;
    return adoptPtr(new ImageBuffer(surface.release()));
}

ImageBuffer::ImageBuffer(PassOwnPtr<ImageBufferSurface> surface)
    : m_surface(surface)
    , m_client(0)
{
    m_surface->setImageBuffer(this);
}

ImageBuffer::~ImageBuffer()
{
}

SkCanvas* ImageBuffer::canvas() const
{
    if (!isSurfaceValid())
        return nullptr;
    return m_surface->canvas();
}

const SkBitmap& ImageBuffer::bitmap() const
{
    return m_surface->bitmap();
}

bool ImageBuffer::isSurfaceValid() const
{
    return m_surface->isValid();
}

bool ImageBuffer::isDirty()
{
    return m_client ? m_client->isDirty() : false;
}

void ImageBuffer::didFinalizeFrame()
{
    if (m_client)
        m_client->didFinalizeFrame();
}

void ImageBuffer::finalizeFrame(const FloatRect &dirtyRect)
{
    m_surface->finalizeFrame(dirtyRect);
    didFinalizeFrame();
}

bool ImageBuffer::restoreSurface() const
{
    return m_surface->isValid() || m_surface->restore();
}

void ImageBuffer::notifySurfaceInvalid()
{
    if (m_client)
        m_client->notifySurfaceInvalid();
}

void ImageBuffer::resetCanvas(SkCanvas* canvas)
{
    if (m_client)
        m_client->restoreCanvasMatrixClipStack();
}

PassRefPtr<SkImage> ImageBuffer::newImageSnapshot() const
{
    return m_surface->newImageSnapshot();
}

static SkBitmap deepSkBitmapCopy(const SkBitmap& bitmap)
{
    SkBitmap tmp;
    if (!bitmap.deepCopyTo(&tmp))
        bitmap.copyTo(&tmp, bitmap.colorType());

    return tmp;
}

PassRefPtr<Image> ImageBuffer::copyImage(BackingStoreCopy copyBehavior, ScaleBehavior) const
{
    if (!isSurfaceValid())
        return BitmapImage::create(SkBitmap());

    const SkBitmap& bitmap = m_surface->bitmap();
    return BitmapImage::create(copyBehavior == CopyBackingStore ? deepSkBitmapCopy(bitmap) : bitmap);
}

BackingStoreCopy ImageBuffer::fastCopyImageMode()
{
    return DontCopyBackingStore;
}

WebLayer* ImageBuffer::platformLayer() const
{
    return m_surface->layer();
}

bool ImageBuffer::copyToPlatformTexture(WebGraphicsContext3D* context, Platform3DObject texture, GLenum internalFormat, GLenum destType, GLint level, bool premultiplyAlpha, bool flipY)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (!m_surface->isAccelerated() || !isSurfaceValid())
        return false;

    if (!Extensions3DUtil::canUseCopyTextureCHROMIUM(GL_TEXTURE_2D, internalFormat, destType, level))
        return false;

    RefPtr<const SkImage> textureImage = m_surface->newImageSnapshot();
    if (!textureImage)
        return false;

    ASSERT(textureImage->isTextureBacked()); // isAccelerated() check above should guarantee this
    // Get the texture ID, flushing pending operations if needed.
    Platform3DObject textureId = textureImage->getTextureHandle(true);
    if (!textureId)
        return false;

    OwnPtr<WebGraphicsContext3DProvider> provider = adoptPtr(Platform::current()->createSharedOffscreenGraphicsContext3DProvider());
    if (!provider)
        return false;
    WebGraphicsContext3D* sharedContext = provider->context3d();
    if (!sharedContext)
        return false;

    OwnPtr<WebExternalTextureMailbox> mailbox = adoptPtr(new WebExternalTextureMailbox);

    // Contexts may be in a different share group. We must transfer the texture through a mailbox first
    sharedContext->genMailboxCHROMIUM(mailbox->name);
    sharedContext->produceTextureDirectCHROMIUM(textureId, GL_TEXTURE_2D, mailbox->name);
    sharedContext->flush();

    mailbox->syncPoint = sharedContext->insertSyncPoint();

    context->waitSyncPoint(mailbox->syncPoint);
    Platform3DObject sourceTexture = context->createAndConsumeTextureCHROMIUM(GL_TEXTURE_2D, mailbox->name);

    // The canvas is stored in a premultiplied format, so unpremultiply if necessary.
    // The canvas is stored in an inverted position, so the flip semantics are reversed.
    context->copyTextureCHROMIUM(GL_TEXTURE_2D, sourceTexture, texture, internalFormat, destType, flipY ? GL_FALSE : GL_TRUE, GL_FALSE, premultiplyAlpha ? GL_FALSE : GL_TRUE);

    context->deleteTexture(sourceTexture);

    context->flush();
    sharedContext->waitSyncPoint(context->insertSyncPoint());

    // Undo grContext texture binding changes introduced in this function
    provider->grContext()->resetContext(kTextureBinding_GrGLBackendState);
#endif // MINIBLINK_NOT_IMPLEMENTED
	notImplemented();
    return true;
}

bool ImageBuffer::copyRenderingResultsFromDrawingBuffer(DrawingBuffer* drawingBuffer, SourceDrawingBuffer sourceBuffer)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (!drawingBuffer || !m_surface->isAccelerated())
        return false;
    OwnPtr<WebGraphicsContext3DProvider> provider = adoptPtr(Platform::current()->createSharedOffscreenGraphicsContext3DProvider());
    if (!provider)
        return false;
    WebGraphicsContext3D* context3D = provider->context3d();
    if (!context3D)
        return false;
    Platform3DObject textureId = m_surface->getBackingTextureHandleForOverwrite();
    if (!textureId)
        return false;

    context3D->flush();

    return drawingBuffer->copyToPlatformTexture(context3D, textureId, GL_RGBA,
        GL_UNSIGNED_BYTE, 0, true, false, sourceBuffer);
#endif // MINIBLINK_NOT_IMPLEMENTED
	notImplemented();
	return false;
}

void ImageBuffer::draw(GraphicsContext* context, const FloatRect& destRect, const FloatRect* srcPtr, SkXfermode::Mode op)
{
    if (!isSurfaceValid())
        return;

    FloatRect srcRect = srcPtr ? *srcPtr : FloatRect(FloatPoint(), size());
    m_surface->draw(context, destRect, srcRect, op);
}

void ImageBuffer::flush()
{
    if (m_surface->canvas()) {
        m_surface->canvas()->flush();
    }
}

bool ImageBuffer::getImageData(Multiply multiplied, const IntRect& rect, WTF::ArrayBufferContents& contents) const
{
    Checked<int, RecordOverflow> dataSize = 4;
    dataSize *= rect.width();
    dataSize *= rect.height();
    if (dataSize.hasOverflowed())
        return false;

    if (!isSurfaceValid()) {
        WTF::ArrayBufferContents result(rect.width() * rect.height(), 4, WTF::ArrayBufferContents::NotShared, WTF::ArrayBufferContents::ZeroInitialize);
        result.transfer(contents);
        return true;
    }

    const bool hasStrayArea =
        rect.x() < 0
        || rect.y() < 0
        || rect.maxX() > m_surface->size().width()
        || rect.maxY() > m_surface->size().height();
    WTF::ArrayBufferContents result(
        rect.width() * rect.height(), 4,
        WTF::ArrayBufferContents::NotShared,
        hasStrayArea
        ? WTF::ArrayBufferContents::ZeroInitialize
        : WTF::ArrayBufferContents::DontInitialize);

    SkAlphaType alphaType = (multiplied == Premultiplied) ? kPremul_SkAlphaType : kUnpremul_SkAlphaType;
    SkImageInfo info = SkImageInfo::Make(rect.width(), rect.height(), kRGBA_8888_SkColorType, alphaType);

    m_surface->willAccessPixels();
    ASSERT(canvas());
    canvas()->readPixels(info, result.data(), 4 * rect.width(), rect.x(), rect.y());
    result.transfer(contents);
    return true;
}

void ImageBuffer::putByteArray(Multiply multiplied, const unsigned char* source, const IntSize& sourceSize, const IntRect& sourceRect, const IntPoint& destPoint)
{
    if (!isSurfaceValid())
        return;

    ASSERT(sourceRect.width() > 0);
    ASSERT(sourceRect.height() > 0);

    int originX = sourceRect.x();
    int destX = destPoint.x() + sourceRect.x();
    ASSERT(destX >= 0);
    ASSERT(destX < m_surface->size().width());
    ASSERT(originX >= 0);
    ASSERT(originX < sourceRect.maxX());

    int originY = sourceRect.y();
    int destY = destPoint.y() + sourceRect.y();
    ASSERT(destY >= 0);
    ASSERT(destY < m_surface->size().height());
    ASSERT(originY >= 0);
    ASSERT(originY < sourceRect.maxY());

    const size_t srcBytesPerRow = 4 * sourceSize.width();
    const void* srcAddr = source + originY * srcBytesPerRow + originX * 4;
    SkAlphaType alphaType = (multiplied == Premultiplied) ? kPremul_SkAlphaType : kUnpremul_SkAlphaType;
    SkImageInfo info = SkImageInfo::Make(sourceRect.width(), sourceRect.height(), kRGBA_8888_SkColorType, alphaType);

    m_surface->willAccessPixels();

    canvas()->writePixels(info, srcAddr, srcBytesPerRow, destX, destY);
}

template <typename T>
static bool encodeImage(T& source, const String& mimeType, const double* quality, Vector<char>* output)
{
    Vector<unsigned char>* encodedImage = reinterpret_cast<Vector<unsigned char>*>(output);
#if 1 // def MINIBLINK_NOT_IMPLEMENTED
    if (mimeType == "image/jpeg") {
        int compressionQuality = JPEGImageEncoder::DefaultCompressionQuality;
        if (quality && *quality >= 0.0 && *quality <= 1.0)
            compressionQuality = static_cast<int>(*quality * 100 + 0.5);
        if (!JPEGImageEncoder::encode(source, compressionQuality, encodedImage))
            return false;

    } else if (mimeType == "image/webp") {
//         int compressionQuality = WEBPImageEncoder::DefaultCompressionQuality;
//         if (quality && *quality >= 0.0 && *quality <= 1.0)
//             compressionQuality = static_cast<int>(*quality * 100 + 0.5);
//         if (!WEBPImageEncoder::encode(source, compressionQuality, encodedImage))
            return false;
    } else {
        if (!PNGImageEncoder::encode(source, encodedImage))
            return false;
        ASSERT(mimeType == "image/png");
    }
#else // MINIBLINK_NOT_IMPLEMENTED
    if (mimeType == "image/jpeg") {
        return GDIPlusImageEncoder::encode(source, GDIPlusImageEncoder::JPG, encodedImage);
    } else if (mimeType == "image/png") {
        return GDIPlusImageEncoder::encode(source, GDIPlusImageEncoder::PNG, encodedImage);
    } else
        return false;
#endif // MINIBLINK_NOT_IMPLEMENTED

    return true;
}

String ImageBuffer::toDataURL(const String& mimeType, const double* quality) const
{
    ASSERT(MIMETypeRegistry::isSupportedImageMIMETypeForEncoding(mimeType));

    Vector<char> encodedImage;
    if (!isSurfaceValid() || !encodeImage(m_surface->bitmap(), mimeType, quality, &encodedImage))
        return "data:,";

    return "data:" + mimeType + ";base64," + base64Encode(encodedImage);
}

String ImageDataBuffer::toDataURL(const String& mimeType, const double* quality) const
{
    ASSERT(MIMETypeRegistry::isSupportedImageMIMETypeForEncoding(mimeType));

    Vector<char> encodedImage;
    if (!encodeImage(*this, mimeType, quality, &encodedImage))
        return "data:,";

    return "data:" + mimeType + ";base64," + base64Encode(encodedImage);
}

} // namespace blink
