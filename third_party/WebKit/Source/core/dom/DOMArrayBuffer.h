// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMArrayBuffer_h
#define DOMArrayBuffer_h

#include "core/CoreExport.h"
#include "core/dom/DOMArrayBufferBase.h"
#include "wtf/ArrayBuffer.h"

namespace blink {

class CORE_EXPORT DOMArrayBuffer final : public DOMArrayBufferBase {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtr<DOMArrayBuffer> create(PassRefPtr<WTF::ArrayBuffer> buffer)
    {
        return adoptRef(new DOMArrayBuffer(buffer));
    }
    static PassRefPtr<DOMArrayBuffer> create(unsigned numElements, unsigned elementByteSize)
    {
        return create(WTF::ArrayBuffer::create(numElements, elementByteSize));
    }
    static PassRefPtr<DOMArrayBuffer> create(const void* source, unsigned byteLength)
    {
        return create(WTF::ArrayBuffer::create(source, byteLength));
    }
    static PassRefPtr<DOMArrayBuffer> create(WTF::ArrayBufferContents& contents)
    {
        return create(WTF::ArrayBuffer::create(contents));
    }

    // Only for use by XMLHttpRequest::responseArrayBuffer and
    // Internals::serializeObject.
    static PassRefPtr<DOMArrayBuffer> createUninitialized(unsigned numElements, unsigned elementByteSize)
    {
        return create(WTF::ArrayBuffer::createUninitialized(numElements, elementByteSize));
    }

    PassRefPtr<DOMArrayBuffer> slice(int begin, int end) const
    {
        return create(buffer()->slice(begin, end));
    }
    PassRefPtr<DOMArrayBuffer> slice(int begin) const
    {
        return create(buffer()->slice(begin));
    }
    bool transfer(WTF::ArrayBufferContents& result) { return buffer()->transfer(result); }
    bool isNeutered() { return buffer()->isNeutered(); }

    v8::Local<v8::Object> wrap(v8::Isolate*, v8::Local<v8::Object> creationContext) override;

private:
    explicit DOMArrayBuffer(PassRefPtr<WTF::ArrayBuffer> buffer)
        : DOMArrayBufferBase(buffer)
    {
    }
};

} // namespace blink

#endif // DOMArrayBuffer_h
