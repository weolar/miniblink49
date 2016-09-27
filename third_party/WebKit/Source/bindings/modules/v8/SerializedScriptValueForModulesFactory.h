// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SerializedScriptValueForModulesFactory_h
#define SerializedScriptValueForModulesFactory_h

#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "wtf/Noncopyable.h"

namespace blink {

class SerializedScriptValueForModulesFactory final : public SerializedScriptValueFactory {
    WTF_MAKE_NONCOPYABLE(SerializedScriptValueForModulesFactory);
public:
    SerializedScriptValueForModulesFactory() : SerializedScriptValueFactory() { }

    PassRefPtr<SerializedScriptValue> create(v8::Isolate*, v8::Local<v8::Value>, MessagePortArray*, ArrayBufferArray*, WebBlobInfoArray*, ExceptionState&) override;
    PassRefPtr<SerializedScriptValue> create(v8::Isolate*, const String&) override;

protected:
    ScriptValueSerializer::Status doSerialize(v8::Local<v8::Value>, SerializedScriptValueWriter&, MessagePortArray*, ArrayBufferArray*, WebBlobInfoArray*, BlobDataHandleMap&, v8::TryCatch&, String& errorMessage, v8::Isolate*) override;

    v8::Local<v8::Value> deserialize(String& data, BlobDataHandleMap& blobDataHandles, ArrayBufferContentsArray*, v8::Isolate*, MessagePortArray* messagePorts, const WebBlobInfoArray*) override;
};

} // namespace blink

#endif // SerializedScriptValueForModulesFactory_h

