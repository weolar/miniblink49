// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BluetoothGATTCharacteristic_h
#define BluetoothGATTCharacteristic_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMArrayPiece.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/bluetooth/WebBluetoothGATTCharacteristic.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ScriptPromise;
class ScriptPromiseResolver;
class ScriptState;

// BluetoothGATTCharacteristic represents a GATT Characteristic, which is a
// basic data element that provides further information about a peripheral's
// service.
//
// Callbacks providing WebBluetoothGATTCharacteristic objects are handled by
// CallbackPromiseAdapter templatized with this class. See this class's
// "Interface required by CallbackPromiseAdapter" section and the
// CallbackPromiseAdapter class comments.
class BluetoothGATTCharacteristic final
    : public GarbageCollectedFinalized<BluetoothGATTCharacteristic>
    , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    explicit BluetoothGATTCharacteristic(PassOwnPtr<WebBluetoothGATTCharacteristic>);

    // Interface required by CallbackPromiseAdapter.
    typedef WebBluetoothGATTCharacteristic WebType;
    static BluetoothGATTCharacteristic* take(ScriptPromiseResolver* , PassOwnPtr<WebBluetoothGATTCharacteristic>);

    // Interface required by garbage collection.
    DEFINE_INLINE_TRACE() { }

    // IDL exposed interface:
    String uuid() { return m_webCharacteristic->uuid; }
    ScriptPromise readValue(ScriptState*);
    ScriptPromise writeValue(ScriptState*, const DOMArrayPiece&);

private:
    OwnPtr<WebBluetoothGATTCharacteristic> m_webCharacteristic;
};

} // namespace blink

#endif // BluetoothGATTCharacteristic_h
