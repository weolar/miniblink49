/*
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.  All rights reserved.
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

#ifndef ImageSource_h
#define ImageSource_h

#include "platform/PlatformExport.h"
#include "platform/graphics/ImageOrientation.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"

class SkBitmap;

namespace blink {

class DeferredImageDecoder;
class ImageOrientation;
class IntPoint;
class IntSize;
class SharedBuffer;

class PLATFORM_EXPORT ImageSource {
    WTF_MAKE_NONCOPYABLE(ImageSource);
public:
    enum AlphaOption {
        AlphaPremultiplied,
        AlphaNotPremultiplied
    };

    enum GammaAndColorProfileOption {
        GammaAndColorProfileApplied,
        GammaAndColorProfileIgnored
    };

    ImageSource(AlphaOption alphaOption = AlphaPremultiplied, GammaAndColorProfileOption gammaAndColorProfileOption = GammaAndColorProfileApplied);
    ~ImageSource();

    // Tells the ImageSource that the Image no longer cares about decoded frame
    // data except for the specified frame. Callers may pass WTF::kNotFound to
    // clear all frames.
    //
    // In response, the ImageSource should delete cached decoded data for other
    // frames where possible to keep memory use low. The expectation is that in
    // the future, the caller may call createFrameAtIndex() with an index larger
    // than the one passed to this function, and the implementation may then
    // make use of the preserved frame data here in decoding that frame.
    // By contrast, callers who call this function and then later ask for an
    // earlier frame may require more work to be done, e.g. redecoding the image
    // from the beginning.
    //
    // Implementations may elect to preserve more frames than the one requested
    // here if doing so is likely to save CPU time in the future, but will pay
    // an increased memory cost to do so.
    //
    // Returns the number of bytes of frame data actually cleared.
    size_t clearCacheExceptFrame(size_t);

    void setData(SharedBuffer& data, bool allDataReceived);
    String filenameExtension() const;

    bool isSizeAvailable();
    bool hasColorProfile() const;
    IntSize size(RespectImageOrientationEnum = DoNotRespectImageOrientation) const;
    IntSize frameSizeAtIndex(size_t, RespectImageOrientationEnum = DoNotRespectImageOrientation) const;

    bool getHotSpot(IntPoint&) const;
    int repetitionCount();

    size_t frameCount() const;

    // Attempts to create the requested frame if necessary, and sets the
    // SkBitmap outparam to the associated bitmap.  Returns whether a valid
    // bitmap was set.
    bool createFrameAtIndex(size_t, SkBitmap*);

    float frameDurationAtIndex(size_t) const;
    bool frameHasAlphaAtIndex(size_t) const; // Whether or not the frame actually used any alpha.
    bool frameIsCompleteAtIndex(size_t) const; // Whether or not the frame is fully received.
    ImageOrientation orientationAtIndex(size_t) const; // EXIF image orientation

    // Returns the number of bytes in the decoded frame. May return 0 if the
    // frame has not yet begun to decode.
    size_t frameBytesAtIndex(size_t) const;

private:
    OwnPtr<DeferredImageDecoder> m_decoder;

    AlphaOption m_alphaOption;
    GammaAndColorProfileOption m_gammaAndColorProfileOption;
};

} // namespace blink

#endif
