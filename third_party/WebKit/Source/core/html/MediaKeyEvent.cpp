/*
 * Copyright (C) 2012 Google Inc.  All rights reserved.
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
#include "core/html/MediaKeyEvent.h"

namespace blink {

MediaKeyEvent::MediaKeyEvent()
{
}

MediaKeyEvent::MediaKeyEvent(const AtomicString& type, const MediaKeyEventInit& initializer)
    : Event(type, initializer)
    , m_systemCode(0)
{
    if (initializer.hasKeySystem())
        m_keySystem = initializer.keySystem();
    if (initializer.hasSessionId())
        m_sessionId = initializer.sessionId();
    if (initializer.hasInitData())
        m_initData = initializer.initData();
    if (initializer.hasMessage())
        m_message = initializer.message();
    if (initializer.hasDefaultURL())
        m_defaultURL = initializer.defaultURL();
    if (initializer.hasErrorCode())
        m_errorCode = initializer.errorCode();
    if (initializer.hasSystemCode())
        m_systemCode = initializer.systemCode();
}

MediaKeyEvent::~MediaKeyEvent()
{
}

const AtomicString& MediaKeyEvent::interfaceName() const
{
    return EventNames::MediaKeyEvent;
}

DEFINE_TRACE(MediaKeyEvent)
{
    visitor->trace(m_errorCode);
    Event::trace(visitor);
}

} // namespace blink
