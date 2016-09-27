/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebRTCDataChannelHandler_h
#define WebRTCDataChannelHandler_h

#include "WebCommon.h"
#include "WebPrivatePtr.h"
#include "WebRTCDataChannelHandlerClient.h"
#include "WebString.h"

namespace blink {

class WebRTCDataChannelHandler {
public:
    virtual ~WebRTCDataChannelHandler() { }

    virtual void setClient(WebRTCDataChannelHandlerClient*) = 0;

    virtual WebString label() = 0;

    // DEPRECATED
    virtual bool isReliable() { return true; }

    virtual bool ordered() const = 0;
    virtual unsigned short maxRetransmitTime() const = 0;
    virtual unsigned short maxRetransmits() const = 0;
    virtual WebString protocol() const = 0;
    virtual bool negotiated() const = 0;
    virtual unsigned short id() const = 0;

    virtual WebRTCDataChannelHandlerClient::ReadyState state() const = 0;
    virtual unsigned long bufferedAmount() = 0;
    virtual bool sendStringData(const WebString&) = 0;
    virtual bool sendRawData(const char*, size_t) = 0;
    virtual void close() = 0;
};

} // namespace blink

#endif // WebRTCDataChannelHandler_h
