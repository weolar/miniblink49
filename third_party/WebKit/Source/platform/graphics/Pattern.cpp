/*
 * Copyright (C) 2006, 2007, 2008 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2013 Google, Inc. All rights reserved.
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
#include "platform/graphics/Pattern.h"

#include "platform/graphics/BitmapPattern.h"
#include "platform/graphics/PicturePattern.h"
#include "platform/graphics/StaticBitmapPattern.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkShader.h"
#include <v8.h>

namespace blink {

PassRefPtr<Pattern> Pattern::createBitmapPattern(PassRefPtr<Image> tileImage, RepeatMode repeatMode)
{
    if (tileImage->skImage())
        return StaticBitmapPattern::create(tileImage, repeatMode);

    return BitmapPattern::create(tileImage, repeatMode);
}

PassRefPtr<Pattern> Pattern::createPicturePattern(PassRefPtr<const SkPicture> picture,
    RepeatMode repeatMode)
{
    return PicturePattern::create(picture, repeatMode);
}

Pattern::Pattern(RepeatMode repeatMode, int64_t externalMemoryAllocated)
    : m_repeatMode(repeatMode)
    , m_externalMemoryAllocated(0)
{
    adjustExternalMemoryAllocated(externalMemoryAllocated);
}

Pattern::~Pattern()
{
    adjustExternalMemoryAllocated(-m_externalMemoryAllocated);
}

SkShader* Pattern::shader()
{
    if (!m_pattern) {
        m_pattern = createShader();
    }

    return m_pattern.get();
}

void Pattern::setPatternSpaceTransform(const AffineTransform& patternSpaceTransformation)
{
    if (patternSpaceTransformation == m_patternSpaceTransformation)
        return;

    m_patternSpaceTransformation = patternSpaceTransformation;
    m_pattern.clear();
}

void Pattern::adjustExternalMemoryAllocated(int64_t delta)
{
    delta = std::max(-m_externalMemoryAllocated, delta);

    v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(delta);

    m_externalMemoryAllocated += delta;
}

} // namespace blink
