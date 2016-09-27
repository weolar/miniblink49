/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef InspectorRuntimeAgent_h
#define InspectorRuntimeAgent_h

#include "core/CoreExport.h"
#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"

namespace blink {

class InjectedScript;
class InjectedScriptManager;
class JSONArray;
class ScriptState;
class V8Debugger;

typedef String ErrorString;

class CORE_EXPORT InspectorRuntimeAgent : public InspectorBaseAgent<InspectorRuntimeAgent, InspectorFrontend::Runtime>, public InspectorBackendDispatcher::RuntimeCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorRuntimeAgent);
public:
    class Client {
    public:
        virtual ~Client() { }

        virtual void resumeStartup() { }
        virtual bool isRunRequired() { return false; }
    };

    virtual ~InspectorRuntimeAgent();
    DECLARE_VIRTUAL_TRACE();

    // Part of the protocol.
    void enable(ErrorString*) override;
    void disable(ErrorString*) override;
    void restore() override;

    void evaluate(ErrorString*,
        const String& expression,
        const String* objectGroup,
        const bool* includeCommandLineAPI,
        const bool* doNotPauseOnExceptionsAndMuteConsole,
        const int* executionContextId,
        const bool* returnByValue,
        const bool* generatePreview,
        RefPtr<TypeBuilder::Runtime::RemoteObject>& result,
        TypeBuilder::OptOutput<bool>* wasThrown,
        RefPtr<TypeBuilder::Debugger::ExceptionDetails>&) final;
    void callFunctionOn(ErrorString*,
                        const String& objectId,
                        const String& expression,
                        const RefPtr<JSONArray>* optionalArguments,
                        const bool* doNotPauseOnExceptionsAndMuteConsole,
                        const bool* returnByValue,
                        const bool* generatePreview,
                        RefPtr<TypeBuilder::Runtime::RemoteObject>& result,
                        TypeBuilder::OptOutput<bool>* wasThrown) final;
    void releaseObject(ErrorString*, const String& objectId) final;
    void getProperties(ErrorString*, const String& objectId, const bool* ownProperties, const bool* accessorPropertiesOnly, const bool* generatePreview, RefPtr<TypeBuilder::Array<TypeBuilder::Runtime::PropertyDescriptor>>& result, RefPtr<TypeBuilder::Array<TypeBuilder::Runtime::InternalPropertyDescriptor>>& internalProperties, RefPtr<TypeBuilder::Debugger::ExceptionDetails>&) final;
    void releaseObjectGroup(ErrorString*, const String& objectGroup) final;
    void run(ErrorString*) override;
    void isRunRequired(ErrorString*, bool* out_result) override;
    void setCustomObjectFormatterEnabled(ErrorString*, bool) final;

protected:
    InspectorRuntimeAgent(InjectedScriptManager*, V8Debugger*, Client*);
    virtual InjectedScript injectedScriptForEval(ErrorString*, const int* executionContextId) = 0;

    virtual void muteConsole() = 0;
    virtual void unmuteConsole() = 0;

    InjectedScriptManager* injectedScriptManager() { return m_injectedScriptManager; }
    void addExecutionContextToFrontend(int executionContextId, const String& type, const String& origin, const String& humanReadableName, const String& frameId);

    bool m_enabled;

private:
    class InjectedScriptCallScope;

    RawPtrWillBeMember<InjectedScriptManager> m_injectedScriptManager;
    V8Debugger* m_debugger;
    Client* m_client;
};

} // namespace blink

#endif // InspectorRuntimeAgent_h
