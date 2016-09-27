// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMSharedArrayBuffer_h
#define DOMSharedArrayBuffer_h

#include "core/CoreExport.h"
#include "core/dom/DOMArrayBufferBase.h"
#include "wtf/ArrayBuffer.h"

namespace blink {

class CORE_EXPORT DOMSharedArrayBuffer final : public DOMArrayBufferBase {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtr<DOMSharedArrayBuffer> create(PassRefPtr<WTF::ArrayBuffer> buffer)
    {
        ASSERT(buffer->isShared());
        return adoptRef(new DOMSharedArrayBuffer(buffer));
    }
    static PassRefPtr<DOMSharedArrayBuffer> create(unsigned numElements, unsigned elementByteSize)
    {
        return create(WTF::ArrayBuffer::createShared(numElements, elementByteSize));
    }
    static PassRefPtr<DOMSharedArrayBuffer> create(const void* source, unsigned byteLength)
    {
        return create(WTF::ArrayBuffer::createShared(source, byteLength));
    }
    static PassRefPtr<DOMSharedArrayBuffer> create(WTF::ArrayBufferContents& contents)
    {
        ASSERT(contents.isShared());
        return create(WTF::ArrayBuffer::create(contents));
    }

    v8::Local<v8::Object> wrap(v8::Isolate*, v8::Local<v8::Object> creationContext) override;

private:
    explicit DOMSharedArrayBuffer(PassRefPtr<WTF::ArrayBuffer> buffer)
        : DOMArrayBufferBase(buffer)
    {
    }
};

} // namespace blink

#endif // DOMSharedArrayBuffer_h
