// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebBatteryStatusListener_h
#define WebBatteryStatusListener_h

#include "WebBatteryStatus.h"
#include "WebPlatformEventListener.h"

namespace blink {

class WebBatteryStatus;

class WebBatteryStatusListener : public WebPlatformEventListener {
public:
    // This method is called when a new battery status is available.
    virtual void updateBatteryStatus(const WebBatteryStatus&) = 0;

    virtual ~WebBatteryStatusListener() { }
};

} // namespace blink

#endif // WebBatteryStatusListener_h
