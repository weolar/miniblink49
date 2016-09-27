// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/push_messaging/PushMessageData.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/V8Binding.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/fileapi/Blob.h"
#include "platform/blob/BlobData.h"
#include <v8.h>

namespace blink {

PushMessageData::PushMessageData()
{
}

PushMessageData::PushMessageData(const String& messageData)
    : m_messageData(messageData)
{
}

PushMessageData::~PushMessageData()
{
}

PassRefPtr<DOMArrayBuffer> PushMessageData::arrayBuffer() const
{
    return DOMArrayBuffer::create(m_messageData.characters8(), m_messageData.length());
}

Blob* PushMessageData::blob() const
{
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->appendText(m_messageData, false);

    // Note that the content type of the Blob object is deliberately not being
    // provided, following the specification.

    const long long blobSize = blobData->length();
    return Blob::create(BlobDataHandle::create(blobData.release(), blobSize));
}

ScriptValue PushMessageData::json(ScriptState* scriptState, ExceptionState& exceptionState) const
{
    v8::Isolate* isolate = scriptState->isolate();

    ScriptState::Scope scope(scriptState);
    v8::Local<v8::String> dataString = v8String(isolate, m_messageData);

    v8::TryCatch block;
    v8::Local<v8::Value> parsed;
    if (!v8Call(v8::JSON::Parse(isolate, dataString), parsed, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return ScriptValue();
    }

    return ScriptValue(scriptState, parsed);
}

const String& PushMessageData::text() const
{
    return m_messageData;
}

DEFINE_TRACE(PushMessageData)
{
}

} // namespace blink
