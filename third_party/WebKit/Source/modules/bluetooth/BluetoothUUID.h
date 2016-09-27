// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BluetoothUUID_h
#define BluetoothUUID_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExceptionState;

// This class provides a way for script to look up UUIDs by name so they don't need to be replicated in each application.
class BluetoothUUID final
    : public GarbageCollected<BluetoothUUID>
    , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:

    // IDL exposed interface:
    static String getService(StringOrUnsignedLong name, ExceptionState&);
    static String getCharacteristic(StringOrUnsignedLong name, ExceptionState&);
    static String getDescriptor(StringOrUnsignedLong name, ExceptionState&);
    static String canonicalUUID(unsigned alias);

    // Interface required by garbage collection.
    DEFINE_INLINE_TRACE() { }
};

} // namespace blink

#endif // BluetoothUUID_h
