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

#include "config.h"
#include "public/web/WebDOMMediaStreamTrack.h"

#include "bindings/modules/v8/V8MediaStreamTrack.h"
#include "modules/mediastream/MediaStreamTrack.h"

namespace blink {

WebDOMMediaStreamTrack::WebDOMMediaStreamTrack(PassRefPtrWillBeRawPtr<MediaStreamTrack> track)
    : m_private(track)
{
}

WebDOMMediaStreamTrack WebDOMMediaStreamTrack::fromV8Value(v8::Local<v8::Value> value)
{
    if (V8MediaStreamTrack::hasInstance(value, v8::Isolate::GetCurrent())) {
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
        return WebDOMMediaStreamTrack(V8MediaStreamTrack::toImpl(object));
    }
    return WebDOMMediaStreamTrack(nullptr);
}

void WebDOMMediaStreamTrack::reset()
{
    m_private.reset();
}

void WebDOMMediaStreamTrack::assign(const WebDOMMediaStreamTrack& b)
{
    m_private = b.m_private;
}

WebMediaStreamTrack WebDOMMediaStreamTrack::component() const
{
    return WebMediaStreamTrack(m_private->component());
}

} // namespace blink
