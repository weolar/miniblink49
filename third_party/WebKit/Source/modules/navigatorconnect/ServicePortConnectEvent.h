// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServicePortConnectEvent_h
#define ServicePortConnectEvent_h

#include "modules/EventModules.h"
#include "modules/ModulesExport.h"
#include "modules/serviceworkers/ExtendableEvent.h"
#include "platform/heap/Handle.h"

namespace blink {

class AcceptConnectionObserver;
class ExceptionState;
class ServicePortConnectEventInit;

// A connect event is dispatched by the client to a service worker's script.
// AcceptConnectionObserver can be used to notify the client about the service
// worker's response.
class MODULES_EXPORT ServicePortConnectEvent final : public ExtendableEvent {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<ServicePortConnectEvent> create();
    static PassRefPtrWillBeRawPtr<ServicePortConnectEvent> create(const AtomicString& type, const ServicePortConnectEventInit&);
    static PassRefPtrWillBeRawPtr<ServicePortConnectEvent> create(const AtomicString& type, const ServicePortConnectEventInit&, AcceptConnectionObserver*);

    // ServicePortConnectEvent.idl
    String targetURL() const { return m_targetURL; }
    String origin() const { return m_origin; }
    ScriptPromise respondWith(ScriptState*, const ScriptPromise& response, ExceptionState&);

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

protected:
    ServicePortConnectEvent();
    ServicePortConnectEvent(const AtomicString& type, const ServicePortConnectEventInit&, AcceptConnectionObserver*);

private:
    PersistentWillBeMember<AcceptConnectionObserver> m_observer;
    String m_targetURL;
    String m_origin;
};

} // namespace blink

#endif // ServicePortConnectEvent_h
