// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/StashedPortCollection.h"

#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "modules/EventTargetModules.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"
#include "modules/serviceworkers/StashedMessagePort.h"
#include "public/platform/WebString.h"

namespace blink {

StashedPortCollection* StashedPortCollection::create(ExecutionContext* context)
{
    return new StashedPortCollection(context);
}

StashedPortCollection::StashedPortCollection(ExecutionContext* context)
    : ContextLifecycleObserver(context)
{
}

StashedPortCollection::~StashedPortCollection()
{
}

void StashedPortCollection::addPorts(const StashedMessagePortArray& ports)
{
    m_ports.appendVector(ports);
}

StashedMessagePort* StashedPortCollection::add(ScriptState* scriptState, const String& name, MessagePort* port)
{
    OwnPtr<WebMessagePortChannel> webChannel = port->disentangle();
    ServiceWorkerGlobalScopeClient::from(executionContext())->stashMessagePort(webChannel.get(), name);
    StashedMessagePort* stashedPort = StashedMessagePort::create(*executionContext(), webChannel.release(), name);
    m_ports.append(stashedPort);
    return stashedPort;
}

const AtomicString& StashedPortCollection::interfaceName() const
{
    return EventTargetNames::StashedPortCollection;
}

ExecutionContext* StashedPortCollection::executionContext() const
{
    return ContextLifecycleObserver::executionContext();
}

DEFINE_TRACE(StashedPortCollection)
{
    visitor->trace(m_ports);
    RefCountedGarbageCollectedEventTargetWithInlineData::trace(visitor);
    ContextLifecycleObserver::trace(visitor);
}

} // namespace blink
