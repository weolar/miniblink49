/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2009, 2011 Google Inc. All Rights Reserved.
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

#include "config.h"
#include "core/workers/WorkerGlobalScope.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScheduledAction.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8CacheOptions.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/AddConsoleMessageTask.h"
#include "core/dom/ContextLifecycleNotifier.h"
#include "core/dom/CrossThreadTask.h"
#include "core/dom/DOMURL.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/MessagePort.h"
#include "core/events/ErrorEvent.h"
#include "core/events/Event.h"
#include "core/fetch/MemoryCache.h"
#include "core/frame/DOMTimer.h"
#include "core/frame/DOMTimerCoordinator.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/ConsoleMessageStorage.h"
#include "core/inspector/InspectorConsoleInstrumentation.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/inspector/WorkerInspectorController.h"
#include "core/loader/WorkerThreadableLoader.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/workers/WorkerNavigator.h"
#include "core/workers/WorkerClients.h"
#include "core/workers/WorkerConsole.h"
#include "core/workers/WorkerLoaderProxy.h"
#include "core/workers/WorkerLocation.h"
#include "core/workers/WorkerNavigator.h"
#include "core/workers/WorkerReportingProxy.h"
#include "core/workers/WorkerScriptLoader.h"
#include "core/workers/WorkerThread.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/WebURLRequest.h"

namespace blink {

WorkerGlobalScope::WorkerGlobalScope(const KURL& url, const String& userAgent, WorkerThread* thread, double timeOrigin, const SecurityOrigin* starterOrigin, PassOwnPtrWillBeRawPtr<WorkerClients> workerClients)
    : m_url(url)
    , m_userAgent(userAgent)
    , m_v8CacheOptions(V8CacheOptionsDefault)
    , m_script(adoptPtr(new WorkerScriptController(*this, thread->isolate())))
    , m_thread(thread)
    , m_workerInspectorController(adoptRefWillBeNoop(new WorkerInspectorController(this)))
    , m_closing(false)
    , m_eventQueue(WorkerEventQueue::create(this))
    , m_workerClients(workerClients)
    , m_timeOrigin(timeOrigin)
    , m_messageStorage(ConsoleMessageStorage::create())
    , m_workerExceptionUniqueIdentifier(0)
{
    setSecurityOrigin(SecurityOrigin::create(url));
    if (starterOrigin)
        securityOrigin()->transferPrivilegesFrom(*starterOrigin);

    if (m_workerClients)
        m_workerClients->reattachThread();

    m_thread->setWorkerInspectorController(m_workerInspectorController.get());
}

WorkerGlobalScope::~WorkerGlobalScope()
{
    ASSERT(!m_script);
    ASSERT(!m_workerInspectorController);
}

void WorkerGlobalScope::applyContentSecurityPolicyFromVector(const Vector<CSPHeaderAndType>& headers)
{
    if (!contentSecurityPolicy()) {
        RefPtr<ContentSecurityPolicy> csp = ContentSecurityPolicy::create();
        setContentSecurityPolicy(csp);
    }
    for (const auto& policyAndType : headers)
        contentSecurityPolicy()->didReceiveHeader(policyAndType.first, policyAndType.second, ContentSecurityPolicyHeaderSourceHTTP);
    contentSecurityPolicy()->bindToExecutionContext(executionContext());
}

ExecutionContext* WorkerGlobalScope::executionContext() const
{
    return const_cast<WorkerGlobalScope*>(this);
}

const KURL& WorkerGlobalScope::virtualURL() const
{
    return m_url;
}

KURL WorkerGlobalScope::virtualCompleteURL(const String& url) const
{
    return completeURL(url);
}

KURL WorkerGlobalScope::completeURL(const String& url) const
{
    // Always return a null URL when passed a null string.
    // FIXME: Should we change the KURL constructor to have this behavior?
    if (url.isNull())
        return KURL();
    // Always use UTF-8 in Workers.
    return KURL(m_url, url);
}

String WorkerGlobalScope::userAgent(const KURL&) const
{
    return m_userAgent;
}

void WorkerGlobalScope::disableEval(const String& errorMessage)
{
    m_script->disableEval(errorMessage);
}

double WorkerGlobalScope::timerAlignmentInterval() const
{
    return DOMTimer::visiblePageAlignmentInterval();
}

DOMTimerCoordinator* WorkerGlobalScope::timers()
{
    return &m_timers;
}

WorkerLocation* WorkerGlobalScope::location() const
{
    if (!m_location)
        m_location = WorkerLocation::create(m_url);
    return m_location.get();
}

void WorkerGlobalScope::close()
{
    // Let current script run to completion, but tell the worker micro task runner to tear down the thread after this task.
    m_closing = true;
}

WorkerConsole* WorkerGlobalScope::console()
{
    if (!m_console)
        m_console = WorkerConsole::create(this);
    return m_console.get();
}

WorkerNavigator* WorkerGlobalScope::navigator() const
{
    if (!m_navigator)
        m_navigator = WorkerNavigator::create(m_userAgent);
    return m_navigator.get();
}

void WorkerGlobalScope::postTask(const WebTraceLocation& location, PassOwnPtr<ExecutionContextTask> task)
{
    thread()->postTask(location, task);
}

void WorkerGlobalScope::clearInspector()
{
    ASSERT(m_workerInspectorController);
    thread()->setWorkerInspectorController(nullptr);
    m_workerInspectorController->dispose();
    m_workerInspectorController.clear();
}

void WorkerGlobalScope::dispose()
{
    ASSERT(thread()->isCurrentThread());
    stopActiveDOMObjects();

    // Event listeners would keep DOMWrapperWorld objects alive for too long. Also, they have references to JS objects,
    // which become dangling once Heap is destroyed.
    removeAllEventListeners();

    clearScript();
    clearInspector();
    m_eventQueue->close();

    // We do not clear the thread field of the
    // WorkerGlobalScope. Other objects keep the worker global scope
    // alive because they need its thread field to check that work is
    // being carried out on the right thread. We therefore cannot clear
    // the thread field before all references to the worker global
    // scope are gone.
}

void WorkerGlobalScope::didEvaluateWorkerScript()
{
}

void WorkerGlobalScope::importScripts(const Vector<String>& urls, ExceptionState& exceptionState)
{
    ASSERT(contentSecurityPolicy());
    ASSERT(executionContext());

    ExecutionContext& executionContext = *this->executionContext();

    Vector<KURL> completedURLs;
    for (const String& urlString : urls) {
        const KURL& url = executionContext.completeURL(urlString);
        if (!url.isValid()) {
            exceptionState.throwDOMException(SyntaxError, "The URL '" + urlString + "' is invalid.");
            return;
        }
        if (!contentSecurityPolicy()->allowScriptFromSource(url)) {
            exceptionState.throwDOMException(NetworkError, "The script at '" + url.elidedString() + "' failed to load.");
            return;
        }
        completedURLs.append(url);
    }

    for (const KURL& completeURL : completedURLs) {
        WorkerScriptLoader scriptLoader;
        scriptLoader.setRequestContext(WebURLRequest::RequestContextScript);
        scriptLoader.loadSynchronously(executionContext, completeURL, AllowCrossOriginRequests);

        // If the fetching attempt failed, throw a NetworkError exception and abort all these steps.
        if (scriptLoader.failed()) {
            exceptionState.throwDOMException(NetworkError, "The script at '" + completeURL.elidedString() + "' failed to load.");
            return;
        }

        InspectorInstrumentation::scriptImported(&executionContext, scriptLoader.identifier(), scriptLoader.script());
        scriptLoaded(scriptLoader.script().length(), scriptLoader.cachedMetadata() ? scriptLoader.cachedMetadata()->size() : 0);

        RefPtrWillBeRawPtr<ErrorEvent> errorEvent = nullptr;
        OwnPtr<Vector<char>> cachedMetaData(scriptLoader.releaseCachedMetadata());
        OwnPtr<CachedMetadataHandler> handler(createWorkerScriptCachedMetadataHandler(completeURL, cachedMetaData.get()));
        m_script->evaluate(ScriptSourceCode(scriptLoader.script(), scriptLoader.responseURL()), &errorEvent, handler.get(), m_v8CacheOptions);
        if (errorEvent) {
            m_script->rethrowExceptionFromImportedScript(errorEvent.release(), exceptionState);
            return;
        }
    }
}

EventTarget* WorkerGlobalScope::errorEventTarget()
{
    return this;
}

void WorkerGlobalScope::logExceptionToConsole(const String& errorMessage, int, const String& sourceURL, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack)
{
    unsigned long exceptionId = ++m_workerExceptionUniqueIdentifier;
    RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, errorMessage, sourceURL, lineNumber);
    consoleMessage->setCallStack(callStack);
    m_pendingMessages.set(exceptionId, consoleMessage);

    thread()->workerReportingProxy().reportException(errorMessage, lineNumber, columnNumber, sourceURL, exceptionId);
}

void WorkerGlobalScope::reportBlockedScriptExecutionToInspector(const String& directiveText)
{
    InspectorInstrumentation::scriptExecutionBlockedByCSP(this, directiveText);
}

void WorkerGlobalScope::addConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage> prpConsoleMessage)
{
    RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = prpConsoleMessage;
    if (!isContextThread()) {
        postTask(FROM_HERE, AddConsoleMessageTask::create(consoleMessage->source(), consoleMessage->level(), consoleMessage->message()));
        return;
    }
    thread()->workerReportingProxy().reportConsoleMessage(consoleMessage);
    addMessageToWorkerConsole(consoleMessage.release());
}

void WorkerGlobalScope::addMessageToWorkerConsole(PassRefPtrWillBeRawPtr<ConsoleMessage> consoleMessage)
{
    ASSERT(isContextThread());
    m_messageStorage->reportMessage(this, consoleMessage);
}

bool WorkerGlobalScope::isContextThread() const
{
    return thread()->isCurrentThread();
}

bool WorkerGlobalScope::isJSExecutionForbidden() const
{
    return m_script->isExecutionForbidden();
}

WorkerEventQueue* WorkerGlobalScope::eventQueue() const
{
    return m_eventQueue.get();
}

void WorkerGlobalScope::countFeature(UseCounter::Feature) const
{
    // FIXME: How should we count features for shared/service workers?
}

void WorkerGlobalScope::countDeprecation(UseCounter::Feature feature) const
{
    // FIXME: How should we count features for shared/service workers?

    ASSERT(isSharedWorkerGlobalScope() || isServiceWorkerGlobalScope() || isCompositorWorkerGlobalScope());
    // For each deprecated feature, send console message at most once
    // per worker lifecycle.
    if (!m_deprecationWarningBits.hasRecordedMeasurement(feature)) {
        m_deprecationWarningBits.recordMeasurement(feature);
        ASSERT(!UseCounter::deprecationMessage(feature).isEmpty());
        ASSERT(executionContext());
        executionContext()->addConsoleMessage(ConsoleMessage::create(DeprecationMessageSource, WarningMessageLevel, UseCounter::deprecationMessage(feature)));
    }
}

ConsoleMessageStorage* WorkerGlobalScope::messageStorage()
{
    return m_messageStorage.get();
}

void WorkerGlobalScope::exceptionHandled(int exceptionId, bool isHandled)
{
    RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = m_pendingMessages.take(exceptionId);
    if (!isHandled)
        addConsoleMessage(consoleMessage.release());
}

bool WorkerGlobalScope::isPrivilegedContext(String& errorMessage, const PrivilegeContextCheck privilegeContextCheck) const
{
    // Until there are APIs that are available in workers and that
    // require a privileged context test that checks ancestors, just do
    // a simple check here. Once we have a need for a real
    // |isPrivilegedContext| check here, we can check the responsible
    // document for a privileged context at worker creation time, pass
    // it in via WorkerThreadStartupData, and check it here.
    return securityOrigin()->isPotentiallyTrustworthy(errorMessage);
}

void WorkerGlobalScope::removeURLFromMemoryCache(const KURL& url)
{
    m_thread->workerLoaderProxy()->postTaskToLoader(createCrossThreadTask(&WorkerGlobalScope::removeURLFromMemoryCacheInternal, url));
}

void WorkerGlobalScope::removeURLFromMemoryCacheInternal(const KURL& url)
{
    memoryCache()->removeURLFromCache(url);
}

v8::Local<v8::Object> WorkerGlobalScope::wrap(v8::Isolate*, v8::Local<v8::Object> creationContext)
{
    // WorkerGlobalScope must never be wrapped with wrap method.  The global
    // object of ECMAScript environment is used as the wrapper.
    RELEASE_ASSERT_NOT_REACHED();
    return v8::Local<v8::Object>();
}

v8::Local<v8::Object> WorkerGlobalScope::associateWithWrapper(v8::Isolate*, const WrapperTypeInfo*, v8::Local<v8::Object> wrapper)
{
    RELEASE_ASSERT_NOT_REACHED(); // same as wrap method
    return v8::Local<v8::Object>();
}

DEFINE_TRACE(WorkerGlobalScope)
{
#if ENABLE(OILPAN)
    visitor->trace(m_console);
    visitor->trace(m_location);
    visitor->trace(m_navigator);
    visitor->trace(m_workerInspectorController);
    visitor->trace(m_eventQueue);
    visitor->trace(m_workerClients);
    visitor->trace(m_timers);
    visitor->trace(m_messageStorage);
    visitor->trace(m_pendingMessages);
    HeapSupplementable<WorkerGlobalScope>::trace(visitor);
#endif
    ExecutionContext::trace(visitor);
    EventTargetWithInlineData::trace(visitor);
}

} // namespace blink
