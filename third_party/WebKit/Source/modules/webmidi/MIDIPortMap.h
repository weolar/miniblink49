// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MIDIPortMap_h
#define MIDIPortMap_h

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/Maplike.h"
#include "bindings/core/v8/V8Binding.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

template <typename T>
class MIDIPortMap : public GarbageCollected<MIDIPortMap<T>>, public Maplike<String, T*> {
public:
    explicit MIDIPortMap(const HeapVector<Member<T>>& entries) : m_entries(entries) { }

    // IDL attributes / methods
    size_t size() const { return m_entries.size(); }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_entries);
    }

private:
    // We use HeapVector here to keep the entry order.
    using Entries = HeapVector<Member<T>>;
    using IteratorType = typename Entries::const_iterator;

    typename PairIterable<String, T*>::IterationSource* startIteration(ScriptState*, ExceptionState&) override
    {
        return new MapIterationSource(this, m_entries.begin(), m_entries.end());
    }

    bool getMapEntry(ScriptState*, const String& key, T*& value, ExceptionState&) override
    {
        // FIXME: This function is not O(1). Perhaps it's OK because in typical
        // cases not so many ports are connected.
        for (const auto& p : m_entries) {
            if (key == p->id()) {
                value = p;
                return true;
            }
        }
        return false;
    }

    // Note: This template class relies on the fact that m_map.m_entries will
    // never be modified once it is created.
    class MapIterationSource final : public PairIterable<String, T*>::IterationSource {
    public:
        MapIterationSource(MIDIPortMap<T>* map, IteratorType iterator, IteratorType end)
            : m_map(map)
            , m_iterator(iterator)
            , m_end(end)
        {
        }

        bool next(ScriptState* scriptState, String& key, T*& value, ExceptionState&) override
        {
            if (m_iterator == m_end)
                return false;
            key = (*m_iterator)->id();
            value = *m_iterator;
            ++m_iterator;
            return true;
        }

        DEFINE_INLINE_VIRTUAL_TRACE()
        {
            visitor->trace(m_map);
            PairIterable<String, T*>::IterationSource::trace(visitor);
        }

    private:
        // m_map is stored just for keeping it alive. It needs to be kept
        // alive while JavaScript holds the iterator to it.
        const Member<const MIDIPortMap<T>> m_map;
        IteratorType m_iterator;
        const IteratorType m_end;
    };

    const Entries m_entries;
};

} // namespace blink

#endif
