/*
 * Copyright (C) 2013 Google Inc.  All rights reserved.
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
#include "core/html/track/TrackBase.h"

#include "core/html/HTMLMediaElement.h"

namespace blink {

static WebMediaPlayer::TrackId nextTrackId()
{
    static WebMediaPlayer::TrackId next = 0;
    return ++next;
}

TrackBase::TrackBase(Type type, const AtomicString& label, const AtomicString& language, const String& id)
    : m_trackId(nextTrackId())
    , m_type(type)
    , m_label(label)
    , m_language(language)
    , m_id(id)
    , m_mediaElement(nullptr)
{
}

TrackBase::~TrackBase()
{
#if !ENABLE(OILPAN)
    ASSERT(!m_mediaElement);
#endif
}


Node* TrackBase::owner() const
{
    return m_mediaElement;
}

DEFINE_TRACE(TrackBase)
{
    visitor->trace(m_mediaElement);
}

void TrackBase::setKind(const AtomicString& kind)
{
    if (isValidKind(kind))
        m_kind = kind;
    else
        m_kind = defaultKind();
}

} // namespace blink
