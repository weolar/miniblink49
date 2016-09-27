// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerRegistrationNotifications_h
#define ServiceWorkerRegistrationNotifications_h

#include "bindings/core/v8/ScriptPromise.h"
#include "modules/serviceworkers/ServiceWorkerRegistration.h"

namespace blink {

class ExceptionState;
class GetNotificationOptions;
class NotificationOptions;
class ScriptState;

class ServiceWorkerRegistrationNotifications {
public:
    static ScriptPromise showNotification(ScriptState*, ServiceWorkerRegistration&, const String& title, const NotificationOptions&, ExceptionState&);
    static ScriptPromise getNotifications(ScriptState*, ServiceWorkerRegistration&, const GetNotificationOptions&);
};

} // namespace blink

#endif // ServiceWorkerRegistrationNotifications_h
