/*
 * Copyright (C) 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
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

#include "config.h"
#include "core/frame/DOMWindowTimers.h"

#include "bindings/core/v8/V8GCForContextDispose.h"
#include "core/dom/Document.h"
#include "core/dom/ExecutionContext.h"
#include "core/events/EventTarget.h"
#include "core/frame/DOMTimer.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/workers/WorkerGlobalScope.h"

namespace blink {

namespace DOMWindowTimers {

static bool isAllowed(ScriptState* scriptState, ExecutionContext* executionContext, bool isEval)
{
    if (executionContext->isDocument()) {
        Document* document = static_cast<Document*>(executionContext);
        if (isEval && !document->contentSecurityPolicy()->allowEval(scriptState, ContentSecurityPolicy::SendReport, ContentSecurityPolicy::WillNotThrowException))
            return false;
        return true;
    }
    if (executionContext->isWorkerGlobalScope()) {
        WorkerGlobalScope* workerGlobalScope = static_cast<WorkerGlobalScope*>(executionContext);
        if (!workerGlobalScope->script())
            return false;
        ContentSecurityPolicy* policy = workerGlobalScope->contentSecurityPolicy();
        if (isEval && policy && !policy->allowEval(scriptState, ContentSecurityPolicy::SendReport, ContentSecurityPolicy::WillNotThrowException))
            return false;
        return true;
    }
    ASSERT_NOT_REACHED();
    return false;
}

int setTimeout(ScriptState* scriptState, EventTarget& eventTarget, const ScriptValue& handler, int timeout, const Vector<ScriptValue>& arguments)
{
    ExecutionContext* executionContext = eventTarget.executionContext();
    if (!isAllowed(scriptState, executionContext, false))
        return 0;
    if (timeout >= 0 && executionContext->isDocument()) {
        // FIXME: Crude hack that attempts to pass idle time to V8. This should
        // be done using the scheduler instead.
        V8GCForContextDispose::instance().notifyIdle();
    }
    OwnPtrWillBeRawPtr<ScheduledAction> action = ScheduledAction::create(scriptState, handler, arguments);
    return DOMTimer::install(executionContext, action.release(), timeout, true);
}

int setTimeout(ScriptState* scriptState, EventTarget& eventTarget, const String& handler, int timeout, const Vector<ScriptValue>&)
{
    ExecutionContext* executionContext = eventTarget.executionContext();
    if (!isAllowed(scriptState, executionContext, true))
        return 0;
    // Don't allow setting timeouts to run empty functions.  Was historically a
    // perfomance issue.
    if (handler.isEmpty())
        return 0;
    if (timeout >= 0 && executionContext->isDocument()) {
        // FIXME: Crude hack that attempts to pass idle time to V8. This should
        // be done using the scheduler instead.
        V8GCForContextDispose::instance().notifyIdle();
    }
    OwnPtrWillBeRawPtr<ScheduledAction> action = ScheduledAction::create(scriptState, handler);
    return DOMTimer::install(executionContext, action.release(), timeout, true);
}

int setInterval(ScriptState* scriptState, EventTarget& eventTarget, const ScriptValue& handler, int timeout, const Vector<ScriptValue>& arguments)
{
    ExecutionContext* executionContext = eventTarget.executionContext();
    if (!isAllowed(scriptState, executionContext, false))
        return 0;
    OwnPtrWillBeRawPtr<ScheduledAction> action = ScheduledAction::create(scriptState, handler, arguments);
    return DOMTimer::install(executionContext, action.release(), timeout, false);
}

int setInterval(ScriptState* scriptState, EventTarget& eventTarget, const String& handler, int timeout, const Vector<ScriptValue>&)
{
    ExecutionContext* executionContext = eventTarget.executionContext();
    if (!isAllowed(scriptState, executionContext, true))
        return 0;
    // Don't allow setting timeouts to run empty functions.  Was historically a
    // perfomance issue.
    if (handler.isEmpty())
        return 0;
    OwnPtrWillBeRawPtr<ScheduledAction> action = ScheduledAction::create(scriptState, handler);
    return DOMTimer::install(executionContext, action.release(), timeout, false);
}

void clearTimeout(EventTarget& eventTarget, int timeoutID)
{
    if (ExecutionContext* context = eventTarget.executionContext())
        DOMTimer::removeByID(context, timeoutID);
}

void clearInterval(EventTarget& eventTarget, int timeoutID)
{
    if (ExecutionContext* context = eventTarget.executionContext())
        DOMTimer::removeByID(context, timeoutID);
}

} // namespace DOMWindowTimers

} // namespace blink
