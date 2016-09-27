// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/streams/ReadableStreamReader.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/V8IteratorResultValue.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "core/streams/ReadableStream.h"

namespace blink {

ReadableStreamReader::ReadableStreamReader(ExecutionContext* executionContext, ReadableStream* stream)
    : ActiveDOMObject(executionContext)
    , m_stream(stream)
    , m_stateAfterRelease(ReadableStream::Closed)
    , m_closed(new ClosedPromise(executionContext, this, ClosedPromise::Closed))
{
    suspendIfNeeded();
    ASSERT(m_stream->isLockedTo(nullptr));
    m_stream->setReader(this);

    if (m_stream->stateInternal() == ReadableStream::Closed || m_stream->stateInternal() == ReadableStream::Errored) {
        // If the stream is already closed or errored the created reader
        // should be closed or errored respectively.
        releaseLock();
    }
}

ScriptPromise ReadableStreamReader::closed(ScriptState* scriptState)
{
    return m_closed->promise(scriptState->world());
}

bool ReadableStreamReader::isActive() const
{
    return m_stream->isLockedTo(this);
}

ScriptPromise ReadableStreamReader::cancel(ScriptState* scriptState)
{
    return cancel(scriptState, ScriptValue(scriptState, v8::Undefined(scriptState->isolate())));
}

ScriptPromise ReadableStreamReader::cancel(ScriptState* scriptState, ScriptValue reason)
{
    if (isActive())
        return m_stream->cancelInternal(scriptState, reason);

    // A method should return a different promise on each call.
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    resolver->resolve(closed(scriptState).v8Value());
    return promise;
}

ScriptPromise ReadableStreamReader::read(ScriptState* scriptState)
{
    if (!isActive()) {
        ASSERT(m_stateAfterRelease == ReadableStream::Closed || m_stateAfterRelease == ReadableStream::Errored);
        if (m_stateAfterRelease == ReadableStream::Closed) {
            // {value: undefined, done: true}
            return ScriptPromise::cast(scriptState, v8IteratorResultDone(scriptState));
        }
        // A method should return a different promise on each call.
        RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
        ScriptPromise promise = resolver->promise();
        resolver->resolve(closed(scriptState).v8Value());
        return promise;
    }

    return m_stream->read(scriptState);
}

void ReadableStreamReader::releaseLock(ExceptionState& es)
{
    if (!isActive())
        return;
    if (m_stream->hasPendingReads()) {
        es.throwTypeError("The stream has pending read operations.");
        return;
    }

    releaseLock();
}

void ReadableStreamReader::releaseLock()
{
    if (!isActive())
        return;

    ASSERT(!m_stream->hasPendingReads());
    if (m_stream->stateInternal() == ReadableStream::Closed) {
        m_stateAfterRelease = ReadableStream::Closed;
        m_closed->resolve(ToV8UndefinedGenerator());
    } else if (m_stream->stateInternal() == ReadableStream::Errored) {
        m_stateAfterRelease = ReadableStream::Errored;
        m_closed->reject(m_stream->storedException());
    } else {
        m_stateAfterRelease = ReadableStream::Closed;
        m_closed->resolve(ToV8UndefinedGenerator());
    }

    // We call setReader(nullptr) after resolving / rejecting |m_closed|
    // because it affects hasPendingActivity.
    m_stream->setReader(nullptr);
    ASSERT(!isActive());
}

bool ReadableStreamReader::hasPendingActivity() const
{
    // We need to extend ReadableStreamReader's wrapper's life while it is
    // active in order to call resolve / reject on ScriptPromiseProperties.
    return isActive();
}

void ReadableStreamReader::stop()
{
    if (isActive()) {
        // Calling |error| will release the lock.
        m_stream->error(DOMException::create(AbortError, "The frame stops working."));
    }
    ActiveDOMObject::stop();
}

DEFINE_TRACE(ReadableStreamReader)
{
    visitor->trace(m_stream);
    visitor->trace(m_closed);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
