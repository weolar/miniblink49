// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/app_banner/BeforeInstallPromptEvent.h"

#include "bindings/core/v8/ScriptPromise.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/app_banner/AppBannerCallbacks.h"
#include "modules/app_banner/BeforeInstallPromptEventInit.h"
#include "public/platform/modules/app_banner/WebAppBannerClient.h"

namespace blink {

BeforeInstallPromptEvent::BeforeInstallPromptEvent()
{
}

BeforeInstallPromptEvent::BeforeInstallPromptEvent(const AtomicString& name, ExecutionContext* executionContext, const Vector<String>& platforms, int requestId, WebAppBannerClient* client)
    : Event(name, false, true)
    , m_platforms(platforms)
    , m_requestId(requestId)
    , m_client(client)
    , m_userChoice(new UserChoiceProperty(executionContext, this, UserChoiceProperty::UserChoice))
    , m_registered(false)
{
}

BeforeInstallPromptEvent::BeforeInstallPromptEvent(const AtomicString& name, const BeforeInstallPromptEventInit& init)
    : Event(name, false, true)
    , m_platforms(init.platforms())
    , m_requestId(-1)
    , m_client(nullptr)
{
}

BeforeInstallPromptEvent::~BeforeInstallPromptEvent()
{
}

Vector<String> BeforeInstallPromptEvent::platforms() const
{
    return m_platforms;
}

ScriptPromise BeforeInstallPromptEvent::userChoice(ScriptState* scriptState)
{
    if (m_userChoice && m_client && m_requestId != -1) {
        if (!m_registered) {
            m_registered = true;
            m_client->registerBannerCallbacks(m_requestId, new AppBannerCallbacks(m_userChoice.get()));
        }
        return m_userChoice->promise(scriptState->world());
    }
    return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(InvalidStateError, "userChoice cannot be accessed on this event."));
}

const AtomicString& BeforeInstallPromptEvent::interfaceName() const
{
    return EventNames::BeforeInstallPromptEvent;
}

ScriptPromise BeforeInstallPromptEvent::prompt(ScriptState* scriptState)
{
    // |m_registered| will be true if userChoice has already been accessed
    // or prompt() has already been called. Return a rejected promise in both
    // these cases, as well as if we have a null client or invalid requestId.
    if (m_registered || !defaultPrevented() || !m_client || m_requestId == -1)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(InvalidStateError, "The prompt() method may only be called once, following preventDefault()."));

    m_registered = true;
    m_client->registerBannerCallbacks(m_requestId, new AppBannerCallbacks(m_userChoice.get()));
    m_client->showAppBanner(m_requestId);
    return ScriptPromise::cast(scriptState, v8::Undefined(scriptState->isolate()));
}

DEFINE_TRACE(BeforeInstallPromptEvent)
{
    visitor->trace(m_userChoice);
    Event::trace(visitor);
}

} // namespace blink
