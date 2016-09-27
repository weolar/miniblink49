// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Permissions_h
#define Permissions_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"

namespace blink {

class ScriptState;
class ScriptValue;

class Permissions final
    : public GarbageCollected<Permissions>
    , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    DEFINE_INLINE_TRACE() { }

    ScriptPromise query(ScriptState*, const ScriptValue&);
};

} // namespace blink

#endif // Permissions_h
