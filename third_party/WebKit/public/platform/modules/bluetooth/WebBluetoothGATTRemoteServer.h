// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebBluetoothGATTRemoteServer_h
#define WebBluetoothGATTRemoteServer_h

#include "public/platform/WebString.h"

namespace blink {

struct WebBluetoothGATTRemoteServer {
    WebBluetoothGATTRemoteServer(const WebString& deviceInstanceID,
        bool connected)
        : deviceInstanceID(deviceInstanceID)
        , connected(connected)
    {
    }

    // Members corresponding to BluetoothGATTRemoteServer attributes as
    // specified in the IDL.
    const WebString deviceInstanceID;
    const bool connected;
};

} // namespace blink

#endif // WebBluetoothGATTRemoteServer_h
