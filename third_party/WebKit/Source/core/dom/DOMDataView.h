// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMDataView_h
#define DOMDataView_h

#include "core/dom/DOMArrayBufferView.h"

namespace blink {

class DOMDataView final : public DOMArrayBufferView {
    DEFINE_WRAPPERTYPEINFO();
public:
    typedef char ValueType;

    static PassRefPtr<DOMDataView> create(PassRefPtr<DOMArrayBufferBase>, unsigned byteOffset, unsigned byteLength);

    v8::Local<v8::Object> wrap(v8::Isolate*, v8::Local<v8::Object> creationContext) override;

private:
    DOMDataView(PassRefPtr<WTF::ArrayBufferView> dataView, PassRefPtr<DOMArrayBufferBase> domArrayBuffer)
        : DOMArrayBufferView(dataView, domArrayBuffer) { }
};

} // namespace blink

#endif // DOMDataView_h
