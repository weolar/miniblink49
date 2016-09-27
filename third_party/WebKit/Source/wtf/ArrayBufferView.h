/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ArrayBufferView_h
#define ArrayBufferView_h

#include "wtf/ArrayBuffer.h"

#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/WTFExport.h"
#include <limits.h>

namespace WTF {

class WTF_EXPORT ArrayBufferView : public RefCounted<ArrayBufferView> {
  public:
    enum ViewType {
        TypeInt8,
        TypeUint8,
        TypeUint8Clamped,
        TypeInt16,
        TypeUint16,
        TypeInt32,
        TypeUint32,
        TypeFloat32,
        TypeFloat64,
        TypeDataView
    };
    virtual ViewType type() const = 0;
    const char* typeName();

    PassRefPtr<ArrayBuffer> buffer() const
    {
        return m_buffer;
    }

    void* baseAddress() const
    {
        return m_baseAddress;
    }

    unsigned byteOffset() const
    {
        return m_byteOffset;
    }

    virtual unsigned byteLength() const = 0;

    void setNeuterable(bool flag) { m_isNeuterable = flag; }
    bool isNeuterable() const { return m_isNeuterable; }
    bool isShared() const { return m_buffer ? m_buffer->isShared() : false; }

    virtual ~ArrayBufferView();

  protected:
    ArrayBufferView(PassRefPtr<ArrayBuffer>, unsigned byteOffset);

    inline bool setImpl(ArrayBufferView*, unsigned byteOffset);

    // Helper to verify that a given sub-range of an ArrayBuffer is
    // within range.
    template <typename T>
    static bool verifySubRange(PassRefPtr<ArrayBuffer> buffer,
                               unsigned byteOffset,
                               unsigned numElements)
    {
        if (!buffer)
            return false;
        if (sizeof(T) > 1 && byteOffset % sizeof(T))
            return false;
        if (byteOffset > buffer->byteLength())
            return false;
        unsigned remainingElements = (buffer->byteLength() - byteOffset) / sizeof(T);
        if (numElements > remainingElements)
            return false;
        return true;
    }

    virtual void neuter();

    // This is the address of the ArrayBuffer's storage, plus the byte offset.
    void* m_baseAddress;

    unsigned m_byteOffset : 31;
    bool m_isNeuterable : 1;

  private:
    friend class ArrayBuffer;
    RefPtr<ArrayBuffer> m_buffer;
    ArrayBufferView* m_prevView;
    ArrayBufferView* m_nextView;
};

bool ArrayBufferView::setImpl(ArrayBufferView* array, unsigned byteOffset)
{
    if (byteOffset > byteLength()
        || byteOffset + array->byteLength() > byteLength()
        || byteOffset + array->byteLength() < byteOffset) {
        // Out of range offset or overflow
        return false;
    }

    char* base = static_cast<char*>(baseAddress());
    memmove(base + byteOffset, array->baseAddress(), array->byteLength());
    return true;
}

} // namespace WTF

using WTF::ArrayBufferView;

#endif // ArrayBufferView_h
