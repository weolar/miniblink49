/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef WebDOMMediaStreamTrack_h
#define WebDOMMediaStreamTrack_h

#include "public/platform/WebMediaStreamTrack.h"
#include "public/platform/WebPrivatePtr.h"

#if BLINK_IMPLEMENTATION
#include "wtf/PassRefPtr.h"
#endif

namespace v8 {
class Value;
template <class T> class Local;
}

namespace blink {

class MediaStreamTrack;

class WebDOMMediaStreamTrack {
public:
    WebDOMMediaStreamTrack(const WebDOMMediaStreamTrack& b) { assign(b); }
    ~WebDOMMediaStreamTrack() { reset(); }

    WebDOMMediaStreamTrack& operator=(const WebDOMMediaStreamTrack& b)
    {
        assign(b);
        return *this;
    }

    bool isNull() const { return m_private.isNull(); }

    BLINK_EXPORT static WebDOMMediaStreamTrack fromV8Value(v8::Local<v8::Value>);

    BLINK_EXPORT void reset();
    BLINK_EXPORT void assign(const WebDOMMediaStreamTrack&);
    BLINK_EXPORT WebMediaStreamTrack component() const;

private:
#if BLINK_IMPLEMENTATION
    WebDOMMediaStreamTrack(PassRefPtrWillBeRawPtr<MediaStreamTrack>);
#endif

    WebPrivatePtr<MediaStreamTrack> m_private;
};

} // namespace blink

#endif
