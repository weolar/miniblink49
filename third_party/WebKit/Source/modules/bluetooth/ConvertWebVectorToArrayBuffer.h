// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ConvertWebVectorToArrayBuffer_h
#define ConvertWebVectorToArrayBuffer_h

#include "core/dom/DOMArrayBuffer.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebVector.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class ScriptPromiseResolver;

// ConvertWebVectorToArrayBuffer is used with CallbackPromiseAdapter to receive
// WebVector responses. See CallbackPromiseAdapter class comments.
class ConvertWebVectorToArrayBuffer {
    WTF_MAKE_NONCOPYABLE(ConvertWebVectorToArrayBuffer);
public:
    // Interface required by CallbackPromiseAdapter:
    typedef WebVector<uint8_t> WebType;
    static PassRefPtr<DOMArrayBuffer> take(ScriptPromiseResolver*, PassOwnPtr<WebVector<uint8_t>>);

private:
    ConvertWebVectorToArrayBuffer() = delete;
};

} // namespace blink

#endif // ConvertWebVectorToArrayBuffer_h
