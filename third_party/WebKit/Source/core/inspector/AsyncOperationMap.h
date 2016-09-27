// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AsyncOperationMap_h
#define AsyncOperationMap_h

#include "core/inspector/InspectorDebuggerAgent.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

template <class K>
class AsyncOperationMap final {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    using MapType = WillBeHeapHashMap<K, int>;
    explicit AsyncOperationMap(InspectorDebuggerAgent* debuggerAgent)
        : m_debuggerAgent(debuggerAgent)
    {
    }

    ~AsyncOperationMap()
    {
        // Verify that this object has been explicitly disposed.
        ASSERT(hasBeenDisposed());
    }

#if ENABLE(ASSERT)
    bool hasBeenDisposed() const
    {
        return !m_debuggerAgent;
    }
#endif

    void dispose()
    {
        clear();
        m_debuggerAgent = nullptr;
    }

    void clear()
    {
        ASSERT(m_debuggerAgent);
        for (auto it : m_asyncOperations)
            m_debuggerAgent->traceAsyncOperationCompleted(it.value);
        m_asyncOperations.clear();
    }

    void set(typename MapType::KeyPeekInType key, int operationId)
    {
        m_asyncOperations.set(key, operationId);
    }

    bool contains(typename MapType::KeyPeekInType key) const
    {
        return m_asyncOperations.contains(key);
    }

    int get(typename MapType::KeyPeekInType key) const
    {
        return m_asyncOperations.get(key);
    }

    void remove(typename MapType::KeyPeekInType key)
    {
        ASSERT(m_debuggerAgent);
        int operationId = m_asyncOperations.take(key);
        if (operationId)
            m_debuggerAgent->traceAsyncOperationCompleted(operationId);
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_debuggerAgent);
        visitor->trace(m_asyncOperations);
    }

private:
    RawPtrWillBeMember<InspectorDebuggerAgent> m_debuggerAgent;
    MapType m_asyncOperations;
};

} // namespace blink


#endif // AsyncOperationMap_h
