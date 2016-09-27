// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BluetoothDevice_h
#define BluetoothDevice_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Heap.h"
#include "public/platform/modules/bluetooth/WebBluetoothDevice.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ScriptPromise;
class ScriptPromiseResolver;
class ScriptState;

// BluetoothDevice represents a physical bluetooth device in the DOM. See IDL.
//
// Callbacks providing WebBluetoothDevice objects are handled by
// CallbackPromiseAdapter templatized with this class. See this class's
// "Interface required by CallbackPromiseAdapter" section and the
// CallbackPromiseAdapter class comments.
class BluetoothDevice final
    : public GarbageCollectedFinalized<BluetoothDevice>
    , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    BluetoothDevice(PassOwnPtr<WebBluetoothDevice>);

    ScriptPromise connectGATT(ScriptState*);

    // Interface required by CallbackPromiseAdapter:
    typedef WebBluetoothDevice WebType;
    static BluetoothDevice* take(ScriptPromiseResolver*, PassOwnPtr<WebBluetoothDevice>);

    // Interface required by Garbage Collection:
    DEFINE_INLINE_TRACE() { }

    // IDL exposed interface:
    String instanceID() { return m_webDevice->instanceID; }
    String name() { return m_webDevice->name; }
    unsigned deviceClass(bool& isNull);
    String vendorIDSource();
    unsigned vendorID(bool& isNull);
    unsigned productID(bool& isNull);
    unsigned productVersion(bool& isNull);
    bool paired();
    Vector<String> uuids();

private:
    OwnPtr<WebBluetoothDevice> m_webDevice;
};

} // namespace blink

#endif // BluetoothDevice_h
