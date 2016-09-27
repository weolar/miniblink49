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

#ifndef MIDIPort_h
#define MIDIPort_h

#include "bindings/core/v8/ScriptPromise.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/ExceptionCode.h"
#include "modules/EventTargetModules.h"
#include "modules/webmidi/MIDIAccessor.h"
#include "platform/heap/Handle.h"

namespace blink {

class MIDIAccess;

class MIDIPort : public RefCountedGarbageCollectedEventTargetWithInlineData<MIDIPort>, public ActiveDOMObject {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(MIDIPort);
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(MIDIPort);
public:
    enum ConnectionState {
        ConnectionStateOpen,
        ConnectionStateClosed,
        ConnectionStatePending
    };

    enum TypeCode {
        TypeInput,
        TypeOutput
    };

    ~MIDIPort() override { }

    String connection() const;
    String id() const { return m_id; }
    String manufacturer() const { return m_manufacturer; }
    String name() const { return m_name; }
    String state() const;
    String type() const;
    String version() const { return m_version; }

    ScriptPromise open(ScriptState*);
    ScriptPromise close(ScriptState*);

    MIDIAccess* midiAccess() const { return m_access; }
    MIDIAccessor::MIDIPortState getState() const { return m_state; }
    void setState(MIDIAccessor::MIDIPortState);
    ConnectionState getConnection() const { return m_connection; }

    DECLARE_VIRTUAL_TRACE();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(statechange);

    // EventTarget
    const AtomicString& interfaceName() const override { return EventTargetNames::MIDIPort; }
    ExecutionContext* executionContext() const final;

    // ActiveDOMObject
    bool hasPendingActivity() const override;
    void stop() override;

protected:
    MIDIPort(MIDIAccess*, const String& id, const String& manufacturer, const String& name, TypeCode, const String& version, MIDIAccessor::MIDIPortState);

    void open();

private:
    ScriptPromise accept(ScriptState*);
    ScriptPromise reject(ScriptState*, ExceptionCode, const String& message);

    void setStates(MIDIAccessor::MIDIPortState, ConnectionState);

    String m_id;
    String m_manufacturer;
    String m_name;
    TypeCode m_type;
    String m_version;
    Member<MIDIAccess> m_access;
    MIDIAccessor::MIDIPortState m_state;
    ConnectionState m_connection;
};

} // namespace blink

#endif // MIDIPort_h
