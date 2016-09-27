// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/bluetooth/BluetoothGATTService.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/bluetooth/BluetoothError.h"
#include "modules/bluetooth/BluetoothGATTCharacteristic.h"
#include "modules/bluetooth/BluetoothUUID.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/bluetooth/WebBluetooth.h"

namespace blink {

BluetoothGATTService::BluetoothGATTService(PassOwnPtr<WebBluetoothGATTService> webService)
    : m_webService(webService)
{
}

BluetoothGATTService* BluetoothGATTService::take(ScriptPromiseResolver*, PassOwnPtr<WebBluetoothGATTService> webService)
{
    if (!webService) {
        return nullptr;
    }
    return new BluetoothGATTService(webService);
}

ScriptPromise BluetoothGATTService::getCharacteristic(ScriptState* scriptState,
    const StringOrUnsignedLong& characteristic, ExceptionState& exceptionState)
{
    WebBluetooth* webbluetooth = Platform::current()->bluetooth();

    String characteristicUUID = BluetoothUUID::getCharacteristic(characteristic, exceptionState);
    if (exceptionState.hadException())
        return exceptionState.reject(scriptState);

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    webbluetooth->getCharacteristic(m_webService->serviceInstanceID, characteristicUUID, new CallbackPromiseAdapter<BluetoothGATTCharacteristic, BluetoothError>(resolver));

    return promise;
}

} // namespace blink
