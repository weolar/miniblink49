/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2012 Google Inc. All Rights Reserved.
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

#ifndef ExecutionContext_h
#define ExecutionContext_h

#include "core/CoreExport.h"
#include "core/dom/ContextLifecycleNotifier.h"
#include "core/dom/ContextLifecycleObserver.h"
#include "core/dom/SecurityContext.h"
#include "core/dom/SuspendableTask.h"
#include "core/fetch/AccessControlStatus.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/ReferrerPolicy.h"
#include "wtf/Deque.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class ActiveDOMObject;
class ConsoleMessage;
class DOMTimerCoordinator;
class ErrorEvent;
class EventQueue;
class EventTarget;
class ExecutionContextTask;
class LocalDOMWindow;
class PublicURLManager;
class SecurityOrigin;
class ScriptCallStack;

class CORE_EXPORT ExecutionContext
    : public ContextLifecycleNotifier, public WillBeHeapSupplementable<ExecutionContext> {
    WTF_MAKE_NONCOPYABLE(ExecutionContext);
public:
    DECLARE_VIRTUAL_TRACE();

    // Used to specify whether |isPrivilegedContext| should walk the
    // ancestor tree to decide whether to restrict usage of a powerful
    // feature.
    enum PrivilegeContextCheck {
        StandardPrivilegeCheck,
        WebCryptoPrivilegeCheck
    };

    virtual bool isDocument() const { return false; }
    virtual bool isWorkerGlobalScope() const { return false; }
    virtual bool isDedicatedWorkerGlobalScope() const { return false; }
    virtual bool isSharedWorkerGlobalScope() const { return false; }
    virtual bool isServiceWorkerGlobalScope() const { return false; }
    virtual bool isCompositorWorkerGlobalScope() const { return false; }
    virtual bool isJSExecutionForbidden() const { return false; }

    virtual bool isContextThread() const { return true; }

    SecurityOrigin* securityOrigin();
    ContentSecurityPolicy* contentSecurityPolicy();
    const KURL& url() const;
    KURL completeURL(const String& url) const;
    virtual void disableEval(const String& errorMessage) = 0;
    virtual LocalDOMWindow* executingWindow() { return 0; }
    virtual String userAgent(const KURL&) const = 0;
    virtual void postTask(const WebTraceLocation&, PassOwnPtr<ExecutionContextTask>) = 0; // Executes the task on context's thread asynchronously.
    virtual double timerAlignmentInterval() const = 0;

    // Gets the DOMTimerCoordinator which maintains the "active timer
    // list" of tasks created by setTimeout and setInterval. The
    // DOMTimerCoordinator is owned by the ExecutionContext and should
    // not be used after the ExecutionContext is destroyed.
    virtual DOMTimerCoordinator* timers() = 0;

    virtual void reportBlockedScriptExecutionToInspector(const String& directiveText) = 0;

    virtual SecurityContext& securityContext() = 0;
    KURL contextURL() const { return virtualURL(); }
    KURL contextCompleteURL(const String& url) const { return virtualCompleteURL(url); }

    bool shouldSanitizeScriptError(const String& sourceURL, AccessControlStatus);
    void reportException(PassRefPtrWillBeRawPtr<ErrorEvent>, int scriptId, PassRefPtrWillBeRawPtr<ScriptCallStack>, AccessControlStatus);

    virtual void addConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage>) = 0;
    virtual void logExceptionToConsole(const String& errorMessage, int scriptId, const String& sourceURL, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack>) = 0;

    PublicURLManager& publicURLManager();

    virtual void removeURLFromMemoryCache(const KURL&);

    void suspendActiveDOMObjects();
    void resumeActiveDOMObjects();
    void stopActiveDOMObjects();
    void postSuspendableTask(PassOwnPtr<SuspendableTask>);
    void notifyContextDestroyed() override;

    virtual void suspendScheduledTasks();
    virtual void resumeScheduledTasks();
    virtual bool tasksNeedSuspension() { return false; }
    virtual void tasksWereSuspended() { }
    virtual void tasksWereResumed() { }

    bool activeDOMObjectsAreSuspended() const { return m_activeDOMObjectsAreSuspended; }
    bool activeDOMObjectsAreStopped() const { return m_activeDOMObjectsAreStopped; }

    // Called after the construction of an ActiveDOMObject to synchronize suspend state.
    void suspendActiveDOMObjectIfNeeded(ActiveDOMObject*);
#if !ENABLE(OILPAN)
    void ref() { refExecutionContext(); }
    void deref() { derefExecutionContext(); }
#endif

    // Gets the next id in a circular sequence from 1 to 2^31-1.
    int circularSequentialID();

    virtual EventTarget* errorEventTarget() = 0;
    virtual EventQueue* eventQueue() const = 0;

    void enforceStrictMixedContentChecking() { m_strictMixedContentCheckingEnforced = true; }
    bool shouldEnforceStrictMixedContentChecking() const { return m_strictMixedContentCheckingEnforced; }

    void enforceSuborigin(const String& name);
    bool hasSuborigin();
    String suboriginName();

    // Methods related to window interaction. It should be used to manage window
    // focusing and window creation permission for an ExecutionContext.
    void allowWindowInteraction();
    void consumeWindowInteraction();
    bool isWindowInteractionAllowed() const;

    // Decides whether this context is privileged, as described in
    // https://w3c.github.io/webappsec/specs/powerfulfeatures/#settings-privileged.
    virtual bool isPrivilegedContext(String& errorMessage, const PrivilegeContextCheck = StandardPrivilegeCheck) const = 0;

    virtual void setReferrerPolicy(ReferrerPolicy);
    ReferrerPolicy referrerPolicy() const { return m_referrerPolicy; }

protected:
    ExecutionContext();
    virtual ~ExecutionContext();

    virtual const KURL& virtualURL() const = 0;
    virtual KURL virtualCompleteURL(const String&) const = 0;

private:
    bool dispatchErrorEvent(PassRefPtrWillBeRawPtr<ErrorEvent>, AccessControlStatus);
    void runSuspendableTasks();

#if !ENABLE(OILPAN)
    virtual void refExecutionContext() = 0;
    virtual void derefExecutionContext() = 0;
#endif
    // LifecycleContext implementation.

    int m_circularSequentialID;

    bool m_inDispatchErrorEvent;
    class PendingException;
    OwnPtrWillBeMember<WillBeHeapVector<OwnPtrWillBeMember<PendingException>>> m_pendingExceptions;

    bool m_activeDOMObjectsAreSuspended;
    bool m_activeDOMObjectsAreStopped;

    OwnPtrWillBeMember<PublicURLManager> m_publicURLManager;

    bool m_strictMixedContentCheckingEnforced;

    // Counter that keeps track of how many window interaction calls are allowed
    // for this ExecutionContext. Callers are expected to call
    // |allowWindowInteraction()| and |consumeWindowInteraction()| in order to
    // increment and decrement the counter.
    int m_windowInteractionTokens;

    Deque<OwnPtr<SuspendableTask>> m_suspendedTasks;
    bool m_isRunSuspendableTasksScheduled;

    ReferrerPolicy m_referrerPolicy;
};

} // namespace blink

#endif // ExecutionContext_h
