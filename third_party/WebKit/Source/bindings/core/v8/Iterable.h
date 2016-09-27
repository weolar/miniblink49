// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Iterable_h
#define Iterable_h

#include "bindings/core/v8/V8IteratorResultValue.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "core/dom/Iterator.h"

namespace blink {

// Typically, use one of ValueIterable<> and PairIterable<> (below) instead!
template <typename KeyType, typename ValueType>
class Iterable {
public:
    Iterator* keysForBinding(ScriptState* scriptState, ExceptionState& exceptionState)
    {
        IterationSource* source = this->startIteration(scriptState, exceptionState);
        if (!source)
            return nullptr;
        return new IterableIterator<KeySelector>(source);
    }

    Iterator* valuesForBinding(ScriptState* scriptState, ExceptionState& exceptionState)
    {
        IterationSource* source = this->startIteration(scriptState, exceptionState);
        if (!source)
            return nullptr;
        return new IterableIterator<ValueSelector>(source);
    }

    Iterator* entriesForBinding(ScriptState* scriptState, ExceptionState& exceptionState)
    {
        IterationSource* source = this->startIteration(scriptState, exceptionState);
        if (!source)
            return nullptr;
        return new IterableIterator<EntrySelector>(source);
    }

    void forEachForBinding(ScriptState* scriptState, const ScriptValue& thisValue, const ScriptValue& callback, const ScriptValue& thisArg, ExceptionState& exceptionState)
    {
        IterationSource* source = this->startIteration(scriptState, exceptionState);

        v8::Isolate* isolate = scriptState->isolate();
        v8::TryCatch tryCatch(isolate);

        v8::Local<v8::Object> creationContext(scriptState->context()->Global());
        v8::Local<v8::Function> v8Callback(callback.v8Value().As<v8::Function>());
        v8::Local<v8::Value> v8ThisArg(thisArg.v8Value());
        v8::Local<v8::Value> args[3];

        args[2] = thisValue.v8Value();

        while (true) {
            KeyType key;
            ValueType value;

            if (!source->next(scriptState, key, value, exceptionState))
                return;

            ASSERT(!exceptionState.hadException());

            args[0] = toV8(value, creationContext, isolate);
            args[1] = toV8(key, creationContext, isolate);
            if (args[0].IsEmpty() || args[1].IsEmpty()) {
                if (tryCatch.HasCaught())
                    exceptionState.rethrowV8Exception(tryCatch.Exception());
                return;
            }

            v8::Local<v8::Value> result;
            if (!V8ScriptRunner::callFunction(v8Callback, scriptState->executionContext(), v8ThisArg, 3, args, isolate).ToLocal(&result)) {
                exceptionState.rethrowV8Exception(tryCatch.Exception());
                return;
            }
        }
    }

    class IterationSource : public GarbageCollectedFinalized<IterationSource> {
    public:
        virtual ~IterationSource() { }

        // If end of iteration has been reached or an exception thrown: return false.
        // Otherwise: set |key| and |value| and return true.
        virtual bool next(ScriptState*, KeyType&, ValueType&, ExceptionState&) = 0;

        DEFINE_INLINE_VIRTUAL_TRACE() { }
    };

private:
    virtual IterationSource* startIteration(ScriptState*, ExceptionState&) = 0;

    struct KeySelector {
        static const KeyType& select(ScriptState*, const KeyType& key, const ValueType& value)
        {
            return key;
        }
    };
    struct ValueSelector {
        static const ValueType& select(ScriptState*, const KeyType& key, const ValueType& value)
        {
            return value;
        }
    };
    struct EntrySelector {
        static Vector<ScriptValue, 2> select(ScriptState* scriptState, const KeyType& key, const ValueType& value)
        {
            v8::Local<v8::Object> creationContext = scriptState->context()->Global();
            v8::Isolate* isolate = scriptState->isolate();

            Vector<ScriptValue, 2> entry;
            entry.append(ScriptValue(scriptState, toV8(key, creationContext, isolate)));
            entry.append(ScriptValue(scriptState, toV8(value, creationContext, isolate)));
            return entry;
        }
    };

    template <typename Selector>
    class IterableIterator final : public Iterator {
    public:
        explicit IterableIterator(IterationSource* source)
            : m_source(source)
        {
        }

        ScriptValue next(ScriptState* scriptState, ExceptionState& exceptionState) override
        {
            KeyType key;
            ValueType value;

            if (!m_source->next(scriptState, key, value, exceptionState))
                return v8IteratorResultDone(scriptState);

            return v8IteratorResult(scriptState, Selector::select(scriptState, key, value));
        }

        ScriptValue next(ScriptState* scriptState, ScriptValue, ExceptionState& exceptionState) override
        {
            return next(scriptState, exceptionState);
        }

        DEFINE_INLINE_VIRTUAL_TRACE()
        {
            visitor->trace(m_source);
            Iterator::trace(visitor);
        }

    private:
        Member<IterationSource> m_source;
    };
};

// Utiltity mixin base-class for classes implementing IDL interfaces with "iterable<T>".
template <typename ValueType>
class ValueIterable : public Iterable<unsigned, ValueType> {
public:
    Iterator* iterator(ScriptState* scriptState, ExceptionState& exceptionState)
    {
        return this->valuesForBinding(scriptState, exceptionState);
    }

    class IterationSource : public Iterable<unsigned, ValueType>::IterationSource {
    public:
        IterationSource()
            : m_index(0)
        {
        }

        ~IterationSource() override { }

        // If end of iteration has been reached or an exception thrown: return false.
        // Otherwise: set |value| and return true.
        // Note: |this->m_index| is the index being accessed.
        virtual bool next(ScriptState*, ValueType&, ExceptionState&) = 0;

    protected:
        unsigned m_index;

    private:
        bool next(ScriptState* scriptState, unsigned& key, ValueType& value, ExceptionState& exceptionState) final
        {
            if (!next(scriptState, value, exceptionState))
                return false;
            key = m_index;
            ++m_index;
            return true;
        }
    };
};

// Utiltity mixin base-class for classes implementing IDL interfaces with "iterable<T1, T2>".
template <typename KeyType, typename ValueType>
class PairIterable : public Iterable<KeyType, ValueType> {
public:
    Iterator* iterator(ScriptState* scriptState, ExceptionState& exceptionState)
    {
        return this->entriesForBinding(scriptState, exceptionState);
    }
};

} // namespace blink

#endif // Iterable_h
