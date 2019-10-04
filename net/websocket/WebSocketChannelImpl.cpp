/*
 * Copyright (C) 2011, 2012 Google Inc.  All rights reserved.
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

#include "net/websocket/WebSocketChannelImpl.h"

#include "net/websocket/SocketStreamError.h"
#include "net/websocket/SocketStreamHandle.h"
#include "net/websocket/WebSocketHandshake.h"
#include "net/ActivatingObjCheck.h"
#include "third_party/WebKit/Source/core/fileapi/Blob.h"
#include "third_party/WebKit/Source/core/fileapi/FileError.h"
#include "third_party/WebKit/Source/core/fileapi/FileReaderLoader.h"
#include "third_party/WebKit/Source/core/loader/CookieJar.h"
#include "third_party/WebKit/Source/core/dom/Document.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "third_party/WebKit/Source/core/frame/Settings.h"
#include "third_party/WebKit/Source/core/page/Page.h"
#include "third_party/WebKit/Source/core/fetch/UniqueIdentifier.h"
#include "third_party/WebKit/Source/core/inspector/ConsoleMessage.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/Logging.h"
#include "third_party/WebKit/public/platform/WebSocketHandle.h"
#include "third_party/WebKit/Source/modules/websockets/WebSocketChannelClient.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "third_party/WebKit/Source/wtf/Deque.h"
#include "third_party/WebKit/Source/wtf/FastMalloc.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/StringHash.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "content/browser/WebPageImpl.h"
#include "content/browser/WebPage.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#include "wke/wkeWebView.h"
#include "wke/wkeString.h"
#include "wke/wkeGlobalVar.h"
#endif

using namespace blink;

namespace net {

const double TCPMaximumSegmentLifetime = 2 * 60.0;

class WebSocketChannelImpl::BlobLoader final : public GarbageCollectedFinalized<WebSocketChannelImpl::BlobLoader>, public FileReaderLoaderClient{
public:
    BlobLoader(PassRefPtr<BlobDataHandle>, WebSocketChannelImpl*);
    ~BlobLoader() override { }

    void cancel();

    // FileReaderLoaderClient functions.
    void didStartLoading() override { }
    void didReceiveData() override { }
    void didFinishLoading() override;
    void didFail(FileError::ErrorCode) override;

    PassRefPtr<DOMArrayBuffer> arrayBufferResult() const
    {
        return m_loader.arrayBufferResult();
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_channel);
    }

private:
    Member<WebSocketChannelImpl> m_channel;
    FileReaderLoader m_loader;
};

WebSocketChannelImpl::BlobLoader::BlobLoader(PassRefPtr<BlobDataHandle> blobDataHandle, WebSocketChannelImpl* channel)
    : m_channel(channel)
    , m_loader(FileReaderLoader::ReadAsArrayBuffer, this)
{
    m_loader.start(channel->executionContext(), blobDataHandle);
}

void WebSocketChannelImpl::BlobLoader::cancel()
{
    m_loader.cancel();
    // didFail will be called immediately.
    // |this| is deleted here.
}

void WebSocketChannelImpl::BlobLoader::didFinishLoading()
{
    m_channel->didFinishLoadingBlob(m_loader.arrayBufferResult());
    // |this| is deleted here.
}

void WebSocketChannelImpl::BlobLoader::didFail(FileError::ErrorCode errorCode)
{
    m_channel->didFailLoadingBlob(errorCode);
    // |this| is deleted here.
}

struct WebsocketHooks {
    void* m_hookUserParam;
    bool(WKE_CALL_TYPE* m_onConnect)(wkeWebView webView, void* param, WebSocketChannelImpl* job);
    bool(WKE_CALL_TYPE* m_onReceive)(wkeWebView webView, void* param, WebSocketChannelImpl* job, int OpCode, const char* buf, size_t len, wkeString new_data);
    bool(WKE_CALL_TYPE* m_onSend)(wkeWebView webView, void* param, WebSocketChannelImpl* job, int OpCode, const char* buf, size_t len, wkeString new_data);
    void(WKE_CALL_TYPE* m_onError)(wkeWebView webView, void* param, WebSocketChannelImpl* job);
};

void WSCI_setHook(void* j, void *isHook)
{
    if (!j || !isHook)
        return;
    WebsocketHooks* hookPtr = (WebsocketHooks*)isHook;
    WebSocketChannelImpl* job = (WebSocketChannelImpl*)j;
    job->m_hookUserParam = hookPtr->m_hookUserParam;
    job->m_onConnect = hookPtr->m_onConnect;
    job->m_onReceive = hookPtr->m_onReceive;
    job->m_onSend = hookPtr->m_onSend;
    job->m_onError = hookPtr->m_onError;
}

void WSCI_sendText(void* j, char* buf, size_t len)
{
    if (j && buf && len) {
        WebSocketChannelImpl* job = (WebSocketChannelImpl*)j;
        job->send(CString(buf, len), true);
    }
}

void WSCI_sendBlob(void* j, char* buf, size_t len)
{
    if (j && buf && len) {
        WebSocketChannelImpl* job = (WebSocketChannelImpl*)j;
        job->send(buf, len, true);
    }
}

WebSocketChannelImpl::WebSocketChannelImpl(ExecutionContext* context, WebSocketChannelClient* client, const String& sourceURL, unsigned lineNumber, WebSocketHandle* handle)
    : ContextLifecycleObserver(context)
    , m_ref(0)
    , m_client(client)
    , m_resumeTimer(this, &WebSocketChannelImpl::resumeTimerFired)
    , m_suspended(false)
    , m_closing(false)
    , m_receivedClosingHandshake(false)
    , m_closingTimer(this, &WebSocketChannelImpl::closingTimerFired)
    , m_closed(false)
    , m_shouldDiscardReceivedData(false)
    , m_unhandledBufferedAmount(0)
    , m_id(0)
    , m_hasContinuousFrame(false)
    , m_closeEventCode(CloseEventCodeAbnormalClosure)
    , m_outgoingFrameQueueStatus(OutgoingFrameQueueOpen)
    , m_blobLoaderStatus(BlobLoaderNotStarted)
    , m_sourceURLAtConstruction(sourceURL)
    , m_lineNumberAtConstruction(lineNumber)
    , m_isClosing(false)
    , m_hookUserParam(nullptr)
    , m_onConnect(nullptr)
    , m_onReceive(nullptr)
    , m_onSend(nullptr)
    , m_onError(nullptr)
{
    m_id = ActivatingObjCheck::inst()->genId();
    ActivatingObjCheck::inst()->add((intptr_t)m_id);
    m_handleId = 0;

    WTF_LOG(Network, "WebSocketChannelImpl %p ctor, identifier %lu", this, m_id);
}

WebSocketChannelImpl::~WebSocketChannelImpl()
{
    ASSERT(!m_blobLoader);
    //ASSERT(0 == m_ref);
    WTF_LOG(Network, "WebSocketChannelImpl %p dtor", this);
    ActivatingObjCheck::inst()->remove((intptr_t)m_id);
}

Document* WebSocketChannelImpl::document()
{
    ASSERT(m_id);
    ExecutionContext* context = executionContext();
    ASSERT(context->isDocument());
    return toDocument(context);
}

typedef bool(WKE_CALL_TYPE* wkeWebSocketCallback)(wkeWebView webView, void* param, void *job, wkeString url);

bool WebSocketChannelImpl::connect(const KURL& url, const String& protocol)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p connect()", this);
    ASSERT(!m_handle);
    ASSERT(!m_suspended);
    KURL kurl = url;
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    WebViewImpl* view = (WebViewImpl*)document()->page()->chromeClient().webView();
    if (view) {
        content::WebPageImpl* pageImpl = (content::WebPageImpl*)view->client();
        if (pageImpl) {
            content::WebPage* page = pageImpl->m_pagePtr;
            if (page && page->wkeHandler().wsCallback) {
                wke::CString u(url.string());
                if (((wkeWebSocketCallback)(page->wkeHandler().wsCallback))(page->wkeWebView(), page->wkeHandler().wsCallbackParam, this, &u)) {
                    kurl = KURL(blink::ParsedURLString, u.string());
                }
            }
        }
    }
#endif
    m_handshake = adoptPtr(new WebSocketHandshake(kurl, protocol, document()));
    m_handshake->reset();
    if (m_deflateFramer.canDeflate())
        m_handshake->addExtensionProcessor(m_deflateFramer.createExtensionProcessor());
    if (m_id)
        InspectorInstrumentation::didCreateWebSocket(document(), m_id, kurl, protocol);

    if (Frame* frame = document()->frame()) {
        ref();
        m_handle = SocketStreamHandle::create(m_handshake->url(), this);
        m_handleId = m_handle->getId();
    }
    return true;
}

void WebSocketChannelImpl::send(const CString& message)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p send() Sending String '%s'", this, message.data());
    enqueueTextFrame(message, false);
    processOutgoingFrameQueue();
    // According to WebSocket API specification, WebSocket.send() should return void instead
    // of boolean. However, our implementation still returns boolean due to compatibility
    // concern (see bug 65850).
    // m_channel->send() may happen later, thus it's not always possible to know whether
    // the message has been sent to the socket successfully. In this case, we have no choice
    // but to return true.
}

void WebSocketChannelImpl::send(const CString& message, bool isHook)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p send() Sending String '%s'", this, message.data());
    enqueueTextFrame(message, isHook);
    processOutgoingFrameQueue();
    // According to WebSocket API specification, WebSocket.send() should return void instead
    // of boolean. However, our implementation still returns boolean due to compatibility
    // concern (see bug 65850).
    // m_channel->send() may happen later, thus it's not always possible to know whether
    // the message has been sent to the socket successfully. In this case, we have no choice
    // but to return true.
}

void WebSocketChannelImpl::send(const DOMArrayBuffer& binaryData, unsigned byteOffset, unsigned byteLength)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p send() Sending ArrayBuffer %p byteOffset=%u byteLength=%u", this, &binaryData, byteOffset, byteLength);
    enqueueRawFrame(WebSocketOneFrame::OpCodeBinary, static_cast<const char*>(binaryData.data()) + byteOffset, byteLength, false);
    processOutgoingFrameQueue();
}

void WebSocketChannelImpl::send(PassRefPtr<BlobDataHandle> blobDataHandle)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p sendBlob(%s, %s, %llu)", this, blobDataHandle->uuid().utf8().data(), blobDataHandle->type().utf8().data(), blobDataHandle->size());
    enqueueBlobFrame(WebSocketOneFrame::OpCodeBinary, blobDataHandle);
    processOutgoingFrameQueue();
}

bool WebSocketChannelImpl::send(const char* data, int length, bool isHook)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p send() Sending char* data=%p length=%d", this, data, length);
    enqueueRawFrame(WebSocketOneFrame::OpCodeBinary, data, length, isHook);
    processOutgoingFrameQueue();
    return true;
}

unsigned long WebSocketChannelImpl::bufferedAmount() const
{
    WTF_LOG(Network, "WebSocketChannelImpl %p bufferedAmount()", this);
    ASSERT(m_handle);
    ASSERT(!m_suspended);

    if (ActivatingObjCheck::inst()->isActivating((intptr_t)m_handleId))
        return m_handle->bufferedAmount();
    return 0;
}

void WebSocketChannelImpl::close(int code, const String& reason)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p close() code=%d reason='%s'", this, code, reason.utf8().data());
    ASSERT(!m_suspended);
    if (!m_handle)
        return;
    m_isClosing = true;
    RefPtr<WebSocketChannelImpl> protect(*this); // An attempt to send closing handshake may fail, which will get the channel closed and dereferenced.
    startClosingHandshake(code, reason);
    if (m_closing && !m_closingTimer.isActive())
        m_closingTimer.startOneShot(2 * TCPMaximumSegmentLifetime, FROM_HERE);
    m_isClosing = false;
}

void WebSocketChannelImpl::fail(const String& reason, MessageLevel, const String& sourceURL, unsigned lineNumber)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p fail() reason='%s'", this, reason.utf8().data());
    ASSERT(!m_suspended);
    if (document()) {
        InspectorInstrumentation::didReceiveWebSocketFrameError(document(), m_id, reason);

        String consoleMessage;
        if (m_handshake)
            consoleMessage = String::format("WebSocket connection to '%s %s %s", m_handshake->url().string().utf8().data(), "' failed: ", reason.utf8().data());
        else
            consoleMessage = String::format("WebSocket connection failed: %s", reason.utf8().data());
        
        document()->addConsoleMessage(ConsoleMessage::create(NetworkMessageSource, ErrorMessageLevel, consoleMessage));
    }
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (m_onError) {
        WebViewImpl* view = (WebViewImpl*)document()->page()->chromeClient().webView();
        if (view) {
            content::WebPageImpl* pageImpl = (content::WebPageImpl*)view->client();
            if (pageImpl) {
                content::WebPage* page = pageImpl->m_pagePtr;
                m_onError(page->wkeWebView(), m_hookUserParam, this);
            }
        }
    }
#endif
    // Hybi-10 specification explicitly states we must not continue to handle incoming data
    // once the WebSocket connection is failed (section 7.1.7).
    RefPtr<WebSocketChannelImpl> protect(*this); // The client can close the channel, potentially removing the last reference.
    m_shouldDiscardReceivedData = true;
    if (!m_buffer.isEmpty())
        skipBuffer(m_buffer.size()); // Save memory.
    m_deflateFramer.didFail();
    m_hasContinuousFrame = false;
    m_continuousFrameData.clear();
    m_client->didError();

    if (m_handle && !m_closed && ActivatingObjCheck::inst()->isActivating((intptr_t)m_handleId))
        m_handle->disconnect(); // Will call didClose().

    // We should be closed by now, but if we never got a handshake then we never even opened.
    ASSERT(m_closed || !m_handshake);
}

void WebSocketChannelImpl::disconnect()
{
    WTF_LOG(Network, "WebSocketChannelImpl %p disconnect()", this);
    if (m_id && document())
        InspectorInstrumentation::didCloseWebSocket(document(), m_id);
    if (m_handshake)
        m_handshake->clearScriptExecutionContext();
    m_client = nullptr;
    if (m_handle && ActivatingObjCheck::inst()->isActivating((intptr_t)m_handleId))
        m_handle->disconnect();
}

void WebSocketChannelImpl::sendTextAsCharVector(PassOwnPtr<Vector<char>> data)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p sendTextAsCharVector(%p, %llu)", this, data.get(), static_cast<unsigned long long>(data->size()));
    if (m_id) {
        // FIXME: Change the inspector API to show the entire message instead
        // of individual frames.
        InspectorInstrumentation::didSendWebSocketFrame(document(), m_id, WebSocketOneFrame::OpCodeText, true, data->data(), data->size());
    }
    send(data->data(), data->size());
}

void WebSocketChannelImpl::sendBinaryAsCharVector(PassOwnPtr<Vector<char>> data)
{
    WTF_LOG(Network, "DocumentWebSocketChannel %p sendBinaryAsCharVector(%p, %llu)", this, data.get(), static_cast<unsigned long long>(data->size()));
    if (m_id) {
        // FIXME: Change the inspector API to show the entire message instead
        // of individual frames.
        InspectorInstrumentation::didSendWebSocketFrame(document(), m_id, WebSocketOneFrame::OpCodeBinary, true, data->data(), data->size());
    }
    send(data->data(), data->size());
}

void WebSocketChannelImpl::suspend()
{
    m_suspended = true;
}

void WebSocketChannelImpl::resume()
{
    m_suspended = false;
    if ((!m_buffer.isEmpty() || m_closed) && m_client && !m_resumeTimer.isActive())
        m_resumeTimer.startOneShot(0, FROM_HERE);
}

void WebSocketChannelImpl::willOpenSocketStream(SocketStreamHandle*)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p willOpenSocketStream()", this);
}

void WebSocketChannelImpl::didOpenSocketStream(SocketStreamHandle* handle)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p didOpenSocketStream()", this);
    ASSERT(handle == m_handle);
    if (!document())
        return;
    if (m_id)
        InspectorInstrumentation::willSendWebSocketHandshakeRequest(document(), m_id, m_handshake->clientHandshakeRequest().get());
    CString handshakeMessage = m_handshake->clientHandshakeMessage();
    if (!handle->send(handshakeMessage.data(), handshakeMessage.length()))
        fail("Failed to send WebSocket handshake.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
}

void WebSocketChannelImpl::didCloseSocketStream(SocketStreamHandle* handle)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p didCloseSocketStream()", this);
    if (m_id && document())
        InspectorInstrumentation::didCloseWebSocket(document(), m_id);
    ASSERT_UNUSED(handle, handle == m_handle || !m_handle);
    m_closed = true;
    if (m_closingTimer.isActive())
        m_closingTimer.stop();
    if (m_outgoingFrameQueueStatus != OutgoingFrameQueueClosed)
        abortOutgoingFrameQueue();
    if (m_handle) {
        if (ActivatingObjCheck::inst()->isActivating((intptr_t)m_handleId))
            m_unhandledBufferedAmount = m_handle->bufferedAmount();
        
        if (m_suspended)
            return;
        WebSocketChannelClient* client = m_client;
        m_client = nullptr;
        m_handle = nullptr;
        m_handleId = 0;
        if (client && !m_isClosing)
            client->didClose(m_receivedClosingHandshake ? WebSocketChannelClient::ClosingHandshakeComplete : WebSocketChannelClient::ClosingHandshakeIncomplete, m_closeEventCode, m_closeEventReason);
    }
    deref();
}

void WebSocketChannelImpl::didReceiveSocketStreamData(SocketStreamHandle* handle, const char* data, int len)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p didReceiveSocketStreamData() Received %d bytes", this, len);
    RefPtr<WebSocketChannelImpl> protect(*this); // The client can close the channel, potentially removing the last reference.
    ASSERT(handle == m_handle);
    if (!document()) {
        return;
    }
    if (len <= 0) {
        handle->disconnect();
        return;
    }
    if (!m_client) {
        m_shouldDiscardReceivedData = true;
        handle->disconnect();
        return;
    }
    if (m_shouldDiscardReceivedData)
        return;
    if (!appendToBuffer(data, len)) {
        m_shouldDiscardReceivedData = true;
        fail("Ran out of memory while receiving WebSocket data.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return;
    }
    while (!m_suspended && m_client && !m_buffer.isEmpty())
        if (!processBuffer())
            break;
}

void WebSocketChannelImpl::didUpdateBufferedAmount(SocketStreamHandle*, size_t bufferedAmount)
{
    if (m_client)
        m_client->didConsumeBufferedAmount(bufferedAmount);
}

void WebSocketChannelImpl::didFailSocketStream(SocketStreamHandle* handle, const SocketStreamError& error)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p didFailSocketStream()", this);
    ASSERT(handle == m_handle || !m_handle);
    if (document()) {
        String message;
        if (error.isNull())
            message = "WebSocket network error";
        else if (error.localizedDescription().isNull())
            message = "WebSocket network error: error code " + String::number(error.errorCode());
        else
            message = "WebSocket network error: " + error.localizedDescription();
        InspectorInstrumentation::didReceiveWebSocketFrameError(document(), m_id, message);
        document()->addConsoleMessage(ConsoleMessage::create(NetworkMessageSource, ErrorMessageLevel, message));
    }
    m_shouldDiscardReceivedData = true;
    handle->disconnect();
}

void WebSocketChannelImpl::didReceiveAuthenticationChallenge(SocketStreamHandle*, const AuthenticationChallenge&)
{
}

void WebSocketChannelImpl::didCancelAuthenticationChallenge(SocketStreamHandle*, const AuthenticationChallenge&)
{
}

void WebSocketChannelImpl::didFinishLoadingBlob(PassRefPtr<DOMArrayBuffer> buffer)
{
    ASSERT(m_handle);
    // The loaded blob is always placed on m_messages[0].
    ASSERT(m_outgoingFrameQueue.size() > 0 && m_outgoingFrameQueue.first()->frameType == QueuedFrameTypeBlob);
    // We replace it with the loaded blob.
    OwnPtr<QueuedFrame>& frame = m_outgoingFrameQueue.first();
    frame->opCode = WebSocketOneFrame::OpCodeBinary;
    frame->isHook = false;
    frame->frameType = QueuedFrameTypeVector;
    frame->vectorData.resize(buffer->byteLength());
    if (buffer->byteLength())
        memcpy(frame->vectorData.data(), buffer->data(), buffer->byteLength());

    processOutgoingFrameQueue();
    m_blobLoader = nullptr;
}

void WebSocketChannelImpl::didFailLoadingBlob(FileError::ErrorCode errorCode)
{
    if (errorCode == FileError::ABORT_ERR) {
        m_blobLoader = nullptr;
        // The error is caused by cancel().
        return;
    }
    // FIXME: Generate human-friendly reason message.
    failAsError("Failed to load Blob: error code = " + String::number(errorCode));
    // |this| can be deleted here.
    m_blobLoader = nullptr;
}

bool WebSocketChannelImpl::appendToBuffer(const char* data, size_t len)
{
    size_t newBufferSize = m_buffer.size() + len;
    if (newBufferSize < m_buffer.size()) {
        //WTF_LOG(Network, "WebSocketChannelImpl %p appendToBuffer() Buffer overflow (%lu bytes already in receive buffer and appending %lu bytes)", this, static_cast<unsigned long>(m_buffer.size()), static_cast<unsigned long>(len));
        return false;
    }
    m_buffer.append(data, len);
    return true;
}

void WebSocketChannelImpl::skipBuffer(size_t len)
{
    ASSERT_WITH_SECURITY_IMPLICATION(len <= m_buffer.size());
    memmove(m_buffer.data(), m_buffer.data() + len, m_buffer.size() - len);
    m_buffer.resize(m_buffer.size() - len);
}

bool WebSocketChannelImpl::processBuffer()
{
    ASSERT(!m_suspended);
    ASSERT(m_client);
    ASSERT(!m_buffer.isEmpty());
    WTF_LOG(Network, "WebSocketChannelImpl %p processBuffer() Receive buffer has %lu bytes", this, static_cast<unsigned long>(m_buffer.size()));

    if (m_shouldDiscardReceivedData)
        return false;

    if (m_receivedClosingHandshake) {
        skipBuffer(m_buffer.size());
        return false;
    }

    RefPtr<WebSocketChannelImpl> protect(*this); // The client can close the channel, potentially removing the last reference.

    if (m_handshake->mode() == WebSocketHandshake::Incomplete) {
        int headerLength = m_handshake->readServerHandshake(m_buffer.data(), m_buffer.size());
        if (headerLength <= 0)
            return false;
        if (m_handshake->mode() == WebSocketHandshake::Connected) {
            bool isHook = false;
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
            if (m_onConnect) {
                WebViewImpl* view = (WebViewImpl*)document()->page()->chromeClient().webView();
                if (view) {
                    content::WebPageImpl* pageImpl = (content::WebPageImpl*)view->client();
                    if (pageImpl) {
                        content::WebPage* page = pageImpl->m_pagePtr;
                        isHook = m_onConnect(page->wkeWebView(), m_hookUserParam, this);
                    }
                }
            }
#endif
            if (!isHook) {
                if (m_id)
                    InspectorInstrumentation::didReceiveWebSocketHandshakeResponse(document(), m_id, m_handshake->clientHandshakeRequest().get(), &m_handshake->serverHandshakeResponse());
                if (!m_handshake->serverSetCookie().isEmpty()) {
                    if (cookiesEnabled(document())) {
                        // Exception (for sandboxed documents) ignored.
                        document()->setCookie(m_handshake->serverSetCookie(), IGNORE_EXCEPTION);
                    }
                }
                // FIXME: handle set-cookie2.
                WTF_LOG(Network, "WebSocketChannelImpl %p Connected", this);
                skipBuffer(headerLength);
                m_client->didConnect("", "");
                WTF_LOG(Network, "WebSocketChannelImpl %p %lu bytes remaining in m_buffer", this, static_cast<unsigned long>(m_buffer.size()));
                return !m_buffer.isEmpty();
            }
        }
        ASSERT(m_handshake->mode() == WebSocketHandshake::Failed);
        WTF_LOG(Network, "WebSocketChannelImpl %p Connection failed", this);
        skipBuffer(headerLength);
        m_shouldDiscardReceivedData = true;
        fail(m_handshake->failureReason(), ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }
    if (m_handshake->mode() != WebSocketHandshake::Connected)
        return false;

    return processFrame();
}

void WebSocketChannelImpl::resumeTimerFired(blink::Timer<WebSocketChannelImpl>*)
{
    RefPtr<WebSocketChannelImpl> protect(*this); // The client can close the channel, potentially removing the last reference.
    while (!m_suspended && m_client && !m_buffer.isEmpty())
        if (!processBuffer())
            break;
    if (!m_suspended && m_client && m_closed && m_handle)
        didCloseSocketStream(m_handle.get());
}

void WebSocketChannelImpl::startClosingHandshake(int code, const String& reason)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p startClosingHandshake() code=%d m_receivedClosingHandshake=%d", this, m_closing, m_receivedClosingHandshake);
    ASSERT(!m_closed);
    if (m_closing)
        return;
    ASSERT(m_handle);

    Vector<char> buf;
    if (!m_receivedClosingHandshake && code != CloseEventCodeNotSpecified) {
        unsigned char highByte = code >> 8;
        unsigned char lowByte = code;
        buf.append(static_cast<char>(highByte));
        buf.append(static_cast<char>(lowByte));
        buf.append(reason.utf8().data(), reason.utf8().length());
    }
    enqueueRawFrame(WebSocketOneFrame::OpCodeClose, buf.data(), buf.size(), false);
    RefPtr<WebSocketChannelImpl> protect(*this); // An attempt to send closing handshake may fail, which will get the channel closed and dereferenced.
    processOutgoingFrameQueue();

    if (m_closed) {
        // The channel got closed because processOutgoingFrameQueue() failed.
        return;
    }

    m_closing = true;
    if (m_client)
        m_client->didStartClosingHandshake();
}

void WebSocketChannelImpl::closingTimerFired(blink::Timer<WebSocketChannelImpl>*)
{
    WTF_LOG(Network, "WebSocketChannelImpl %p closingTimerFired()", this);
    if (m_handle && ActivatingObjCheck::inst()->isActivating((intptr_t)m_handleId))
        m_handle->disconnect();
}

bool WebSocketChannelImpl::processFrame()
{
    ASSERT(!m_buffer.isEmpty());

    WebSocketOneFrame frame;
    const char* frameEnd;
    String errorString;
    WebSocketOneFrame::ParseFrameResult result = WebSocketOneFrame::parseFrame(m_buffer.data(), m_buffer.size(), frame, frameEnd, errorString);
    if (result == WebSocketOneFrame::FrameIncomplete)
        return false;
    if (result == WebSocketOneFrame::FrameError) {
        fail(errorString, ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }

    ASSERT(m_buffer.data() < frameEnd);
    ASSERT(frameEnd <= m_buffer.data() + m_buffer.size());

    auto inflateResult = m_deflateFramer.inflate(frame);
    if (!inflateResult->succeeded()) {
        fail(inflateResult->failureReason(), ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }

    // Validate the frame data.
    if (WebSocketOneFrame::isReservedOpCode(frame.opCode)) {
        fail("Unrecognized frame opcode: " + String::number(frame.opCode), ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }

    if (frame.reserved2 || frame.reserved3) {
        fail("One or more reserved bits are on: reserved2 = " + String::number(frame.reserved2) + ", reserved3 = " + String::number(frame.reserved3), ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }

    if (frame.masked) {
        fail("A server must not mask any frames that it sends to the client.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }

    // All control frames must not be fragmented.
    if (WebSocketOneFrame::isControlOpCode(frame.opCode) && !frame.final) {
        fail(String::format("Received fragmented control frame: opcode = %d", frame.opCode), ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }

    // All control frames must have a payload of 125 bytes or less, which means the frame must not contain
    // the "extended payload length" field.
    if (WebSocketOneFrame::isControlOpCode(frame.opCode) && WebSocketOneFrame::needsExtendedLengthField(frame.payloadLength)) {
        fail(String::format("Received control frame having too long payload: %d bytes", frame.payloadLength), ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }

    // A new data frame is received before the previous continuous frame finishes.
    // Note that control frames are allowed to come in the middle of continuous frames.
    if (m_hasContinuousFrame && frame.opCode != WebSocketOneFrame::OpCodeContinuation && !WebSocketOneFrame::isControlOpCode(frame.opCode)) {
        fail("Received new data frame but previous continuous frame is unfinished.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }
    const char* payload = frame.payload;
    size_t payloadLength = frame.payloadLength;
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CString new_data("", 0);
    if (m_onReceive) {
        WebViewImpl* view = (WebViewImpl*)document()->page()->chromeClient().webView();
        if (view) {
            content::WebPageImpl* pageImpl = (content::WebPageImpl*)view->client();
            if (pageImpl) {
                content::WebPage* page = pageImpl->m_pagePtr;
                if (m_onReceive(page->wkeWebView(), m_hookUserParam, this, frame.opCode, frame.payload, frame.payloadLength, &new_data)) {
                    return !m_buffer.isEmpty();
                }
            }
        }
    }
    if (new_data.length() > 0) {
        payload = new_data.string();
        payloadLength = new_data.length();
    }
#endif
    InspectorInstrumentation::didReceiveWebSocketFrame(document(), m_id, frame.opCode, frame.masked, payload, payloadLength);

    switch (frame.opCode) {
    case WebSocketOneFrame::OpCodeContinuation:
        // An unexpected continuation frame is received without any leading frame.
        if (!m_hasContinuousFrame) {
            fail("Received unexpected continuation frame.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
            return false;
        }
        m_continuousFrameData.append(payload, payloadLength);
        skipBuffer(frameEnd - m_buffer.data());
        if (frame.final) {
            // onmessage handler may eventually call the other methods of this channel,
            // so we should pretend that we have finished to read this frame and
            // make sure that the member variables are in a consistent state before
            // the handler is invoked.
            OwnPtr<Vector<char>> continuousFrameData = adoptPtr(new Vector<char>());
            continuousFrameData->appendVector(m_continuousFrameData);
            m_continuousFrameData.clear();
            m_hasContinuousFrame = false;
            if (m_continuousFrameOpCode == WebSocketOneFrame::OpCodeText) {
                String message;
                if (continuousFrameData->size())
                    message = String::fromUTF8(continuousFrameData->data(), continuousFrameData->size());
                else
                    message = emptyString();
                if (message.isNull())
                    fail("Could not decode a text frame as UTF-8.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
                else
                    m_client->didReceiveTextMessage(message);
            } else if (m_continuousFrameOpCode == WebSocketOneFrame::OpCodeBinary)
                m_client->didReceiveBinaryMessage(continuousFrameData.release());
        }
        break;

    case WebSocketOneFrame::OpCodeText:
        if (frame.final) {
            String message;
            if (payloadLength)
                message = String::fromUTF8(payload, payloadLength);
            else
                message = emptyString();
            skipBuffer(frameEnd - m_buffer.data());
            if (message.isNull())
                fail("Could not decode a text frame as UTF-8.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
            else
                m_client->didReceiveTextMessage(message);
        } else {
            m_hasContinuousFrame = true;
            m_continuousFrameOpCode = WebSocketOneFrame::OpCodeText;
            ASSERT(m_continuousFrameData.isEmpty());
            m_continuousFrameData.append(payload, payloadLength);
            skipBuffer(frameEnd - m_buffer.data());
        }
        break;

    case WebSocketOneFrame::OpCodeBinary:
        if (frame.final) {
            OwnPtr<Vector<char>> binaryData = adoptPtr(new Vector<char>(frame.payloadLength));
            memcpy(binaryData->data(), payload, payloadLength);
            skipBuffer(frameEnd - m_buffer.data());
            m_client->didReceiveBinaryMessage(binaryData.release());
        } else {
            m_hasContinuousFrame = true;
            m_continuousFrameOpCode = WebSocketOneFrame::OpCodeBinary;
            ASSERT(m_continuousFrameData.isEmpty());
            m_continuousFrameData.append(payload, payloadLength);
            skipBuffer(frameEnd - m_buffer.data());
        }
        break;

    case WebSocketOneFrame::OpCodeClose:
        if (!payloadLength)
            m_closeEventCode = CloseEventCodeNoStatusRcvd;
        else if (payloadLength == 1) {
            m_closeEventCode = CloseEventCodeAbnormalClosure;
            fail("Received a broken close frame containing an invalid size body.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
            return false;
        } else {
            unsigned char highByte = static_cast<unsigned char>(payload[0]);
            unsigned char lowByte = static_cast<unsigned char>(payload[1]);
            m_closeEventCode = highByte << 8 | lowByte;
            if (m_closeEventCode == CloseEventCodeNoStatusRcvd || m_closeEventCode == CloseEventCodeAbnormalClosure || m_closeEventCode == CloseEventCodeTLSHandshake) {
                m_closeEventCode = CloseEventCodeAbnormalClosure;
                fail("Received a broken close frame containing a reserved status code.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
                return false;
            }
        }
        if (payloadLength >= 3)
            m_closeEventReason = String::fromUTF8(&payload[2], payloadLength - 2);
        else
            m_closeEventReason = emptyString();
        skipBuffer(frameEnd - m_buffer.data());
        m_receivedClosingHandshake = true;
        startClosingHandshake(m_closeEventCode, m_closeEventReason);
        if (m_closing) {
            if (m_outgoingFrameQueueStatus == OutgoingFrameQueueOpen)
                m_outgoingFrameQueueStatus = OutgoingFrameQueueClosing;
            processOutgoingFrameQueue();
        }
        break;

    case WebSocketOneFrame::OpCodePing:
        enqueueRawFrame(WebSocketOneFrame::OpCodePong, payload, payloadLength, false);
        skipBuffer(frameEnd - m_buffer.data());
        processOutgoingFrameQueue();
        break;

    case WebSocketOneFrame::OpCodePong:
        // A server may send a pong in response to our ping, or an unsolicited pong which is not associated with
        // any specific ping. Either way, there's nothing to do on receipt of pong.
        skipBuffer(frameEnd - m_buffer.data());
        break;

    default:
        ASSERT_NOT_REACHED();
        skipBuffer(frameEnd - m_buffer.data());
        break;
    }

    return !m_buffer.isEmpty();
}

void WebSocketChannelImpl::enqueueTextFrame(const CString& string, bool isHook)
{
    ASSERT(m_outgoingFrameQueueStatus == OutgoingFrameQueueOpen);
    PassOwnPtr<QueuedFrame> frame = adoptPtr(new QueuedFrame());
    frame->opCode = WebSocketOneFrame::OpCodeText;
    frame->isHook = isHook;
    frame->frameType = QueuedFrameTypeString;
    frame->stringData = string;
    m_outgoingFrameQueue.append(frame);
}

void WebSocketChannelImpl::enqueueRawFrame(WebSocketOneFrame::OpCode opCode, const char* data, size_t dataLength, bool isHook)
{
    ASSERT(m_outgoingFrameQueueStatus == OutgoingFrameQueueOpen);
    PassOwnPtr<QueuedFrame> frame = adoptPtr(new QueuedFrame());
    frame->opCode = opCode;
    frame->isHook = isHook;
    frame->frameType = QueuedFrameTypeVector;
    frame->vectorData.resize(dataLength);
    if (dataLength)
        memcpy(frame->vectorData.data(), data, dataLength);
    m_outgoingFrameQueue.append(frame);
}

void WebSocketChannelImpl::enqueueBlobFrame(WebSocketOneFrame::OpCode opCode, PassRefPtr<BlobDataHandle> blob)
{
    ASSERT(m_outgoingFrameQueueStatus == OutgoingFrameQueueOpen);
    PassOwnPtr<QueuedFrame> frame = adoptPtr(new QueuedFrame());
    frame->opCode = opCode;
    frame->isHook = false;
    frame->frameType = QueuedFrameTypeBlob;
    frame->blobData = blob;
    m_outgoingFrameQueue.append(frame);
}

void WebSocketChannelImpl::processOutgoingFrameQueue()
{
    if (m_outgoingFrameQueueStatus == OutgoingFrameQueueClosed)
        return;

    RefPtr<WebSocketChannelImpl> protect(*this); // Any call to fail() will get the channel closed and dereferenced.

    while (!m_outgoingFrameQueue.isEmpty()) {
        PassOwnPtr<QueuedFrame> frame = m_outgoingFrameQueue.takeFirst();
        switch (frame->frameType) {
        case QueuedFrameTypeString: {
            if (!sendFrame(frame->opCode, frame->stringData.data(), frame->stringData.length(), false))
                fail("Failed to send WebSocket frame.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
            break;
        }

        case QueuedFrameTypeVector:
            if (!sendFrame(frame->opCode, frame->vectorData.data(), frame->vectorData.size(), false))
                fail("Failed to send WebSocket frame.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
            break;

        case QueuedFrameTypeBlob: {
            switch (m_blobLoaderStatus) {
            case BlobLoaderNotStarted:
                ref(); // Will be derefed after didFinishLoading() or didFail().
                ASSERT(!m_blobLoader);
                m_blobLoader = new BlobLoader(frame->blobData, this);
                m_blobLoaderStatus = BlobLoaderStarted;
                m_outgoingFrameQueue.prepend(frame);
                return;

            case BlobLoaderStarted:
            case BlobLoaderFailed:
                m_outgoingFrameQueue.prepend(frame);
                return;

            case BlobLoaderFinished: {
                RefPtr<DOMArrayBuffer> result = m_blobLoader->arrayBufferResult();
                m_blobLoaderStatus = BlobLoaderNotStarted;
                if (!sendFrame(frame->opCode, static_cast<const char*>(result->data()), result->byteLength(), false))
                    fail("Failed to send WebSocket frame.", ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
                m_blobLoader = nullptr;
                break;
            }
            }
            break;
        }

        default:
            ASSERT_NOT_REACHED();
            break;
        }
    }

    ASSERT(m_outgoingFrameQueue.isEmpty());
    if (m_outgoingFrameQueueStatus == OutgoingFrameQueueClosing) {
        m_outgoingFrameQueueStatus = OutgoingFrameQueueClosed;
        if (ActivatingObjCheck::inst()->isActivating((intptr_t)m_handleId))
            m_handle->close();
    }
}

void WebSocketChannelImpl::abortOutgoingFrameQueue()
{
    m_outgoingFrameQueue.clear();
    m_outgoingFrameQueueStatus = OutgoingFrameQueueClosed;
    if (m_blobLoaderStatus == BlobLoaderStarted) {
        if (m_blobLoader)
            m_blobLoader->cancel();
        didFail(FileError::ABORT_ERR);
    }
}

void WebSocketChannelImpl::didFail(FileError::ErrorCode errorCode)
{
    WTF_LOG(Network, "WebSocketChannel %p didFail() errorCode=%d", this, errorCode);
    ASSERT(m_blobLoader);
    ASSERT(m_blobLoaderStatus == BlobLoaderStarted);
    m_blobLoader = nullptr;
    m_blobLoaderStatus = BlobLoaderFailed;
    fail(String::format("Failed to load Blob: error code = %d", errorCode), ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction); // FIXME: Generate human-friendly reason message.
    deref();
}

bool WebSocketChannelImpl::sendFrame(WebSocketOneFrame::OpCode opCode, const char* data, size_t dataLength, bool isHook)
{
    ASSERT(m_handle);
    ASSERT(!m_suspended);
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CString new_data("", 0);
    if (&isHook && m_onSend) {
        WebViewImpl* view = (WebViewImpl*)document()->page()->chromeClient().webView();
        if (view) {
            content::WebPageImpl* pageImpl = (content::WebPageImpl*)view->client();
            if (pageImpl) {
                content::WebPage* page = pageImpl->m_pagePtr;
                if (m_onSend(page->wkeWebView(), m_hookUserParam, this, opCode, data, dataLength, &new_data)) {
                    return true;
                }
            }
        }
    }
    if (new_data.length() > 0) {
        data = new_data.string();
        dataLength = new_data.length();
    }
#endif
    WebSocketOneFrame frame(opCode, true, false, true, data, dataLength);
    InspectorInstrumentation::didSendWebSocketFrame(document(), m_id, frame.opCode, frame.masked, frame.payload, frame.payloadLength);

    auto deflateResult = m_deflateFramer.deflate(frame);
    if (!deflateResult->succeeded()) {
        fail(deflateResult->failureReason(), ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction);
        return false;
    }

    Vector<char> frameData;
    frame.makeFrameData(frameData);

    if (ActivatingObjCheck::inst()->isActivating((intptr_t)m_handleId))
        return m_handle->send(frameData.data(), frameData.size());
    return false;
}

void WebSocketChannelImpl::ref()
{
    m_ref++;
}

void WebSocketChannelImpl::deref()
{
    m_ref--;
}

DEFINE_TRACE(WebSocketChannelImpl)
{
    visitor->trace(m_blobLoader);
    visitor->trace(m_client);
    WebSocketChannel::trace(visitor);
    ContextLifecycleObserver::trace(visitor);
}

}  // namespace net
