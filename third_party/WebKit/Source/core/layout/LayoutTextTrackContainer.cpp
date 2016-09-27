/*
 * Copyright (C) 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Google Inc.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutTextTrackContainer.h"

#include "core/frame/DeprecatedScheduleStyleRecalcDuringLayout.h"
#include "core/layout/LayoutVideo.h"

namespace blink {

LayoutTextTrackContainer::LayoutTextTrackContainer(Element* element)
    : LayoutBlockFlow(element)
    , m_fontSize(0)
{
}

void LayoutTextTrackContainer::layout()
{
    LayoutBlockFlow::layout();
    if (style()->display() == NONE)
        return;

    DeprecatedScheduleStyleRecalcDuringLayout marker(node()->document().lifecycle());

    LayoutObject* mediaLayoutObject = parent();
    if (!mediaLayoutObject || !mediaLayoutObject->isVideo())
        return;
    if (updateSizes(toLayoutVideo(*mediaLayoutObject)))
        toElement(node())->setInlineStyleProperty(CSSPropertyFontSize, m_fontSize, CSSPrimitiveValue::CSS_PX);
}

bool LayoutTextTrackContainer::updateSizes(const LayoutVideo& videoLayoutObject)
{
    // FIXME: The video size is used to calculate the font size (a workaround
    // for lack of per-spec vh/vw support) but the whole media element is used
    // for cue rendering. This is inconsistent. See also the somewhat related
    // spec bug: https://www.w3.org/Bugs/Public/show_bug.cgi?id=28105
    IntSize videoSize = videoLayoutObject.videoBox().size();
    if (m_videoSize == videoSize)
        return false;
    m_videoSize = videoSize;

    float smallestDimension = std::min(m_videoSize.height(), m_videoSize.width());

    float fontSize = smallestDimension * 0.05f;
    if (m_fontSize == fontSize)
        return false;
    m_fontSize = fontSize;
    return true;
}

} // namespace blink
