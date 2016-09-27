// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DOMSharedArrayBuffer.h"

#include "bindings/core/v8/DOMDataStore.h"

namespace blink {

v8::Local<v8::Object> DOMSharedArrayBuffer::wrap(v8::Isolate* isolate, v8::Local<v8::Object> creationContext)
{
    // It's possible that no one except for the new wrapper owns this object at
    // this moment, so we have to prevent GC to collect this object until the
    // object gets associated with the wrapper.
    RefPtr<DOMSharedArrayBuffer> protect(this);

    ASSERT(!DOMDataStore::containsWrapper(this, isolate));

    const WrapperTypeInfo* wrapperTypeInfo = this->wrapperTypeInfo();
#ifdef MINIBLINK_NOT_IMPLEMENTED
    v8::Local<v8::Object> wrapper = v8::SharedArrayBuffer::New(isolate, data(), byteLength());
#else
    v8::Local<v8::Object> wrapper;
    notImplemented();
#endif
    return associateWithWrapper(isolate, wrapperTypeInfo, wrapper);
}

} // namespace blink
