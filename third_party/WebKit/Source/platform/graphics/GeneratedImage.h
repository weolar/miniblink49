/*
 * Copyright (C) 2008 Apple Computer, Inc.  All rights reserved.
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

#ifndef GeneratedImage_h
#define GeneratedImage_h

#include "platform/geometry/IntSize.h"
#include "platform/graphics/Image.h"
#include "wtf/RefPtr.h"

namespace blink {

class PLATFORM_EXPORT GeneratedImage : public Image {
public:
    bool currentFrameHasSingleSecurityOrigin() const override { return true; }

    void setContainerSize(const IntSize& size) override { m_size = size; }
    bool usesContainerSize() const override { return true; }
    bool hasRelativeWidth() const override { return true; }
    bool hasRelativeHeight() const override { return true; }
    void computeIntrinsicDimensions(Length& intrinsicWidth, Length& intrinsicHeight, FloatSize& intrinsicRatio) override;

    IntSize size() const override { return m_size; }

    // Assume that generated content has no decoded data we need to worry about
    void destroyDecodedData(bool) override { }

protected:
    void drawPattern(GraphicsContext*, const FloatRect&,
        const FloatSize&, const FloatPoint&, SkXfermode::Mode,
        const FloatRect&, const IntSize& repeatSpacing) final;

    // FIXME: Implement this to be less conservative.
    bool currentFrameKnownToBeOpaque() override { return false; }

    GeneratedImage(const IntSize& size) : m_size(size) { }

    virtual void drawTile(GraphicsContext*, const FloatRect&) = 0;

    IntSize m_size;
};

} // namespace blink

#endif
