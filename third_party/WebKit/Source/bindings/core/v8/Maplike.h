// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Maplike_h
#define Maplike_h

#include "bindings/core/v8/Iterable.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ToV8.h"

namespace blink {

template <typename KeyType, typename ValueType>
class Maplike : public PairIterable<KeyType, ValueType> {
public:
    bool hasForBinding(ScriptState* scriptState, const KeyType& key, ExceptionState& exceptionState)
    {
        ValueType value;
        return getMapEntry(scriptState, key, value, exceptionState);
    }

    ScriptValue getForBinding(ScriptState* scriptState, const KeyType& key, ExceptionState& exceptionState)
    {
        ValueType value;
        if (getMapEntry(scriptState, key, value, exceptionState))
            return ScriptValue(scriptState, toV8(value, scriptState->context()->Global(), scriptState->isolate()));
        return ScriptValue(scriptState, v8::Undefined(scriptState->isolate()));
    }

private:
    virtual bool getMapEntry(ScriptState*, const KeyType&, ValueType&, ExceptionState&) = 0;
};

} // namespace blink

#endif // Maplike_h
