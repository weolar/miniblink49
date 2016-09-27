// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DOMArrayBuffer.h"

#include "bindings/core/v8/DOMDataStore.h"

namespace blink {

v8::Local<v8::Object> DOMArrayBuffer::wrap(v8::Isolate* isolate, v8::Local<v8::Object> creationContext)
{
    // It's possible that no one except for the new wrapper owns this object at
    // this moment, so we have to prevent GC to collect this object until the
    // object gets associated with the wrapper.
    RefPtr<DOMArrayBuffer> protect(this);

    ASSERT(!DOMDataStore::containsWrapper(this, isolate));

    const WrapperTypeInfo* wrapperTypeInfo = this->wrapperTypeInfo();
    v8::Local<v8::Object> wrapper = v8::ArrayBuffer::New(isolate, data(), byteLength());

    return associateWithWrapper(isolate, wrapperTypeInfo, wrapper);
}

} // namespace blink
