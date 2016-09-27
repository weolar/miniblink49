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
#include "modules/webmidi/MIDIInput.h"

#include "modules/webmidi/MIDIAccess.h"
#include "modules/webmidi/MIDIMessageEvent.h"
#include "platform/heap/Handle.h"

namespace blink {

using PortState = MIDIAccessor::MIDIPortState;

MIDIInput* MIDIInput::create(MIDIAccess* access, const String& id, const String& manufacturer, const String& name, const String& version, PortState state)
{
    ASSERT(access);
    MIDIInput* input = new MIDIInput(access, id, manufacturer, name, version, state);
    input->suspendIfNeeded();
    return input;
}

MIDIInput::MIDIInput(MIDIAccess* access, const String& id, const String& manufacturer, const String& name, const String& version, PortState state)
    : MIDIPort(access, id, manufacturer, name, TypeInput, version, state)
{
}

EventListener* MIDIInput::onmidimessage()
{
    return getAttributeEventListener(EventTypeNames::midimessage);
}

void MIDIInput::setOnmidimessage(PassRefPtr<EventListener> listener)
{
    // Implicit open. It does nothing if the port is already opened.
    // See http://www.w3.org/TR/webmidi/#widl-MIDIPort-open-Promise-MIDIPort
    open();

    setAttributeEventListener(EventTypeNames::midimessage, listener);
}

void MIDIInput::didReceiveMIDIData(unsigned portIndex, const unsigned char* data, size_t length, double timeStamp)
{
    ASSERT(isMainThread());

    if (!length)
        return;

    if (getConnection() != ConnectionStateOpen)
        return;

    // Drop sysex message here when the client does not request it. Note that this is not a security check but an
    // automatic filtering for clients that do not want sysex message. Also note that sysex message will never be sent
    // unless the current process has an explicit permission to handle sysex message.
    if (data[0] == 0xf0 && !midiAccess()->sysexEnabled())
        return;
    RefPtr<DOMUint8Array> array = DOMUint8Array::create(data, length);
    dispatchEvent(MIDIMessageEvent::create(timeStamp, array));
}

DEFINE_TRACE(MIDIInput)
{
    MIDIPort::trace(visitor);
}

} // namespace blink
