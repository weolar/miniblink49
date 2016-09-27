// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FetchEvent_h
#define FetchEvent_h

#include "modules/EventModules.h"
#include "modules/ModulesExport.h"
#include "modules/fetch/Request.h"
#include "modules/serviceworkers/ExtendableEvent.h"
#include "modules/serviceworkers/FetchEventInit.h"
#include "modules/serviceworkers/RespondWithObserver.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExceptionState;
class Request;
class RespondWithObserver;

// A fetch event is dispatched by the client to a service worker's script
// context. RespondWithObserver can be used to notify the client about the
// service worker's response.
class MODULES_EXPORT FetchEvent final : public ExtendableEvent {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<FetchEvent> create();
    static PassRefPtrWillBeRawPtr<FetchEvent> create(const AtomicString& type, const FetchEventInit&);
    static PassRefPtrWillBeRawPtr<FetchEvent> create(const AtomicString& type, const FetchEventInit&, RespondWithObserver*);

    Request* request() const;
    bool isReload() const;

    void respondWith(ScriptState*, const ScriptValue&, ExceptionState&);

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

protected:
    FetchEvent();
    FetchEvent(const AtomicString& type, const FetchEventInit&, RespondWithObserver*);

private:
    PersistentWillBeMember<RespondWithObserver> m_observer;
    PersistentWillBeMember<Request> m_request;
    bool m_isReload;
};

} // namespace blink

#endif // FetchEvent_h
