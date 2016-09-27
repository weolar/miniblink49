/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebBlob_h
#define WebBlob_h

#include "public/platform/WebBlobData.h"
#include "public/platform/WebCommon.h"
#include "public/platform/WebPrivatePtr.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"

#if BLINK_IMPLEMENTATION
#include "platform/heap/Handle.h"
#endif

namespace v8 {
class Isolate;
class Object;
class Value;
template <class T> class Local;
}

namespace blink {

class Blob;

class WebBlob {
public:
    ~WebBlob() { reset(); }

    WebBlob() { }
    WebBlob(const WebBlob& b) { assign(b); }
    WebBlob& operator=(const WebBlob& b)
    {
        assign(b);
        return *this;
    }

    BLINK_EXPORT static WebBlob createFromUUID(const WebString& uuid, const WebString& type, long long size);
    BLINK_EXPORT static WebBlob createFromFile(const WebString& path, long long size);
    BLINK_EXPORT static WebBlob fromV8Value(v8::Local<v8::Value>);

    BLINK_EXPORT void reset();
    BLINK_EXPORT void assign(const WebBlob&);
    BLINK_EXPORT WebString uuid();

    bool isNull() const { return m_private.isNull(); }

    BLINK_EXPORT v8::Local<v8::Value>  toV8Value(v8::Local<v8::Object> creationContext, v8::Isolate*);

#if BLINK_IMPLEMENTATION
    WebBlob(Blob*);
    WebBlob& operator=(Blob*);
#endif

protected:
    WebPrivatePtr<Blob> m_private;
};

} // namespace blink

#endif // WebBlob_h
