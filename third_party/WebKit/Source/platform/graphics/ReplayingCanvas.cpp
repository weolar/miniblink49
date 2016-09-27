/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "platform/graphics/ReplayingCanvas.h"

#include "third_party/skia/include/core/SkBitmapDevice.h"

namespace blink {

CanvasInterceptor<ReplayingCanvas>::~CanvasInterceptor()
{
    if (topLevelCall())
        canvas()->updateInRange();
}

ReplayingCanvas::ReplayingCanvas(SkBitmap bitmap, unsigned fromStep, unsigned toStep)
    : InterceptingCanvas(bitmap), m_fromStep(fromStep), m_toStep(toStep), m_abortDrawing(false)
{
}

void ReplayingCanvas::updateInRange()
{
    if (m_abortDrawing)
        return;
    unsigned step = callCount() + 1;
    if (m_toStep && step > m_toStep)
        m_abortDrawing = true;
    if (step == m_fromStep)
        this->SkCanvas::clear(SkColorSetARGB(255, 255, 255, 255)); // FIXME: fill with nine patch instead.
}

bool ReplayingCanvas::abort()
{
    return m_abortDrawing;
}

SkCanvas::SaveLayerStrategy ReplayingCanvas::willSaveLayer(const SkRect* bounds, const SkPaint* paint, SaveFlags flags)
{
    // We're about to create a layer and we have not cleared the device yet.
    // Let's clear now, so it has effect on all layers.
    if (callCount() <= m_fromStep)
        this->SkCanvas::clear(SkColorSetARGB(255, 255, 255, 255)); // FIXME: fill with nine patch instead.

    return this->InterceptingCanvas<ReplayingCanvas>::willSaveLayer(bounds, paint, flags);
}

void ReplayingCanvas::onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint)
{
    this->unrollDrawPicture(picture, matrix, paint, this);
}

} // namespace blink
