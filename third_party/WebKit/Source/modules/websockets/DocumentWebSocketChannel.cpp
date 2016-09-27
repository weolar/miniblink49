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
#include "modules/websockets/DocumentWebSocketChannel.h"

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/Document.h"
#include "core/dom/ExecutionContext.h"
#include "core/fetch/UniqueIdentifier.h"
#include "core/fileapi/FileReaderLoader.h"
#include "core/fileapi/FileReaderLoaderClient.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/MixedContentChecker.h"
#include "modules/websockets/InspectorWebSocketEvents.h"
#include "modules/websockets/WebSocketChannelClient.h"
#include "modules/websockets/WebSocketFrame.h"
#include "platform/Logging.h"
#include "platform/network/WebSocketHandshakeRequest.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "public/platform/WebSerializedOrigin.h"
#include "public/platform/WebSocketHandshakeRequestInfo.h"
#include "public/platform/WebSocketHandshakeResponseInfo.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebVector.h"

using blink::WebSocketHandle;

namespace blink {

class DocumentWebSocketChannel::BlobLoader final : public GarbageCollectedFinalized<DocumentWebSocketChannel::BlobLoader>, public FileReaderLoaderClient {
public:
    BlobLoader(PassRefPtr<BlobDataHandle>, DocumentWebSocketChannel*);
    ~BlobLoader() override { }

    void cancel();

    // FileReaderLoaderClient functions.
    void didStartLoading() override { }
    void didReceiveData() override { }
    void didFinishLoading() override;
    void didFail(FileError::ErrorCode) override;

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_channel);
    }

private:
    Member<DocumentWebSocketChannel> m_channel;
    FileReaderLoader m_loader;
};

DocumentWebSocketChannel::BlobLoader::BlobLoader(PassRefPtr<BlobDataHandle> blobDataHandle, DocumentWebSocketChannel* channel)
    : m_channel(channel)
    , m_loader(FileReaderLoader::ReadAsArrayBuffer, this)
{
    m_loader.start(channel->executionContext(), blobDataHandle);
}

void DocumentWebSocketChannel::BlobLoader::cancel()
{
    m_loader.cancel();
    // didFail will be called immediately.
    // |this| is deleted here.
}

void DocumentWebSocketChannel::BlobLoader::didFinishLoading()
{
    m_channel->didFinishLoadingBlob(m_loader.arrayBufferResult());
    // |this| is deleted here.
}

void DocumentWebSocketChannel::BlobLoader::didFail(FileError::ErrorCode errorCode)
{
    m_channel->didFailLoadingBlob(errorCode);
    // |this| is deleted here.
}

DocumentWebSocketChannel::DocumentWebSocketChannel(ExecutionContext* context, WebSocketChannelClient* client, const String& sourceURL, unsigned lineNumber, WebSocketHandle *handle)
    : ContextLifecycleObserver(context)
    , m_handle(adoptPtr(handle ? handle : Platform::current()->createWebSocketHandle()))
    , m_client(client)
    , m_identifier(0)
    , m_sendingQuota(0)
    , m_receivedDataSizeForFlowControl(receivedDataSizeForFlowControlHighWaterMark * 2) // initial quota
    , m_sentSizeOfTopMessage(0)
    , m_sourceURLAtConstruction(sourceURL)
    , m_lineNumberAtConstruction(lineNumber)
{
    if (context->isDocument())
        m_identifier = createUniqueIdentifier();
}

DocumentWebSocketChannel::~DocumentWebSocketChannel()
{
    ASSERT(!m_blobLoader);
}

bool DocumentWebSocketChannel::connect(const KURL& url, const String& protocol)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p connect()", this);
    if (!m_handle)
        return false;

    if (executionContext()->isDocument() && document()->frame()) {
        if (MixedContentChecker::shouldBlockWebSocket(document()->frame(), url))
            return false;
    }
    if (MixedContentChecker::isMixedContent(document()->securityOrigin(), url)) {
        String message = "Connecting to a non-secure WebSocket server from a secure origin is deprecated.";
        document()->addConsoleMessage(ConsoleMessage::create(JSMessageSource, WarningMessageLevel, message));
    }

    m_url = url;
    Vector<String> protocols;
    // Avoid placing an empty token in the Vector when the protocol string is
    // empty.
    if (!protocol.isEmpty()) {
        // Since protocol is already verified and escaped, we can simply split
        // it.
        protocol.split(", ", true, protocols);
    }
    WebVector<WebString> webProtocols(protocols.size());
    for (size_t i = 0; i < protocols.size(); ++i) {
        webProtocols[i] = protocols[i];
    }

    if (executionContext()->isDocument() && document()->frame())
        document()->frame()->loader().client()->dispatchWillOpenWebSocket(m_handle.get());
    m_handle->connect(url, webProtocols, *executionContext()->securityOrigin(), this);

    flowControlIfNecessary();
    if (m_identifier) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "WebSocketCreate", TRACE_EVENT_SCOPE_THREAD, "data", InspectorWebSocketCreateEvent::data(document(), m_identifier, url, protocol));
        InspectorInstrumentation::didCreateWebSocket(document(), m_identifier, url, protocol);
    }
    return true;
}

void DocumentWebSocketChannel::send(const CString& message)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p sendText(%s)", this, message.data());
    if (m_identifier) {
        // FIXME: Change the inspector API to show the entire message instead
        // of individual frames.
        InspectorInstrumentation::didSendWebSocketFrame(document(), m_identifier, WebSocketFrame::OpCodeText, true, message.data(), message.length());
    }
    m_messages.append(adoptPtr(new Message(message)));
    processSendQueue();
}

void DocumentWebSocketChannel::send(PassRefPtr<BlobDataHandle> blobDataHandle)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p sendBlob(%s, %s, %llu)", this, blobDataHandle->uuid().utf8().data(), blobDataHandle->type().utf8().data(), blobDataHandle->size());
    if (m_identifier) {
        // FIXME: Change the inspector API to show the entire message instead
        // of individual frames.
        // FIXME: We can't access the data here.
        // Since Binary data are not displayed in Inspector, this does not
        // affect actual behavior.
        InspectorInstrumentation::didSendWebSocketFrame(document(), m_identifier, WebSocketFrame::OpCodeBinary, true, "", 0);
    }
    m_messages.append(adoptPtr(new Message(blobDataHandle)));
    processSendQueue();
}

void DocumentWebSocketChannel::send(const DOMArrayBuffer& buffer, unsigned byteOffset, unsigned byteLength)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p sendArrayBuffer(%p, %u, %u)", this, buffer.data(), byteOffset, byteLength);
    if (m_identifier) {
        // FIXME: Change the inspector API to show the entire message instead
        // of individual frames.
        InspectorInstrumentation::didSendWebSocketFrame(document(), m_identifier, WebSocketFrame::OpCodeBinary, true, static_cast<const char*>(buffer.data()) + byteOffset, byteLength);
    }
    // buffer.slice copies its contents.
    // FIXME: Reduce copy by sending the data immediately when we don't need to
    // queue the data.
    m_messages.append(adoptPtr(new Message(buffer.slice(byteOffset, byteOffset + byteLength))));
    processSendQueue();
}

void DocumentWebSocketChannel::sendTextAsCharVector(PassOwnPtr<Vector<char>> data)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p sendTextAsCharVector(%p, %llu)", this, data.get(), static_cast<unsigned long long>(data->size()));
    if (m_identifier) {
        // FIXME: Change the inspector API to show the entire message instead
        // of individual frames.
        InspectorInstrumentation::didSendWebSocketFrame(document(), m_identifier, WebSocketFrame::OpCodeText, true, data->data(), data->size());
    }
    m_messages.append(adoptPtr(new Message(data, MessageTypeTextAsCharVector)));
    processSendQueue();
}

void DocumentWebSocketChannel::sendBinaryAsCharVector(PassOwnPtr<Vector<char>> data)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p sendBinaryAsCharVector(%p, %llu)", this, data.get(), static_cast<unsigned long long>(data->size()));
    if (m_identifier) {
        // FIXME: Change the inspector API to show the entire message instead
        // of individual frames.
        InspectorInstrumentation::didSendWebSocketFrame(document(), m_identifier, WebSocketFrame::OpCodeBinary, true, data->data(), data->size());
    }
    m_messages.append(adoptPtr(new Message(data, MessageTypeBinaryAsCharVector)));
    processSendQueue();
}

void DocumentWebSocketChannel::close(int code, const String& reason)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p close(%d, %s)", this, code, reason.utf8().data());
    ASSERT(m_handle);
    unsigned short codeToSend = static_cast<unsigned short>(code == CloseEventCodeNotSpecified ? CloseEventCodeNoStatusRcvd : code);
    m_messages.append(adoptPtr(new Message(codeToSend, reason)));
    processSendQueue();
}

void DocumentWebSocketChannel::fail(const String& reason, MessageLevel level, const String& sourceURL, unsigned lineNumber)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p fail(%s)", this, reason.utf8().data());
    // m_handle and m_client can be null here.

    if (m_identifier)
        InspectorInstrumentation::didReceiveWebSocketFrameError(document(), m_identifier, reason);
    const String message = "WebSocket connection to '" + m_url.elidedString() + "' failed: " + reason;
    executionContext()->addConsoleMessage(ConsoleMessage::create(JSMessageSource, level, message, sourceURL, lineNumber));

    if (m_client)
        m_client->didError();
    // |reason| is only for logging and should not be provided for scripts,
    // hence close reason must be empty.
    handleDidClose(false, CloseEventCodeAbnormalClosure, String());
    // handleDidClose may delete this object.
}

void DocumentWebSocketChannel::disconnect()
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p disconnect()", this);
    if (m_identifier) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "WebSocketDestroy", TRACE_EVENT_SCOPE_THREAD, "data", InspectorWebSocketEvent::data(document(), m_identifier));
        InspectorInstrumentation::didCloseWebSocket(document(), m_identifier);
    }
    abortAsyncOperations();
    m_handle.clear();
    m_client = nullptr;
    m_identifier = 0;
}

DocumentWebSocketChannel::Message::Message(const CString& text)
    : type(MessageTypeText)
    , text(text) { }

DocumentWebSocketChannel::Message::Message(PassRefPtr<BlobDataHandle> blobDataHandle)
    : type(MessageTypeBlob)
    , blobDataHandle(blobDataHandle) { }

DocumentWebSocketChannel::Message::Message(PassRefPtr<DOMArrayBuffer> arrayBuffer)
    : type(MessageTypeArrayBuffer)
    , arrayBuffer(arrayBuffer) { }

DocumentWebSocketChannel::Message::Message(PassOwnPtr<Vector<char>> vectorData, MessageType type)
    : type(type)
    , vectorData(vectorData)
{
    ASSERT(type == MessageTypeTextAsCharVector || type == MessageTypeBinaryAsCharVector);
}

DocumentWebSocketChannel::Message::Message(unsigned short code, const String& reason)
    : type(MessageTypeClose)
    , code(code)
    , reason(reason) { }

void DocumentWebSocketChannel::sendInternal(WebSocketHandle::MessageType messageType, const char* data, size_t totalSize, uint64_t* consumedBufferedAmount)
{
    WebSocketHandle::MessageType frameType =
        m_sentSizeOfTopMessage ? WebSocketHandle::MessageTypeContinuation : messageType;
    ASSERT(totalSize >= m_sentSizeOfTopMessage);
    // The first cast is safe since the result of min() never exceeds
    // the range of size_t. The second cast is necessary to compile
    // min() on ILP32.
    size_t size = static_cast<size_t>(std::min(m_sendingQuota, static_cast<uint64_t>(totalSize - m_sentSizeOfTopMessage)));
    bool final = (m_sentSizeOfTopMessage + size == totalSize);

    m_handle->send(final, frameType, data + m_sentSizeOfTopMessage, size);

    m_sentSizeOfTopMessage += size;
    m_sendingQuota -= size;
    *consumedBufferedAmount += size;

    if (final) {
        m_messages.removeFirst();
        m_sentSizeOfTopMessage = 0;
    }
}

void DocumentWebSocketChannel::processSendQueue()
{
    ASSERT(m_handle);
    uint64_t consumedBufferedAmount = 0;
    while (!m_messages.isEmpty() && !m_blobLoader) {
        Message* message = m_messages.first().get();
        if (m_sendingQuota == 0 && message->type != MessageTypeClose)
            break;
        switch (message->type) {
        case MessageTypeText:
            sendInternal(WebSocketHandle::MessageTypeText, message->text.data(), message->text.length(), &consumedBufferedAmount);
            break;
        case MessageTypeBlob:
            ASSERT(!m_blobLoader);
            m_blobLoader = new BlobLoader(message->blobDataHandle, this);
            break;
        case MessageTypeArrayBuffer:
            sendInternal(WebSocketHandle::MessageTypeBinary, static_cast<const char*>(message->arrayBuffer->data()), message->arrayBuffer->byteLength(), &consumedBufferedAmount);
            break;
        case MessageTypeTextAsCharVector:
            sendInternal(WebSocketHandle::MessageTypeText, message->vectorData->data(), message->vectorData->size(), &consumedBufferedAmount);
            break;
        case MessageTypeBinaryAsCharVector:
            sendInternal(WebSocketHandle::MessageTypeBinary, message->vectorData->data(), message->vectorData->size(), &consumedBufferedAmount);
            break;
        case MessageTypeClose: {
            // No message should be sent from now on.
            ASSERT(m_messages.size() == 1);
            ASSERT(m_sentSizeOfTopMessage == 0);
            m_handle->close(message->code, message->reason);
            m_messages.removeFirst();
            break;
        }
        }
    }
    if (m_client && consumedBufferedAmount > 0)
        m_client->didConsumeBufferedAmount(consumedBufferedAmount);
}

void DocumentWebSocketChannel::flowControlIfNecessary()
{
    if (!m_handle || m_receivedDataSizeForFlowControl < receivedDataSizeForFlowControlHighWaterMark) {
        return;
    }
    m_handle->flowControl(m_receivedDataSizeForFlowControl);
    m_receivedDataSizeForFlowControl = 0;
}

void DocumentWebSocketChannel::abortAsyncOperations()
{
    if (m_blobLoader) {
        m_blobLoader->cancel();
        m_blobLoader.clear();
    }
}

void DocumentWebSocketChannel::handleDidClose(bool wasClean, unsigned short code, const String& reason)
{
    m_handle.clear();
    abortAsyncOperations();
    if (!m_client) {
        return;
    }
    WebSocketChannelClient* client = m_client;
    m_client = nullptr;
    WebSocketChannelClient::ClosingHandshakeCompletionStatus status =
        wasClean ? WebSocketChannelClient::ClosingHandshakeComplete : WebSocketChannelClient::ClosingHandshakeIncomplete;
    client->didClose(status, code, reason);
    // client->didClose may delete this object.
}

Document* DocumentWebSocketChannel::document()
{
    ASSERT(m_identifier);
    ExecutionContext* context = executionContext();
    ASSERT(context->isDocument());
    return toDocument(context);
}

void DocumentWebSocketChannel::didConnect(WebSocketHandle* handle, const WebString& selectedProtocol, const WebString& extensions)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p didConnect(%p, %s, %s)", this, handle, selectedProtocol.utf8().c_str(), extensions.utf8().c_str());

    ASSERT(m_handle);
    ASSERT(handle == m_handle);
    ASSERT(m_client);

    m_client->didConnect(selectedProtocol, extensions);
}

void DocumentWebSocketChannel::didStartOpeningHandshake(WebSocketHandle* handle, const WebSocketHandshakeRequestInfo& request)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p didStartOpeningHandshake(%p)", this, handle);

    ASSERT(m_handle);
    ASSERT(handle == m_handle);

    if (m_identifier) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "WebSocketSendHandshakeRequest", TRACE_EVENT_SCOPE_THREAD, "data", InspectorWebSocketEvent::data(document(), m_identifier));
        InspectorInstrumentation::willSendWebSocketHandshakeRequest(document(), m_identifier, &request.toCoreRequest());
        m_handshakeRequest = WebSocketHandshakeRequest::create(request.toCoreRequest());
    }
}

void DocumentWebSocketChannel::didFinishOpeningHandshake(WebSocketHandle* handle, const WebSocketHandshakeResponseInfo& response)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p didFinishOpeningHandshake(%p)", this, handle);

    ASSERT(m_handle);
    ASSERT(handle == m_handle);

    if (m_identifier) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "WebSocketReceiveHandshakeResponse", TRACE_EVENT_SCOPE_THREAD, "data", InspectorWebSocketEvent::data(document(), m_identifier));
        InspectorInstrumentation::didReceiveWebSocketHandshakeResponse(document(), m_identifier, m_handshakeRequest.get(), &response.toCoreResponse());
    }
    m_handshakeRequest.clear();
}

void DocumentWebSocketChannel::didFail(WebSocketHandle* handle, const WebString& message)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p didFail(%p, %s)", this, handle, message.utf8().data());

    ASSERT(m_handle);
    ASSERT(handle == m_handle);

    // This function is called when the browser is required to fail the
    // WebSocketConnection. Hence we fail this channel by calling
    // |this->failAsError| function.
    failAsError(message);
    // |this| may be deleted.
}

void DocumentWebSocketChannel::didReceiveData(WebSocketHandle* handle, bool fin, WebSocketHandle::MessageType type, const char* data, size_t size)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p didReceiveData(%p, %d, %d, (%p, %zu))", this, handle, fin, type, data, size);

    ASSERT(m_handle);
    ASSERT(handle == m_handle);
    ASSERT(m_client);
    // Non-final frames cannot be empty.
    ASSERT(fin || size);

    switch (type) {
    case WebSocketHandle::MessageTypeText:
        ASSERT(m_receivingMessageData.isEmpty());
        m_receivingMessageTypeIsText = true;
        break;
    case WebSocketHandle::MessageTypeBinary:
        ASSERT(m_receivingMessageData.isEmpty());
        m_receivingMessageTypeIsText = false;
        break;
    case WebSocketHandle::MessageTypeContinuation:
        ASSERT(!m_receivingMessageData.isEmpty());
        break;
    }

    m_receivingMessageData.append(data, size);
    m_receivedDataSizeForFlowControl += size;
    flowControlIfNecessary();
    if (!fin) {
        return;
    }
    if (m_identifier) {
        // FIXME: Change the inspector API to show the entire message instead
        // of individual frames.
        WebSocketFrame::OpCode opcode = m_receivingMessageTypeIsText ? WebSocketFrame::OpCodeText : WebSocketFrame::OpCodeBinary;
        WebSocketFrame frame(opcode, m_receivingMessageData.data(), m_receivingMessageData.size(), WebSocketFrame::Final);
        InspectorInstrumentation::didReceiveWebSocketFrame(document(), m_identifier, frame.opCode, frame.masked, frame.payload, frame.payloadLength);
    }
    if (m_receivingMessageTypeIsText) {
        String message = m_receivingMessageData.isEmpty() ? emptyString() : String::fromUTF8(m_receivingMessageData.data(), m_receivingMessageData.size());
        m_receivingMessageData.clear();
        if (message.isNull()) {
            failAsError("Could not decode a text frame as UTF-8.");
            // failAsError may delete this object.
        } else {
            m_client->didReceiveTextMessage(message);
        }
    } else {
        OwnPtr<Vector<char>> binaryData = adoptPtr(new Vector<char>);
        binaryData->swap(m_receivingMessageData);
        m_client->didReceiveBinaryMessage(binaryData.release());
    }
}

void DocumentWebSocketChannel::didClose(WebSocketHandle* handle, bool wasClean, unsigned short code, const WebString& reason)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p didClose(%p, %d, %u, %s)", this, handle, wasClean, code, String(reason).utf8().data());

    ASSERT(m_handle);
    ASSERT(handle == m_handle);

    m_handle.clear();

    if (m_identifier) {
        TRACE_EVENT_INSTANT1("devtools.timeline", "WebSocketDestroy", TRACE_EVENT_SCOPE_THREAD, "data", InspectorWebSocketEvent::data(document(), m_identifier));
        InspectorInstrumentation::didCloseWebSocket(document(), m_identifier);
        m_identifier = 0;
    }

    handleDidClose(wasClean, code, reason);
    // handleDidClose may delete this object.
}

void DocumentWebSocketChannel::didReceiveFlowControl(WebSocketHandle* handle, int64_t quota)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p didReceiveFlowControl(%p, %ld)", this, handle, static_cast<long>(quota));

    ASSERT(m_handle);
    ASSERT(handle == m_handle);
    ASSERT(quota >= 0);

    m_sendingQuota += quota;
    processSendQueue();
}

void DocumentWebSocketChannel::didStartClosingHandshake(WebSocketHandle* handle)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p didStartClosingHandshake(%p)", this, handle);

    ASSERT(m_handle);
    ASSERT(handle == m_handle);

    if (m_client)
        m_client->didStartClosingHandshake();
}

void DocumentWebSocketChannel::didFinishLoadingBlob(PassRefPtr<DOMArrayBuffer> buffer)
{
    m_blobLoader.clear();
    ASSERT(m_handle);
    // The loaded blob is always placed on m_messages[0].
    ASSERT(m_messages.size() > 0 && m_messages.first()->type == MessageTypeBlob);
    // We replace it with the loaded blob.
    m_messages.first() = adoptPtr(new Message(buffer));
    processSendQueue();
}

void DocumentWebSocketChannel::didFailLoadingBlob(FileError::ErrorCode errorCode)
{
    m_blobLoader.clear();
    if (errorCode == FileError::ABORT_ERR) {
        // The error is caused by cancel().
        return;
    }
    // FIXME: Generate human-friendly reason message.
    failAsError("Failed to load Blob: error code = " + String::number(errorCode));
    // |this| can be deleted here.
}

DEFINE_TRACE(DocumentWebSocketChannel)
{
    visitor->trace(m_blobLoader);
    visitor->trace(m_client);
    WebSocketChannel::trace(visitor);
    ContextLifecycleObserver::trace(visitor);
}

} // namespace blink
