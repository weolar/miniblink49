/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#include "core/workers/SharedWorkerGlobalScope.h"

#include "core/events/MessageEvent.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/workers/SharedWorkerThread.h"
#include "core/workers/WorkerClients.h"
#include "wtf/CurrentTime.h"

namespace blink {

PassRefPtrWillBeRawPtr<MessageEvent> createConnectEvent(MessagePort* port)
{
    RefPtrWillBeRawPtr<MessageEvent> event = MessageEvent::create(new MessagePortArray(1, port), String(), String(), port);
    event->initEvent(EventTypeNames::connect, false, false);
    return event.release();
}

// static
PassRefPtrWillBeRawPtr<SharedWorkerGlobalScope> SharedWorkerGlobalScope::create(const String& name, SharedWorkerThread* thread, PassOwnPtr<WorkerThreadStartupData> startupData)
{
    // Note: startupData is finalized on return. After the relevant parts has been
    // passed along to the created 'context'.
    RefPtrWillBeRawPtr<SharedWorkerGlobalScope> context = adoptRefWillBeNoop(new SharedWorkerGlobalScope(name, startupData->m_scriptURL, startupData->m_userAgent, thread, startupData->m_starterOrigin, startupData->m_workerClients.release()));
    context->applyContentSecurityPolicyFromVector(*startupData->m_contentSecurityPolicyHeaders);
    return context.release();
}

SharedWorkerGlobalScope::SharedWorkerGlobalScope(const String& name, const KURL& url, const String& userAgent, SharedWorkerThread* thread, const SecurityOrigin* starterOrigin, PassOwnPtrWillBeRawPtr<WorkerClients> workerClients)
    : WorkerGlobalScope(url, userAgent, thread, monotonicallyIncreasingTime(), starterOrigin, workerClients)
    , m_name(name)
{
}

SharedWorkerGlobalScope::~SharedWorkerGlobalScope()
{
}

const AtomicString& SharedWorkerGlobalScope::interfaceName() const
{
    return EventTargetNames::SharedWorkerGlobalScope;
}

SharedWorkerThread* SharedWorkerGlobalScope::thread()
{
    return static_cast<SharedWorkerThread*>(Base::thread());
}

void SharedWorkerGlobalScope::logExceptionToConsole(const String& errorMessage, int scriptId, const String& sourceURL, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack)
{
    WorkerGlobalScope::logExceptionToConsole(errorMessage, scriptId, sourceURL, lineNumber, columnNumber, callStack);
    RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, errorMessage, sourceURL, lineNumber);
    consoleMessage->setScriptId(scriptId);
    consoleMessage->setCallStack(callStack);
    addMessageToWorkerConsole(consoleMessage.release());
}

DEFINE_TRACE(SharedWorkerGlobalScope)
{
    WorkerGlobalScope::trace(visitor);
}

} // namespace blink
