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

#ifndef MIDIConnectionEvent_h
#define MIDIConnectionEvent_h

#include "modules/EventModules.h"
#include "modules/webmidi/MIDIPort.h"

namespace blink {

class MIDIConnectionEventInit;

class MIDIConnectionEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<MIDIConnectionEvent> create()
    {
        return adoptRefWillBeNoop(new MIDIConnectionEvent());
    }

    static PassRefPtrWillBeRawPtr<MIDIConnectionEvent> create(MIDIPort* port)
    {
        return adoptRefWillBeNoop(new MIDIConnectionEvent(port));
    }

    static PassRefPtrWillBeRawPtr<MIDIConnectionEvent> create(const AtomicString& type, const MIDIConnectionEventInit& initializer)
    {
        return adoptRefWillBeNoop(new MIDIConnectionEvent(type, initializer));
    }

    MIDIPort* port() { return m_port; }

    const AtomicString& interfaceName() const override { return EventNames::MIDIConnectionEvent; }

    DECLARE_VIRTUAL_TRACE();

private:
    MIDIConnectionEvent()
        : Event(EventTypeNames::statechange, false, false) { }

    MIDIConnectionEvent(MIDIPort* port)
        : Event(EventTypeNames::statechange, false, false)
        , m_port(port) { }

    MIDIConnectionEvent(const AtomicString&, const MIDIConnectionEventInit&);

    PersistentWillBeMember<MIDIPort> m_port;
};

} // namespace blink

#endif // MIDIConnectionEvent_h
