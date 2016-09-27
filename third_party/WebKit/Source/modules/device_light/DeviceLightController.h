// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DeviceLightController_h
#define DeviceLightController_h

#include "core/dom/Document.h"
#include "core/frame/DeviceSingleWindowEventController.h"
#include "modules/ModulesExport.h"

namespace blink {

class Event;

class MODULES_EXPORT DeviceLightController final : public DeviceSingleWindowEventController, public WillBeHeapSupplement<Document> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DeviceLightController);
public:
    ~DeviceLightController() override;

    static const char* supplementName();
    static DeviceLightController& from(Document&);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit DeviceLightController(Document&);

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

#endif // DeviceLightController_h
