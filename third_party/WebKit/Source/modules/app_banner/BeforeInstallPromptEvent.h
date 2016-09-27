// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BeforeInstallPromptEvent_h
#define BeforeInstallPromptEvent_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseProperty.h"
#include "modules/EventModules.h"
#include "modules/app_banner/AppBannerPromptResult.h"
#include "platform/heap/Handle.h"

namespace blink {

class BeforeInstallPromptEvent;
class BeforeInstallPromptEventInit;
class WebAppBannerClient;

using UserChoiceProperty = ScriptPromiseProperty<RawPtrWillBeMember<BeforeInstallPromptEvent>, Member<AppBannerPromptResult>, ToV8UndefinedGenerator>;

class BeforeInstallPromptEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~BeforeInstallPromptEvent() override;

    // For EventModules.cpp
    static PassRefPtrWillBeRawPtr<BeforeInstallPromptEvent> create()
    {
        return adoptRefWillBeNoop(new BeforeInstallPromptEvent());
    }

    static PassRefPtrWillBeRawPtr<BeforeInstallPromptEvent> create(const AtomicString& name, ExecutionContext* executionContext, const Vector<String>& platforms, int requestId, WebAppBannerClient* client)
    {
        return adoptRefWillBeNoop(new BeforeInstallPromptEvent(name, executionContext, platforms, requestId, client));
    }

    static PassRefPtrWillBeRawPtr<BeforeInstallPromptEvent> create(const AtomicString& name, const BeforeInstallPromptEventInit& init)
    {
        return adoptRefWillBeNoop(new BeforeInstallPromptEvent(name, init));
    }

    Vector<String> platforms() const;
    ScriptPromise userChoice(ScriptState*);
    ScriptPromise prompt(ScriptState*);

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    BeforeInstallPromptEvent();
    BeforeInstallPromptEvent(const AtomicString& name, ExecutionContext*, const Vector<String>& platforms, int requestId, WebAppBannerClient*);
    BeforeInstallPromptEvent(const AtomicString& name, const BeforeInstallPromptEventInit&);

    Vector<String> m_platforms;

    int m_requestId;
    WebAppBannerClient* m_client;
    PersistentWillBeMember<UserChoiceProperty> m_userChoice;
    bool m_registered;
};

DEFINE_TYPE_CASTS(BeforeInstallPromptEvent, Event, event, event->interfaceName() == EventNames::BeforeInstallPromptEvent, event.interfaceName() == EventNames::BeforeInstallPromptEvent);

} // namespace blink

#endif // BeforeInstallPromptEvent_h
