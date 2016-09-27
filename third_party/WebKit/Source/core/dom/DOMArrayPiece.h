// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMArrayPiece_h
#define DOMArrayPiece_h

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMArrayBufferView.h"
#include "wtf/ArrayPiece.h"

namespace blink {

class ArrayBufferOrArrayBufferView;

// This class is for passing around un-owned bytes as a pointer + length.
// It supports implicit conversion from several other data types.
//
// ArrayPiece has the concept of being "null". This is different from an empty
// byte range. It is invalid to call methods other than isNull() on such
// instances.
//
// IMPORTANT: The data contained by ArrayPiece is NOT OWNED, so caution must be
//            taken to ensure it is kept alive.
class CORE_EXPORT DOMArrayPiece : public WTF::ArrayPiece {
public:
    enum InitWithUnionOption {
        // Initialize this object as "null" when initialized with an union which
        // holds null.
        TreatNullAsNull,
        // Initialize this object so this points to null pointer with zero size
        // when initialized with an union which holds null.
        AllowNullPointToNullWithZeroSize,
    };

    DOMArrayPiece() { }
    DOMArrayPiece(DOMArrayBuffer* buffer)
        : ArrayPiece(buffer->buffer()) { }
    DOMArrayPiece(DOMArrayBufferView* view)
        : ArrayPiece(view->view()) { }
    DOMArrayPiece(const ArrayBufferOrArrayBufferView&, InitWithUnionOption = TreatNullAsNull);

    bool operator==(const DOMArrayBuffer& other) const
    {
        return byteLength() == other.byteLength() && memcmp(data(), other.data(), byteLength()) == 0;
    }

    bool operator==(const DOMArrayBufferView& other) const
    {
        return byteLength() == other.byteLength() && memcmp(data(), other.baseAddress(), byteLength()) == 0;
    }
};

} // namespace blink

#endif // DOMArrayPiece_h
