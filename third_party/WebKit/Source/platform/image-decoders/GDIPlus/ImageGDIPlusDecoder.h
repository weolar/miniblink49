/*
 * Copyright (C) 2013 kingsoft, Inc.  All rights reserved.
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

#ifndef ImageGDIPlusDecoder_h
#define ImageGDIPlusDecoder_h

#include "platform/image-decoders/bmp/BMPImageDecoder.h"
#include <wtf/OwnPtr.h>

namespace Gdiplus {
class Bitmap;
}

namespace blink {

class GDIPlusReader;

// This class decodes the PNG image format.
class ImageGDIPlusDecoder : public ImageDecoder {
public:
    enum GDIPlusDecoderType{
        GDIPlusDecoderPNG,
        GDIPlusDecoderJPG
    };

    ImageGDIPlusDecoder(ImageSource::AlphaOption, ImageSource::GammaAndColorProfileOption, GDIPlusDecoderType type, size_t maxDecodedBytes);
    virtual ~ImageGDIPlusDecoder();

    virtual String filenameExtension() const override;

    virtual void setData(SharedBuffer*, bool allDataReceived) override;

    // ImageDecoder:
    // CAUTION: setFailed() deletes |m_reader|.  Be careful to avoid
    // accessing deleted memory, especially when calling this from inside
    // BMPImageReader!
    bool setFailed() override;

protected:
    GDIPlusDecoderType m_type;

    // ImageDecoder:
    void decodeSize() override { decode(true); }
    void decode(size_t) override { decode(false); }

    // Decodes the image.  If |onlySize| is true, stops decoding after
    // calculating the image size. If decoding fails but there is no more
    // data coming, sets the "decode failure" flag.
    void decode(bool onlySize);

    // Decodes the image.  If |onlySize| is true, stops decoding after
    // calculating the image size. Returns whether decoding succeeded.
    bool decodeHelper(bool onlySize);

    // The reader used to do most of the BMP decoding.
    OwnPtr<GDIPlusReader> m_reader;

    Gdiplus::Bitmap* m_gdipBitmap;
};

} // namespace blink

#endif // ImageGDIPlusDecoder_h