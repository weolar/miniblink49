/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "ServiceWorker.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/MessagePort.h"
#include "core/events/Event.h"
#include "modules/EventTargetModules.h"
#include "public/platform/WebMessagePortChannel.h"
#include "public/platform/WebServiceWorkerState.h"
#include "public/platform/WebString.h"

namespace blink {

const AtomicString& ServiceWorker::interfaceName() const
{
    return EventTargetNames::ServiceWorker;
}

void ServiceWorker::postMessage(ExecutionContext* context, PassRefPtr<SerializedScriptValue> message, const MessagePortArray* ports, ExceptionState& exceptionState)
{
    // Disentangle the port in preparation for sending it to the remote context.
    OwnPtr<MessagePortChannelArray> channels = MessagePort::disentanglePorts(context, ports, exceptionState);
    if (exceptionState.hadException())
        return;
    if (m_outerWorker->state() == WebServiceWorkerStateRedundant) {
        exceptionState.throwDOMException(InvalidStateError, "ServiceWorker is in redundant state.");
        return;
    }

    WebString messageString = message->toWireString();
    OwnPtr<WebMessagePortChannelArray> webChannels = MessagePort::toWebMessagePortChannelArray(channels.release());
    m_outerWorker->postMessage(messageString, webChannels.leakPtr());
}

void ServiceWorker::internalsTerminate()
{
    m_outerWorker->terminate();
}

void ServiceWorker::dispatchStateChangeEvent()
{
    this->dispatchEvent(Event::create(EventTypeNames::statechange));
}

String ServiceWorker::scriptURL() const
{
    return m_outerWorker->url().string();
}

String ServiceWorker::state() const
{
    switch (m_outerWorker->state()) {
    case WebServiceWorkerStateUnknown:
        // The web platform should never see this internal state
        ASSERT_NOT_REACHED();
        return "unknown";
    case WebServiceWorkerStateInstalling:
        return "installing";
    case WebServiceWorkerStateInstalled:
        return "installed";
    case WebServiceWorkerStateActivating:
        return "activating";
    case WebServiceWorkerStateActivated:
        return "activated";
    case WebServiceWorkerStateRedundant:
        return "redundant";
    default:
        ASSERT_NOT_REACHED();
        return nullAtom;
    }
}

PassRefPtrWillBeRawPtr<ServiceWorker> ServiceWorker::from(ExecutionContext* executionContext, WebType* worker)
{
    if (!worker)
        return nullptr;

    RefPtrWillBeRawPtr<ServiceWorker> serviceWorker = getOrCreate(executionContext, worker);
    return serviceWorker.release();
}

bool ServiceWorker::hasPendingActivity() const
{
    if (AbstractWorker::hasPendingActivity())
        return true;
    if (m_wasStopped)
        return false;
    return m_outerWorker->state() != WebServiceWorkerStateRedundant;
}

void ServiceWorker::stop()
{
    m_wasStopped = true;
}

PassRefPtrWillBeRawPtr<ServiceWorker> ServiceWorker::getOrCreate(ExecutionContext* executionContext, WebType* outerWorker)
{
    if (!outerWorker)
        return nullptr;

    ServiceWorker* existingServiceWorker = static_cast<ServiceWorker*>(outerWorker->proxy());
    if (existingServiceWorker) {
        ASSERT(existingServiceWorker->executionContext() == executionContext);
        return existingServiceWorker;
    }

    RefPtrWillBeRawPtr<ServiceWorker> worker = adoptRefWillBeNoop(new ServiceWorker(executionContext, adoptPtr(outerWorker)));
    worker->suspendIfNeeded();
    return worker.release();
}

ServiceWorker::ServiceWorker(ExecutionContext* executionContext, PassOwnPtr<WebServiceWorker> worker)
    : AbstractWorker(executionContext)
    , m_outerWorker(worker)
    , m_wasStopped(false)
{
    ASSERT(m_outerWorker);
    m_outerWorker->setProxy(this);
}

ServiceWorker::~ServiceWorker()
{
}

} // namespace blink
