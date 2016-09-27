// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CrossOriginConnectEvent_h
#define CrossOriginConnectEvent_h

#include "modules/EventModules.h"
#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"

namespace blink {

class AcceptConnectionObserver;
class CrossOriginServiceWorkerClient;

// A crossoriginconnect event is dispatched by the client to a service worker's
// script. AcceptConnectionObserver can be used to notify the client about the
// service worker's response.
class MODULES_EXPORT CrossOriginConnectEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<CrossOriginConnectEvent> create();
    static PassRefPtrWillBeRawPtr<CrossOriginConnectEvent> create(AcceptConnectionObserver*, CrossOriginServiceWorkerClient*);

    CrossOriginServiceWorkerClient* client() const;

    void acceptConnection(ScriptState*, const ScriptPromise&, ExceptionState&);

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

protected:
    CrossOriginConnectEvent();
    CrossOriginConnectEvent(AcceptConnectionObserver*, CrossOriginServiceWorkerClient*);

private:
    PersistentWillBeMember<AcceptConnectionObserver> m_observer;
    PersistentWillBeMember<CrossOriginServiceWorkerClient> m_client;
};

} // namespace blink

#endif // CrossOriginConnectEvent_h
