/*
 * Copyright (c) 2010, Google Inc. All rights reserved.
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

#ifndef V8Debugger_h
#define V8Debugger_h

#include "core/CoreExport.h"
#include "core/InspectorTypeBuilder.h"
#include "core/inspector/ScriptBreakpoint.h"
#include "core/inspector/ScriptDebugListener.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

#include <v8-debug.h>
#include <v8.h>

namespace blink {

class ScriptState;
class ScriptDebugListener;
class ScriptValue;
class JavaScriptCallFrame;

class CORE_EXPORT V8Debugger : public NoBaseWillBeGarbageCollectedFinalized<V8Debugger> {
    WTF_MAKE_NONCOPYABLE(V8Debugger);
public:
    class CORE_EXPORT Client : public WillBeGarbageCollectedMixin {
    public:
        virtual ~Client() { }
        virtual v8::Local<v8::Object> compileDebuggerScript() = 0;
        virtual ScriptDebugListener* getDebugListenerForContext(v8::Local<v8::Context>) = 0;
        virtual void runMessageLoopOnPause(v8::Local<v8::Context>) = 0;
        virtual void quitMessageLoopOnPause() = 0;

        DEFINE_INLINE_VIRTUAL_TRACE() { }
    };

    static PassOwnPtrWillBeRawPtr<V8Debugger> create(v8::Isolate* isolate, Client* client)
    {
        return adoptPtrWillBeNoop(new V8Debugger(isolate, client));
    }

    virtual ~V8Debugger();
    DECLARE_VIRTUAL_TRACE();

    void enable();
    void disable();
    bool enabled() const;

    static void setContextDebugData(v8::Local<v8::Context>, const String& contextDebugData);
    // Each script inherits debug data from v8::Context where it has been compiled.
    // Only scripts whose debug data contains |contextDebugDataSubstring| substring will be reported.
    // Passing empty string will result in reporting all scripts.
    void getCompiledScripts(const String& contextDebugDataSubstring, Vector<ScriptDebugListener::ParsedScript>&);

    String setBreakpoint(const String& sourceID, const ScriptBreakpoint&, int* actualLineNumber, int* actualColumnNumber, bool interstatementLocation);
    void removeBreakpoint(const String& breakpointId);
    void setBreakpointsActivated(bool);

    enum PauseOnExceptionsState {
        DontPauseOnExceptions,
        PauseOnAllExceptions,
        PauseOnUncaughtExceptions
    };
    PauseOnExceptionsState pauseOnExceptionsState();
    void setPauseOnExceptionsState(PauseOnExceptionsState);

    void setPauseOnNextStatement(bool);
    bool pausingOnNextStatement();
    bool canBreakProgram();
    void breakProgram();
    void continueProgram();
    void stepIntoStatement();
    void stepOverStatement();
    void stepOutOfFunction();
    void clearStepping();

    bool setScriptSource(const String& sourceID, const String& newContent, bool preview, String* error, RefPtr<TypeBuilder::Debugger::SetScriptSourceError>&, ScriptValue* newCallFrames, RefPtr<JSONObject>* result);
    ScriptValue currentCallFrames();
    ScriptValue currentCallFramesForAsyncStack();
    PassRefPtr<JavaScriptCallFrame> callFrameNoScopes(int index);
    int frameCount();

    static PassRefPtr<JavaScriptCallFrame> toJavaScriptCallFrameUnsafe(const ScriptValue&);

    bool isPaused();

    v8::Local<v8::Value> functionScopes(v8::Local<v8::Function>);
    v8::Local<v8::Value> generatorObjectDetails(v8::Local<v8::Object>&);
    v8::Local<v8::Value> collectionEntries(v8::Local<v8::Object>&);
    v8::MaybeLocal<v8::Value> setFunctionVariableValue(v8::Local<v8::Value> functionValue, int scopeNumber, const String& variableName, v8::Local<v8::Value> newValue);

    v8::Isolate* isolate() const { return m_isolate; }

private:
    V8Debugger(v8::Isolate*, Client*);

    void compileDebuggerScript();
    v8::MaybeLocal<v8::Value> callDebuggerMethod(const char* functionName, int argc, v8::Local<v8::Value> argv[]);
    v8::Local<v8::Object> debuggerScriptLocal() const;
    v8::Local<v8::Context> debuggerContext() const;
    void clearBreakpoints();

    ScriptDebugListener::ParsedScript createParsedScript(v8::Local<v8::Object> sourceObject, CompileResult);

    static void breakProgramCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    void handleProgramBreak(ScriptState* pausedScriptState, v8::Local<v8::Object> executionState, v8::Local<v8::Value> exception, v8::Local<v8::Array> hitBreakpoints, bool isPromiseRejection = false);
    static void v8DebugEventCallback(const v8::Debug::EventDetails&);
    v8::Local<v8::Value> callInternalGetterFunction(v8::Local<v8::Object>, const char* functionName);
    void handleV8DebugEvent(const v8::Debug::EventDetails&);

    v8::Local<v8::String> v8InternalizedString(const char*) const;

    enum ScopeInfoDetails {
        AllScopes,
        FastAsyncScopes,
        NoScopes // Should be the last option.
    };
    ScriptValue currentCallFramesInner(ScopeInfoDetails);
    PassRefPtr<JavaScriptCallFrame> wrapCallFrames(int maximumLimit, ScopeInfoDetails);
    void handleV8AsyncTaskEvent(ScriptDebugListener*, ScriptState* pausedScriptState, v8::Local<v8::Object> executionState, v8::Local<v8::Object> eventData);
    void handleV8PromiseEvent(ScriptDebugListener*, ScriptState* pausedScriptState, v8::Local<v8::Object> executionState, v8::Local<v8::Object> eventData);

    v8::Isolate* m_isolate;
    Client* m_client;
    bool m_breakpointsActivated;
    v8::UniquePersistent<v8::FunctionTemplate> m_breakProgramCallbackTemplate;
    v8::UniquePersistent<v8::Object> m_debuggerScript;
    v8::UniquePersistent<v8::Context> m_debuggerContext;
    v8::UniquePersistent<v8::FunctionTemplate> m_callFrameWrapperTemplate;
    v8::Local<v8::Object> m_executionState;
    RefPtr<ScriptState> m_pausedScriptState;
    bool m_runningNestedMessageLoop;
};

} // namespace blink


#endif // V8Debugger_h
