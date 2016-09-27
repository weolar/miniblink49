/*
 * Copyright (C) 2008, 2009 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef WorkerGlobalScope_h
#define WorkerGlobalScope_h

#include "bindings/core/v8/V8CacheOptions.h"
#include "bindings/core/v8/WorkerScriptController.h"
#include "core/CoreExport.h"
#include "core/dom/ExecutionContext.h"
#include "core/events/EventListener.h"
#include "core/events/EventTarget.h"
#include "core/fetch/CachedMetadataHandler.h"
#include "core/frame/DOMTimerCoordinator.h"
#include "core/frame/DOMWindowBase64.h"
#include "core/frame/UseCounter.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/workers/WorkerEventQueue.h"
#include "platform/heap/Handle.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "wtf/Assertions.h"
#include "wtf/HashMap.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/text/AtomicStringHash.h"

namespace blink {

class ConsoleMessage;
class ConsoleMessageStorage;
class ExceptionState;
class WorkerClients;
class WorkerConsole;
class WorkerInspectorController;
class WorkerLocation;
class WorkerNavigator;
class WorkerThread;

class CORE_EXPORT WorkerGlobalScope : public EventTargetWithInlineData, public RefCountedWillBeNoBase<WorkerGlobalScope>, public SecurityContext, public ExecutionContext, public WillBeHeapSupplementable<WorkerGlobalScope>, public DOMWindowBase64 {
    DEFINE_WRAPPERTYPEINFO();
    REFCOUNTED_EVENT_TARGET(WorkerGlobalScope);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(WorkerGlobalScope);
public:
    virtual ~WorkerGlobalScope();

    bool isWorkerGlobalScope() const override final { return true; }

    ExecutionContext* executionContext() const override final;

    virtual void countFeature(UseCounter::Feature) const;
    virtual void countDeprecation(UseCounter::Feature) const;

    const KURL& url() const { return m_url; }
    KURL completeURL(const String&) const;

    String userAgent(const KURL&) const override final;
    void disableEval(const String& errorMessage) override final;

    WorkerScriptController* script() { return m_script.get(); }

    virtual void didEvaluateWorkerScript();
    void dispose();

    WorkerThread* thread() const { return m_thread; }

    void postTask(const WebTraceLocation&, PassOwnPtr<ExecutionContextTask>) override final; // Executes the task on context's thread asynchronously.

    // WorkerGlobalScope
    WorkerGlobalScope* self() { return this; }
    WorkerConsole* console();
    WorkerLocation* location() const;
    void close();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(rejectionhandled);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(unhandledrejection);

    // WorkerUtils
    virtual void importScripts(const Vector<String>& urls, ExceptionState&);
    // Returns null if caching is not supported.
    virtual PassOwnPtr<CachedMetadataHandler> createWorkerScriptCachedMetadataHandler(const KURL& scriptURL, const Vector<char>* metaData) { return nullptr; }

    WorkerNavigator* navigator() const;

    // ScriptWrappable
    v8::Local<v8::Object> wrap(v8::Isolate*, v8::Local<v8::Object> creationContext) override final;
    v8::Local<v8::Object> associateWithWrapper(v8::Isolate*, const WrapperTypeInfo*, v8::Local<v8::Object> wrapper) override final;

    // ExecutionContextClient
    WorkerEventQueue* eventQueue() const override final;
    SecurityContext& securityContext() override final { return *this; }

    bool isContextThread() const override final;
    bool isJSExecutionForbidden() const override final;

    double timerAlignmentInterval() const override final;
    DOMTimerCoordinator* timers() override final;

    WorkerInspectorController* workerInspectorController() { return m_workerInspectorController.get(); }

    bool isClosing() { return m_closing; }

    double timeOrigin() const { return m_timeOrigin; }

    WorkerClients* clients() { return m_workerClients.get(); }

    using SecurityContext::securityOrigin;
    using SecurityContext::contentSecurityPolicy;

    void addConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage>) override final;
    ConsoleMessageStorage* messageStorage();

    void exceptionHandled(int exceptionId, bool isHandled);

    virtual void scriptLoaded(size_t scriptSize, size_t cachedMetadataSize) { }

    bool isPrivilegedContext(String& errorMessage, const PrivilegeContextCheck = StandardPrivilegeCheck) const override;

    DECLARE_VIRTUAL_TRACE();

protected:
    WorkerGlobalScope(const KURL&, const String& userAgent, WorkerThread*, double timeOrigin, const SecurityOrigin*, PassOwnPtrWillBeRawPtr<WorkerClients>);
    void applyContentSecurityPolicyFromVector(const Vector<CSPHeaderAndType>& headers);

    void logExceptionToConsole(const String& errorMessage, int scriptId, const String& sourceURL, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack>) override;
    void addMessageToWorkerConsole(PassRefPtrWillBeRawPtr<ConsoleMessage>);
    void setV8CacheOptions(V8CacheOptions v8CacheOptions) { m_v8CacheOptions = v8CacheOptions; }

    void removeURLFromMemoryCache(const KURL&) override;

private:
#if !ENABLE(OILPAN)
    void refExecutionContext() override final { ref(); }
    void derefExecutionContext() override final { deref(); }
#endif

    const KURL& virtualURL() const override final;
    KURL virtualCompleteURL(const String&) const override final;

    void reportBlockedScriptExecutionToInspector(const String& directiveText) override final;

    EventTarget* errorEventTarget() override final;
    void didUpdateSecurityOrigin() override final { }

    void clearScript() { m_script.clear(); }
    void clearInspector();

    static void removeURLFromMemoryCacheInternal(const KURL&);

    KURL m_url;
    String m_userAgent;
    V8CacheOptions m_v8CacheOptions;

    mutable PersistentWillBeMember<WorkerConsole> m_console;
    mutable PersistentWillBeMember<WorkerLocation> m_location;
    mutable PersistentWillBeMember<WorkerNavigator> m_navigator;

    mutable UseCounter::CountBits m_deprecationWarningBits;

    OwnPtr<WorkerScriptController> m_script;
    WorkerThread* m_thread;

    RefPtrWillBeMember<WorkerInspectorController> m_workerInspectorController;
    bool m_closing;

    OwnPtrWillBeMember<WorkerEventQueue> m_eventQueue;

    OwnPtrWillBeMember<WorkerClients> m_workerClients;

    DOMTimerCoordinator m_timers;

    double m_timeOrigin;

    OwnPtrWillBeMember<ConsoleMessageStorage> m_messageStorage;

    unsigned long m_workerExceptionUniqueIdentifier;
    WillBeHeapHashMap<unsigned long, RefPtrWillBeMember<ConsoleMessage>> m_pendingMessages;
};

DEFINE_TYPE_CASTS(WorkerGlobalScope, ExecutionContext, context, context->isWorkerGlobalScope(), context.isWorkerGlobalScope());

} // namespace blink

#endif // WorkerGlobalScope_h
