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

#ifndef WebSocketChannelImpl_h
#define WebSocketChannelImpl_h

#include "net/websocket/SocketStreamHandleClient.h"
#include "net/websocket/WebSocketDeflateFramer.h"
#include "net/websocket/WebSocketOneFrame.h"
#include "net/websocket/WebSocketHandshake.h"
#include "third_party/WebKit/Source/modules/websockets/WebSocketChannel.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/Source/core/fileapi/FileReaderLoaderClient.h"
#include "third_party/WebKit/Source/core/dom/ContextLifecycleObserver.h"
#include "third_party/WebKit/Source/platform/heap/Handle.h"
#include "third_party/WebKit/public/platform/WebSocketHandleClient.h"
#include <wtf/Deque.h>
#include <wtf/Forward.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#include "wke/wkeWebView.h"
#include "wke/wkeString.h"
#endif
namespace blink {
class KURL;
class Document;
class FileReaderLoader;
class SocketStreamError;
class WebSocketChannelClient;
class ExecutionContext;
}

namespace net {

class WebSocketChannelImpl
    : public blink::WebSocketChannel
    , public SocketStreamHandleClient
    , public blink::ContextLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(WebSocketChannelImpl);
public:
    static WebSocketChannelImpl* create(blink::ExecutionContext* context, blink::WebSocketChannelClient* client, const String& sourceURL, unsigned lineNumber, blink::WebSocketHandle* handle)
    {
        return new WebSocketChannelImpl(context, client, sourceURL, lineNumber, handle);
    }
    virtual ~WebSocketChannelImpl();

    bool send(const char* data, int length, bool hook);

    // WebSocketChannel functions.
    virtual bool connect(const blink::KURL&, const String& protocol) override;
//     virtual String subprotocol() override;
//     virtual String extensions() override;
    void send(const CString&, bool isHook);
    virtual void send(const CString&) override;
    virtual void send(PassRefPtr<blink::BlobDataHandle>) override;
    virtual void send(const blink::DOMArrayBuffer&, unsigned byteOffset, unsigned byteLength) override;
    /*virtual*/ unsigned long bufferedAmount() const /*override*/;
    virtual void close(int code, const String& reason) override; // Start closing handshake.
    virtual void fail(const String& reason, blink::MessageLevel, const String& sourceURL, unsigned lineNumber) override;
    virtual void disconnect() override;
    virtual void sendTextAsCharVector(PassOwnPtr<Vector<char>> data) override;
    virtual void sendBinaryAsCharVector(PassOwnPtr<Vector<char>> data) override;

    void failAsError(const String& reason) { fail(reason, blink::ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction); }

    /*virtual*/ void suspend() /*override*/;
    /*virtual*/ void resume() /*override*/;

    // SocketStreamHandleClient functions.
    virtual void willOpenSocketStream(SocketStreamHandle*) override;
    virtual void didOpenSocketStream(SocketStreamHandle*) override;
    virtual void didCloseSocketStream(SocketStreamHandle*) override;
    virtual void didReceiveSocketStreamData(SocketStreamHandle*, const char*, int) override;
    virtual void didUpdateBufferedAmount(SocketStreamHandle*, size_t bufferedAmount) override;
    virtual void didFailSocketStream(SocketStreamHandle*, const SocketStreamError&) override;
    virtual void didReceiveAuthenticationChallenge(SocketStreamHandle*, const blink::AuthenticationChallenge&) override;
    virtual void didCancelAuthenticationChallenge(SocketStreamHandle*, const blink::AuthenticationChallenge&) override;

    virtual unsigned long getId() const override { return m_id; }

    enum CloseEventCode {
        CloseEventCodeNotSpecified = -1,
        CloseEventCodeNormalClosure = 1000,
        CloseEventCodeGoingAway = 1001,
        CloseEventCodeProtocolError = 1002,
        CloseEventCodeUnsupportedData = 1003,
        CloseEventCodeFrameTooLarge = 1004,
        CloseEventCodeNoStatusRcvd = 1005,
        CloseEventCodeAbnormalClosure = 1006,
        CloseEventCodeInvalidFramePayloadData = 1007,
        CloseEventCodePolicyViolation = 1008,
        CloseEventCodeMessageTooBig = 1009,
        CloseEventCodeMandatoryExt = 1010,
        CloseEventCodeInternalError = 1011,
        CloseEventCodeTLSHandshake = 1015,
        CloseEventCodeMinimumUserDefined = 3000,
        CloseEventCodeMaximumUserDefined = 4999
    };

    void didFail(blink::FileError::ErrorCode errorCode);

    DECLARE_VIRTUAL_TRACE();
    
    void ref();
    void deref();
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    //hook
    bool(WKE_CALL_TYPE* m_onConnect)(wkeWebView webView, void* param, WebSocketChannelImpl* job);
    bool(WKE_CALL_TYPE* m_onReceive)(wkeWebView webView, void* param, WebSocketChannelImpl* job, int OpCode, const char* buf, size_t len, wkeString new_data);
    bool(WKE_CALL_TYPE* m_onSend)(wkeWebView webView, void* param, WebSocketChannelImpl* job, int OpCode, const char* buf, size_t len, wkeString new_data);
    void(WKE_CALL_TYPE* m_onError)(wkeWebView webView, void* param, WebSocketChannelImpl* job);
    void* m_hookUserParam;
#endif
protected:

private:
    WebSocketChannelImpl(blink::ExecutionContext* context, blink::WebSocketChannelClient* client, const String& sourceURL, unsigned lineNumber, blink::WebSocketHandle* handle);

    bool appendToBuffer(const char* data, size_t len);
    void skipBuffer(size_t len);
    bool processBuffer();
    void resumeTimerFired(blink::Timer<WebSocketChannelImpl>*);
    void startClosingHandshake(int code, const String& reason);
    void closingTimerFired(blink::Timer<WebSocketChannelImpl>*);

    bool processFrame();

    // It is allowed to send a Blob as a binary frame if hybi-10 protocol is in use. Sending a Blob
    // can be delayed because it must be read asynchronously. Other types of data (String or
    // ArrayBuffer) may also be blocked by preceding sending request of a Blob.
    //
    // To address this situation, messages to be sent need to be stored in a queue. Whenever a new
    // data frame is going to be sent, it first must go to the queue. Items in the queue are processed
    // in the order they were put into the queue. Sending request of a Blob blocks further processing
    // until the Blob is completely read and sent to the socket stream.
    enum QueuedFrameType {
        QueuedFrameTypeString,
        QueuedFrameTypeVector,
        QueuedFrameTypeBlob
    };
    struct QueuedFrame {
        WebSocketOneFrame::OpCode opCode;
        QueuedFrameType frameType;
        // Only one of the following items is used, according to the value of frameType.
        CString stringData;
        Vector<char> vectorData;
        RefPtr<blink::BlobDataHandle> blobData;
        bool isHook;
    };
    void enqueueTextFrame(const CString&, bool isHook);
    void enqueueRawFrame(WebSocketOneFrame::OpCode, const char* data, size_t dataLength, bool isHook);
    void enqueueBlobFrame(WebSocketOneFrame::OpCode, PassRefPtr<blink::BlobDataHandle>);

    void processOutgoingFrameQueue();
    void abortOutgoingFrameQueue();

    enum OutgoingFrameQueueStatus {
        // It is allowed to put a new item into the queue.
        OutgoingFrameQueueOpen,
        // Close frame has already been put into the queue but may not have been sent yet;
        // m_handle->close() will be called as soon as the queue is cleared. It is not
        // allowed to put a new item into the queue.
        OutgoingFrameQueueClosing,
        // Close frame has been sent or the queue was aborted. It is not allowed to put
        // a new item to the queue.
        OutgoingFrameQueueClosed
    };

    // If you are going to send a hybi-10 frame, you need to use the outgoing frame queue
    // instead of call sendFrame() directly.
    bool sendFrame(WebSocketOneFrame::OpCode, const char* data, size_t dataLength, bool isHook);

    enum BlobLoaderStatus {
        BlobLoaderNotStarted,
        BlobLoaderStarted,
        BlobLoaderFinished,
        BlobLoaderFailed
    };

    blink::Document* document();

    // Methods for BlobLoader.
    class BlobLoader;
    void didFinishLoadingBlob(PassRefPtr<blink::DOMArrayBuffer>);
    void didFailLoadingBlob(blink::FileError::ErrorCode);
    int m_ref;

    blink::Member<blink::WebSocketChannelClient> m_client;
    OwnPtr<WebSocketHandshake> m_handshake;
    RefPtr<SocketStreamHandle> m_handle;
    int m_handleId;

    Vector<char> m_buffer;

    blink::Timer<WebSocketChannelImpl> m_resumeTimer;
    bool m_suspended;
    bool m_closing;
    bool m_receivedClosingHandshake;
    blink::Timer<WebSocketChannelImpl> m_closingTimer;
    bool m_closed;
    bool m_shouldDiscardReceivedData;
    unsigned long m_unhandledBufferedAmount;

    unsigned long m_id; // m_identifier == 0 means that we could not obtain a valid identifier.

    // Private members only for hybi-10 protocol.
    bool m_hasContinuousFrame;
    WebSocketOneFrame::OpCode m_continuousFrameOpCode;
    Vector<char> m_continuousFrameData;
    unsigned short m_closeEventCode;
    String m_closeEventReason;

    Deque<OwnPtr<QueuedFrame> > m_outgoingFrameQueue;
    OutgoingFrameQueueStatus m_outgoingFrameQueueStatus;

    // FIXME: Load two or more Blobs simultaneously for better performance.
    blink::Member<BlobLoader> m_blobLoader;
    BlobLoaderStatus m_blobLoaderStatus;

    WebSocketDeflateFramer m_deflateFramer;

    String m_sourceURLAtConstruction;
    unsigned m_lineNumberAtConstruction;

    bool m_isClosing;
};

} // namespace net

#endif // WebSocketChannelImpl_h
