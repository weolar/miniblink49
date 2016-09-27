/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010-2011 Google Inc. All rights reserved.
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

#ifndef InspectorDebuggerAgent_h
#define InspectorDebuggerAgent_h

#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "core/CoreExport.h"
#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "core/inspector/PromiseTracker.h"
#include "core/inspector/ScriptBreakpoint.h"
#include "core/inspector/ScriptDebugListener.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/ListHashSet.h"
#include "wtf/PassRefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/StringHash.h"

namespace blink {

class AsyncCallChain;
class AsyncCallStack;
class DevToolsFunctionInfo;
class InjectedScript;
class InjectedScriptManager;
class JavaScriptCallFrame;
class JSONObject;
class ScriptAsyncCallStack;
class ScriptRegexp;
class V8AsyncCallTracker;
class V8Debugger;

typedef String ErrorString;

class CORE_EXPORT InspectorDebuggerAgent
    : public InspectorBaseAgent<InspectorDebuggerAgent, InspectorFrontend::Debugger>
    , public ScriptDebugListener
    , public InspectorBackendDispatcher::DebuggerCommandHandler
    , public PromiseTracker::Listener {
    WTF_MAKE_NONCOPYABLE(InspectorDebuggerAgent);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InspectorDebuggerAgent);
public:
    enum BreakpointSource {
        UserBreakpointSource,
        DebugCommandBreakpointSource,
        MonitorCommandBreakpointSource
    };

    static const char backtraceObjectGroup[];

    ~InspectorDebuggerAgent() override;
    DECLARE_VIRTUAL_TRACE();

    void canSetScriptSource(ErrorString*, bool* result) final { *result = true; }

    void init() final;
    void restore() override;
    void disable(ErrorString*) final;

    bool isPaused();

    // Part of the protocol.
    void enable(ErrorString*) override;
    void setBreakpointsActive(ErrorString*, bool active) final;
    void setSkipAllPauses(ErrorString*, bool skipped) final;

    void setBreakpointByUrl(ErrorString*, int lineNumber, const String* optionalURL, const String* optionalURLRegex, const int* optionalColumnNumber, const String* optionalCondition, TypeBuilder::Debugger::BreakpointId*, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::Location> >& locations) final;
    void setBreakpoint(ErrorString*, const RefPtr<JSONObject>& location, const String* optionalCondition, TypeBuilder::Debugger::BreakpointId*, RefPtr<TypeBuilder::Debugger::Location>& actualLocation) final;
    void removeBreakpoint(ErrorString*, const String& breakpointId) final;
    void continueToLocation(ErrorString*, const RefPtr<JSONObject>& location, const bool* interstateLocationOpt) final;
    void getStepInPositions(ErrorString*, const String& callFrameId, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::Location> >& positions) final;
    void getBacktrace(ErrorString*, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::CallFrame> >&, RefPtr<TypeBuilder::Debugger::StackTrace>&) final;
    void searchInContent(ErrorString*, const String& scriptId, const String& query, const bool* optionalCaseSensitive, const bool* optionalIsRegex, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::SearchMatch>>&) final;
    void setScriptSource(ErrorString*, RefPtr<TypeBuilder::Debugger::SetScriptSourceError>&, const String& scriptId, const String& newContent, const bool* preview, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::CallFrame> >& newCallFrames, RefPtr<JSONObject>& result, RefPtr<TypeBuilder::Debugger::StackTrace>& asyncStackTrace) final;
    void restartFrame(ErrorString*, const String& callFrameId, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::CallFrame> >& newCallFrames, RefPtr<JSONObject>& result, RefPtr<TypeBuilder::Debugger::StackTrace>& asyncStackTrace) final;
    void getScriptSource(ErrorString*, const String& scriptId, String* scriptSource) final;
    void getFunctionDetails(ErrorString*, const String& functionId, RefPtr<TypeBuilder::Debugger::FunctionDetails>&) final;
    void getGeneratorObjectDetails(ErrorString*, const String& objectId, RefPtr<TypeBuilder::Debugger::GeneratorObjectDetails>&) final;
    void getCollectionEntries(ErrorString*, const String& objectId, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::CollectionEntry> >&) final;
    void pause(ErrorString*) final;
    void resume(ErrorString*) final;
    void stepOver(ErrorString*) final;
    void stepInto(ErrorString*) final;
    void stepOut(ErrorString*) final;
    void stepIntoAsync(ErrorString*) final;
    void setPauseOnExceptions(ErrorString*, const String& pauseState) final;
    void evaluateOnCallFrame(ErrorString*,
        const String& callFrameId,
        const String& expression,
        const String* objectGroup,
        const bool* includeCommandLineAPI,
        const bool* doNotPauseOnExceptionsAndMuteConsole,
        const bool* returnByValue,
        const bool* generatePreview,
        RefPtr<TypeBuilder::Runtime::RemoteObject>& result,
        TypeBuilder::OptOutput<bool>* wasThrown,
        RefPtr<TypeBuilder::Debugger::ExceptionDetails>&) final;
    void compileScript(ErrorString*, const String& expression, const String& sourceURL, bool persistScript, const int* executionContextId, TypeBuilder::OptOutput<TypeBuilder::Debugger::ScriptId>*, RefPtr<TypeBuilder::Debugger::ExceptionDetails>&) override;
    void runScript(ErrorString*, const TypeBuilder::Debugger::ScriptId&, const int* executionContextId, const String* objectGroup, const bool* doNotPauseOnExceptionsAndMuteConsole, RefPtr<TypeBuilder::Runtime::RemoteObject>& result, RefPtr<TypeBuilder::Debugger::ExceptionDetails>&) override;
    void setVariableValue(ErrorString*, int in_scopeNumber, const String& in_variableName, const RefPtr<JSONObject>& in_newValue, const String* in_callFrame, const String* in_functionObjectId) final;
    void skipStackFrames(ErrorString*, const String* pattern, const bool* skipContentScripts) final;
    void setAsyncCallStackDepth(ErrorString*, int depth) final;
    void enablePromiseTracker(ErrorString*, const bool* captureStacks) final;
    void disablePromiseTracker(ErrorString*) final;
    void getPromiseById(ErrorString*, int promiseId, const String* objectGroup, RefPtr<TypeBuilder::Runtime::RemoteObject>& promise) final;
    void flushAsyncOperationEvents(ErrorString*) final;
    void setAsyncOperationBreakpoint(ErrorString*, int operationId) final;
    void removeAsyncOperationBreakpoint(ErrorString*, int operationId) final;

    void schedulePauseOnNextStatement(InspectorFrontend::Debugger::Reason::Enum breakReason, PassRefPtr<JSONObject> data);
    void didFireTimer();
    void didHandleEvent();
    bool canBreakProgram();
    void breakProgram(InspectorFrontend::Debugger::Reason::Enum breakReason, PassRefPtr<JSONObject> data);
    void scriptExecutionBlockedByCSP(const String& directiveText);
    void willCallFunction(ExecutionContext*, const DevToolsFunctionInfo&);
    void didCallFunction();
    void willEvaluateScript(LocalFrame*, const String& url, int lineNumber);
    void didEvaluateScript();
    bool getEditedScript(const String& url, String* content);

    class CORE_EXPORT Listener : public WillBeGarbageCollectedMixin {
    public:
        virtual ~Listener() { }
        virtual void debuggerWasEnabled() = 0;
        virtual void debuggerWasDisabled() = 0;
        virtual bool canPauseOnPromiseEvent() = 0;
        virtual void didCreatePromise() = 0;
        virtual void didResolvePromise() = 0;
        virtual void didRejectPromise() = 0;
    };
    void setListener(Listener* listener) { m_listener = listener; }

    bool enabled();

    virtual V8Debugger& debugger() = 0;

    void setBreakpoint(const String& scriptId, int lineNumber, int columnNumber, BreakpointSource, const String& condition = String());
    void removeBreakpoint(const String& scriptId, int lineNumber, int columnNumber, BreakpointSource);

    // Async call stacks implementation
    PassRefPtrWillBeRawPtr<ScriptAsyncCallStack> currentAsyncStackTraceForConsole();
    static const int unknownAsyncOperationId;
    int traceAsyncOperationStarting(const String& description);
    void traceAsyncCallbackStarting(int operationId);
    void traceAsyncCallbackCompleted();
    void traceAsyncOperationCompleted(int operationId);
    bool trackingAsyncCalls() const { return m_maxAsyncCallStackDepth; }

    class CORE_EXPORT AsyncCallTrackingListener : public WillBeGarbageCollectedMixin {
    public:
        virtual ~AsyncCallTrackingListener() { }
        DEFINE_INLINE_VIRTUAL_TRACE() { }
        virtual void asyncCallTrackingStateChanged(bool tracking) = 0;
        virtual void resetAsyncOperations() = 0;
    };
    void addAsyncCallTrackingListener(AsyncCallTrackingListener*);
    void removeAsyncCallTrackingListener(AsyncCallTrackingListener*);

    // PromiseTracker::Listener
    void didUpdatePromise(InspectorFrontend::Debugger::EventType::Enum, PassRefPtr<TypeBuilder::Debugger::PromiseDetails>) final;

protected:
    InspectorDebuggerAgent(InjectedScriptManager*, v8::Isolate*);

    virtual void startListeningV8Debugger() = 0;
    virtual void stopListeningV8Debugger() = 0;
    virtual void muteConsole() = 0;
    virtual void unmuteConsole() = 0;
    InjectedScriptManager* injectedScriptManager() { return m_injectedScriptManager; }
    virtual InjectedScript injectedScriptForEval(ErrorString*, const int* executionContextId) = 0;

    virtual void enable();
    virtual void disable();
    SkipPauseRequest didPause(ScriptState*, const ScriptValue& callFrames, const ScriptValue& exception, const Vector<String>& hitBreakpoints, bool isPromiseRejection) final;
    void didContinue() final;
    void reset();
    void resetModifiedSources();

private:
    bool checkEnabled(ErrorString*);

    SkipPauseRequest shouldSkipExceptionPause();
    SkipPauseRequest shouldSkipStepPause();

    void schedulePauseOnNextStatementIfSteppingInto();
    void cancelPauseOnNextStatement();

    PassRefPtr<TypeBuilder::Array<TypeBuilder::Debugger::CallFrame> > currentCallFrames();
    PassRefPtr<TypeBuilder::Debugger::StackTrace> currentAsyncStackTrace();

    void clearCurrentAsyncOperation();
    void resetAsyncCallTracker();

    void changeJavaScriptRecursionLevel(int step);

    void didParseSource(const ParsedScript&) final;
    bool v8AsyncTaskEventsEnabled() const final;
    void didReceiveV8AsyncTaskEvent(ScriptState*, const String& eventType, const String& eventName, int id) final;
    bool v8PromiseEventsEnabled() const final;
    void didReceiveV8PromiseEvent(ScriptState*, v8::Local<v8::Object> promise, v8::Local<v8::Value> parentPromise, int status) final;

    void setPauseOnExceptionsImpl(ErrorString*, int);

    PassRefPtr<TypeBuilder::Debugger::Location> resolveBreakpoint(const String& breakpointId, const String& scriptId, const ScriptBreakpoint&, BreakpointSource);
    void removeBreakpoint(const String& breakpointId);
    void clear();
    void clearStepIntoAsync();
    bool assertPaused(ErrorString*);
    void clearBreakDetails();

    String sourceMapURLForScript(const Script&, CompileResult);

    bool isCallStackEmptyOrBlackboxed();
    bool isTopCallFrameBlackboxed();
    bool isCallFrameWithUnknownScriptOrBlackboxed(PassRefPtr<JavaScriptCallFrame>);
    PromiseTracker& promiseTracker() const { return *m_promiseTracker; }

    void internalSetAsyncCallStackDepth(int);
    void increaseCachedSkipStackGeneration();
    PassRefPtr<TypeBuilder::Debugger::ExceptionDetails> createExceptionDetails(v8::Isolate*, v8::Local<v8::Message>);

    typedef HashMap<String, Script> ScriptsMap;
    typedef HashMap<String, Vector<String>> BreakpointIdToDebuggerBreakpointIdsMap;
    typedef HashMap<String, std::pair<String, BreakpointSource>> DebugServerBreakpointToBreakpointIdAndSourceMap;

    enum DebuggerStep {
        NoStep = 0,
        StepInto,
        StepOver,
        StepOut
    };

    RawPtrWillBeMember<InjectedScriptManager> m_injectedScriptManager;
    RefPtr<ScriptState> m_pausedScriptState;
    ScriptValue m_currentCallStack;
    ScriptsMap m_scripts;
    BreakpointIdToDebuggerBreakpointIdsMap m_breakpointIdToDebuggerBreakpointIds;
    DebugServerBreakpointToBreakpointIdAndSourceMap m_serverBreakpoints;
    String m_continueToLocationBreakpointId;
    InspectorFrontend::Debugger::Reason::Enum m_breakReason;
    RefPtr<JSONObject> m_breakAuxData;
    DebuggerStep m_scheduledDebuggerStep;
    bool m_skipNextDebuggerStepOut;
    bool m_javaScriptPauseScheduled;
    bool m_steppingFromFramework;
    bool m_pausingOnNativeEvent;
    bool m_pausingOnAsyncOperation;
    RawPtrWillBeMember<Listener> m_listener;

    int m_skippedStepFrameCount;
    int m_recursionLevelForStepOut;
    int m_recursionLevelForStepFrame;
    bool m_skipAllPauses;
    bool m_skipContentScripts;
    OwnPtr<ScriptRegexp> m_cachedSkipStackRegExp;
    unsigned m_cachedSkipStackGeneration;
    WillBeHeapHashSet<RawPtrWillBeWeakMember<AsyncCallTrackingListener>> m_asyncCallTrackingListeners;
    // This field must be destroyed before the listeners set above.
    OwnPtrWillBeMember<V8AsyncCallTracker> m_v8AsyncCallTracker;
    OwnPtrWillBeMember<PromiseTracker> m_promiseTracker;
    HashMap<String, String> m_editedScripts;

    using AsyncOperationIdToAsyncCallChain = WillBeHeapHashMap<int, RefPtrWillBeMember<AsyncCallChain>>;
    AsyncOperationIdToAsyncCallChain m_asyncOperations;
    int m_lastAsyncOperationId;
    ListHashSet<int> m_asyncOperationNotifications;
    HashSet<int> m_asyncOperationBreakpoints;
    HashSet<int> m_pausingAsyncOperations;
    unsigned m_maxAsyncCallStackDepth;
    RefPtrWillBeMember<AsyncCallChain> m_currentAsyncCallChain;
    unsigned m_nestedAsyncCallCount;
    int m_currentAsyncOperationId;
    bool m_pendingTraceAsyncOperationCompleted;
    bool m_startingStepIntoAsync;
    V8GlobalValueMap<String, v8::Script, v8::kNotWeak> m_compiledScripts;
};

} // namespace blink


#endif // InspectorDebuggerAgent_h
