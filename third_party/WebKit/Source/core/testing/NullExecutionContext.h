// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NullExecutionContext_h
#define NullExecutionContext_h

#include "core/dom/ExecutionContext.h"
#include "core/dom/SecurityContext.h"
#include "core/events/EventQueue.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/ScriptCallStack.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/RefCounted.h"

namespace blink {

class NullExecutionContext final : public RefCountedWillBeGarbageCollectedFinalized<NullExecutionContext>, public SecurityContext, public ExecutionContext {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(NullExecutionContext);
public:
    NullExecutionContext();

    virtual void disableEval(const String&) override { }
    virtual String userAgent(const KURL&) const override { return String(); }

    virtual void postTask(const WebTraceLocation&, PassOwnPtr<ExecutionContextTask>) override;

    virtual EventTarget* errorEventTarget() override { return nullptr; }
    virtual EventQueue* eventQueue() const override { return m_queue.get(); }

    virtual bool tasksNeedSuspension() override { return m_tasksNeedSuspension; }
    void setTasksNeedSuspension(bool flag) { m_tasksNeedSuspension = flag; }

    virtual void reportBlockedScriptExecutionToInspector(const String& directiveText) override { }
    virtual void didUpdateSecurityOrigin() override { }
    virtual SecurityContext& securityContext() override { return *this; }
    virtual DOMTimerCoordinator* timers() override { return nullptr; }

    double timerAlignmentInterval() const;

    virtual void addConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage>) override { }
    virtual void logExceptionToConsole(const String& errorMessage, int scriptId, const String& sourceURL, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack>) override { }

    bool isPrivilegedContext(String& errorMessage, const PrivilegeContextCheck = StandardPrivilegeCheck) const override;

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_queue);
        ExecutionContext::trace(visitor);
    }

#if !ENABLE(OILPAN)
    using RefCounted<NullExecutionContext>::ref;
    using RefCounted<NullExecutionContext>::deref;

    virtual void refExecutionContext() override { ref(); }
    virtual void derefExecutionContext() override { deref(); }
#endif

protected:
    virtual const KURL& virtualURL() const override { return m_dummyURL; }
    virtual KURL virtualCompleteURL(const String&) const override { return m_dummyURL; }

private:
    bool m_tasksNeedSuspension;
    OwnPtrWillBeMember<EventQueue> m_queue;

    KURL m_dummyURL;
};

} // namespace blink

#endif // NullExecutionContext_h
