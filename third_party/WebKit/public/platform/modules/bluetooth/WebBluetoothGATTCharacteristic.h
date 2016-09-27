// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebBluetoothGATTCharacteristic_h
#define WebBluetoothGATTCharacteristic_h

#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"

namespace blink {

// Contains members corresponding to BluetoothGATTCharacteristic attributes as
// specified in the IDL.
struct WebBluetoothGATTCharacteristic {
    WebBluetoothGATTCharacteristic(const WebString& characteristicInstanceID,
        const WebString& serviceInstanceID,
        const WebString& uuid)
        : characteristicInstanceID(characteristicInstanceID)
        , serviceInstanceID(serviceInstanceID)
        , uuid(uuid)
    {
    }

    const WebString characteristicInstanceID;
    const WebString serviceInstanceID;
    const WebString uuid;
    // TODO(ortuno): Add 'properties' once CharacteristicProperties is implemented.
    // https://crbug.com/483345
    // const WebCharacteristicProperties properties;
    const WebVector<uint8_t> value;
};

} // namespace blink

#endif // WebBluetoothGATTCharacteristic_h
