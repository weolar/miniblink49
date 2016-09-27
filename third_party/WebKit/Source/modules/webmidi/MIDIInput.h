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

#ifndef MIDIInput_h
#define MIDIInput_h

#include "modules/EventTargetModules.h"
#include "modules/webmidi/MIDIAccessor.h"
#include "modules/webmidi/MIDIPort.h"

namespace blink {

class MIDIAccess;

class MIDIInput final : public MIDIPort {
    DEFINE_WRAPPERTYPEINFO();
public:
    static MIDIInput* create(MIDIAccess*, const String& id, const String& manufacturer, const String& name, const String& version, MIDIAccessor::MIDIPortState);
    ~MIDIInput() override { }

    EventListener* onmidimessage();
    void setOnmidimessage(PassRefPtr<EventListener>);

    // EventTarget
    const AtomicString& interfaceName() const override { return EventTargetNames::MIDIInput; }

    // |timeStamp| is a DOMHighResTimeStamp in the time coordinate system of performance.now().
    void didReceiveMIDIData(unsigned portIndex, const unsigned char* data, size_t length, double timeStamp);

    DECLARE_VIRTUAL_TRACE();

private:
    MIDIInput(MIDIAccess*, const String& id, const String& manufacturer, const String& name, const String& version, MIDIAccessor::MIDIPortState);
};

} // namespace blink

#endif // MIDIInput_h
