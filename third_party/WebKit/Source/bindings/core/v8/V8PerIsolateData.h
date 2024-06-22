/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef V8PerIsolateData_h
#define V8PerIsolateData_h

#include "bindings/core/v8/ScopedPersistent.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "bindings/core/v8/WrapperTypeInfo.h"
#include "bindings/core/v8/ScriptWrappable.h"

#include "core/dom/ActiveDOMObject.h"
#include "core/CoreExport.h"
#include "core/inspector/ScriptDebuggerBase.h"
#include "gin/public/isolate_holder.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/Vector.h"
#include <v8.h>

namespace gin {
class V8ForegroundTaskRunner;
}

namespace v8 {
class TaskRunner;
}

namespace blink {

class DOMDataStore;
class StringCache;
class V8Debugger;
struct WrapperTypeInfo;
class UnifiedHeapController;
class ActiveScriptWrappableManager;
class ActiveDOMObject;
class ScriptWrappable;
class WebThread;

typedef WTF::Vector<DOMDataStore*> DOMDataStoreList;

class CORE_EXPORT V8PerIsolateData {
    WTF_MAKE_NONCOPYABLE(V8PerIsolateData);
public:
    class EndOfScopeTask {
    public:
        virtual ~EndOfScopeTask() { }
        virtual void run() = 0;
    };

    static v8::Isolate* initialize();
    static V8PerIsolateData* from(v8::Isolate* isolate)
    {
        ASSERT(isolate);
        ASSERT(isolate->GetData(gin::kEmbedderBlink));
        return static_cast<V8PerIsolateData*>(isolate->GetData(gin::kEmbedderBlink));
    }

    static void willBeDestroyed(v8::Isolate*);
    static void destroy(v8::Isolate*);
    static v8::Isolate* mainThreadIsolate();

    bool destructionPending() const { return m_destructionPending; }
    v8::Isolate* isolate() { return m_isolateHolder->isolate(); }

    WebThread* getThread() const; // for V8Platform::GetForegroundTaskRunner

    v8::Local<v8::FunctionTemplate> toStringTemplate();

    StringCache* stringCache() { return m_stringCache.get(); }

    v8::Persistent<v8::Value>& ensureLiveRoot();

    int recursionLevel() const { return m_recursionLevel; }
    int incrementRecursionLevel() { return ++m_recursionLevel; }
    int decrementRecursionLevel() { return --m_recursionLevel; }
    bool isHandlingRecursionLevelError() const { return m_isHandlingRecursionLevelError; }
    void setIsHandlingRecursionLevelError(bool value) { m_isHandlingRecursionLevelError = value; }

    bool isReportingException() const { return m_isReportingException; }
    void setReportingException(bool value) { m_isReportingException = value; }

    bool performingMicrotaskCheckpoint() const { return m_performingMicrotaskCheckpoint; }
    void setPerformingMicrotaskCheckpoint(bool performingMicrotaskCheckpoint) { m_performingMicrotaskCheckpoint = performingMicrotaskCheckpoint; }

#if ENABLE(ASSERT)
    int internalScriptRecursionLevel() const { return m_internalScriptRecursionLevel; }
    int incrementInternalScriptRecursionLevel() { return ++m_internalScriptRecursionLevel; }
    int decrementInternalScriptRecursionLevel() { return --m_internalScriptRecursionLevel; }
#endif

    using ActiveScriptWrappableSet = HeapHashSet<WeakMember<ActiveDOMObject>>;
    void addActiveScriptWrappable(ActiveDOMObject*);
    const ActiveScriptWrappableSet* activeScriptWrappables() const;

#if V8_MAJOR_VERSION >= 7
    static std::shared_ptr<v8::TaskRunner> getThreadRunner(v8::Isolate* isolate);
#endif

    V8HiddenValue* hiddenValue() { return m_hiddenValue.get(); }

    v8::Local<v8::FunctionTemplate> domTemplate(const void* domTemplateKey, v8::FunctionCallback = 0, v8::Local<v8::Value> data = v8::Local<v8::Value>(), v8::Local<v8::Signature> = v8::Local<v8::Signature>(), int length = 0);
    v8::Local<v8::FunctionTemplate> existingDOMTemplate(const void* domTemplateKey);
    void setDOMTemplate(const void* domTemplateKey, v8::Local<v8::FunctionTemplate>);

    bool hasInstance(const WrapperTypeInfo* untrusted, v8::Local<v8::Value>);
    v8::Local<v8::Object> findInstanceInPrototypeChain(const WrapperTypeInfo*, v8::Local<v8::Value>);

    v8::Local<v8::Context> ensureScriptRegexpContext();

    const char* previousSamplingState() const { return m_previousSamplingState; }
    void setPreviousSamplingState(const char* name) { m_previousSamplingState = name; }

#if V8_MAJOR_VERSION >= 7
    UnifiedHeapController* getUnifiedHeapController(v8::Isolate* isolate);

    v8::EmbedderHeapTracer* getEmbedderHeapTracer(v8::Isolate* isolate)
    {
        return (v8::EmbedderHeapTracer*)(getUnifiedHeapController(isolate));
    }

    std::vector<std::pair<void*, void*>>* leakV8References();

    ActiveScriptWrappableManager* getActiveScriptWrappableManager() const;
#endif

    // EndOfScopeTasks are run by V8RecursionScope when control is returning
    // to C++ from script, after executing a script task (e.g. callback,
    // event) or microtasks (e.g. promise). This is explicitly needed for
    // Indexed DB transactions per spec, but should in general be avoided.
    void addEndOfScopeTask(PassOwnPtr<EndOfScopeTask>);
    void runEndOfScopeTasks();
    void clearEndOfScopeTasks();

    void setScriptDebugger(PassOwnPtrWillBeRawPtr<ScriptDebuggerBase>);

private:
    V8PerIsolateData();
    ~V8PerIsolateData();

    typedef HashMap<const void*, v8::Eternal<v8::FunctionTemplate>> DOMTemplateMap;
    DOMTemplateMap& currentDOMTemplateMap();
    bool hasInstance(const WrapperTypeInfo* untrusted, v8::Local<v8::Value>, DOMTemplateMap&);
    v8::Local<v8::Object> findInstanceInPrototypeChain(const WrapperTypeInfo*, v8::Local<v8::Value>, DOMTemplateMap&);

    bool m_destructionPending;
#if V8_MAJOR_VERSION >= 7
    std::shared_ptr<gin::V8ForegroundTaskRunner> m_threadRunner;
    OwnPtr<UnifiedHeapController> m_unifiedHeapController;
    OwnPtr<ActiveScriptWrappableManager> m_activeScriptWrappableManager;
#endif

    gin::IsolateHolder* m_isolateHolder;

    DOMTemplateMap m_domTemplateMapForMainWorld;
    DOMTemplateMap m_domTemplateMapForNonMainWorld;
    ScopedPersistent<v8::FunctionTemplate> m_toStringTemplate;
    OwnPtr<StringCache> m_stringCache;
    OwnPtr<V8HiddenValue> m_hiddenValue;

    ScopedPersistent<v8::Value> m_liveRoot;
    RefPtr<ScriptState> m_scriptRegexpScriptState;

    const char* m_previousSamplingState;

    bool m_constructorMode;
    friend class ConstructorMode;

    int m_recursionLevel;
    bool m_isHandlingRecursionLevelError;
    bool m_isReportingException;

    WebThread* m_thread;

#if ENABLE(ASSERT)
    int m_internalScriptRecursionLevel;
#endif
    bool m_performingMicrotaskCheckpoint;

    Persistent<ActiveScriptWrappableSet>* m_activeScriptWrappables;

    Vector<OwnPtr<EndOfScopeTask>> m_endOfScopeTasks;
#if ENABLE(OILPAN)
    CrossThreadPersistent<ScriptDebuggerBase> m_scriptDebugger;
#else
    OwnPtr<ScriptDebuggerBase> m_scriptDebugger;
#endif
};

} // namespace blink

#endif // V8PerIsolateData_h
