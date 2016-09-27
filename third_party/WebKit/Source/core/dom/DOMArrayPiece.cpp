// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DOMArrayPiece.h"

#include "bindings/core/v8/UnionTypesCore.h"

namespace blink {

DOMArrayPiece::DOMArrayPiece(const ArrayBufferOrArrayBufferView& arrayBufferOrView, InitWithUnionOption option)
{
    if (arrayBufferOrView.isArrayBuffer()) {
        RefPtr<DOMArrayBuffer> arrayBuffer = arrayBufferOrView.getAsArrayBuffer();
        initWithData(arrayBuffer->data(), arrayBuffer->byteLength());
    } else if (arrayBufferOrView.isArrayBufferView()) {
        RefPtr<DOMArrayBufferView> arrayBufferView =
            arrayBufferOrView.getAsArrayBufferView();
        initWithData(arrayBufferView->baseAddress(), arrayBufferView->byteLength());
    } else if (arrayBufferOrView.isNull() && option == AllowNullPointToNullWithZeroSize) {
        initWithData(nullptr, 0);
    } // Otherwise, leave the obejct as null.
}

} // namespace blink
