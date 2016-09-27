/*
 * Copyright (C) 2011, 2012 Google Inc. All rights reserved.
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
#include "web/WebSocketImpl.h"

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/Document.h"
#include "core/frame/ConsoleTypes.h"
#include "modules/websockets/DocumentWebSocketChannel.h"
#include "modules/websockets/WebSocketChannel.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/web/WebArrayBuffer.h"
#include "public/web/WebDocument.h"
#include "web/WebSocketChannelClientProxy.h"
#include "wtf/text/CString.h"
#include "wtf/text/WTFString.h"

namespace blink {

WebSocketImpl::WebSocketImpl(const WebDocument& document, WebSocketClient* client)
    : m_client(client)
    , m_channelProxy(WebSocketChannelClientProxy::create(this))
    , m_binaryType(BinaryTypeBlob)
    , m_isClosingOrClosed(false)
    , m_bufferedAmount(0)
    , m_bufferedAmountAfterClose(0)
{
    RefPtrWillBeRawPtr<Document> coreDocument = PassRefPtrWillBeRawPtr<Document>(document);
    m_private = DocumentWebSocketChannel::create(coreDocument.get(), m_channelProxy.get());
}

WebSocketImpl::~WebSocketImpl()
{
    m_private->disconnect();
}

WebSocket::BinaryType WebSocketImpl::binaryType() const
{
    return m_binaryType;
}

bool WebSocketImpl::setBinaryType(BinaryType binaryType)
{
    if (binaryType > BinaryTypeArrayBuffer)
        return false;
    m_binaryType = binaryType;
    return true;
}

void WebSocketImpl::connect(const WebURL& url, const WebString& protocol)
{
    m_private->connect(url, protocol);
}

WebString WebSocketImpl::subprotocol()
{
    return m_subprotocol;
}

WebString WebSocketImpl::extensions()
{
    return m_extensions;
}

bool WebSocketImpl::sendText(const WebString& message)
{
    String coreMessage = message;
    CString encodedMessage = coreMessage.utf8();
    size_t size = encodedMessage.length();
    m_bufferedAmount += size;
    if (m_isClosingOrClosed)
        m_bufferedAmountAfterClose += size;

    // FIXME: Deprecate this call.
    m_client->didUpdateBufferedAmount(m_bufferedAmount);

    if (m_isClosingOrClosed)
        return true;

    m_private->send(encodedMessage);
    return true;
}

bool WebSocketImpl::sendArrayBuffer(const WebArrayBuffer& webArrayBuffer)
{
    size_t size = webArrayBuffer.byteLength();
    m_bufferedAmount += size;
    if (m_isClosingOrClosed)
        m_bufferedAmountAfterClose += size;

    // FIXME: Deprecate this call.
    m_client->didUpdateBufferedAmount(m_bufferedAmount);

    if (m_isClosingOrClosed)
        return true;

    RefPtr<DOMArrayBuffer> arrayBuffer = PassRefPtr<DOMArrayBuffer>(webArrayBuffer);
    m_private->send(*arrayBuffer, 0, arrayBuffer->byteLength());
    return true;
}

unsigned long WebSocketImpl::bufferedAmount() const
{
    return m_bufferedAmount;
}

void WebSocketImpl::close(int code, const WebString& reason)
{
    m_isClosingOrClosed = true;
    m_private->close(code, reason);
}

void WebSocketImpl::fail(const WebString& reason)
{
    m_private->fail(reason, ErrorMessageLevel, String(), 0);
}

void WebSocketImpl::disconnect()
{
    m_private->disconnect();
    m_client = nullptr;
}

void WebSocketImpl::didConnect(const String& subprotocol, const String& extensions)
{
    m_client->didConnect(subprotocol, extensions);

    // FIXME: Deprecate these statements.
    m_subprotocol = subprotocol;
    m_extensions = extensions;
    m_client->didConnect();
}

void WebSocketImpl::didReceiveTextMessage(const String& payload)
{
    m_client->didReceiveMessage(WebString(payload));
}

void WebSocketImpl::didReceiveBinaryMessage(PassOwnPtr<Vector<char>> payload)
{
    switch (m_binaryType) {
    case BinaryTypeBlob:
        // FIXME: Handle Blob after supporting WebBlob.
        break;
    case BinaryTypeArrayBuffer:
        m_client->didReceiveArrayBuffer(WebArrayBuffer(DOMArrayBuffer::create(payload->data(), payload->size())));
        break;
    }
}

void WebSocketImpl::didError()
{
    m_client->didReceiveMessageError();
}

void WebSocketImpl::didConsumeBufferedAmount(unsigned long consumed)
{
    m_client->didConsumeBufferedAmount(consumed);

    // FIXME: Deprecate the following statements.
    m_bufferedAmount -= consumed;
    m_client->didUpdateBufferedAmount(m_bufferedAmount);
}

void WebSocketImpl::didStartClosingHandshake()
{
    m_client->didStartClosingHandshake();
}

void WebSocketImpl::didClose(WebSocketChannelClient::ClosingHandshakeCompletionStatus status, unsigned short code, const String& reason)
{
    m_isClosingOrClosed = true;
    m_client->didClose(static_cast<WebSocketClient::ClosingHandshakeCompletionStatus>(status), code, WebString(reason));

    // FIXME: Deprecate this call.
    m_client->didClose(m_bufferedAmount - m_bufferedAmountAfterClose, static_cast<WebSocketClient::ClosingHandshakeCompletionStatus>(status), code, WebString(reason));
}

} // namespace blink
