// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NetworkInformation_h
#define NetworkInformation_h

#include "core/dom/ActiveDOMObject.h"
#include "core/events/EventTarget.h"
#include "core/page/NetworkStateNotifier.h"
#include "public/platform/WebConnectionType.h"

namespace blink {

class ExecutionContext;

class NetworkInformation final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<NetworkInformation>
    , public ActiveDOMObject
    , public NetworkStateNotifier::NetworkStateObserver {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(NetworkInformation);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(NetworkInformation);
    DEFINE_WRAPPERTYPEINFO();
public:
    static NetworkInformation* create(ExecutionContext*);
    ~NetworkInformation() override;

    String type() const;

    void connectionTypeChange(WebConnectionType) override;

    // EventTarget overrides.
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;
    bool addEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture = false) override;
    bool removeEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture = false) override;
    void removeAllEventListeners() override;

    // ActiveDOMObject overrides.
    bool hasPendingActivity() const override;
    void stop() override;

    DECLARE_VIRTUAL_TRACE();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(typechange);

private:
    explicit NetworkInformation(ExecutionContext*);
    void startObserving();
    void stopObserving();

    // Touched only on context thread.
    WebConnectionType m_type;

    // Whether this object is listening for events from NetworkStateNotifier.
    bool m_observing;

    // Whether ActiveDOMObject::stop has been called.
    bool m_contextStopped;
};

} // namespace blink

#endif // NetworkInformation_h
