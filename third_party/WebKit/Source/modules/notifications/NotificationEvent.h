// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NotificationEvent_h
#define NotificationEvent_h

#include "modules/EventModules.h"
#include "modules/ModulesExport.h"
#include "modules/notifications/Notification.h"
#include "modules/notifications/NotificationEventInit.h"
#include "modules/serviceworkers/ExtendableEvent.h"
#include "platform/heap/Handle.h"

namespace blink {

class MODULES_EXPORT NotificationEvent final : public ExtendableEvent {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<NotificationEvent> create()
    {
        return adoptRefWillBeNoop(new NotificationEvent);
    }
    static PassRefPtrWillBeRawPtr<NotificationEvent> create(const AtomicString& type, const NotificationEventInit& initializer)
    {
        return adoptRefWillBeNoop(new NotificationEvent(type, initializer));
    }
    static PassRefPtrWillBeRawPtr<NotificationEvent> create(const AtomicString& type, const NotificationEventInit& initializer, WaitUntilObserver* observer)
    {
        return adoptRefWillBeNoop(new NotificationEvent(type, initializer, observer));
    }

    ~NotificationEvent() override;

    Notification* notification() const { return m_notification.get(); }

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    NotificationEvent();
    NotificationEvent(const AtomicString& type, const NotificationEventInit&);
    NotificationEvent(const AtomicString& type, const NotificationEventInit&, WaitUntilObserver*);

    PersistentWillBeMember<Notification> m_notification;
};

} // namespace blink

#endif // NotificationEvent_h
