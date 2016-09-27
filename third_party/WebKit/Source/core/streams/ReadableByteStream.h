// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ReadableByteStream_h
#define ReadableByteStream_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/streams/ReadableStreamImpl.h"

namespace blink {

class DOMArrayBufferView;
class ExceptionState;
class ExecutionContext;
class ReadableByteStreamReader;
class ScriptState;
class Strategy;
class UnderlyingSource;

class CORE_EXPORT ReadableByteStream : public ReadableStreamImpl<ReadableStreamChunkTypeTraits<DOMArrayBufferView>> {
    DEFINE_WRAPPERTYPEINFO();
public:
    explicit ReadableByteStream(UnderlyingSource* source) : ReadableStreamImpl<ReadableStreamChunkTypeTraits<DOMArrayBufferView>>(source) { }
    ReadableByteStream(UnderlyingSource* source, Strategy* strategy) : ReadableStreamImpl<ReadableStreamChunkTypeTraits<DOMArrayBufferView>>(source, strategy) { }

    ReadableByteStreamReader* getBytesReader(ExecutionContext*, ExceptionState&);
};

} // namespace blink

#endif // ReadableByteStream_h
