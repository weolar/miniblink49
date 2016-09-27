// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerGlobalScopeSync_h
#define ServiceWorkerGlobalScopeSync_h

#include "core/events/EventTarget.h"

namespace blink {

class ServiceWorkerGlobalScopeSync {
public:
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(sync);
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(periodicsync);
};

} // namespace blink

#endif // ServiceWorkerGlobalScopeSync_h
