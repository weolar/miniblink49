/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IDBKeyRange_h
#define IDBKeyRange_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/ModulesExport.h"
#include "modules/indexeddb/IDBKey.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class ScriptState;
class ScriptValue;

class MODULES_EXPORT IDBKeyRange final : public GarbageCollected<IDBKeyRange>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    enum LowerBoundType {
        LowerBoundOpen,
        LowerBoundClosed
    };
    enum UpperBoundType {
        UpperBoundOpen,
        UpperBoundClosed
    };

    static IDBKeyRange* create(IDBKey* lower, IDBKey* upper, LowerBoundType lowerType, UpperBoundType upperType)
    {
        return new IDBKeyRange(lower, upper, lowerType, upperType);
    }
    // Null if the script value is null or undefined, the range if it is one, otherwise tries to convert to a key and throws if it fails.
    static IDBKeyRange* fromScriptValue(ExecutionContext*, const ScriptValue&, ExceptionState&);

    DECLARE_TRACE();

    // Implement the IDBKeyRange IDL
    IDBKey* lower() const { return m_lower.get(); }
    IDBKey* upper() const { return m_upper.get(); }

    ScriptValue lowerValue(ScriptState*) const;
    ScriptValue upperValue(ScriptState*) const;
    bool lowerOpen() const { return m_lowerType == LowerBoundOpen; }
    bool upperOpen() const { return m_upperType == UpperBoundOpen; }

    static IDBKeyRange* only(ExecutionContext*, const ScriptValue& key, ExceptionState&);
    static IDBKeyRange* lowerBound(ExecutionContext*, const ScriptValue& bound, bool open, ExceptionState&);
    static IDBKeyRange* upperBound(ExecutionContext*, const ScriptValue& bound, bool open, ExceptionState&);
    static IDBKeyRange* bound(ExecutionContext*, const ScriptValue& lower, const ScriptValue& upper, bool lowerOpen, bool upperOpen, ExceptionState&);

    static IDBKeyRange* only(IDBKey* value, ExceptionState&);

private:
    IDBKeyRange(IDBKey* lower, IDBKey* upper, LowerBoundType lowerType, UpperBoundType upperType);

    Member<IDBKey> m_lower;
    Member<IDBKey> m_upper;
    LowerBoundType m_lowerType;
    UpperBoundType m_upperType;
};

} // namespace blink

#endif // IDBKeyRange_h
