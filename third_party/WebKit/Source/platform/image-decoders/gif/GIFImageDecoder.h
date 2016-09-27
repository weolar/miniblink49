/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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

#ifndef GIFImageDecoder_h
#define GIFImageDecoder_h

#include "platform/image-decoders/ImageDecoder.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"

class GIFImageReader;

typedef Vector<unsigned char> GIFRow;

namespace blink {

// This class decodes the GIF image format.
class PLATFORM_EXPORT GIFImageDecoder : public ImageDecoder {
    WTF_MAKE_NONCOPYABLE(GIFImageDecoder);
public:
    GIFImageDecoder(ImageSource::AlphaOption, ImageSource::GammaAndColorProfileOption, size_t maxDecodedBytes);
    ~GIFImageDecoder() override;

    enum GIFParseQuery { GIFSizeQuery, GIFFrameCountQuery };

    // ImageDecoder:
    String filenameExtension() const override { return "gif"; }
    void setData(SharedBuffer* data, bool allDataReceived) override;
    int repetitionCount() const override;
    bool frameIsCompleteAtIndex(size_t) const override;
    float frameDurationAtIndex(size_t) const override;
    size_t clearCacheExceptFrame(size_t) override;
    // CAUTION: setFailed() deletes |m_reader|.  Be careful to avoid
    // accessing deleted memory, especially when calling this from inside
    // GIFImageReader!
    bool setFailed() override;

    // Callbacks from the GIF reader.
    bool haveDecodedRow(size_t frameIndex, GIFRow::const_iterator rowBegin, size_t width, size_t rowNumber, unsigned repeatCount, bool writeTransparentPixels);
    bool frameComplete(size_t frameIndex);

    // For testing.
    bool parseCompleted() const;

private:
    // ImageDecoder:
    void clearFrameBuffer(size_t frameIndex) override;
    virtual void decodeSize() { parse(GIFSizeQuery); }
    size_t decodeFrameCount() override;
    void initializeNewFrame(size_t) override;
    void decode(size_t) override;

    // Parses as much as is needed to answer the query, ignoring bitmap
    // data. If parsing fails, sets the "decode failure" flag.
    void parse(GIFParseQuery);

    // Called to initialize the frame buffer with the given index, based on
    // the previous frame's disposal method. Returns true on success. On
    // failure, this will mark the image as failed.
    bool initFrameBuffer(size_t frameIndex);

    bool m_currentBufferSawAlpha;
    mutable int m_repetitionCount;
    OwnPtr<GIFImageReader> m_reader;
};

} // namespace blink

#endif
