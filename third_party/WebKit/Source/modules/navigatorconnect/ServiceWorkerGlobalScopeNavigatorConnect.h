// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerGlobalScopeNavigatorConnect_h
#define ServiceWorkerGlobalScopeNavigatorConnect_h

#include "core/events/EventTarget.h"

namespace blink {

class ServiceWorkerGlobalScopeNavigatorConnect {
public:
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(crossoriginconnect);
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(crossoriginmessage);
};

} // namespace blink

#endif // ServiceWorkerGlobalScopeNavigatorConnect_h
