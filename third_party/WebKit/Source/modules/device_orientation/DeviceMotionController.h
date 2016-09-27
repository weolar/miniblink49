// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DeviceMotionController_h
#define DeviceMotionController_h

#include "core/dom/Document.h"
#include "core/frame/DeviceSingleWindowEventController.h"
#include "modules/ModulesExport.h"

namespace blink {

class Event;

class MODULES_EXPORT DeviceMotionController final : public DeviceSingleWindowEventController, public WillBeHeapSupplement<Document> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DeviceMotionController);
public:
    ~DeviceMotionController() override;

    static const char* supplementName();
    static DeviceMotionController& from(Document&);

    // DeviceSingleWindowEventController
    void didAddEventListener(LocalDOMWindow*, const AtomicString& eventType) override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit DeviceMotionController(Document&);

    // Inherited from DeviceEventControllerBase.
    void registerWithDispatcher() override;
    void unregisterWithDispatcher() override;
    bool hasLastData() override;

    // Inherited from DeviceSingleWindowEventController.
    PassRefPtrWillBeRawPtr<Event> lastEvent() const override;
    const AtomicString& eventTypeName() const override;
    bool isNullEvent(Event*) const override;
};

} // namespace blink

#endif // DeviceMotionController_h
