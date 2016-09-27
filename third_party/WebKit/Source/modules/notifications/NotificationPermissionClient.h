// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NotificationPermissionClient_h
#define NotificationPermissionClient_h

#include "modules/ModulesExport.h"
#include "modules/notifications/NotificationPermissionCallback.h"
#include "platform/Supplementable.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class ExecutionContext;
class LocalFrame;

class NotificationPermissionClient : public WillBeHeapSupplement<LocalFrame> {
public:
    virtual ~NotificationPermissionClient() { }

    // Requests user permission to show platform notifications from the origin
    // of the current frame. The provided callback will be ran when the user
    // has made a decision.
    virtual void requestPermission(ExecutionContext*, NotificationPermissionCallback*) = 0;

    // WillBeHeapSupplement requirements.
    static const char* supplementName();
    static NotificationPermissionClient* from(ExecutionContext*);
};

MODULES_EXPORT void provideNotificationPermissionClientTo(LocalFrame&, PassOwnPtrWillBeRawPtr<NotificationPermissionClient>);

} // namespace blink

#endif // NotificationPermissionClient_h
