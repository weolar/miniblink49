/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#include "config.h"
#include "platform/graphics/DeferredImageDecoder.h"

#include "platform/graphics/DecodingImageGenerator.h"
#include "platform/graphics/ImageDecodingStore.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

namespace {

// URI label for SkDiscardablePixelRef.
const char labelDiscardable[] = "discardable";

} // namespace

bool DeferredImageDecoder::s_enabled = true;

DeferredImageDecoder::DeferredImageDecoder(PassOwnPtr<ImageDecoder> actualDecoder)
    : m_allDataReceived(false)
    , m_lastDataSize(0)
    , m_actualDecoder(actualDecoder)
    , m_repetitionCount(cAnimationNone)
    , m_hasColorProfile(false)
{
}

DeferredImageDecoder::~DeferredImageDecoder()
{
}

PassOwnPtr<DeferredImageDecoder> DeferredImageDecoder::create(const SharedBuffer& data, ImageSource::AlphaOption alphaOption, ImageSource::GammaAndColorProfileOption gammaAndColorOption)
{
    OwnPtr<ImageDecoder> actualDecoder = ImageDecoder::create(data, alphaOption, gammaAndColorOption);
    return actualDecoder ? adoptPtr(new DeferredImageDecoder(actualDecoder.release())) : nullptr;
}

PassOwnPtr<DeferredImageDecoder> DeferredImageDecoder::createForTesting(PassOwnPtr<ImageDecoder> decoder)
{
    return adoptPtr(new DeferredImageDecoder(decoder));
}

bool DeferredImageDecoder::isLazyDecoded(const SkBitmap& bitmap)
{
    return bitmap.pixelRef()
        && bitmap.pixelRef()->getURI()
        && !memcmp(bitmap.pixelRef()->getURI(), labelDiscardable, sizeof(labelDiscardable));
}

void DeferredImageDecoder::setEnabled(bool enabled)
{
    s_enabled = enabled;
}

bool DeferredImageDecoder::enabled()
{
    return s_enabled;
}

String DeferredImageDecoder::filenameExtension() const
{
    return m_actualDecoder ? m_actualDecoder->filenameExtension() : m_filenameExtension;
}

bool DeferredImageDecoder::createFrameAtIndex(size_t index, SkBitmap* bitmap)
{
    prepareLazyDecodedFrames();
    if (index < m_frameData.size()) {
        // ImageFrameGenerator has the latest known alpha state. There will be a
        // performance boost if this frame is opaque.
        *bitmap = createBitmap(index);
        FrameData* frameData = &m_frameData[index];
        frameData->m_hasAlpha = m_frameGenerator->hasAlpha(index);
        bitmap->setAlphaType(frameData->m_hasAlpha ? kPremul_SkAlphaType : kOpaque_SkAlphaType);
        frameData->m_frameBytes = m_size.area() *  sizeof(ImageFrame::PixelData);
        return true;
    }
    if (!m_actualDecoder)
        return false;
    ImageFrame* buffer = m_actualDecoder->frameBufferAtIndex(index);
    if (!buffer || buffer->status() == ImageFrame::FrameEmpty)
        return false;
    *bitmap = buffer->bitmap();
    return true;
}

void DeferredImageDecoder::setData(SharedBuffer& data, bool allDataReceived)
{
    if (m_actualDecoder) {
        m_data = RefPtr<SharedBuffer>(data);
        m_lastDataSize = data.size();
        m_allDataReceived = allDataReceived;
        m_actualDecoder->setData(&data, allDataReceived);
        prepareLazyDecodedFrames();
    }

    if (m_frameGenerator)
        m_frameGenerator->setData(&data, allDataReceived);
}

bool DeferredImageDecoder::isSizeAvailable()
{
    // m_actualDecoder is 0 only if image decoding is deferred and that means
    // the image header decoded successfully and the size is available.
    return m_actualDecoder ? m_actualDecoder->isSizeAvailable() : true;
}

bool DeferredImageDecoder::hasColorProfile() const
{
    return m_actualDecoder ? m_actualDecoder->hasColorProfile() : m_hasColorProfile;
}

IntSize DeferredImageDecoder::size() const
{
    return m_actualDecoder ? m_actualDecoder->size() : m_size;
}

IntSize DeferredImageDecoder::frameSizeAtIndex(size_t index) const
{
    // FIXME: LocalFrame size is assumed to be uniform. This might not be true for
    // future supported codecs.
    return m_actualDecoder ? m_actualDecoder->frameSizeAtIndex(index) : m_size;
}

size_t DeferredImageDecoder::frameCount()
{
    return m_actualDecoder ? m_actualDecoder->frameCount() : m_frameData.size();
}

int DeferredImageDecoder::repetitionCount() const
{
    return m_actualDecoder ? m_actualDecoder->repetitionCount() : m_repetitionCount;
}

size_t DeferredImageDecoder::clearCacheExceptFrame(size_t clearExceptFrame)
{
    if (m_actualDecoder)
        return m_actualDecoder->clearCacheExceptFrame(clearExceptFrame);
    size_t frameBytesCleared = 0;
    for (size_t i = 0; i < m_frameData.size(); ++i) {
        if (i != clearExceptFrame) {
            frameBytesCleared += m_frameData[i].m_frameBytes;
            m_frameData[i].m_frameBytes = 0;
        }
    }
    return frameBytesCleared;
}

bool DeferredImageDecoder::frameHasAlphaAtIndex(size_t index) const
{
    if (m_actualDecoder)
        return m_actualDecoder->frameHasAlphaAtIndex(index);
    if (!m_frameGenerator->isMultiFrame())
        return m_frameGenerator->hasAlpha(index);
    return true;
}

bool DeferredImageDecoder::frameIsCompleteAtIndex(size_t index) const
{
    if (m_actualDecoder)
        return m_actualDecoder->frameIsCompleteAtIndex(index);
    if (index < m_frameData.size())
        return m_frameData[index].m_isComplete;
    return false;
}

float DeferredImageDecoder::frameDurationAtIndex(size_t index) const
{
    if (m_actualDecoder)
        return m_actualDecoder->frameDurationAtIndex(index);
    if (index < m_frameData.size())
        return m_frameData[index].m_duration;
    return 0;
}

size_t DeferredImageDecoder::frameBytesAtIndex(size_t index) const
{
    if (m_actualDecoder)
        return m_actualDecoder->frameBytesAtIndex(index);
    if (index < m_frameData.size())
        return m_frameData[index].m_frameBytes;
    return 0;
}

ImageOrientation DeferredImageDecoder::orientationAtIndex(size_t index) const
{
    if (m_actualDecoder)
        return m_actualDecoder->orientation();
    if (index < m_frameData.size())
        return m_frameData[index].m_orientation;
    return DefaultImageOrientation;
}

void DeferredImageDecoder::activateLazyDecoding()
{
    if (m_frameGenerator)
        return;
    m_size = m_actualDecoder->size();
    m_filenameExtension = m_actualDecoder->filenameExtension();
    m_hasColorProfile = m_actualDecoder->hasColorProfile();
    const bool isSingleFrame = m_actualDecoder->repetitionCount() == cAnimationNone || (m_allDataReceived && m_actualDecoder->frameCount() == 1u);
    m_frameGenerator = ImageFrameGenerator::create(SkISize::Make(m_actualDecoder->decodedSize().width(), m_actualDecoder->decodedSize().height()), m_data, m_allDataReceived, !isSingleFrame);
}

void DeferredImageDecoder::prepareLazyDecodedFrames()
{
    if (!s_enabled
        || !m_actualDecoder
        || !m_actualDecoder->isSizeAvailable()
        || m_actualDecoder->filenameExtension() == "ico")
        return;

    activateLazyDecoding();

    const size_t previousSize = m_frameData.size();
    m_frameData.resize(m_actualDecoder->frameCount());

    // We have encountered a broken image file. Simply bail.
    if (m_frameData.size() < previousSize)
        return;

    for (size_t i = previousSize; i < m_frameData.size(); ++i) {
        m_frameData[i].m_haveMetadata = true;
        m_frameData[i].m_duration = m_actualDecoder->frameDurationAtIndex(i);
        m_frameData[i].m_orientation = m_actualDecoder->orientation();
        m_frameData[i].m_isComplete = m_actualDecoder->frameIsCompleteAtIndex(i);
    }

    // The last lazy decoded frame created from previous call might be
    // incomplete so update its state.
    if (previousSize) {
        const size_t lastFrame = previousSize - 1;
        m_frameData[lastFrame].m_isComplete = m_actualDecoder->frameIsCompleteAtIndex(lastFrame);
    }

    if (m_allDataReceived) {
        m_repetitionCount = m_actualDecoder->repetitionCount();
        m_actualDecoder.clear();
        m_data = nullptr;
    }
}

// Creates a SkBitmap that is backed by SkDiscardablePixelRef.
SkBitmap DeferredImageDecoder::createBitmap(size_t index)
{
    SkISize decodedSize = m_frameGenerator->getFullSize();
    ASSERT(decodedSize.width() > 0);
    ASSERT(decodedSize.height() > 0);

#if SK_B32_SHIFT // Little-endian RGBA pixels. (Android)
    const SkColorType colorType = kRGBA_8888_SkColorType;
#else
    const SkColorType colorType = kBGRA_8888_SkColorType;
#endif
    const SkImageInfo info = SkImageInfo::Make(decodedSize.width(), decodedSize.height(), colorType, kPremul_SkAlphaType);

    SkBitmap bitmap;
    DecodingImageGenerator* generator = new DecodingImageGenerator(m_frameGenerator, info, index);
    bool installed = SkInstallDiscardablePixelRef(generator, &bitmap);
    ASSERT_UNUSED(installed, installed);
    bitmap.pixelRef()->setURI(labelDiscardable);
    generator->setGenerationId(bitmap.getGenerationID());
    return bitmap;
}

bool DeferredImageDecoder::hotSpot(IntPoint& hotSpot) const
{
    // TODO: Implement.
    return m_actualDecoder ? m_actualDecoder->hotSpot(hotSpot) : false;
}

} // namespace blink
