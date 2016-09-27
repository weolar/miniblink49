/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebIDBKey_h
#define WebIDBKey_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebData.h"
#include "public/platform/WebPrivatePtr.h"
#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"
#include "public/platform/modules/indexeddb/WebIDBTypes.h"

namespace blink {

class IDBKey;

class WebIDBKey {
public:
    // Please use one of the factory methods. This is public only to allow WebVector.
    WebIDBKey() { }
    ~WebIDBKey() { reset(); }

    BLINK_EXPORT static WebIDBKey createArray(const WebVector<WebIDBKey>&);
    BLINK_EXPORT static WebIDBKey createBinary(const WebData&);
    BLINK_EXPORT static WebIDBKey createString(const WebString&);
    BLINK_EXPORT static WebIDBKey createDate(double);
    BLINK_EXPORT static WebIDBKey createNumber(double);
    BLINK_EXPORT static WebIDBKey createInvalid();
    BLINK_EXPORT static WebIDBKey createNull();

    WebIDBKey(const WebIDBKey& e) { assign(e); }
    WebIDBKey& operator=(const WebIDBKey& e)
    {
        assign(e);
        return *this;
    }

    BLINK_EXPORT void assign(const WebIDBKey&);
    BLINK_EXPORT void assignArray(const WebVector<WebIDBKey>&);
    BLINK_EXPORT void assignBinary(const WebData&);
    BLINK_EXPORT void assignString(const WebString&);
    BLINK_EXPORT void assignDate(double);
    BLINK_EXPORT void assignNumber(double);
    BLINK_EXPORT void assignInvalid();
    BLINK_EXPORT void assignNull();
#if !BLINK_WEB_IMPLEMENTATION && LINK_CORE_MODULES_SEPARATELY
    void reset()
    {
        m_private.reset();
    }
#else
    BLINK_EXPORT void reset();
#endif

    BLINK_EXPORT WebIDBKeyType keyType() const;
    BLINK_EXPORT bool isValid() const;
    BLINK_EXPORT WebVector<WebIDBKey> array() const; // Only valid for ArrayType.
    BLINK_EXPORT WebData binary() const; // Only valid for BinaryType.
    BLINK_EXPORT WebString string() const; // Only valid for StringType.
    BLINK_EXPORT double date() const; // Only valid for DateType.
    BLINK_EXPORT double number() const; // Only valid for NumberType.

#if BLINK_IMPLEMENTATION
    WebIDBKey(IDBKey* value)
        : m_private(value)
    { }
    WebIDBKey& operator=(IDBKey* value)
    {
        m_private = value;
        return *this;
    }
    operator IDBKey*() const
    {
        return m_private.get();
    }
#endif

private:
    WebPrivatePtr<IDBKey> m_private;
};

} // namespace blink

#endif // WebIDBKey_h
