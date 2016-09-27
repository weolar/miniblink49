// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerGlobalScopeNotifications_h
#define ServiceWorkerGlobalScopeNotifications_h

#include "core/events/EventTarget.h"

namespace blink {

class ServiceWorkerGlobalScopeNotifications {
public:
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(notificationclick);
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(notificationerror);
};

} // namespace blink

#endif // ServiceWorkerGlobalScopeNotifications_h
