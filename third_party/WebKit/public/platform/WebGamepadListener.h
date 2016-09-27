// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGamepadListener_h
#define WebGamepadListener_h

#include "WebPlatformEventListener.h"

namespace blink {

class WebGamepad;

class WebGamepadListener : public WebPlatformEventListener {
public:
    virtual void didConnectGamepad(unsigned index, const WebGamepad&) = 0;
    virtual void didDisconnectGamepad(unsigned index, const WebGamepad&) = 0;

protected:
    virtual ~WebGamepadListener() { }
};

} // namespace blink

#endif
