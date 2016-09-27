// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Bluetooth_h
#define Bluetooth_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"

namespace blink {

class BluetoothUUIDs;
class RequestDeviceOptions;
class ScriptPromise;
class ScriptState;

class Bluetooth
    : public GarbageCollected<Bluetooth>
    , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static Bluetooth* create()
    {
        return new Bluetooth();
    }

    // BluetoothDiscovery interface
    ScriptPromise requestDevice(ScriptState*, const RequestDeviceOptions&, ExceptionState&);

    DEFINE_INLINE_TRACE() { }

};

} // namespace blink

#endif // Bluetooth_h
