// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "FetchEvent.h"

#include "modules/fetch/Request.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScope.h"
#include "wtf/RefPtr.h"

namespace blink {

PassRefPtrWillBeRawPtr<FetchEvent> FetchEvent::create()
{
    return adoptRefWillBeNoop(new FetchEvent());
}

PassRefPtrWillBeRawPtr<FetchEvent> FetchEvent::create(const AtomicString& type, const FetchEventInit& initializer)
{
    return adoptRefWillBeNoop(new FetchEvent(type, initializer, nullptr));
}

PassRefPtrWillBeRawPtr<FetchEvent> FetchEvent::create(const AtomicString& type, const FetchEventInit& initializer, RespondWithObserver* observer)
{
    return adoptRefWillBeNoop(new FetchEvent(type, initializer, observer));
}

Request* FetchEvent::request() const
{
    return m_request;
}

bool FetchEvent::isReload() const
{
    return m_isReload;
}

void FetchEvent::respondWith(ScriptState* scriptState, const ScriptValue& value, ExceptionState& exceptionState)
{
    stopImmediatePropagation();
    m_observer->respondWith(scriptState, value, exceptionState);
}

const AtomicString& FetchEvent::interfaceName() const
{
    return EventNames::FetchEvent;
}

FetchEvent::FetchEvent()
    : m_isReload(false)
{
}

FetchEvent::FetchEvent(const AtomicString& type, const FetchEventInit& initializer, RespondWithObserver* observer)
    : ExtendableEvent(type, initializer)
    , m_observer(observer)
{
    if (initializer.hasRequest())
        m_request = initializer.request();
    m_isReload = initializer.isReload();
}

DEFINE_TRACE(FetchEvent)
{
    visitor->trace(m_observer);
    visitor->trace(m_request);
    ExtendableEvent::trace(visitor);
}

} // namespace blink
