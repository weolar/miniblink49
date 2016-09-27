/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
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

#ifndef CloseEvent_h
#define CloseEvent_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/EventTypeNames.h"
#include "core/events/Event.h"
#include "modules/EventModules.h"
#include "modules/websockets/CloseEventInit.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class CloseEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<CloseEvent> create()
    {
        return adoptRefWillBeNoop(new CloseEvent());
    }

    static PassRefPtrWillBeRawPtr<CloseEvent> create(bool wasClean, unsigned short code, const String& reason)
    {
        return adoptRefWillBeNoop(new CloseEvent(wasClean, code, reason));
    }

    static PassRefPtrWillBeRawPtr<CloseEvent> create(const AtomicString& type, const CloseEventInit& initializer)
    {
        return adoptRefWillBeNoop(new CloseEvent(type, initializer));
    }

    bool wasClean() const { return m_wasClean; }
    unsigned short code() const { return m_code; }
    String reason() const { return m_reason; }

    // Event function.
    const AtomicString& interfaceName() const override { return EventNames::CloseEvent; }

    DEFINE_INLINE_VIRTUAL_TRACE() { Event::trace(visitor); }

private:
    CloseEvent()
        : Event(EventTypeNames::close, false, false)
        , m_wasClean(false)
        , m_code(0) { }

    CloseEvent(bool wasClean, int code, const String& reason)
        : Event(EventTypeNames::close, false, false)
        , m_wasClean(wasClean)
        , m_code(code)
        , m_reason(reason) { }

    CloseEvent(const AtomicString& type, const CloseEventInit& initializer);

    bool m_wasClean;
    unsigned short m_code;
    String m_reason;
};

} // namespace blink

#endif // CloseEvent_h
