// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ReadableByteStreamReader_h
#define ReadableByteStreamReader_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/streams/ReadableStreamReader.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExceptionState;
class ScriptState;

class CORE_EXPORT ReadableByteStreamReader final : public GarbageCollectedFinalized<ReadableByteStreamReader>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    explicit ReadableByteStreamReader(ReadableStreamReader* reader) : m_reader(reader) { }

    ScriptPromise closed(ScriptState* scriptState) { return m_reader->closed(scriptState); }
    ScriptPromise read(ScriptState* scriptState) { return m_reader->read(scriptState); }
    ScriptPromise cancel(ScriptState* scriptState) { return m_reader->cancel(scriptState); }
    ScriptPromise cancel(ScriptState* scriptState, ScriptValue reason) { return m_reader->cancel(scriptState, reason); }
    void releaseLock(ExceptionState& es) { return m_reader->releaseLock(es); }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_reader);
    }

private:
    Member<ReadableStreamReader> m_reader;
};

} // namespace blink

#endif // ReadableByteStreamReader_h
