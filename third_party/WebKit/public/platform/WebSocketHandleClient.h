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

#ifndef WebSocketHandleClient_h
#define WebSocketHandleClient_h

#include "WebCommon.h"
#include "WebString.h"
#include "public/platform/WebSocketHandle.h"

namespace blink {

class WebSocketHandshakeRequestInfo;
class WebSocketHandshakeResponseInfo;

class WebSocketHandleClient {
public:
    // Called when the handle is opened.
    virtual void didConnect(WebSocketHandle*, const WebString& selectedProtocol, const WebString& extensions) = 0;

    // Called when the browser starts the opening handshake.
    // This notification can be omitted when the inspector is not active.
    virtual void didStartOpeningHandshake(WebSocketHandle*, const WebSocketHandshakeRequestInfo&) = 0;

    // Called when the browser finishes the opening handshake.
    // This notification precedes didConnect.
    // This notification can be omitted when the inspector is not active.
    virtual void didFinishOpeningHandshake(WebSocketHandle*, const WebSocketHandshakeResponseInfo&) = 0;

    // Called when the browser is required to fail the connection.
    // |message| can be displayed in the inspector, but should not be passed
    // to scripts.
    // This message also implies that channel is closed with
    // (wasClean = false, code = 1006, reason = "") and
    // |handle| becomes unavailable.
    virtual void didFail(WebSocketHandle* /* handle */, const WebString& message) = 0;

    // Called when data are received.
    virtual void didReceiveData(WebSocketHandle*, bool fin, WebSocketHandle::MessageType, const char* data, size_t /* size */) = 0;

    // Called when the handle is closed.
    // |handle| becomes unavailable once this notification arrives.
    virtual void didClose(WebSocketHandle* /* handle */, bool wasClean, unsigned short code, const WebString& reason) = 0;

    virtual void didReceiveFlowControl(WebSocketHandle*, int64_t quota) = 0;

    // Called when the browser receives a Close frame from the remote
    // server. Not called when the renderer initiates the closing handshake.
    virtual void didStartClosingHandshake(WebSocketHandle*) = 0;
};

} // namespace blink

#endif // WebSocketHandleClient_h
