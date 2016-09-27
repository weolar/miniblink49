// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerGlobalScopePush_h
#define ServiceWorkerGlobalScopePush_h

#include "core/events/EventTarget.h"

namespace blink {

class ServiceWorkerGlobalScopePush {
public:
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(push);
};

} // namespace blink

#endif // ServiceWorkerGlobalScopePush_h
