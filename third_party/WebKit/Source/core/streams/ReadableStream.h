// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ReadableStream_h
#define ReadableStream_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseProperty.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/V8Binding.h"
#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class DOMException;
class ExceptionState;
class ExecutionContext;
class ReadableStreamReader;
class UnderlyingSource;

class CORE_EXPORT ReadableStream : public GarbageCollectedFinalized<ReadableStream>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    enum State {
        Readable,
        Closed,
        Errored,
    };

    // After ReadableStream construction, |didSourceStart| must be called when
    // |source| initialization succeeds and |error| must be called when
    // |source| initialization fails.
    explicit ReadableStream(UnderlyingSource* /* source */);
    virtual ~ReadableStream();

    bool isStarted() const { return m_isStarted; }
    bool isDraining() const { return m_isDraining; }
    bool isPulling() const { return m_isPulling; }
    State stateInternal() const { return m_state; }
    DOMException* storedException() { return m_exception.get(); }

    virtual ScriptPromise read(ScriptState*) = 0;
    ScriptPromise cancel(ScriptState*);
    ScriptPromise cancel(ScriptState*, ScriptValue reason);
    ScriptPromise cancelInternal(ScriptState*, ScriptValue reason);

    virtual bool hasPendingReads() const = 0;
    virtual void resolveAllPendingReadsAsDone() = 0;
    virtual void rejectAllPendingReads(DOMException*) = 0;

    void close();
    void error(DOMException*);

    void didSourceStart();

    // This function is not a getter. It creates an ReadableStreamReader and
    // returns it.
    ReadableStreamReader* getReader(ExecutionContext*, ExceptionState&);
    // Only ReadableStreamReader methods should call this function.
    void setReader(ReadableStreamReader*);

    bool isLocked() const { return m_reader; }
    bool isLockedTo(const ReadableStreamReader* reader) const { return m_reader == reader; }

    DECLARE_VIRTUAL_TRACE();

protected:
    bool enqueuePreliminaryCheck();
    bool enqueuePostAction();
    void readInternalPostAction();

private:
    using WaitPromise = ScriptPromiseProperty<Member<ReadableStream>, ToV8UndefinedGenerator, Member<DOMException>>;
    using ClosedPromise = ScriptPromiseProperty<Member<ReadableStream>, ToV8UndefinedGenerator, Member<DOMException>>;

    virtual bool isQueueEmpty() const = 0;
    virtual void clearQueue() = 0;
    // This function will call ReadableStream::error on error.
    virtual bool shouldApplyBackpressure() = 0;

    void callPullIfNeeded();
    void closeInternal();

    Member<UnderlyingSource> m_source;
    bool m_isStarted;
    bool m_isDraining;
    bool m_isPulling;
    State m_state;

    Member<DOMException> m_exception;
    Member<ReadableStreamReader> m_reader;
};

} // namespace blink

#endif // ReadableStream_h
