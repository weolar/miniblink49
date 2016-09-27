// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PeriodicSyncEvent_h
#define PeriodicSyncEvent_h

#include "modules/EventModules.h"
#include "modules/serviceworkers/ExtendableEvent.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class PeriodicSyncEventInit;
class PeriodicSyncRegistration;

class PeriodicSyncEvent final : public ExtendableEvent {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<PeriodicSyncEvent> create()
    {
        return adoptRefWillBeNoop(new PeriodicSyncEvent);
    }
    static PassRefPtrWillBeRawPtr<PeriodicSyncEvent> create(const AtomicString& type, PeriodicSyncRegistration* syncRegistration, WaitUntilObserver* observer)
    {
        return adoptRefWillBeNoop(new PeriodicSyncEvent(type, syncRegistration, observer));
    }
    static PassRefPtrWillBeRawPtr<PeriodicSyncEvent> create(const AtomicString& type, const PeriodicSyncEventInit& init)
    {
        return adoptRefWillBeNoop(new PeriodicSyncEvent(type, init));
    }

    ~PeriodicSyncEvent() override;

    const AtomicString& interfaceName() const override;

    PeriodicSyncRegistration* registration();

    DECLARE_VIRTUAL_TRACE();

private:
    PeriodicSyncEvent();
    PeriodicSyncEvent(const AtomicString& type, PeriodicSyncRegistration*, WaitUntilObserver*);
    PeriodicSyncEvent(const AtomicString& type, const PeriodicSyncEventInit&);

    PersistentWillBeMember<PeriodicSyncRegistration> m_periodicRegistration;
};

} // namespace blink

#endif // PeriodicSyncEvent_h
