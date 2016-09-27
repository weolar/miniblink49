// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/bluetooth/BluetoothGATTCharacteristic.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/bluetooth/BluetoothError.h"
#include "modules/bluetooth/ConvertWebVectorToArrayBuffer.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/bluetooth/WebBluetooth.h"

namespace blink {

BluetoothGATTCharacteristic::BluetoothGATTCharacteristic(PassOwnPtr<WebBluetoothGATTCharacteristic> webCharacteristic)
    : m_webCharacteristic(webCharacteristic)
{
}

BluetoothGATTCharacteristic* BluetoothGATTCharacteristic::take(ScriptPromiseResolver*, PassOwnPtr<WebBluetoothGATTCharacteristic> webCharacteristic)
{
    if (!webCharacteristic) {
        return nullptr;
    }
    return new BluetoothGATTCharacteristic(webCharacteristic);
}

ScriptPromise BluetoothGATTCharacteristic::readValue(ScriptState* scriptState)
{
    WebBluetooth* webbluetooth = Platform::current()->bluetooth();

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    webbluetooth->readValue(m_webCharacteristic->characteristicInstanceID, new CallbackPromiseAdapter<ConvertWebVectorToArrayBuffer, BluetoothError>(resolver));

    return promise;
}

ScriptPromise BluetoothGATTCharacteristic::writeValue(ScriptState* scriptState, const DOMArrayPiece& value)
{
    WebBluetooth* webbluetooth = Platform::current()->bluetooth();
    // Partial implementation of writeValue algorithm:
    // https://webbluetoothchrome.github.io/web-bluetooth/#dom-bluetoothgattcharacteristic-writevalue

    // Let valueVector be a copy of the bytes held by value.
    std::vector<uint8_t> valueVector(value.bytes(), value.bytes() + value.byteLength());
    // If bytes is more than 512 bytes long (the maximum length of an attribute
    // value, per Long Attribute Values) return a promise rejected with an
    // InvalidModificationError and abort.
    if (valueVector.size() > 512)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(InvalidModificationError, "Value can't exceed 512 bytes."));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);

    ScriptPromise promise = resolver->promise();
    webbluetooth->writeValue(m_webCharacteristic->characteristicInstanceID, valueVector, new CallbackPromiseAdapter<void, BluetoothError>(resolver));

    return promise;
}

} // namespace blink
