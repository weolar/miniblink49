// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ReadableStreamReader_h
#define ReadableStreamReader_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseProperty.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/ToV8.h"
#include "core/CoreExport.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/streams/ReadableStream.h"
#include "platform/heap/Handle.h"

namespace blink {

class DOMException;
class ExceptionState;
class ExecutionContext;
class ScriptState;

// ReadableStreamReader corresponds to the same-name class in the Streams spec
// https://streams.spec.whatwg.org/.
class CORE_EXPORT ReadableStreamReader final : public GarbageCollectedFinalized<ReadableStreamReader>, public ScriptWrappable, public ActiveDOMObject {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ReadableStreamReader);
public:
    // The stream must not be locked to any ReadableStreamReader when called.
    ReadableStreamReader(ExecutionContext*, ReadableStream*);

    ScriptPromise closed(ScriptState*);
    bool isActive() const;
    ScriptPromise cancel(ScriptState*);
    ScriptPromise cancel(ScriptState*, ScriptValue reason);
    ScriptPromise read(ScriptState*);
    void releaseLock(ExceptionState&);
    void releaseLock();

    bool hasPendingActivity() const override;
    void stop() override;

    DECLARE_TRACE();

private:
    using ClosedPromise = ScriptPromiseProperty<Member<ReadableStreamReader>, ToV8UndefinedGenerator, Member<DOMException>>;

    const Member<ReadableStream> m_stream;
    ReadableStream::State m_stateAfterRelease;
    Member<ClosedPromise> m_closed;
};

} // namespace blink

#endif // ReadableStreamReader_h
