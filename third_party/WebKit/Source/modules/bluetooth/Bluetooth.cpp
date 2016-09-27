// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/bluetooth/Bluetooth.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/bluetooth/BluetoothDevice.h"
#include "modules/bluetooth/BluetoothError.h"
#include "modules/bluetooth/BluetoothUUID.h"
#include "modules/bluetooth/RequestDeviceOptions.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/bluetooth/WebBluetooth.h"
#include "public/platform/modules/bluetooth/WebRequestDeviceOptions.h"

namespace blink {

// Returns a DOMException if the conversion fails, or null if it succeeds.
static void convertRequestDeviceOptions(const RequestDeviceOptions& options, WebRequestDeviceOptions& result, ExceptionState& exceptionState)
{
    if (options.hasFilters()) {
        Vector<WebBluetoothScanFilter> filters;
        for (const BluetoothScanFilter& filter : options.filters()) {
            Vector<WebString> services;
            for (const StringOrUnsignedLong& service : filter.services()) {
                const String& validatedService = BluetoothUUID::getService(service, exceptionState);
                if (exceptionState.hadException())
                    return;
                services.append(validatedService);
            }
            filters.append(WebBluetoothScanFilter(services));
        }
        result.filters.assign(filters);
    }
    if (options.hasOptionalServices()) {
        Vector<WebString> optionalServices;
        for (const StringOrUnsignedLong& optionalService : options.optionalServices()) {
            const String& validatedOptionalService = BluetoothUUID::getService(optionalService, exceptionState);
            if (exceptionState.hadException())
                return;
            optionalServices.append(validatedOptionalService);
        }
        result.optionalServices.assign(optionalServices);
    }
}

ScriptPromise Bluetooth::requestDevice(ScriptState* scriptState, const RequestDeviceOptions& options, ExceptionState& exceptionState)
{
    WebBluetooth* webbluetooth = Platform::current()->bluetooth();
    if (!webbluetooth)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError));

    WebRequestDeviceOptions webOptions;
    convertRequestDeviceOptions(options, webOptions, exceptionState);
    if (exceptionState.hadException())
        return exceptionState.reject(scriptState);

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    webbluetooth->requestDevice(webOptions, new CallbackPromiseAdapter<BluetoothDevice, BluetoothError>(resolver));
    return promise;

}

} // namespace blink
