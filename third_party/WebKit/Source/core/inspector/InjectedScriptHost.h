/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InjectedScriptHost_h
#define InjectedScriptHost_h

#include "bindings/core/v8/ScriptState.h"
#include "core/InspectorTypeBuilder.h"
#include "core/inspector/InjectedScriptHostClient.h"
#include "wtf/Functional.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class EventTarget;
class InjectedScriptHostClient;
class InspectorConsoleAgent;
class InspectorDebuggerAgent;
class JSONValue;
class Node;
class ScriptValue;
class V8Debugger;

struct EventListenerInfo;

// SECURITY NOTE: Although the InjectedScriptHost is intended for use solely by the inspector,
// a reference to the InjectedScriptHost may be leaked to the page being inspected. Thus, the
// InjectedScriptHost must never implemment methods that have more power over the page than the
// page already has itself (e.g. origin restriction bypasses).

class InjectedScriptHost : public RefCountedWillBeGarbageCollectedFinalized<InjectedScriptHost> {
public:
    static PassRefPtrWillBeRawPtr<InjectedScriptHost> create();
    ~InjectedScriptHost();
    DECLARE_TRACE();

    using InspectCallback = Function<void(PassRefPtr<TypeBuilder::Runtime::RemoteObject>, PassRefPtr<JSONObject>)>;

    void init(InspectorConsoleAgent* consoleAgent, InspectorDebuggerAgent* debuggerAgent, PassOwnPtr<InspectCallback> inspectCallback, V8Debugger* debugger, PassOwnPtr<InjectedScriptHostClient> injectedScriptHostClient)
    {
        m_consoleAgent = consoleAgent;
        m_debuggerAgent = debuggerAgent;
        m_inspectCallback = inspectCallback;
        m_debugger = debugger;
        m_client = injectedScriptHostClient;
    }

    static Node* scriptValueAsNode(ScriptState*, ScriptValue);
    static ScriptValue nodeAsScriptValue(ScriptState*, Node*);
    static EventTarget* scriptValueAsEventTarget(ScriptState*, ScriptValue);

    void disconnect();

    class InspectableObject {
        WTF_MAKE_FAST_ALLOCATED(InspectableObject);
    public:
        virtual ScriptValue get(ScriptState*);
        virtual ~InspectableObject() { }
    };
    void addInspectedObject(PassOwnPtr<InspectableObject>);
    void clearInspectedObjects();
    InspectableObject* inspectedObject(unsigned num);

    void inspectImpl(PassRefPtr<JSONValue> objectToInspect, PassRefPtr<JSONValue> hints);
    void getEventListenersImpl(EventTarget*, Vector<EventListenerInfo>& listenersArray);

    void clearConsoleMessages();
    void debugFunction(const String& scriptId, int lineNumber, int columnNumber);
    void undebugFunction(const String& scriptId, int lineNumber, int columnNumber);
    void monitorFunction(const String& scriptId, int lineNumber, int columnNumber, const String& functionName);
    void unmonitorFunction(const String& scriptId, int lineNumber, int columnNumber);

    V8Debugger& debugger() { return *m_debugger; }
    InjectedScriptHostClient* client() { return m_client.get(); }

    // FIXME: store this template in per isolate data
    void setWrapperTemplate(v8::Local<v8::FunctionTemplate> wrapperTemplate, v8::Isolate* isolate) { m_wrapperTemplate.Reset(isolate, wrapperTemplate); }
    v8::Local<v8::FunctionTemplate> wrapperTemplate(v8::Isolate* isolate) { return v8::Local<v8::FunctionTemplate>::New(isolate, m_wrapperTemplate); }

private:
    InjectedScriptHost();

    RawPtrWillBeMember<InspectorConsoleAgent> m_consoleAgent;
    RawPtrWillBeMember<InspectorDebuggerAgent> m_debuggerAgent;
    OwnPtr<InspectCallback> m_inspectCallback;
    RawPtrWillBeMember<V8Debugger> m_debugger;
    Vector<OwnPtr<InspectableObject> > m_inspectedObjects;
    OwnPtr<InspectableObject> m_defaultInspectableObject;
    OwnPtr<InjectedScriptHostClient> m_client;
    v8::Global<v8::FunctionTemplate> m_wrapperTemplate;
};

} // namespace blink

#endif // InjectedScriptHost_h
