// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebDeviceLightListener_h
#define WebDeviceLightListener_h

#include "WebPlatformEventListener.h"

namespace blink {

class WebDeviceLightListener : public WebPlatformEventListener {
public:
    // This method is called every time new device light data is available.
    virtual void didChangeDeviceLight(double) = 0;

    virtual ~WebDeviceLightListener() { }
};

} // namespace blink

#endif // WebDeviceLightListener_h
