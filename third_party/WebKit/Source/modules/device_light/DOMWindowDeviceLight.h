// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMWindowDeviceLight_h
#define DOMWindowDeviceLight_h

#include "modules/EventTargetModules.h"

namespace blink {

class DOMWindowDeviceLight {
public:
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(devicelight);
};

} // namespace blink

#endif // DOMWindowDeviceLight_h
