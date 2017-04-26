/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#ifndef InjectedScript_h
#define InjectedScript_h

#include "platform/inspector_protocol/Allocator.h"
#include "platform/inspector_protocol/Platform.h"
#include "platform/v8_inspector/InjectedScriptNative.h"
#include "platform/v8_inspector/InspectedContext.h"
#include "platform/v8_inspector/V8Console.h"
#include "platform/v8_inspector/V8Debugger.h"
#include "platform/v8_inspector/protocol/Runtime.h"

#include <v8.h>

namespace blink {

class RemoteObjectId;
class V8FunctionCall;
class V8InspectorImpl;
class V8InspectorSessionImpl;

namespace protocol {
class DictionaryValue;
}

using protocol::Maybe;

class InjectedScript final {
    PROTOCOL_DISALLOW_COPY(InjectedScript);
public:
    static std::unique_ptr<InjectedScript> create(InspectedContext*);
    ~InjectedScript();

    InspectedContext* context() const { return m_context; }

    void getProperties(ErrorString*, v8::Local<v8::Object>, const String16& groupName, bool ownProperties, bool accessorPropertiesOnly, bool generatePreview, std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>>* result, Maybe<protocol::Runtime::ExceptionDetails>*);
    void releaseObject(const String16& objectId);

    std::unique_ptr<protocol::Runtime::RemoteObject> wrapObject(ErrorString*, v8::Local<v8::Value>, const String16& groupName, bool forceValueType = false, bool generatePreview = false) const;
    bool wrapObjectProperty(ErrorString*, v8::Local<v8::Object>, v8::Local<v8::Value> key, const String16& groupName, bool forceValueType = false, bool generatePreview = false) const;
    bool wrapPropertyInArray(ErrorString*, v8::Local<v8::Array>, v8::Local<v8::String> property, const String16& groupName, bool forceValueType = false, bool generatePreview = false) const;
    bool wrapObjectsInArray(ErrorString*, v8::Local<v8::Array>, const String16& groupName, bool forceValueType = false, bool generatePreview = false) const;
    std::unique_ptr<protocol::Runtime::RemoteObject> wrapTable(v8::Local<v8::Value> table, v8::Local<v8::Value> columns) const;

    bool findObject(ErrorString*, const RemoteObjectId&, v8::Local<v8::Value>*) const;
    String16 objectGroupName(const RemoteObjectId&) const;
    void releaseObjectGroup(const String16&);
    void setCustomObjectFormatterEnabled(bool);
    v8::MaybeLocal<v8::Value> resolveCallArgument(ErrorString*, protocol::Runtime::CallArgument*);

    std::unique_ptr<protocol::Runtime::ExceptionDetails> createExceptionDetails(v8::Local<v8::Message>);
    void wrapEvaluateResult(ErrorString*,
        v8::MaybeLocal<v8::Value> maybeResultValue,
        const v8::TryCatch&,
        const String16& objectGroup,
        bool returnByValue,
        bool generatePreview,
        std::unique_ptr<protocol::Runtime::RemoteObject>* result,
        Maybe<bool>* wasThrown,
        Maybe<protocol::Runtime::ExceptionDetails>*);
    v8::Local<v8::Value> lastEvaluationResult() const;

    class Scope {
    public:
        bool initialize();
        bool installCommandLineAPI();
        void ignoreExceptionsAndMuteConsole();
        void pretendUserGesture();
        v8::Local<v8::Context> context() const { return m_context; }
        InjectedScript* injectedScript() const { return m_injectedScript; }
        const v8::TryCatch& tryCatch() const { return m_tryCatch; }

    protected:
        Scope(ErrorString*, V8InspectorImpl*, int contextGroupId);
        ~Scope();
        virtual void findInjectedScript(V8InspectorSessionImpl*) = 0;

        ErrorString* m_errorString;
        V8InspectorImpl* m_inspector;
        int m_contextGroupId;
        InjectedScript* m_injectedScript;

    private:
        void cleanup();
        V8Debugger::PauseOnExceptionsState setPauseOnExceptionsState(V8Debugger::PauseOnExceptionsState);

        v8::HandleScope m_handleScope;
        v8::TryCatch m_tryCatch;
        v8::Local<v8::Context> m_context;
        std::unique_ptr<V8Console::CommandLineAPIScope> m_commandLineAPIScope;
        bool m_ignoreExceptionsAndMuteConsole;
        V8Debugger::PauseOnExceptionsState m_previousPauseOnExceptionsState;
        bool m_userGesture;
    };

    class ContextScope: public Scope {
        PROTOCOL_DISALLOW_COPY(ContextScope);
    public:
        ContextScope(ErrorString*, V8InspectorImpl*, int contextGroupId, int executionContextId);
        ~ContextScope();
    private:
        void findInjectedScript(V8InspectorSessionImpl*) override;
        int m_executionContextId;
    };

    class ObjectScope: public Scope {
        PROTOCOL_DISALLOW_COPY(ObjectScope);
    public:
        ObjectScope(ErrorString*, V8InspectorImpl*, int contextGroupId, const String16& remoteObjectId);
        ~ObjectScope();
        const String16& objectGroupName() const { return m_objectGroupName; }
        v8::Local<v8::Value> object() const { return m_object; }
    private:
        void findInjectedScript(V8InspectorSessionImpl*) override;
        String16 m_remoteObjectId;
        String16 m_objectGroupName;
        v8::Local<v8::Value> m_object;
    };

    class CallFrameScope: public Scope {
        PROTOCOL_DISALLOW_COPY(CallFrameScope);
    public:
        CallFrameScope(ErrorString*, V8InspectorImpl*, int contextGroupId, const String16& remoteCallFrameId);
        ~CallFrameScope();
        size_t frameOrdinal() const { return m_frameOrdinal; }
    private:
        void findInjectedScript(V8InspectorSessionImpl*) override;
        String16 m_remoteCallFrameId;
        size_t m_frameOrdinal;
    };

private:
    InjectedScript(InspectedContext*, v8::Local<v8::Object>, std::unique_ptr<InjectedScriptNative>);
    v8::Local<v8::Value> v8Value() const;
    v8::MaybeLocal<v8::Value> wrapValue(ErrorString*, v8::Local<v8::Value>, const String16& groupName, bool forceValueType, bool generatePreview) const;
    v8::Local<v8::Object> commandLineAPI();

    InspectedContext* m_context;
    v8::Global<v8::Value> m_value;
    v8::Global<v8::Value> m_lastEvaluationResult;
    std::unique_ptr<InjectedScriptNative> m_native;
    v8::Global<v8::Object> m_commandLineAPI;
};

} // namespace blink

#endif
