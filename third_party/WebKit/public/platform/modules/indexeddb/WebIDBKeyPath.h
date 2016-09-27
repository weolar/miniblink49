/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef WebIDBKeyPath_h
#define WebIDBKeyPath_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"
#include "public/platform/modules/indexeddb/WebIDBTypes.h"

namespace blink {

class WebIDBKeyPath {
public:
    // FIXME: Update callers use constructors directly, and remove these.
    static WebIDBKeyPath create(const WebString& string) { return WebIDBKeyPath(string); }
    static WebIDBKeyPath create(const WebVector<WebString>& array) { return WebIDBKeyPath(array); }
    static WebIDBKeyPath createNull() { return WebIDBKeyPath(); }

    WebIDBKeyPath() : m_type(WebIDBKeyPathTypeNull) { }

    explicit WebIDBKeyPath(const WebString& string)
        : m_type(WebIDBKeyPathTypeString)
        , m_string(string)
    {
    }

    explicit WebIDBKeyPath(const WebVector<WebString>& array)
        : m_type(WebIDBKeyPathTypeArray)
        , m_array(array)
    {
    }

    WebIDBKeyPath(const WebIDBKeyPath& keyPath)
        : m_type(keyPath.m_type)
        , m_array(keyPath.m_array)
        , m_string(keyPath.m_string)
    {
    }

    ~WebIDBKeyPath() { }

    WebIDBKeyPath& operator=(const WebIDBKeyPath& keyPath)
    {
        m_type = keyPath.m_type;
        m_array = keyPath.m_array;
        m_string = keyPath.m_string;
        return *this;
    }

    WebIDBKeyPathType keyPathType() const { return m_type; }
    const WebVector<WebString>& array() const { return m_array; } // Only valid for ArrayType.
    const WebString& string() const { return m_string; } // Only valid for StringType.

private:
    WebIDBKeyPathType m_type;
    WebVector<WebString> m_array;
    WebString m_string;
};

} // namespace blink

#endif // WebIDBKeyPath_h
