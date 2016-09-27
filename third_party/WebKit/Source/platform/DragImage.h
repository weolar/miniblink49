/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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

#ifndef DragImage_h
#define DragImage_h

#include "platform/geometry/IntSize.h"
#include "platform/graphics/GraphicsTypes.h"
#include "platform/graphics/ImageOrientation.h"
#include "platform/graphics/paint/DisplayItemClient.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "wtf/Forward.h"

namespace blink {

class FontDescription;
class Image;
class KURL;

class PLATFORM_EXPORT DragImage {
public:
    static PassOwnPtr<DragImage> create(Image*, RespectImageOrientationEnum = DoNotRespectImageOrientation, float deviceScaleFactor = 1, InterpolationQuality = InterpolationHigh);
    static PassOwnPtr<DragImage> create(const KURL&, const String& label, const FontDescription& systemFont, float deviceScaleFactor);
    ~DragImage();

    const SkBitmap& bitmap() { return m_bitmap; }
    float resolutionScale() const { return m_resolutionScale; }
    IntSize size() const { return IntSize(m_bitmap.width(), m_bitmap.height()); }

    void fitToMaxSize(const IntSize& srcSize, const IntSize& maxSize);
    void scale(float scaleX, float scaleY);
    void dissolveToFraction(float fraction);

private:
    DragImage(const SkBitmap&, float resolutionScale, InterpolationQuality);

    DisplayItemClient displayItemClient() const { return toDisplayItemClient(this); }

    SkBitmap m_bitmap;
    float m_resolutionScale;
    InterpolationQuality m_interpolationQuality;
};

}

#endif // DragImage_h
