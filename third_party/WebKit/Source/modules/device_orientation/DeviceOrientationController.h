// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DeviceOrientationController_h
#define DeviceOrientationController_h

#include "core/dom/Document.h"
#include "core/frame/DeviceSingleWindowEventController.h"
#include "modules/ModulesExport.h"

namespace blink {

class DeviceOrientationData;
class Event;

class MODULES_EXPORT DeviceOrientationController final : public DeviceSingleWindowEventController, public WillBeHeapSupplement<Document> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DeviceOrientationController);
public:
    ~DeviceOrientationController() override;

    static const char* supplementName();
    static DeviceOrientationController& from(Document&);

    // Inherited from DeviceSingleWindowEventController.
    void didUpdateData() override;
    void didAddEventListener(LocalDOMWindow*, const AtomicString& eventType) override;

    void setOverride(DeviceOrientationData*);
    void clearOverride();

    DECLARE_VIRTUAL_TRACE();

private:
    explicit DeviceOrientationController(Document&);

    // Inherited from DeviceEventControllerBase.
    void registerWithDispatcher() override;
    void unregisterWithDispatcher() override;
    bool hasLastData() override;

    // Inherited from DeviceSingleWindowEventController.
    PassRefPtrWillBeRawPtr<Event> lastEvent() const override;
    const AtomicString& eventTypeName() const override;
    bool isNullEvent(Event*) const override;

    DeviceOrientationData* lastData() const;

    PersistentWillBeMember<DeviceOrientationData> m_overrideOrientationData;
};

} // namespace blink

#endif // DeviceOrientationController_h
