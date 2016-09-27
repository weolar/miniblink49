/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WEBPImageDecoder_h
#define WEBPImageDecoder_h

#include "platform/image-decoders/ImageDecoder.h"

#include "webp/decode.h"
#include "webp/demux.h"

namespace blink {

class PLATFORM_EXPORT WEBPImageDecoder : public ImageDecoder {
    WTF_MAKE_NONCOPYABLE(WEBPImageDecoder);
public:
    WEBPImageDecoder(ImageSource::AlphaOption, ImageSource::GammaAndColorProfileOption, size_t maxDecodedBytes);
    ~WEBPImageDecoder() override;

    // ImageDecoder:
    String filenameExtension() const override { return "webp"; }
    bool hasColorProfile() const override { return m_hasColorProfile; }
    void setData(SharedBuffer* data, bool allDataReceived) override;
    int repetitionCount() const override;
    bool frameIsCompleteAtIndex(size_t) const override;
    float frameDurationAtIndex(size_t) const override;
    size_t clearCacheExceptFrame(size_t) override;

private:
    // ImageDecoder:
    virtual void decodeSize() { updateDemuxer(); }
    size_t decodeFrameCount() override;
    void initializeNewFrame(size_t) override;
    void decode(size_t) override;

    bool decodeSingleFrame(const uint8_t* dataBytes, size_t dataSize, size_t frameIndex);

    WebPIDecoder* m_decoder;
    WebPDecBuffer m_decoderBuffer;
    int m_formatFlags;
    bool m_frameBackgroundHasAlpha;
    bool m_hasColorProfile;

#if USE(QCMSLIB)
    qcms_transform* colorTransform() const { return m_transform; }
    bool createColorTransform(const char* data, size_t);
    void clearColorTransform();
    void readColorProfile();

    qcms_transform* m_transform;
#endif

    bool updateDemuxer();
    bool initFrameBuffer(size_t frameIndex);
    void applyPostProcessing(size_t frameIndex);
    void clearFrameBuffer(size_t frameIndex) override;

    WebPDemuxer* m_demux;
    WebPDemuxState m_demuxState;
    bool m_haveAlreadyParsedThisData;
    int m_repetitionCount;
    int m_decodedHeight;

    typedef void (*AlphaBlendFunction)(ImageFrame&, ImageFrame&, int, int, int);
    AlphaBlendFunction m_blendFunction;

    void clear();
    void clearDecoder();
};

} // namespace blink

#endif
