// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ReadableStreamImpl_h
#define ReadableStreamImpl_h

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8IteratorResultValue.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMArrayBufferView.h"
#include "core/dom/DOMException.h"
#include "core/streams/ReadableStream.h"
#include "wtf/Deque.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"
#include <utility>

namespace blink {

// We define the default ChunkTypeTraits for frequently used types.
template<typename ChunkType>
class ReadableStreamChunkTypeTraits { };

template<>
class ReadableStreamChunkTypeTraits<String> {
public:
    typedef String HoldType;
    typedef const String& PassType;

    static size_t size(const String& chunk) { return chunk.length(); }
    static ScriptValue toScriptValue(ScriptState* scriptState, const HoldType& value)
    {
        return ScriptValue(scriptState, v8String(scriptState->isolate(), value));
    }
};

template<>
class ReadableStreamChunkTypeTraits<DOMArrayBuffer> {
public:
    typedef RefPtr<DOMArrayBuffer> HoldType;
    typedef PassRefPtr<DOMArrayBuffer> PassType;

    static size_t size(const PassType& chunk) { return chunk->byteLength(); }
    static ScriptValue toScriptValue(ScriptState* scriptState, const HoldType& value)
    {
        return ScriptValue(scriptState, toV8(value.get(), scriptState->context()->Global(), scriptState->isolate()));
    }
};

template<>
class ReadableStreamChunkTypeTraits<DOMArrayBufferView> {
public:
    typedef RefPtr<DOMArrayBufferView> HoldType;
    typedef PassRefPtr<DOMArrayBufferView> PassType;

    static size_t size(const PassType& chunk) { return chunk->byteLength(); }
    static ScriptValue toScriptValue(ScriptState* scriptState, const HoldType& value)
    {
        return ScriptValue(scriptState, toV8(value.get(), scriptState->context()->Global(), scriptState->isolate()));
    }
};

// ReadableStreamImpl<ChunkTypeTraits> is a ReadableStream subtype. It has a
// queue whose type depends on ChunkTypeTraits and it implements queue-related
// ReadableStream pure virtual methods.
template <typename ChunkTypeTraits>
class ReadableStreamImpl : public ReadableStream {
public:
    class Strategy : public GarbageCollectedFinalized<Strategy> {
    public:
        virtual ~Strategy() { }

        // These functions call ReadableStream::error on error.
        virtual size_t size(const typename ChunkTypeTraits::PassType& chunk, ReadableStream*) { return ChunkTypeTraits::size(chunk); }
        virtual bool shouldApplyBackpressure(size_t totalQueueSize, ReadableStream*) = 0;

        DEFINE_INLINE_VIRTUAL_TRACE() { }
    };

    class DefaultStrategy : public Strategy {
    public:
        size_t size(const typename ChunkTypeTraits::PassType& chunk, ReadableStream*) override { return 1; }
        bool shouldApplyBackpressure(size_t totalQueueSize, ReadableStream*) override { return totalQueueSize > 1; }
    };

    class StrictStrategy : public Strategy {
    public:
        size_t size(const typename ChunkTypeTraits::PassType& chunk, ReadableStream*) override { return 1; }
        bool shouldApplyBackpressure(size_t totalQueueSize, ReadableStream*) override { return true; }
    };

    explicit ReadableStreamImpl(UnderlyingSource* source)
        : ReadableStreamImpl(source, new DefaultStrategy) { }
    ReadableStreamImpl(UnderlyingSource* source, Strategy* strategy)
        : ReadableStream(source)
        , m_strategy(strategy)
        , m_totalQueueSize(0) { }
    ~ReadableStreamImpl() override { }

    // ReadableStream methods
    ScriptPromise read(ScriptState*) override;

    bool enqueue(typename ChunkTypeTraits::PassType);

    // This function is intended to be used by internal code to withdraw
    // queued data. This pulls all data from this stream's queue, but
    // ReadableStream public APIs can work with the behavior (i.e. it behaves
    // as if multiple read-one-buffer calls were made).
    void readInternal(Deque<std::pair<typename ChunkTypeTraits::HoldType, size_t>>& queue);

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_strategy);
#if ENABLE(OILPAN)
        visitor->trace(m_pendingReads);
#endif
        ReadableStream::trace(visitor);
    }

private:
#if ENABLE(OILPAN)
    using PendingReads = HeapDeque<Member<ScriptPromiseResolver>>;
#else
    using PendingReads = Deque<RefPtr<ScriptPromiseResolver>>;
#endif

    // ReadableStream methods
    bool isQueueEmpty() const override { return m_queue.isEmpty(); }
    void clearQueue() override
    {
        m_queue.clear();
        m_totalQueueSize = 0;
    }

    void resolveAllPendingReadsAsDone() override
    {
        for (auto& resolver : m_pendingReads) {
            ScriptState::Scope scope(resolver->scriptState());
            resolver->resolve(v8IteratorResultDone(resolver->scriptState()));
        }
        m_pendingReads.clear();
    }

    void rejectAllPendingReads(DOMException* reason) override
    {
        for (auto& resolver : m_pendingReads)
            resolver->reject(reason);
        m_pendingReads.clear();
    }

    bool shouldApplyBackpressure() override
    {
        return m_strategy->shouldApplyBackpressure(m_totalQueueSize, this);
    }
    bool hasPendingReads() const override { return !m_pendingReads.isEmpty(); }

    Member<Strategy> m_strategy;
    Deque<std::pair<typename ChunkTypeTraits::HoldType, size_t>> m_queue;
    PendingReads m_pendingReads;
    size_t m_totalQueueSize;
};

template <typename ChunkTypeTraits>
bool ReadableStreamImpl<ChunkTypeTraits>::enqueue(typename ChunkTypeTraits::PassType chunk)
{
    size_t size = m_strategy->size(chunk, this);
    if (!enqueuePreliminaryCheck())
        return false;

    if (m_pendingReads.isEmpty()) {
        m_queue.append(std::make_pair(chunk, size));
        m_totalQueueSize += size;
        return enqueuePostAction();
    }

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = m_pendingReads.takeFirst();
    ScriptState* scriptState = resolver->scriptState();
    ScriptState::Scope scope(scriptState);
    resolver->resolve(v8IteratorResult(scriptState, chunk));
    return enqueuePostAction();
}

template <typename ChunkTypeTraits>
ScriptPromise ReadableStreamImpl<ChunkTypeTraits>::read(ScriptState* scriptState)
{
    ASSERT(stateInternal() == Readable);
    if (m_queue.isEmpty()) {
        m_pendingReads.append(ScriptPromiseResolver::create(scriptState));
        ScriptPromise promise = m_pendingReads.last()->promise();
        readInternalPostAction();
        return promise;
    }

    auto pair = m_queue.takeFirst();
    typename ChunkTypeTraits::HoldType chunk = pair.first;
    size_t size = pair.second;
    ASSERT(m_totalQueueSize >= size);
    m_totalQueueSize -= size;
    readInternalPostAction();

    return ScriptPromise::cast(scriptState, v8IteratorResult(scriptState, chunk));
}

template <typename ChunkTypeTraits>
void ReadableStreamImpl<ChunkTypeTraits>::readInternal(Deque<std::pair<typename ChunkTypeTraits::HoldType, size_t>>& queue)
{
    // We omit the preliminary check. Check it by yourself.
    ASSERT(stateInternal() == Readable);
    ASSERT(m_pendingReads.isEmpty());
    ASSERT(queue.isEmpty());

    queue.swap(m_queue);
    m_totalQueueSize = 0;
    readInternalPostAction();
}

} // namespace blink

#endif // ReadableStreamImpl_h
