// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/netinfo/NetworkInformation.h"

#include "core/dom/ExecutionContext.h"
#include "core/events/Event.h"
#include "core/page/NetworkStateNotifier.h"
#include "modules/EventTargetModules.h"
#include "wtf/text/WTFString.h"

namespace {

using namespace blink;

String connectionTypeToString(WebConnectionType type)
{
    switch (type) {
    case ConnectionTypeCellular:
        return "cellular";
    case ConnectionTypeBluetooth:
        return "bluetooth";
    case ConnectionTypeEthernet:
        return "ethernet";
    case ConnectionTypeWifi:
        return "wifi";
    case ConnectionTypeOther:
        return "other";
    case ConnectionTypeNone:
        return "none";
    case ConnectionTypeUnknown:
        return "unknown";
    }
    ASSERT_NOT_REACHED();
    return "none";
}

} // namespace

namespace blink {

NetworkInformation* NetworkInformation::create(ExecutionContext* context)
{
    NetworkInformation* connection = new NetworkInformation(context);
    connection->suspendIfNeeded();
    return connection;
}

NetworkInformation::~NetworkInformation()
{
    ASSERT(!m_observing);
}

String NetworkInformation::type() const
{
    // m_type is only updated when listening for events, so ask networkStateNotifier
    // if not listening (crbug.com/379841).
    if (!m_observing)
        return connectionTypeToString(networkStateNotifier().connectionType());

    // If observing, return m_type which changes when the event fires, per spec.
    return connectionTypeToString(m_type);
}

void NetworkInformation::connectionTypeChange(WebConnectionType type)
{
    ASSERT(executionContext()->isContextThread());

    // This can happen if the observer removes and then adds itself again
    // during notification.
    if (m_type == type)
        return;

    m_type = type;
    dispatchEvent(Event::create(EventTypeNames::typechange));
}

const AtomicString& NetworkInformation::interfaceName() const
{
    return EventTargetNames::NetworkInformation;
}

ExecutionContext* NetworkInformation::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

bool NetworkInformation::addEventListener(const AtomicString& eventType, PassRefPtr<EventListener> listener, bool useCapture)
{
    if (!EventTargetWithInlineData::addEventListener(eventType, listener, useCapture))
        return false;
    startObserving();
    return true;
}

bool NetworkInformation::removeEventListener(const AtomicString& eventType, PassRefPtr<EventListener> listener, bool useCapture)
{
    if (!EventTargetWithInlineData::removeEventListener(eventType, listener, useCapture))
        return false;
    if (!hasEventListeners())
        stopObserving();
    return true;
}

void NetworkInformation::removeAllEventListeners()
{
    EventTargetWithInlineData::removeAllEventListeners();
    ASSERT(!hasEventListeners());
    stopObserving();
}

bool NetworkInformation::hasPendingActivity() const
{
    ASSERT(m_contextStopped || m_observing == hasEventListeners());

    // Prevent collection of this object when there are active listeners.
    return m_observing;
}

void NetworkInformation::stop()
{
    m_contextStopped = true;
    stopObserving();
}

void NetworkInformation::startObserving()
{
    if (!m_observing && !m_contextStopped) {
        m_type = networkStateNotifier().connectionType();
        networkStateNotifier().addObserver(this, executionContext());
        m_observing = true;
    }
}

void NetworkInformation::stopObserving()
{
    if (m_observing) {
        networkStateNotifier().removeObserver(this, executionContext());
        m_observing = false;
    }
}

NetworkInformation::NetworkInformation(ExecutionContext* context)
    : ActiveDOMObject(context)
    , m_type(networkStateNotifier().connectionType())
    , m_observing(false)
    , m_contextStopped(false)
{
}

DEFINE_TRACE(NetworkInformation)
{
    RefCountedGarbageCollectedEventTargetWithInlineData<NetworkInformation>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
