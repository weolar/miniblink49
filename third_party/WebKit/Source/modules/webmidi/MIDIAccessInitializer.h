// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MIDIAccessInitializer_h
#define MIDIAccessInitializer_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "modules/ModulesExport.h"
#include "modules/webmidi/MIDIAccessor.h"
#include "modules/webmidi/MIDIAccessorClient.h"
#include "modules/webmidi/MIDIPort.h"
#include "wtf/OwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class MIDIOptions;
class ScriptState;

class MODULES_EXPORT MIDIAccessInitializer : public ScriptPromiseResolver, public MIDIAccessorClient {
public:
    struct PortDescriptor {
        String id;
        String manufacturer;
        String name;
        MIDIPort::TypeCode type;
        String version;
        MIDIAccessor::MIDIPortState state;

        PortDescriptor(const String& id, const String& manufacturer, const String& name, MIDIPort::TypeCode type, const String& version, MIDIAccessor::MIDIPortState state)
            : id(id)
            , manufacturer(manufacturer)
            , name(name)
            , type(type)
            , version(version)
            , state(state) { }
    };

    static ScriptPromise start(ScriptState* scriptState, const MIDIOptions& options)
    {
        RefPtrWillBeRawPtr<MIDIAccessInitializer> resolver = adoptRefWillBeNoop(new MIDIAccessInitializer(scriptState, options));
        resolver->keepAliveWhilePending();
        resolver->suspendIfNeeded();
        return resolver->start();
    }

    ~MIDIAccessInitializer() override;

    // Eager finalization to allow dispose() operation access
    // other (non eager) heap objects.
    EAGERLY_FINALIZE();

    // MIDIAccessorClient
    void didAddInputPort(const String& id, const String& manufacturer, const String& name, const String& version, MIDIAccessor::MIDIPortState) override;
    void didAddOutputPort(const String& id, const String& manufacturer, const String& name, const String& version, MIDIAccessor::MIDIPortState) override;
    void didSetInputPortState(unsigned portIndex, MIDIAccessor::MIDIPortState) override;
    void didSetOutputPortState(unsigned portIndex, MIDIAccessor::MIDIPortState) override;
    void didStartSession(bool success, const String& error, const String& message) override;
    void didReceiveMIDIData(unsigned portIndex, const unsigned char* data, size_t length, double timeStamp) override { }

    void resolveSysexPermission(bool allowed);
    SecurityOrigin* securityOrigin() const;

private:
    MIDIAccessInitializer(ScriptState*, const MIDIOptions&);

    ExecutionContext* executionContext() const;
    ScriptPromise start();
    void dispose();

    void contextDestroyed() override;

    OwnPtr<MIDIAccessor> m_accessor;
    Vector<PortDescriptor> m_portDescriptors;
    bool m_requestSysex;
    bool m_hasBeenDisposed;
    bool m_sysexPermissionResolved;
};

} // namespace blink

#endif // MIDIAccessInitializer_h
