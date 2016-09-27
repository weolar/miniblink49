// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UnderlyingSource_h
#define UnderlyingSource_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptValue.h"
#include "platform/heap/Heap.h"

namespace blink {

class ScriptState;

class UnderlyingSource : public GarbageCollectedMixin {
public:
    virtual ~UnderlyingSource() { }

    virtual void pullSource() = 0;
    virtual ScriptPromise cancelSource(ScriptState*, ScriptValue reason) = 0;
    DEFINE_INLINE_VIRTUAL_TRACE() { }
};

} // namespace blink

#endif // UnderlyingSource_h

