// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BluetoothGATTService_h
#define BluetoothGATTService_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/bluetooth/WebBluetoothGATTService.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ScriptPromise;
class ScriptPromiseResolver;
class ScriptState;

// Represents a GATT Service within a Bluetooth Peripheral, a collection of
// characteristics and relationships to other services that encapsulate the
// behavior of part of a device.
//
// Callbacks providing WebBluetoothGATTService objects are handled by
// CallbackPromiseAdapter templatized with this class. See this class's
// "Interface required by CallbackPromiseAdapter" section and the
// CallbackPromiseAdapter class comments.
class BluetoothGATTService final
    : public GarbageCollectedFinalized<BluetoothGATTService>
    , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    explicit BluetoothGATTService(PassOwnPtr<WebBluetoothGATTService>);

    // Interface required by CallbackPromiseAdapter:
    typedef WebBluetoothGATTService WebType;
    static BluetoothGATTService* take(ScriptPromiseResolver*, PassOwnPtr<WebBluetoothGATTService>);

    // Interface required by garbage collection.
    DEFINE_INLINE_TRACE() { }

    // IDL exposed interface:
    String uuid() { return m_webService->uuid; }
    bool isPrimary() { return m_webService->isPrimary; }
    ScriptPromise getCharacteristic(ScriptState*, const StringOrUnsignedLong& characteristic, ExceptionState&);

private:
    OwnPtr<WebBluetoothGATTService> m_webService;
};

} // namespace blink

#endif // BluetoothGATTService_h
