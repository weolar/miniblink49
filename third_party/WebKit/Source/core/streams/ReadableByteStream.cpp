// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/streams/ReadableByteStream.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/streams/ReadableByteStreamReader.h"

namespace blink {

ReadableByteStreamReader* ReadableByteStream::getBytesReader(ExecutionContext* executionContext, ExceptionState& es)
{
    ReadableStreamReader* reader = getReader(executionContext, es);
    if (es.hadException())
        return nullptr;
    return new ReadableByteStreamReader(reader);
}

} // namespace blink
