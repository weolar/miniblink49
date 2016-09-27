// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DeviceSingleWindowEventController_h
#define DeviceSingleWindowEventController_h

#include "core/CoreExport.h"
#include "core/frame/DOMWindowLifecycleObserver.h"
#include "core/frame/PlatformEventController.h"
#include "platform/heap/Handle.h"

namespace blink {

class Document;
class Event;

class CORE_EXPORT DeviceSingleWindowEventController : public NoBaseWillBeGarbageCollectedFinalized<DeviceSingleWindowEventController>, public PlatformEventController, public DOMWindowLifecycleObserver {
public:
    virtual ~DeviceSingleWindowEventController();

    // Inherited from DeviceEventControllerBase.
    virtual void didUpdateData() override;
    DECLARE_VIRTUAL_TRACE();

    // Inherited from DOMWindowLifecycleObserver.
    virtual void didAddEventListener(LocalDOMWindow*, const AtomicString&) override;
    virtual void didRemoveEventListener(LocalDOMWindow*, const AtomicString&) override;
    virtual void didRemoveAllEventListeners(LocalDOMWindow*) override;

protected:
    explicit DeviceSingleWindowEventController(Document&);

    Document& document() const { return *m_document; }

    void dispatchDeviceEvent(const PassRefPtrWillBeRawPtr<Event>);

    virtual PassRefPtrWillBeRawPtr<Event> lastEvent() const = 0;
    virtual const AtomicString& eventTypeName() const = 0;
    virtual bool isNullEvent(Event*) const = 0;

private:
    bool m_needsCheckingNullEvents;
    RawPtrWillBeMember<Document> m_document;
};

} // namespace blink

#endif // DeviceSingleWindowEventController_h
