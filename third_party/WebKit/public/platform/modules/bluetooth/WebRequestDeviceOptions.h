// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebRequestDeviceOptions_h
#define WebRequestDeviceOptions_h

#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"

namespace blink {

// Contains members corresponding to BluetoothScanFilter members as
// specified in the IDL.
struct WebBluetoothScanFilter {
    WebBluetoothScanFilter() { }
    WebBluetoothScanFilter(const WebVector<WebString>& services)
        : services(services)
    {
    }

    WebVector<WebString> services;
};

// Contains members corresponding to RequestDeviceOptions members as
// specified in the IDL.
struct WebRequestDeviceOptions {
    WebRequestDeviceOptions() { }

    WebVector<WebBluetoothScanFilter> filters;
    WebVector<WebString> optionalServices;
};

} // namespace blink

#endif // WebRequestDeviceOptions_h
