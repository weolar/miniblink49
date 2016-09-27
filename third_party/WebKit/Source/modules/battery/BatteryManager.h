// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BatteryManager_h
#define BatteryManager_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseProperty.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/ContextLifecycleObserver.h"
#include "core/frame/PlatformEventController.h"
#include "modules/EventTargetModules.h"
#include "platform/heap/Handle.h"

namespace blink {

class BatteryStatus;

class BatteryManager final : public RefCountedGarbageCollectedEventTargetWithInlineData<BatteryManager>, public ActiveDOMObject, public PlatformEventController {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(BatteryManager);
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(BatteryManager);
public:
    static BatteryManager* create(ExecutionContext*);
    ~BatteryManager() override;

    // Returns a promise object that will be resolved with this BatteryManager.
    ScriptPromise startRequest(ScriptState*);

    // EventTarget implementation.
    const WTF::AtomicString& interfaceName() const override { return EventTargetNames::BatteryManager; }
    ExecutionContext* executionContext() const override { return ContextLifecycleObserver::executionContext(); }

    bool charging();
    double chargingTime();
    double dischargingTime();
    double level();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(chargingchange);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(chargingtimechange);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(dischargingtimechange);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(levelchange);

    // Inherited from PlatformEventController.
    void didUpdateData() override;
    void registerWithDispatcher() override;
    void unregisterWithDispatcher() override;
    bool hasLastData() override;

    // ActiveDOMObject implementation.
    void suspend() override;
    void resume() override;
    void stop() override;
    bool hasPendingActivity() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit BatteryManager(ExecutionContext*);

    using BatteryProperty = ScriptPromiseProperty<Member<BatteryManager>, Member<BatteryManager>, Member<DOMException>>;
    Member<BatteryProperty> m_batteryProperty;
    Member<BatteryStatus> m_batteryStatus;
};

} // namespace blink

#endif // BatteryManager_h
